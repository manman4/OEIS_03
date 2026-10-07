// Exact Ulam(1,2) enumeration; writes A346216 milestones to b346216.txt.
// Ideas: Philip Gibbs (2015, 2017), Donald Knuth (2016).
// New implementation. Empirical rules select fast searches, never answers.
// Build: xcrun --sdk macosx clang++ -std=c++17 -O3 346216_02.cpp -o ulam02
#include <algorithm>
#include <array>
#include <cerrno>
#include <chrono>
#include <csignal>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fcntl.h>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/file.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

using U = std::uint64_t;
using W = __uint128_t;
using I = std::uint32_t;
constexpr U MiB = 1024 * 1024, PageSize = 65536, Prefix = 18000, Width = 18;
constexpr U MaxValue = 2000000000000ULL;
constexpr I Mask = (1U << Width) - 1;
static volatile std::sig_atomic_t interrupted = 0;
static bool stderr_safe = true;
static void on_signal(int signal) { interrupted = signal; }

static U number(const std::string& s) {
    if (s.empty() || s.find_first_not_of("0123456789") != std::string::npos)
        throw std::runtime_error("invalid unsigned integer: " + s);
    std::size_t used = 0;
    U value = std::stoull(s, &used);
    if (used != s.size()) throw std::runtime_error("invalid integer");
    return value;
}
static void io_error(const char* operation) {
    throw std::runtime_error(std::string(operation) + " failed, errno=" + std::to_string(errno));
}
static void write_all(int fd, const void* buffer, std::size_t size) {
    const auto* bytes = static_cast<const char*>(buffer);
    while (size) {
        ssize_t n = write(fd, bytes, size);
        if (n < 0 && errno == EINTR) continue;
        if (n <= 0) io_error("write");
        auto count = static_cast<std::size_t>(n);
        bytes += count; size -= count;
    }
}
static std::string json_string(const std::string& value) {
    const char* hex = "0123456789abcdef";
    std::string result = "\"";
    for (char byte : value) {
        auto c = static_cast<unsigned char>(byte);
        if (c == '"' || c == '\\') { result += '\\'; result += byte; }
        else if (c < 32) {
            result += "\\u00"; result += hex[c >> 4]; result += hex[c & 15];
        } else result += byte;
    }
    return result + '"';
}
static void separate_streams(const struct stat& file, const char* label) {
    for (int fd : {STDOUT_FILENO, STDERR_FILENO}) {
        struct stat stream{};
        if (!fstat(fd, &stream) && stream.st_dev == file.st_dev && stream.st_ino == file.st_ino)
        {
            if (fd == STDERR_FILENO) stderr_safe = false;
            throw std::runtime_error(std::string(fd == STDOUT_FILENO ? "stdout" : "stderr") +
                                     " is the " + label + "; omit shell redirection to that file");
        }
    }
}
struct Options {
    U terms = 10000000000ULL, max_value = MaxValue, memory_mib = 256, cache_mib = 8;
    U seconds = 0, window = 100000, threshold = 100, report = 10000000;
    U verify_until = 0, anchor_limit = 500000, dictionary_limit = 255;
    U p = 856371966, q = 350477575;
    std::string disk = "cache_346216_02.bin", bfile = "b346216.txt";
    std::string summary = "346216_02_summary.json";
    bool self_test = false;
};

