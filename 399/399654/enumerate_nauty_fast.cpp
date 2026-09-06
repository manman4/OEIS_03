#include <algorithm>
#include <array>
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unordered_map>
#include <utility>
#include <vector>
#include <unistd.h>

namespace fs = std::filesystem;

namespace {

struct Options {
    int min_n = 3;
    int max_n = 8;
    std::string geng = "geng";
    fs::path output = "results_nauty_fast.json";
};

constexpr std::array<std::uint64_t, 12> known_graph_counts = {
    0, 1, 2, 4, 11, 34, 156, 1'044, 12'346, 274'668, 12'005'168,
    1'018'997'864,
};
constexpr std::uint64_t progress_interval = 10'000'000;

struct GengProcess {
    pid_t pid = -1;
    FILE* stream = nullptr;

    GengProcess(const std::string& executable, int n) {
        int output_pipe[2];
        if (pipe(output_pipe) != 0) {
            throw std::runtime_error("pipe failed: " + std::string(std::strerror(errno)));
        }

        const std::string n_text = std::to_string(n);
        pid = fork();
        if (pid < 0) {
            close(output_pipe[0]);
            close(output_pipe[1]);
            throw std::runtime_error("fork failed: " + std::string(std::strerror(errno)));
        }

        if (pid == 0) {
            close(output_pipe[0]);
            if (dup2(output_pipe[1], STDOUT_FILENO) < 0) {
                _exit(126);
            }
            close(output_pipe[1]);
            execlp(executable.c_str(), executable.c_str(), "-q", n_text.c_str(),
                   static_cast<char*>(nullptr));
            _exit(127);
        }

        close(output_pipe[1]);
        stream = fdopen(output_pipe[0], "r");
        if (stream == nullptr) {
            close(output_pipe[0]);
            kill(pid, SIGTERM);
            waitpid(pid, nullptr, 0);
            throw std::runtime_error("fdopen failed: " + std::string(std::strerror(errno)));
        }
    }

    GengProcess(const GengProcess&) = delete;
    GengProcess& operator=(const GengProcess&) = delete;

    int finish() {
        if (stream != nullptr) {
            fclose(stream);
            stream = nullptr;
        }
        int status = 0;
        if (pid >= 0 && waitpid(pid, &status, 0) < 0) {
            throw std::runtime_error("waitpid failed: " + std::string(std::strerror(errno)));
        }
        pid = -1;
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
        return 128;
    }