// A locked, separate summary file; publish a fully written JSON by atomic rename.
// Canonical parent names and inode checks also reject hard-link aliases.
class Summary {
    std::string path;
    std::vector<std::string> protected_paths;
    int directory = -1, lock = -1;
    static std::string full_name(const std::string& name) {
        namespace fs = std::filesystem;
        fs::path p(name), parent = p.parent_path();
        if (p.filename().empty() || p.filename() == "." || p.filename() == "..")
            throw std::runtime_error("invalid summary/protected filename");
        return (fs::canonical(parent.empty() ? fs::path(".") : parent) / p.filename()).string();
    }
    static bool same_file(const std::string& a, const std::string& b) {
        if (full_name(a) == full_name(b)) return true;
        struct stat x{}, y{};
        return !stat(a.c_str(), &x) && !stat(b.c_str(), &y) &&
               x.st_dev == y.st_dev && x.st_ino == y.st_ino;
    }
    void check(const std::string& name, const char* label) const {
        for (const auto& protected_path : protected_paths)
            if (same_file(name, protected_path))
                throw std::runtime_error("summary or its lock overlaps b-file/working table");
        struct stat st{};
        if (!lstat(name.c_str(), &st)) {
            if (!S_ISREG(st.st_mode)) throw std::runtime_error("summary/lock must be a regular file, not a symlink");
            separate_streams(st, label);
        } else if (errno != ENOENT) io_error("stat summary/lock");
    }
public:
    explicit Summary(const Options& o) {
        if (o.summary.empty()) return;
        if (!o.bfile.empty()) protected_paths.push_back(o.bfile);
        if (!o.disk.empty()) { protected_paths.push_back(o.disk); protected_paths.push_back(o.disk + ".raw"); }
        path = full_name(o.summary);
        const std::string lock_path = path + ".lock";
        check(path, "summary"); check(lock_path, "summary lock");
        try {
            directory = open(std::filesystem::path(path).parent_path().c_str(), O_RDONLY | O_DIRECTORY);
            if (directory < 0) io_error("open summary directory");
            lock = open(lock_path.c_str(), O_CREAT | O_RDWR | O_NOFOLLOW, 0600);
            if (lock < 0) io_error("open summary lock");
            struct stat st{};
            if (fstat(lock, &st)) io_error("stat summary lock");
            if (!S_ISREG(st.st_mode)) throw std::runtime_error("invalid summary lock type");
            check(lock_path, "summary lock");
            if (flock(lock, LOCK_EX | LOCK_NB)) throw std::runtime_error("summary locked by another process");
        } catch (...) {
            if (lock >= 0) close(lock);
            if (directory >= 0) close(directory);
            lock = directory = -1; throw;
        }
    }
    Summary(const Summary&) = delete;
    Summary& operator=(const Summary&) = delete;
    ~Summary() { if (lock >= 0) close(lock); if (directory >= 0) close(directory); }
    void save(const std::string& report) const {
        if (path.empty()) return;
        check(path, "summary");
        std::string pattern = path + ".tmp.XXXXXX";
        std::vector<char> temporary(pattern.begin(), pattern.end()); temporary.push_back('\0');
        int fd = mkstemp(temporary.data());
        if (fd < 0) io_error("create summary temporary file");
        bool published = false;
        try {
            write_all(fd, report.data(), report.size());
            if (fsync(fd)) io_error("fsync summary");
            int result = close(fd); fd = -1;
            if (result) io_error("close summary");
            check(path, "summary");
            if (rename(temporary.data(), path.c_str())) io_error("rename summary");
            published = true;
            if (fsync(directory)) io_error("fsync summary directory");
        } catch (...) {
            if (fd >= 0) close(fd);
            if (!published) unlink(temporary.data());
            throw;
        }
    }
};
struct Budget {
    U used = 0, limit;
    explicit Budget(U bytes) : limit(bytes) {}
    void acquire(U bytes) {
        if (bytes > limit - used) throw std::runtime_error("RAM tables exceed --memory-mib; use --disk");
        used += bytes;
    }
};

// Bounded FIFO cache with a direct page directory (4 bytes per disk page).
// Cache slots and their data buffers are reused on eviction, without hashing.
// Files are exclusively created; the .bin is working storage, not a checkpoint.
class Bytes {
    struct Page { std::vector<std::uint8_t> data; U key = 0; bool dirty = false; };
    std::vector<std::uint8_t> ram;
    std::vector<Page> pages;
    std::vector<I> directory;
    static constexpr I Missing = std::numeric_limits<I>::max();
    std::size_t hand = 0, capacity = 0;
    int fd = -1;
    U length = 0, maximum, hot_key = std::numeric_limits<U>::max();
    std::uint8_t* hot = nullptr;
    Page* hot_page = nullptr;
    Budget& budget;
    void write_page(U key, Page& page) {
        if (!page.dirty) return;
        std::size_t done = 0;
        while (done < PageSize) {
            ssize_t n = pwrite(fd, page.data.data() + done, PageSize - done,
                               static_cast<off_t>(key * PageSize + done));
            if (n < 0 && errno == EINTR) continue;
            if (n <= 0) io_error("pwrite");
            done += static_cast<std::size_t>(n);
        }
        page.dirty = false; ++writes;
    }
    std::uint8_t* load(U key, bool dirty) {
        if (key == hot_key) {
            if (dirty) hot_page->dirty = true;
            return hot;
        }
        I slot = directory[static_cast<std::size_t>(key)];
        if (slot == Missing) {
            if (pages.size() == capacity) {
                slot = static_cast<I>(hand);
                auto& victim = pages[slot];
                write_page(victim.key, victim);
                directory[static_cast<std::size_t>(victim.key)] = Missing;
                hand = (hand + 1) % capacity;
            } else {
                slot = static_cast<I>(pages.size());
                pages.push_back(Page{std::vector<std::uint8_t>(PageSize), key, false});
            }
            auto& page = pages[slot];
            page.key = key; page.dirty = false;
            std::size_t done = 0;
            while (done < PageSize) {
                ssize_t n = pread(fd, page.data.data() + done, PageSize - done,
                                  static_cast<off_t>(key * PageSize + done));
                if (n < 0 && errno == EINTR) continue;
                if (n < 0) io_error("pread");
                if (!n) throw std::runtime_error("unexpected end of working table");
                done += static_cast<std::size_t>(n);
            }
            ++reads;
            directory[static_cast<std::size_t>(key)] = slot;
        }
        hot_key = key; hot_page = &pages[slot]; hot = hot_page->data.data();
        if (dirty) hot_page->dirty = true;
        return hot;
    }
    void grow(U required) {
        if (required > maximum) throw std::runtime_error("table address exceeds supported range");
        if (required <= length) return;
        U size = std::min(maximum, std::max(required, std::max(PageSize, length * 2)));
        if (fd < 0) {
            budget.acquire(size - length);
            ram.resize(static_cast<std::size_t>(size));
        } else {
            U rounded = (size + PageSize - 1) / PageSize * PageSize;
            if (ftruncate(fd, static_cast<off_t>(rounded))) io_error("ftruncate");
            directory.resize(static_cast<std::size_t>(rounded / PageSize), Missing);
        }
        length = size;
    }
public:
    U reads = 0, writes = 0;
    Bytes(U initial, U max_size, const std::string& path, U cache_mib, Budget& b)
        : maximum(max_size), budget(b) {
        if (!path.empty()) {
            capacity = static_cast<std::size_t>(cache_mib * MiB / PageSize);
            fd = open(path.c_str(), O_CREAT | O_EXCL | O_RDWR, 0600);
            if (fd < 0) throw std::runtime_error("cannot create new disk file: " + path);
            try { pages.reserve(capacity); grow(initial); }
            catch (...) { close(fd); fd = -1; throw; }
        } else grow(initial);
    }
    Bytes(const Bytes&) = delete;
    Bytes& operator=(const Bytes&) = delete;
    ~Bytes() { if (fd >= 0) close(fd); }
    std::uint8_t get(U x) {
        if (x >= length) throw std::runtime_error("unwritten table address");
        return fd < 0 ? ram[static_cast<std::size_t>(x)] : load(x / PageSize, false)[x % PageSize];
    }
    void set(U x, std::uint8_t value) {
        grow(x + 1);
        if (fd < 0) ram[static_cast<std::size_t>(x)] = value;
        else load(x / PageSize, true)[x % PageSize] = value;
    }
    I get32(U x) {
        if (x % 4 || x + 4 > length) throw std::runtime_error("invalid raw table address");
        const auto* data = fd < 0 ? ram.data() + x : load(x / PageSize, false) + x % PageSize;
        return static_cast<I>(data[0]) | (static_cast<I>(data[1]) << 8) |
               (static_cast<I>(data[2]) << 16) | (static_cast<I>(data[3]) << 24);
    }
    void set32(U x, I value) {
        if (x % 4) throw std::runtime_error("unaligned raw table address");
        grow(x + 4);
        auto* data = fd < 0 ? ram.data() + x : load(x / PageSize, true) + x % PageSize;
        for (unsigned i = 0; i < 4; ++i) data[i] = static_cast<std::uint8_t>(value >> (8 * i));
    }
    void flush() {
        if (fd < 0) return;
        for (auto& page : pages) write_page(page.key, page);
        if (fsync(fd)) io_error("fsync working table");
    }
};

// Codes 0..254 encode common 18-bit patterns; 255 is an exact raw escape.
// No assumption on how many distinct patterns occur is needed.
class Membership {
    const Options& options;
    Budget& budget;
    Bytes storage;
    std::unique_ptr<Bytes> raw;
    std::array<std::uint8_t, Prefix> prefix{};
    std::vector<std::uint16_t> inverse = std::vector<std::uint16_t>(1U << Width, 65535);
    std::array<I, 255> codes{};
    U current = Prefix, seen = 0, common = 1, distinct = 1;
    I bits = 0;
    void finish_block() {
        auto& code = inverse[bits];
        if (code == 65535) {
            ++distinct;
            if (common < options.dictionary_limit) {
                code = static_cast<std::uint16_t>(common); codes[common++] = bits;
            } else code = 65534;
        }
        U block = current / Width;
        if (code == 65534) {
            if (!raw) raw = std::make_unique<Bytes>(0, 4 * (options.max_value / Width + 1),
                options.disk.empty() ? "" : options.disk + ".raw", 1, budget);
            raw->set32(block * 4, bits);
            storage.set(block, 255); ++escaped_blocks;
        } else storage.set(block, static_cast<std::uint8_t>(code));
        current += Width; bits = 0;
    }
public:
    U escaped_blocks = 0;
    Membership(const Options& o, Budget& b)
        : options(o), budget(b), storage(std::min(o.max_value, 14 * o.terms) / Width + 1,
          o.max_value / Width + 1, o.disk, o.cache_mib, b) { inverse[0] = 0; }
    I block(U index) {
        U base = index * Width;
        if (base > seen) throw std::runtime_error("membership block beyond known prefix");
        if (base < Prefix) {
            I value = 0;
            for (U i = 0; i < Width; ++i) value |= static_cast<I>(prefix[base + i]) << i;
            return value;
        }
        if (base == current) return bits;
        auto code = storage.get(index);
        if (code != 255) return codes[code];
        if (!raw) throw std::runtime_error("missing raw escape table");
        I value = raw->get32(index * 4);
        if (value > Mask) throw std::runtime_error("invalid raw escape pattern");
        return value;
    }
    bool get(U x) {
        if (x > seen) throw std::runtime_error("membership query beyond known prefix");
        if (x < Prefix) return prefix[x] != 0;
        if (x >= current) return ((bits >> (x - current)) & 1U) != 0;
        auto code = storage.get(x / Width);
        I value = code == 255 ? raw->get32((x / Width) * 4) : codes[code];
        return ((value >> (x % Width)) & 1U) != 0;
    }
    void put(U x, bool yes) {
        if (x != seen + 1) throw std::runtime_error("nonsequential membership write");
        if (x < Prefix) prefix[x] = static_cast<std::uint8_t>(yes);
        else {
            if (x == current + Width) finish_block();
            if (yes) bits |= 1U << (x - current);
        }
        seen = x;
    }
    void flush() { if (raw) raw->flush(); storage.flush(); }
    U patterns() const { return distinct; }
    U reads() const { return storage.reads + (raw ? raw->reads : 0); }
    U writes() const { return storage.writes + (raw ? raw->writes : 0); }
};