    ~GengProcess() {
        if (stream != nullptr) {
            fclose(stream);
        }
        if (pid >= 0) {
            kill(pid, SIGTERM);
            waitpid(pid, nullptr, 0);
        }
    }
};

struct Graph {
    int n = 0;
    std::array<std::uint64_t, 62> adjacency{};
};

struct SearchState {
    std::uint64_t visited;
    std::uint8_t last;
    std::uint8_t path_length;
};

struct Result {
    int n = 0;
    std::uint64_t graph_count = 0;
    std::unordered_map<std::uint64_t, std::string> witnesses;
};

[[nodiscard]] int lowest_vertex(std::uint64_t bits) {
    return __builtin_ctzll(bits);
}

[[nodiscard]] Graph decode_graph6(std::string_view record) {
    constexpr std::string_view header = ">>graph6<<";
    if (record.starts_with(header)) {
        record.remove_prefix(header.size());
    }
    if (record.empty()) {
        throw std::runtime_error("empty graph6 record");
    }

    Graph graph;
    graph.n = static_cast<unsigned char>(record.front()) - 63;
    if (graph.n < 0 || graph.n > 62) {
        throw std::runtime_error("only one-byte graph6 orders (n <= 62) are supported");
    }

    const int edge_count = graph.n * (graph.n - 1) / 2;
    const int payload_size = (edge_count + 5) / 6;
    record.remove_prefix(1);
    if (static_cast<int>(record.size()) != payload_size) {
        throw std::runtime_error("invalid graph6 payload length");
    }

    int pair_index = 0;
    for (int high = 1; high < graph.n; ++high) {
        for (int low = 0; low < high; ++low, ++pair_index) {
            const int encoded = static_cast<unsigned char>(record[pair_index / 6]);
            if (encoded < 63 || encoded > 126) {
                throw std::runtime_error("graph6 byte outside 63..126");
            }
            const int value = encoded - 63;
            const int shift = 5 - pair_index % 6;
            if ((value & (1 << shift)) != 0) {
                graph.adjacency[low] |= std::uint64_t{1} << high;
                graph.adjacency[high] |= std::uint64_t{1} << low;
            }
        }
    }
    return graph;
}

// Bit k represents the presence of a cycle of length k+3. This is an exact
// port of the minimum-rooted DFS in enumerate_nauty.py.
[[nodiscard]] std::uint64_t cycle_length_mask(const Graph& graph) {
    if (graph.n < 3) {
        return 0;
    }

    const std::uint64_t all_vertices = (std::uint64_t{1} << graph.n) - 1;
    const std::uint64_t all_lengths = (std::uint64_t{1} << (graph.n - 2)) - 1;
    std::uint64_t found = 0;
    std::vector<SearchState> stack;
    stack.reserve(4096);

    for (int start = 0; start < graph.n; ++start) {
        const std::uint64_t start_bit = std::uint64_t{1} << start;
        const std::uint64_t smaller_or_equal = (std::uint64_t{1} << (start + 1)) - 1;
        const std::uint64_t allowed = all_vertices ^ smaller_or_equal;
        std::uint64_t first_vertices = graph.adjacency[start] & allowed;

        while (first_vertices != 0) {
            const std::uint64_t first_bit = first_vertices & -first_vertices;
            first_vertices ^= first_bit;
            const int first = lowest_vertex(first_bit);
            stack.clear();
            stack.push_back({start_bit | first_bit, static_cast<std::uint8_t>(first), 2});

            while (!stack.empty()) {
                const SearchState state = stack.back();
                stack.pop_back();

                if (state.path_length >= 3 &&
                    (graph.adjacency[state.last] & start_bit) != 0) {
                    found |= std::uint64_t{1} << (state.path_length - 3);
                    if (found == all_lengths) {
                        return found;
                    }
                }

                std::uint64_t candidates =
                    graph.adjacency[state.last] & allowed & ~state.visited;
                while (candidates != 0) {
                    const std::uint64_t next_bit = candidates & -candidates;
                    candidates ^= next_bit;
                    stack.push_back({
                        state.visited | next_bit,
                        static_cast<std::uint8_t>(lowest_vertex(next_bit)),
                        static_cast<std::uint8_t>(state.path_length + 1),
                    });
                }
            }
        }
    }
    return found;
}

[[nodiscard]] std::vector<int> mask_to_lengths(std::uint64_t mask, int n) {
    std::vector<int> lengths;
    for (int length = 3; length <= n; ++length) {
        if ((mask & (std::uint64_t{1} << (length - 3))) != 0) {
            lengths.push_back(length);
        }
    }
    return lengths;
}

[[nodiscard]] std::string json_escape(std::string_view input) {
    std::string output;
    output.reserve(input.size() + 8);
    for (const unsigned char c : input) {
        switch (c) {
            case '\"': output += "\\\""; break;
            case '\\': output += "\\\\"; break;
            case '\b': output += "\\b"; break;
            case '\f': output += "\\f"; break;
            case '\n': output += "\\n"; break;
            case '\r': output += "\\r"; break;
            case '\t': output += "\\t"; break;
            default:
                if (c < 0x20) {
                    char buffer[7];
                    std::snprintf(buffer, sizeof(buffer), "\\u%04x", c);
                    output += buffer;
                } else {
                    output.push_back(static_cast<char>(c));
                }
        }
    }
    return output;
}

[[nodiscard]] Result result_for_n(const std::string& geng, int n) {
    GengProcess process(geng, n);
    Result result;
    result.n = n;

    char* line = nullptr;
    std::size_t capacity = 0;
    while (true) {
        const ssize_t size = getline(&line, &capacity, process.stream);
        if (size < 0) {
            break;
        }
        std::size_t length = static_cast<std::size_t>(size);
        while (length > 0 && (line[length - 1] == '\n' || line[length - 1] == '\r')) {
            --length;
        }
        if (length == 0) {
            continue;
        }

        const std::string_view raw(line, length);
        const Graph graph = decode_graph6(raw);
        if (graph.n != n) {
            std::free(line);
            throw std::runtime_error("geng returned a graph of the wrong order");
        }
        const std::uint64_t mask = cycle_length_mask(graph);
        if (!result.witnesses.contains(mask)) {
            result.witnesses.emplace(mask, raw);
        }
        ++result.graph_count;
        if (result.graph_count % progress_interval == 0) {
            std::cerr << "n=" << n << ": processed " << result.graph_count
                      << " graphs, current f(n)=" << result.witnesses.size() << '\n';
        }
    }
    std::free(line);

    if (ferror(process.stream)) {
        throw std::runtime_error("failed while reading geng output");
    }
    const int return_code = process.finish();
    if (return_code != 0) {
        throw std::runtime_error("geng exited with status " + std::to_string(return_code));
    }
    return result;
}

void write_json(const fs::path& path, const std::string& geng,
                const std::vector<Result>& results) {
    std::ofstream output(path);
    if (!output) {
        throw std::runtime_error("cannot open output file: " + path.string());
    }

    output << "{\n"
           << "  \"method\": \"nauty geng; C++ graph6 decoder; minimum-rooted DFS\",\n"
           << "  \"geng_executable\": \"" << json_escape(geng) << "\",\n"
           << "  \"values\": [\n";

    for (std::size_t result_index = 0; result_index < results.size(); ++result_index) {
        const Result& result = results[result_index];
        std::vector<std::pair<std::vector<int>, std::string>> entries;
        entries.reserve(result.witnesses.size());
        for (const auto& [mask, witness] : result.witnesses) {
            entries.emplace_back(mask_to_lengths(mask, result.n), witness);
        }
        std::sort(entries.begin(), entries.end(),
                  [](const auto& left, const auto& right) { return left.first < right.first; });

        output << "    {\n"
               << "      \"n\": " << result.n << ",\n"
               << "      \"unlabeled_graph_count\": " << result.graph_count << ",\n"
               << "      \"f_n\": " << entries.size() << ",\n"
               << "      \"cycle_sets\": [\n";

        for (std::size_t entry_index = 0; entry_index < entries.size(); ++entry_index) {
            const auto& [lengths, witness] = entries[entry_index];
            output << "        {\n          \"lengths\": [";
            for (std::size_t i = 0; i < lengths.size(); ++i) {
                if (i != 0) output << ", ";
                output << lengths[i];
            }
            output << "],\n"
                   << "          \"witness_graph6\": \"" << json_escape(witness) << "\"\n"
                   << "        }" << (entry_index + 1 == entries.size() ? "\n" : ",\n");
        }

        output << "      ]\n"
               << "    }" << (result_index + 1 == results.size() ? "\n" : ",\n");
    }
    output << "  ]\n}\n";
    if (!output) {
        throw std::runtime_error("failed while writing output file: " + path.string());
    }
}

[[noreturn]] void usage(const char* program, int status) {
    std::ostream& stream = status == 0 ? std::cout : std::cerr;
    stream << "Usage: " << program
           << " [--min-n N] [--max-n N] [--geng PATH] [--output PATH]\n";
    std::exit(status);
}

[[nodiscard]] int parse_int(const char* value, std::string_view option) {
    try {
        std::size_t used = 0;
        const int result = std::stoi(value, &used);
        if (used != std::strlen(value)) {
            throw std::invalid_argument("trailing characters");
        }
        return result;
    } catch (const std::exception&) {
        throw std::runtime_error("invalid value for " + std::string(option) + ": " + value);
    }
}

[[nodiscard]] Options parse_options(int argc, char** argv) {
    Options options;
    for (int i = 1; i < argc; ++i) {
        const std::string_view option(argv[i]);
        if (option == "--help" || option == "-h") {
            usage(argv[0], 0);
        }
        if (i + 1 >= argc) {
            usage(argv[0], 2);
        }
        const char* value = argv[++i];
        if (option == "--min-n") {
            options.min_n = parse_int(value, option);
        } else if (option == "--max-n") {
            options.max_n = parse_int(value, option);
        } else if (option == "--geng") {
            options.geng = value;
        } else if (option == "--output") {
            options.output = value;
        } else {
            std::cerr << "Unknown option: " << option << '\n';
            usage(argv[0], 2);
        }
    }
    if (options.min_n < 1 || options.min_n > options.max_n || options.max_n > 11) {
        throw std::runtime_error("require 1 <= min-n <= max-n <= 11");
    }
    return options;
}

}  // namespace

int main(int argc, char** argv) {
    try {
        const Options options = parse_options(argc, argv);
        std::vector<Result> results;
        results.reserve(options.max_n - options.min_n + 1);
        for (int n = options.min_n; n <= options.max_n; ++n) {
            results.push_back(result_for_n(options.geng, n));
            const Result& result = results.back();
            if (result.graph_count != known_graph_counts[n]) {
                throw std::runtime_error(
                    "incomplete geng stream for n=" + std::to_string(n) +
                    ": got " + std::to_string(result.graph_count) +
                    ", expected " + std::to_string(known_graph_counts[n]));
            }
            std::cout << "n=" << n << ": graphs=" << result.graph_count
                      << ", f(n)=" << result.witnesses.size() << '\n';
        }
        write_json(options.output, options.geng, results);
        std::cout << "wrote " << options.output << '\n';
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "error: " << error.what() << '\n';
        return 1;
    }
}