// Flat treap for insertion; linked sorted indices for cache-friendly searches.
// The retained prefix is complete below cutoff, including empirical outliers.
// When storage is pruned, a possibly incomplete search uses the exact fallback.
class Anchors {
    struct Node { U value; I key, left, right, parent, next, priority; };
    std::vector<Node> nodes{Node{}};
    I root = 0, head = 0, free_head = 0;
    U active = 0, limit, p, threshold;
    static bool less(I key, U value, const Node& node) {
        return key < node.key || (key == node.key && value < node.value);
    }
    static I priority(U x) {
        x += 0x9e3779b97f4a7c15ULL;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
        return static_cast<I>(x ^ (x >> 31));
    }
    void rotate_up(I child) {
        I parent = nodes[child].parent, grand = nodes[parent].parent;
        bool is_left = nodes[parent].left == child;
        I middle = is_left ? nodes[child].right : nodes[child].left;
        if (is_left) { nodes[parent].left = middle; nodes[child].right = parent; }
        else { nodes[parent].right = middle; nodes[child].left = parent; }
        if (middle) nodes[middle].parent = parent;
        nodes[parent].parent = child; nodes[child].parent = grand;
        if (!grand) root = child;
        else if (nodes[grand].left == parent) nodes[grand].left = child;
        else nodes[grand].right = child;
    }
    void trim(U key) {
        cutoff = std::min(cutoff, key);
        I cur = root, first = 0, predecessor = 0;
        while (cur) {
            if (nodes[cur].key >= cutoff) { first = cur; cur = nodes[cur].left; }
            else { predecessor = cur; cur = nodes[cur].right; }
        }
        if (predecessor) nodes[predecessor].next = 0; else head = 0;
        cur = root;
        I parent = 0;
        while (cur) {
            if (nodes[cur].key >= cutoff) {
                I child = nodes[cur].left;
                if (!parent) root = child; else nodes[parent].right = child;
                if (child) nodes[child].parent = parent;
                cur = child;
            } else { parent = cur; cur = nodes[cur].right; }
        }
        while (first) {
            I next = nodes[first].next;
            nodes[first].left = free_head; free_head = first;
            --active; first = next;
        }
    }
public:
    U cutoff, examined = 0, peak = 0, omitted = 0;
    Anchors(U modulus, U t, U cap) : limit(cap), p(modulus), threshold(t), cutoff(p / 2 + 1) {
        nodes.reserve(static_cast<std::size_t>(cap + 1));
    }
    void add(U key, U value) {
        if (key >= cutoff) { ++omitted; return; }
        U delta = 3 * key > p ? 3 * key - p : 0;
        W right = static_cast<W>(3 * p) * threshold;
        if ((delta && static_cast<W>(delta) * delta * value >= right * right) || active == limit) {
            trim(key); ++omitted; return;
        }
        I cur = root, parent = 0, predecessor = 0, successor = 0;
        I k = static_cast<I>(key);
        while (cur) {
            parent = cur;
            if (less(k, value, nodes[cur])) { successor = cur; cur = nodes[cur].left; }
            else { predecessor = cur; cur = nodes[cur].right; }
        }
        I index;
        if (free_head) { index = free_head; free_head = nodes[index].left; }
        else { index = static_cast<I>(nodes.size()); nodes.push_back(Node{}); }
        nodes[index] = Node{value, k, 0, 0, parent, successor, priority(value)};
        if (predecessor) nodes[predecessor].next = index; else head = index;
        if (!parent) root = index;
        else if (less(k, value, nodes[parent])) nodes[parent].left = index;
        else nodes[parent].right = index;
        while (nodes[index].parent && nodes[index].priority < nodes[nodes[index].parent].priority)
            rotate_up(index);
        ++active; peak = std::max(peak, active);
    }
    int search(U target_key, U u, Membership& members, int hits) {
        U bound = target_key / 2;
        for (I index = head; index && nodes[index].key <= bound; index = nodes[index].next) {
            const auto& node = nodes[index];
            if (2ULL * node.key == target_key && node.value >= u - node.value) continue;
            ++examined;
            if (members.get(u - node.value) && ++hits == 2) return 2;
        }
        return hits;
    }
    U size() const { return active; }
    void audit() const {
        std::vector<I> stack;
        I cur = root, linked = head, previous = 0;
        U count = 0;
        if (root && nodes[root].parent) throw std::runtime_error("treap root parent");
        while (cur || !stack.empty()) {
            while (cur) {
                for (I child : {nodes[cur].left, nodes[cur].right})
                    if (child && (nodes[child].parent != cur || nodes[child].priority < nodes[cur].priority))
                        throw std::runtime_error("treap edge invariant");
                stack.push_back(cur); cur = nodes[cur].left;
            }
            cur = stack.back(); stack.pop_back();
            if (cur != linked || nodes[cur].key >= cutoff ||
                (previous && !less(nodes[previous].key, nodes[previous].value, nodes[cur])))
                throw std::runtime_error("treap sorted prefix invariant");
            previous = cur; linked = nodes[cur].next; ++count; cur = nodes[cur].right;
        }
        if (linked || count != active) throw std::runtime_error("treap size invariant");
    }
};

// Independent, unpruned pair-sum oracle; saturates only at two representations.
class Oracle {
    U limit;
    std::vector<std::uint8_t> sums;
    std::vector<U> terms;
public:
    explicit Oracle(U n) : limit(n), sums(n ? static_cast<std::size_t>(n + 1) : 0, 0) {}
    void add(U u) {
        if (!limit || u > limit) return;
        for (U a : terms) {
            if (a > limit - u) break;
            auto& count = sums[a + u];
            if (count < 2) ++count;
        }
        terms.push_back(u);
    }
    void check(U u, int hits) const {
        if (limit && u <= limit && sums[u] != hits)
            throw std::runtime_error("independent oracle disagreement at candidate " + std::to_string(u));
    }
};

// Existing milestones are recomputed and compared, never used for pruning.
// flock rejects a second writer. Each new complete line is written and fsynced.
class BFile {
    int fd = -1;
    std::vector<std::pair<U, U>> existing;
    std::size_t checked = 0;
public:
    explicit BFile(const std::string& path, U target) {
        if (path.empty()) return;
        struct stat existing_status{};
        if (!stat(path.c_str(), &existing_status)) separate_streams(existing_status, "b-file");
        fd = open(path.c_str(), O_CREAT | O_RDWR | O_NOFOLLOW, 0644);
        if (fd < 0) io_error("open b-file");
        try {
            if (flock(fd, LOCK_EX | LOCK_NB)) throw std::runtime_error("b-file locked by another process");
            struct stat st{};
            if (fstat(fd, &st)) io_error("fstat b-file");
            if (!S_ISREG(st.st_mode) || st.st_size < 0 || st.st_size > 65536)
                throw std::runtime_error("invalid b-file type or size");
            separate_streams(st, "b-file");
            std::string text(static_cast<std::size_t>(st.st_size), '\0');
            std::size_t done = 0;
            while (done < text.size()) {
                ssize_t n = pread(fd, &text[done], text.size() - done, static_cast<off_t>(done));
                if (n < 0 && errno == EINTR) continue;
                if (n <= 0) io_error("read b-file");
                done += static_cast<std::size_t>(n);
            }
            if (!text.empty() && text.back() != '\n')
                throw std::runtime_error("b-file has an incomplete final line; inspect it before retrying");
            std::istringstream lines(text);
            std::string line;
            U power = 1, previous = 0;
            while (std::getline(lines, line)) {
                auto comment = line.find('#');
                if (comment != std::string::npos) line.resize(comment);
                std::istringstream fields(line);
                std::string a, b, extra;
                if (!(fields >> a)) continue;
                if (!(fields >> b) || (fields >> extra)) throw std::runtime_error("malformed b-file row");
                U index = number(a), value = number(b);
                if (index != existing.size() || value <= previous || value > MaxValue || power > target)
                    throw std::runtime_error("b-file indices/values/target are inconsistent");
                existing.emplace_back(index, value); previous = value;
                if (power <= 100000000000ULL) power *= 10;
            }
            if (lseek(fd, 0, SEEK_END) < 0) io_error("seek b-file");
            auto slash = path.find_last_of('/');
            std::string parent = slash == std::string::npos ? "." : (slash ? path.substr(0, slash) : "/");
            int directory = open(parent.c_str(), O_RDONLY | O_DIRECTORY);
            if (directory < 0) io_error("open b-file directory");
            int result = fsync(directory), saved_errno = errno;
            close(directory);
            if (result) { errno = saved_errno; io_error("fsync b-file directory"); }
        } catch (...) { close(fd); fd = -1; throw; }
    }
    BFile(const BFile&) = delete;
    BFile& operator=(const BFile&) = delete;
    ~BFile() { if (fd >= 0) close(fd); }
    void record(U index, U value) {
        std::string line = std::to_string(index) + " " + std::to_string(value) + "\n";
        if (checked < existing.size()) {
            if (existing[checked] != std::make_pair(index, value))
                throw std::runtime_error("recomputed result disagrees with b-file at index " + std::to_string(index));
        } else if (fd >= 0) {
            write_all(fd, line.data(), line.size());
            if (fsync(fd)) io_error("fsync b-file");
        }
        ++checked;
        std::cout << line << std::flush;
        if (!std::cout) throw std::runtime_error("stdout write failed");
    }
};

class Control {
    const Options& options;
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
public:
    explicit Control(const Options& o) : options(o) {}
    double seconds() const { return std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count(); }
    void check() const {
        if (interrupted) throw std::runtime_error("interrupted by signal " + std::to_string(interrupted));
        if (options.seconds && seconds() >= static_cast<double>(options.seconds))
            throw std::runtime_error("time cap reached; partial computation only");
    }
};

// Exhaustive fallback: all earlier larger endpoints a with u/2 < a < u.
// No residue filter, density assumption, or Gibbs rule is used here.
static int exhaustive(U u, U largest, int hits, Membership& members, const Control& control, U& tests) {
    U lower = u / 2 + 1;
    if (largest < lower) return hits;
    U first = lower / Width, index = largest / Width;
    for (;;) {
        if ((index & 4095U) == 0) control.check();
        U base = index * Width;
        I bits = members.block(index);
        if (base < lower) bits &= Mask << (lower - base);
        if (base + Width - 1 > largest) bits &= (1U << (largest - base + 1)) - 1;
        while (bits) {
            unsigned bit = 31U - static_cast<unsigned>(__builtin_clz(bits));
            bits &= ~(1U << bit); ++tests;
            if (members.get(u - (base + bit)) && ++hits == 2) return 2;
        }
        if (index == first) break;
        --index;
    }
    return hits;
}

static void self_test(const Options& options) {
    for (U p : {U{22}, U{856371966}, U{120500181}}) {
        U q = p == 22 ? 9 : (p == 856371966 ? 350477575 : 49315733);
        for (U base : {U{0}, U{4294967290ULL}, U{135180000000ULL}, U{1999999999900ULL}}) {
            U r = static_cast<U>(static_cast<W>(base) * q % p);
            for (U x = base; x < base + 100; ++x) {
                if (r != static_cast<U>(static_cast<W>(x) * q % p)) throw std::runtime_error("residue test failed");
                r += q; if (r >= p) r -= p;
            }
        }
    }
    Options test; test.disk.clear(); test.terms = 20000; test.max_value = 26000; test.dictionary_limit = 2;
    Budget budget(8 * MiB);
    Membership members(test, budget);
    U end = Prefix + 301 * Width;
    auto expected = [](U x) {
        return x >= Prefix && (((((x - Prefix) / Width) * 65537 & Mask) >> ((x - Prefix) % Width)) & 1U);
    };
    for (U x = 1; x <= end; ++x) members.put(x, expected(x));
    for (U x = 0; x <= end; ++x)
        if (members.get(x) != expected(x)) throw std::runtime_error("raw escape round trip failed");
    if (members.patterns() <= 256 || !members.escaped_blocks) throw std::runtime_error("escape test incomplete");
    if (!options.disk.empty()) {
        Budget b(1 * MiB);
        U base = 1ULL << 32;
        Bytes data(0, base + 64 * PageSize, options.disk + ".address-test", 1, b);
        for (U i = 0; i < 32; ++i) data.set32(base + i * PageSize + 124, static_cast<I>(0x12340001ULL + i));
        for (U i = 0; i < 32; ++i)
            if (data.get32(base + i * PageSize + 124) != 0x12340001ULL + i)
                throw std::runtime_error("64-bit address/eviction test failed");
        data.flush();
    }
    std::cerr << "self_test=passed (residues, >256 patterns, raw escapes, optional 64-bit disk addresses)\n";
}

static Options parse(int argc, char** argv) {
    Options o;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--self-test") { o.self_test = true; continue; }
        if (arg == "--no-bfile") { o.bfile.clear(); continue; }
        if (arg == "--no-summary") { o.summary.clear(); continue; }
        if (arg == "--ram") { o.disk.clear(); continue; }
        if (arg == "--help") {
            std::cout << "Usage: ulam02 [--terms N] [--disk NEW_FILE | --ram] [--bfile FILE | --no-bfile]\n"
                         "  --summary 346216_02_summary.json | --no-summary\n"
                         "  --cache-mib 8 --memory-mib 256 --report 10000000 --seconds 0\n"
                         "  --seconds 0: no time limit. Existing b-file rows are recomputed and checked.\n"
                         "  --max-value 2000000000000 --window 100000 --threshold 100\n"
                         "  --verify-until 0 --anchor-limit 500000 --dictionary-limit 255\n"
                         "  --p 856371966 --q 350477575 --self-test\n";
            std::exit(0);
        }
        if (i + 1 == argc) throw std::runtime_error("missing option value: " + arg);
        std::string s = argv[++i];
        if (arg == "--disk") { if (s.empty()) throw std::runtime_error("empty disk filename"); o.disk = s; continue; }
        if (arg == "--bfile") { if (s.empty()) throw std::runtime_error("empty b-file filename"); o.bfile = s; continue; }
        if (arg == "--summary") { if (s.empty()) throw std::runtime_error("empty summary filename"); o.summary = s; continue; }
        U value = number(s);
        if (arg == "--terms") o.terms = value;
        else if (arg == "--max-value") o.max_value = value;
        else if (arg == "--memory-mib") o.memory_mib = value;
        else if (arg == "--cache-mib") o.cache_mib = value;
        else if (arg == "--seconds") o.seconds = value;
        else if (arg == "--window") o.window = value;
        else if (arg == "--threshold") o.threshold = value;
        else if (arg == "--report") o.report = value;
        else if (arg == "--verify-until") o.verify_until = value;
        else if (arg == "--anchor-limit") o.anchor_limit = value;
        else if (arg == "--dictionary-limit") o.dictionary_limit = value;
        else if (arg == "--p") o.p = value;
        else if (arg == "--q") o.q = value;
        else throw std::runtime_error("unknown option: " + arg);
    }
    if (!o.terms || o.terms > 100000000000ULL || o.max_value < 2 || o.max_value > MaxValue ||
        !o.memory_mib || o.memory_mib > 1024 || !o.cache_mib || o.cache_mib > 256 ||
        o.verify_until > 200000 || o.seconds > 315360000 || !o.window || o.window > 1000000 ||
        o.threshold > 1000000 || o.anchor_limit > 1000000 || !o.dictionary_limit || o.dictionary_limit > 255 ||
        o.p > 1000000000 || !o.q || o.q > o.p / 2 || o.p > 3 * o.q)
        throw std::runtime_error("option outside supported limits");
    if (sizeof(off_t) < 8 || sizeof(std::size_t) < 8) throw std::runtime_error("64-bit platform required");
    return o;
}

int main(int argc, char** argv) {
    U n = 0, last = 0, completed = 0;
    Options o;
    std::unique_ptr<Summary> summary;
    try {
        o = parse(argc, argv);
        struct sigaction action{};
        action.sa_handler = on_signal; sigemptyset(&action.sa_mask);
        if (sigaction(SIGINT, &action, nullptr) || sigaction(SIGTERM, &action, nullptr)) io_error("sigaction");
        if (o.self_test) { self_test(o); return 0; }
        // Do not let an error diagnostic itself overwrite an aliased data file.
        for (const auto& path : {o.bfile, o.disk, o.disk.empty() ? "" : o.disk + ".raw"}) {
            struct stat data{}, stream{};
            if (!path.empty() && !stat(path.c_str(), &data) && !fstat(STDERR_FILENO, &stream) &&
                data.st_dev == stream.st_dev && data.st_ino == stream.st_ino) stderr_safe = false;
        }
        summary = std::make_unique<Summary>(o);
        BFile bfile(o.bfile, o.terms);
        Budget budget(o.memory_mib * MiB);
        Membership members(o, budget);
        Oracle oracle(o.verify_until);
        Anchors lo(o.p, o.threshold, o.anchor_limit), hi(o.p, o.threshold, o.anchor_limit);
        std::vector<U> window(static_cast<std::size_t>(o.window), 0);
        Control control(o);
        U cursor = 0, r = 0, hash = 14695981039346656037ULL, next_power = 1, index = 0;
        U brute_tests = 0, brute_max = 0, fallback_calls = 0, fallback_tests = 0;
        U outliers_low = 0, outliers_high = 0;
        auto remember = [&](U x) {
            ++n; last = x;
            if (3 * r < o.p) ++outliers_low;
            if (3 * r > 2 * o.p) ++outliers_high;
            window[cursor] = x; cursor = (cursor + 1) % o.window;
            if (r <= o.p / 2) lo.add(r, x); else hi.add(o.p - r, x);
            oracle.add(x); hash = (hash ^ x) * 1099511628211ULL;
            if (n == next_power) { bfile.record(index++, x); next_power *= 10; }
        };
        bool complete = false;
        std::string reason;
        try {
            for (U x = 1; x <= std::min(U{2}, o.terms); ++x) {
                r += o.q; if (r >= o.p) r -= o.p;
                members.put(x, true); completed = x; remember(x);
            }
            for (U u = 3; n < o.terms; ++u) {
                if (interrupted || (u & 65535U) == 0) control.check();
                if (u > o.max_value) throw std::runtime_error("max-value reached before requested term");
                r += o.q; if (r >= o.p) r -= o.p;
                int hits = 0;
                if (r <= o.p / 4 || o.p - r <= o.p / 4) {
                    U count = 0, available = std::min(n, o.window), pos = cursor, oldest = u;
                    bool covered = false;
                    while (count < available) {
                        pos = pos ? pos - 1 : o.window - 1;
                        U a = window[pos];
                        if (a <= u / 2) { covered = true; break; }
                        oldest = a; ++count; ++brute_tests;
                        if (members.get(u - a) && ++hits == 2) break;
                    }
                    brute_max = std::max(brute_max, count);
                    if (hits < 2 && !covered && n > available) {
                        ++fallback_calls;
                        hits = exhaustive(u, oldest - 1, hits, members, control, fallback_tests);
                    }
                } else {
                    hits = lo.search(r, u, members, hits);
                    if (hits < 2) hits = hi.search(o.p - r, u, members, hits);
                    if (hits < 2 && (r / 2 >= lo.cutoff || (o.p - r) / 2 >= hi.cutoff)) {
                        ++fallback_calls;
                        hits = exhaustive(u, u - 1, 0, members, control, fallback_tests);
                    }
                }
                oracle.check(u, hits);
                members.put(u, hits == 1); completed = u;
                if (hits == 1) {
                    remember(u);
                    if (o.report && n % o.report == 0) {
                        double elapsed = control.seconds();
                        U percent = static_cast<U>(static_cast<W>(n) * 10000 / o.terms);
                        double eta = elapsed * static_cast<double>(o.terms - n) / static_cast<double>(n - 2);
                        std::cerr << "progress terms=" << n << " value=" << u << " seconds=" << elapsed
                                  << " target=" << o.terms << " percent=" << percent / 100 << '.'
                                  << (percent % 100 < 10 ? "0" : "") << percent % 100 << " eta_seconds=" << eta
                                  << " fallback_calls=" << fallback_calls << " escaped_blocks=" << members.escaped_blocks << '\n';
                    }
                }
            }
            members.flush();
            if (o.verify_until) { lo.audit(); hi.audit(); }
            complete = true;
        } catch (const std::exception& e) { reason = e.what(); }
        struct rusage usage{};
        bool have_rss = !getrusage(RUSAGE_SELF, &usage);
        U rss = static_cast<U>(usage.ru_maxrss);
#ifndef __APPLE__
        rss *= 1024;
#endif
        auto report = [&]() {
            std::ostringstream text;
            text << "{\"status\":" << json_string(complete ? "complete" : "incomplete")
                  << ",\"target_terms\":" << o.terms << ",\"p\":" << o.p << ",\"q\":" << o.q
                  << ",\"terms\":" << n << ",\"value\":" << last
                  << ",\"completed_candidate\":" << completed << ",\"seconds\":" << control.seconds()
                  << ",\"rss_bytes\":" << (have_rss ? std::to_string(rss) : "null")
                  << ",\"patterns\":" << members.patterns()
                  << ",\"escaped_blocks\":" << members.escaped_blocks << ",\"digest64\":" << hash
                  << ",\"residue_outliers\":" << outliers_low + outliers_high
                  << ",\"residue_outliers_low\":" << outliers_low << ",\"residue_outliers_high\":" << outliers_high
                  << ",\"anchor_tests\":" << lo.examined + hi.examined << ",\"brute_tests\":" << brute_tests
                  << ",\"brute_max\":" << brute_max << ",\"fallback_calls\":" << fallback_calls
                  << ",\"fallback_tests\":" << fallback_tests << ",\"lo_size\":" << lo.size()
                  << ",\"hi_size\":" << hi.size() << ",\"lo_peak\":" << lo.peak << ",\"hi_peak\":" << hi.peak
                  << ",\"lo_cutoff\":" << lo.cutoff << ",\"hi_cutoff\":" << hi.cutoff
                  << ",\"disk_reads\":" << members.reads() << ",\"disk_writes\":" << members.writes()
                  << ",\"verified_candidates_through\":" << std::min(o.verify_until, completed);
            if (!complete) text << ",\"reason\":" << json_string(reason);
            text << "}\n";
            return text.str();
        };
        std::string final = report();
        try { summary->save(final); }
        catch (const std::exception& e) {
            complete = false; reason = std::string("summary save failed: ") + e.what(); final = report();
            // If publication succeeded but the directory sync failed, try once
            // to replace that report with the same incomplete JSON we display.
            try { summary->save(final); } catch (const std::exception&) {}
        }
        if (!complete && stderr_safe)
            std::cerr << "status=incomplete terms=" << n << " last=" << last << " completed_candidate=" << completed
                      << " reason=" << reason << '\n';
        if (stderr_safe) std::cerr << final;
        return complete ? 0 : 1;
    } catch (const std::exception& e) {
        if (stderr_safe)
            std::cerr << "status=incomplete terms=" << n << " last=" << last << " completed_candidate=" << completed
                      << " reason=" << e.what() << '\n';
        if (summary) {
            std::ostringstream text;
            text << "{\"status\":\"incomplete\",\"setup_complete\":false,\"target_terms\":" << o.terms
                 << ",\"p\":" << o.p << ",\"q\":" << o.q << ",\"terms\":" << n << ",\"value\":" << last
                 << ",\"completed_candidate\":" << completed << ",\"reason\":" << json_string(e.what()) << "}\n";
            try { summary->save(text.str()); }
            catch (const std::exception& error) {
                if (stderr_safe) std::cerr << "summary save failed: " << error.what() << '\n';
            }
            if (stderr_safe) std::cerr << text.str();
        }
        return 1;
    }
}
