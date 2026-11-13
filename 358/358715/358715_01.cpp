/*
 * A358715: tile an equilateral triangle of side n with integer-sided
 * equilateral triangles. Count geometric placements with the outer triangle
 * fixed. See https://oeis.org/A358715 for the definition.
 *
 * Build (C++17 with Clang or GCC, GMP, POSIX file APIs):
 *   c++ -O3 -std=c++17 -Wall -Wextra -Wpedantic 358715_01.cpp \
 *     $(pkg-config --cflags --libs gmpxx) -o 358715_01
 *   ./358715_01 20              # extend ./b358715.txt through n=20
 *   ./358715_01 --no-bfile 20   # compute only a(20)
 *   ./358715_01 --crt --no-bfile 21
 *
 * Tiles touching along a positive-length edge have the same three edge
 * directions. Such adjacencies connect the tiling (take a path avoiding
 * the vertices), so the directions agree with the board. On each maximal
 * straight segment of the edges, each incident side partitions that
 * segment into full tile edges. Its endpoint and all tile corners on it
 * are therefore separated by integer lengths. The edge graph is connected
 * to the board corners, so all tile corners are on the unit lattice.
 *
 * At a vertex, every tile meeting it contributes either a 60-degree
 * corner or a 180-degree straight side. The only allowed ray sets are
 * no rays, two opposite rays, four consecutive rays, or all six rays.
 * Force the board edges, and treat all outside rays as present, splitting
 * the exterior into 60-degree sectors. Conversely these rules give faces
 * whose boundary walks have only 0 and 120-degree left turns. To see
 * this even at a repeated vertex, take the boundary slightly inside the
 * face, passing through each incident sector separately. The resulting
 * simple curves have the same total turning as the walks. An inner
 * boundary would have negative turning. Each outer boundary has three
 * 120-degree turns and hence is an equilateral lattice triangle.
 *
 * Visit the vertices in rows, left to right. A frontier key keeps the
 * horizontal bit and two bits per column for edges between consecutive
 * rows. The three old bits determine which local patterns are possible.
 * The replacement stores three new bits, and equal keys can be summed
 * since all remaining constraints depend only on their frontier.
 *
 * Use checked 192-bit counters for n <= 20 by default. With --crt, or
 * for n = 21, store a single 64-bit residue per state and run separately
 * for each modulus. The moduli are powers of distinct small primes.
 * Reconstruct only after their product exceeds a proved upper bound and
 * check the result with one further modulus. If a state or counter
 * limit is reached, the current term is abandoned.
 * The b-file is the saved prefix, with flock, a synced temporary file
 * and atomic replacement after each completed term.
 */
#ifndef _DARWIN_C_SOURCE
#define _DARWIN_C_SOURCE
#endif

#include <algorithm>
#include <array>
#include <cerrno>
#include <charconv>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include <fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>
#include <gmpxx.h>

namespace {

constexpr int max_n = 21;
constexpr std::size_t max_states = std::size_t{1} << 26;
constexpr unsigned limb_count = 3;
constexpr std::size_t max_file_bytes = 65536;
constexpr char bfile_path[] = "b358715.txt";
constexpr char lock_path[] = "b358715.txt.lock";
static_assert(2 * (max_n + 1) + 1 < 64);
static_assert(max_states < std::numeric_limits<uint32_t>::max());
static_assert(sizeof(std::size_t) >= sizeof(uint64_t));

[[noreturn]] void fail(const char* message)
{
    throw std::runtime_error(message);
}

[[noreturn]] void fail_io(const char* operation)
{
    throw std::system_error(errno, std::generic_category(), operation);
}

struct Count {
    std::array<uint64_t, limb_count> limbs{};

    void add(const Count& other)
    {
        bool carry = false;
        for (unsigned i = 0; i < limb_count; ++i) {
            uint64_t sum;
            const bool first = __builtin_add_overflow(limbs[i], other.limbs[i], &sum);
            const bool second = __builtin_add_overflow(sum, uint64_t{carry}, &limbs[i]);
            carry = first || second;
        }
        if (carry) fail("192-bit count capacity exceeded");
    }

    std::string decimal() const
    {
        mpz_class value;
        mpz_import(value.get_mpz_t(), limbs.size(), -1, sizeof(limbs[0]), 0, 0,
                   limbs.data());
        return value.get_str();
    }
};

/* Contiguous entries keep scans sequential. Buckets contain entry indices
 * plus one. Hash collisions always compare the complete frontier mask. */
template<class Value>
class Frontier {
public:
    struct Entry {
        uint64_t key;
        Value count;
    };

    explicit Frontier(uint64_t modulus) : modulus_(modulus) {}
    void set_modulus(uint64_t modulus) { modulus_ = modulus; }

    void clear(std::size_t source_size = 0)
    {
        entries_.clear();
        // Reuse the allocation, but avoid clearing a peak-sized table
        // on small layers of the next pass or near the tip.
        while (buckets_.size() > 16 && buckets_.size() / 4 > source_size)
            buckets_.resize(buckets_.size() / 2);
        std::fill(buckets_.begin(), buckets_.end(), 0);
    }

    void add(uint64_t key, const Value& count)
    {
        std::size_t bucket = locate(key);
        if (buckets_[bucket] != 0) {
            auto& value = entries_[buckets_[bucket] - 1].count;
            if constexpr (std::is_same_v<Value, uint64_t>) {
                // modulus <= 2^63, so the sum of two residues fits.
                const uint64_t sum = value + count;
                value = sum >= modulus_ ? sum - modulus_ : sum;
            } else {
                value.add(count);
            }
            return;
        }

        if (entries_.size() == max_states)
            fail("frontier state limit exceeded");
        if (entries_.size() >= buckets_.size() - buckets_.size() / 4) {
            buckets_.assign(buckets_.size() * 2, 0);
            for (std::size_t i = 0; i < entries_.size(); ++i)
                buckets_[locate(entries_[i].key)] = static_cast<uint32_t>(i + 1);
            bucket = locate(key);
        }
        if (entries_.size() == entries_.capacity()) {
            const std::size_t capacity = entries_.capacity();
            entries_.reserve(capacity == 0 ? 16 : std::min(max_states, 2 * capacity));
        }
        entries_.push_back({key, count});
        buckets_[bucket] = static_cast<uint32_t>(entries_.size());
    }

    const std::vector<Entry>& entries() const { return entries_; }

private:
    std::size_t locate(uint64_t key) const
    {
        // Multiplication here is deliberately modulo 2^64.
        uint64_t hash;
        (void)__builtin_mul_overflow(key ^ (key >> 23),
                                     UINT64_C(0x9e3779b97f4a7c15), &hash);
        hash ^= hash >> 29;
        const std::size_t mask = buckets_.size() - 1;
        std::size_t bucket = hash & mask;
        while (buckets_[bucket] != 0 && entries_[buckets_[bucket] - 1].key != key)
            bucket = (bucket + 1) & mask;
        return bucket;
    }

    std::vector<Entry> entries_;
    std::vector<uint32_t> buckets_ = std::vector<uint32_t>(16);
    uint64_t modulus_;
};

struct Choices {
    std::array<unsigned, 3> outgoing{};
    unsigned size = 0;
};

struct Transfer {
    std::array<Choices, 8> choices{};
    unsigned forced_incoming;
};

Transfer make_transfer(int n, int x, int y)
{
    constexpr int dx[] = {1, 0, -1, -1, 0, 1};
    constexpr int dy[] = {0, 1, 1, 0, -1, -1};
    constexpr unsigned patterns[] = {0, 63, 9, 18, 36, 15, 30, 60, 57, 51, 39};
    unsigned exterior = 0;
    unsigned required = 0;
    for (unsigned d = 0; d < 6; ++d) {
        const int u = x + dx[d], v = y + dy[d];
        if (u < 0 || v < 0 || u + v > n)
            exterior |= 1u << d;
        else if ((x == 0 && u == 0) || (y == 0 && v == 0)
                 || (x + y == n && u + v == n))
            required |= 1u << d;
    }
    required |= exterior;
    Transfer transfer{};
    transfer.forced_incoming = exterior >> 3;
    for (unsigned pattern : patterns) {
        if ((pattern & required) != required) continue;
        auto& choices = transfer.choices[pattern >> 3];
        // Remove the virtual rays before putting actual edges in the key.
        const unsigned actual = pattern & ~exterior;
        // Bit order pairs (west, east), (south, northwest),
        // and (southeast, north). The end-of-row shift maps northwest
        // to the previous column and north to the same column.
        const unsigned outgoing = (actual & 1u) | ((actual >> 1) & 2u)
                                  | ((actual << 1) & 4u);
        choices.outgoing[choices.size++] = outgoing;
    }
    return transfer;
}

template<class Value>
Value compute(int n, Value one, Frontier<Value>& first, Frontier<Value>& second,
              std::size_t& peak)
{
    auto* current = &first;
    auto* next = &second;
    current->clear();
    current->add(0, one);
    peak = std::max<std::size_t>(peak, 1);
    for (int y = 0; y <= n; ++y) {
        const int width = n - y + 1;
        for (int x = 0; x < width; ++x) {
            const auto transfer = make_transfer(n, x, y);
            const unsigned shift = static_cast<unsigned>(1 + 2 * x);
            const uint64_t fields = (uint64_t{3} << shift) | 1;
            const bool end_of_row = x + 1 == width;
            const uint64_t row_mask = (uint64_t{1} << (2 * (width - 1))) - 1;
            next->clear(current->entries().size());
            for (const auto& entry : current->entries()) {
                const uint64_t key = entry.key;
                const auto incoming = static_cast<unsigned>((key & 1) | (((key >> shift) & 3) << 1));
                const auto& choices = transfer.choices[incoming | transfer.forced_incoming];
                const uint64_t other = key & ~fields;
                for (unsigned i = 0; i < choices.size; ++i) {
                    const unsigned outgoing = choices.outgoing[i];
                    uint64_t new_key = other | (outgoing & 1u) | (uint64_t{outgoing >> 1} << shift);
                    if (end_of_row) new_key = ((new_key >> 2) & row_mask) << 1;
                    next->add(new_key, entry.count);
                }
            }
            std::swap(current, next);
            peak = std::max(peak, current->entries().size());
        }
    }
    if (current->entries().size() != 1 || current->entries()[0].key != 0)
        fail("the final frontier is inconsistent");
    return current->entries()[0].count;
}

/* Keep the constraints within each block, but choose the boundary
 * rays on its side independently of the neighboring block. Restricting
 * a global solution to the blocks is injective, so their product bounds
 * the global count. Use GMP because the unrestricted rays at the
 * artificial boundary can increase intermediate counts.
 */
mpz_class block_count(int n, int x0, int y0)
{
    constexpr int side = 11;
    const auto inside = [=](int x, int y) {
        return x >= x0 && x < x0 + side && y >= y0 && y < y0 + side && x + y <= n;
    };
    std::unordered_map<uint32_t, mpz_class> current{{0, 1}}, next;
    for (int y = y0; y < y0 + side && x0 + y <= n; ++y) {
        const int width = std::min(side, n - y - x0 + 1);
        for (int column = 0; column < width; ++column) {
            const int x = x0 + column;
            const auto transfer = make_transfer(n, x, y);
            const unsigned check = unsigned(inside(x - 1, y))
                | (unsigned(inside(x, y - 1)) << 1) | (unsigned(inside(x + 1, y - 1)) << 2);
            const unsigned keep = unsigned(inside(x + 1, y))
                | (unsigned(inside(x - 1, y + 1)) << 1) | (unsigned(inside(x, y + 1)) << 2);
            const unsigned shift = static_cast<unsigned>(1 + 2 * column);
            const uint32_t fields = (3u << shift) | 1u;
            next.clear();
            for (const auto& [key, value] : current) {
                const unsigned incoming = (key & 1u) | (((key >> shift) & 3u) << 1);
                for (unsigned p = 0; p < 8; ++p) {
                    if (((p ^ incoming) & check) != 0) continue;
                    const auto& choices = transfer.choices[p];
                    for (unsigned i = 0; i < choices.size; ++i) {
                        const unsigned bits = choices.outgoing[i] & keep;
                        uint32_t target = (key & ~fields) | (bits & 1u) | ((bits >> 1) << shift);
                        if (column + 1 == width) target = (target >> 2) << 1;
                        next[target] += value;
                    }
                }
            }
            current.swap(next);
        }
    }
    if (current.size() != 1 || current.begin()->first != 0)
        fail("the block frontier is inconsistent");
    return current.begin()->second;
}

mpz_class count_bound(int n)
{
    constexpr int side = 11;
    mpz_class bound = 1;
    for (int y = 0; y <= n; y += side)
        for (int x = 0; x + y <= n; x += side)
            bound *= block_count(n, x, y);
    return bound;
}

mpz_class to_integer(uint64_t value)
{
    mpz_class result;
    mpz_import(result.get_mpz_t(), 1, 1, sizeof(value), 0, 0, &value);
    return result;
}

std::string compute_crt(int n, std::size_t& peak)
{
    // 2^63, 3^39, 5^27, 7^22 and 11^18 are pairwise coprime.
    constexpr uint64_t moduli[] = {
        UINT64_C(9223372036854775808), UINT64_C(4052555153018976267),
        UINT64_C(7450580596923828125), UINT64_C(3909821048582988049),
        UINT64_C(5559917313492231481),
    };
    const mpz_class bound = count_bound(n);
    mpz_class capacity = 1;
    std::size_t passes = 0;
    for (; passes + 1 < std::size(moduli) && capacity <= bound; ++passes)
        capacity *= to_integer(moduli[passes]);
    if (capacity <= bound) fail("insufficient CRT moduli for the proved bound");
    std::fprintf(stderr, "n=%d: %zu CRT passes and one check\n", n, passes);

    Frontier<uint64_t> first(moduli[0]), second(moduli[0]);
    mpz_class value = 0, product = 1, inverse;
    for (std::size_t i = 0; i <= passes; ++i) {
        std::fprintf(stderr, "n=%d: modulus %zu/%zu\n", n, i + 1, passes + 1);
        first.set_modulus(moduli[i]);
        second.set_modulus(moduli[i]);
        const auto residue = to_integer(compute(n, uint64_t{1}, first, second, peak));
        const auto modulus = to_integer(moduli[i]);
        if (i == passes) {
            if (value <= 0 || value > bound || value % modulus != residue)
                fail("CRT result failed the bound or check modulus");
            break;
        }
        if (mpz_invert(inverse.get_mpz_t(), product.get_mpz_t(), modulus.get_mpz_t()) == 0)
            fail("CRT moduli are not coprime");
        mpz_class correction = (residue - value) * inverse;
        mpz_mod(correction.get_mpz_t(), correction.get_mpz_t(), modulus.get_mpz_t());
        value += product * correction;
        product *= modulus;
    }
    return value.get_str();
}

int parse_n(std::string_view text)
{
    int n = 0;
    const auto result = std::from_chars(text.data(), text.data() + text.size(), n);
    if (result.ec != std::errc{} || result.ptr != text.data() + text.size()
        || n < 1 || n > max_n)
        fail("n is outside the supported range or is malformed");
    return n;
}

class Descriptor {
public:
    explicit Descriptor(int fd) : fd_(fd) {}
    ~Descriptor() { if (fd_ >= 0) close(fd_); }
    Descriptor(const Descriptor&) = delete;
    Descriptor& operator=(const Descriptor&) = delete;
    int get() const { return fd_; }
private:
    int fd_;
};

struct Saved {
    std::string bytes;
    struct stat snapshot{};
    int last = 0;
    bool exists = false;
};

void require_regular(const struct stat& info)
{
    if (!S_ISREG(info.st_mode) || info.st_nlink != 1)
        fail("b-file and lock must be regular files with one name");
}

bool same_file(const struct stat& a, const struct stat& b)
{
    return a.st_dev == b.st_dev && a.st_ino == b.st_ino && a.st_size == b.st_size
           && a.st_mtime == b.st_mtime && a.st_ctime == b.st_ctime
#ifdef __APPLE__
           && a.st_mtimespec.tv_nsec == b.st_mtimespec.tv_nsec
           && a.st_ctimespec.tv_nsec == b.st_ctimespec.tv_nsec
#else
           && a.st_mtim.tv_nsec == b.st_mtim.tv_nsec && a.st_ctim.tv_nsec == b.st_ctim.tv_nsec
#endif
           && S_ISREG(b.st_mode) && b.st_nlink == 1;
}

Saved load_saved()
{
    Saved saved;
    const Descriptor input(open(bfile_path, O_RDONLY | O_NOFOLLOW | O_NONBLOCK));
    if (input.get() < 0) {
        if (errno == ENOENT) return saved;
        fail_io("open b-file");
    }
    if (fstat(input.get(), &saved.snapshot) != 0) fail_io("stat b-file");
    require_regular(saved.snapshot);
    saved.exists = true;
    char buffer[4096];
    for (;;) {
        const auto length = read(input.get(), buffer, sizeof(buffer));
        if (length < 0) {
            if (errno == EINTR) continue;
            fail_io("read b-file");
        }
        if (length == 0) break;
        if (static_cast<std::size_t>(length) > max_file_bytes - saved.bytes.size())
            fail("b-file is too large");
        saved.bytes.append(buffer, static_cast<std::size_t>(length));
    }

    std::string_view remaining(saved.bytes);
    constexpr std::string_view whitespace = " \t\r";
    constexpr std::string_view digits = "0123456789";
    while (!remaining.empty()) {
        const auto end = remaining.find('\n');
        if (end == std::string_view::npos || end > 1023)
            fail("b-file has an incomplete or oversized line");
        auto line = remaining.substr(0, end);
        remaining.remove_prefix(end + 1);
        if (line.find('\0') != std::string_view::npos) fail("b-file contains a NUL byte");
        const auto start = line.find_first_not_of(whitespace);
        if (start == std::string_view::npos || line[start] == '#') continue;
        line.remove_prefix(start);
        const auto split = line.find_first_of(whitespace);
        if (split == std::string_view::npos) fail("b-file needs an index and a value");
        const int n = parse_n(line.substr(0, split));
        if (n != saved.last + 1) fail("b-file indices must be contiguous from 1");
        line.remove_prefix(split);
        const auto number_start = line.find_first_not_of(whitespace);
        if (number_start == std::string_view::npos) fail("b-file has a missing value");
        line.remove_prefix(number_start);
        line = line.substr(0, line.find_last_not_of(whitespace) + 1);
        if (line.size() > 78 || line.find_first_not_of(digits) != std::string_view::npos)
            fail("b-file has a malformed value");
        const mpz_class value(std::string(line), 10);
        if (value <= 0 || mpz_sizeinbase(value.get_mpz_t(), 2) > 4 * 64)
            fail("b-file value is outside the count capacity");
        saved.last = n;
    }
    return saved;
}

void require_unchanged(const Saved& saved)
{
    struct stat info{};
    if (lstat(bfile_path, &info) != 0) {
        if (errno == ENOENT && !saved.exists) return;
        if (errno != ENOENT) fail_io("stat output path");
    } else if (saved.exists && same_file(saved.snapshot, info)) {
        return;
    }
    fail("b-file changed during computation");
}

void write_all(int fd, std::string_view bytes)
{
    while (!bytes.empty()) {
        const auto written = write(fd, bytes.data(), bytes.size());
        if (written < 0 && errno == EINTR) continue;
        if (written < 0) fail_io("write b-file");
        if (written == 0) fail("short write to b-file");
        bytes.remove_prefix(static_cast<std::size_t>(written));
    }
}

void save_term(Saved& saved, int directory, const std::string& term)
{
    require_unchanged(saved);
    if (term.size() > max_file_bytes - saved.bytes.size())
        fail("the updated b-file would exceed its size limit");
    std::string updated = saved.bytes + term;
    char temporary[] = "b358715.txt.tmp.XXXXXX";
    const Descriptor output(mkstemp(temporary));
    if (output.get() < 0) fail_io("create temporary b-file");
    try {
        if (saved.exists && fchmod(output.get(), saved.snapshot.st_mode & 0777) != 0)
            fail_io("preserve b-file permissions");
        write_all(output.get(), updated);
        if (fsync(output.get()) != 0) fail_io("sync temporary b-file");
        require_unchanged(saved);
        if (rename(temporary, bfile_path) != 0) fail_io("replace b-file");
        if (fsync(directory) != 0) fail_io("sync output directory");
        if (fstat(output.get(), &saved.snapshot) != 0) fail_io("stat updated b-file");
        saved.bytes.swap(updated);
        saved.exists = true;
        ++saved.last;
    } catch (...) {
        unlink(temporary);
        throw;
    }
}

void run_term(int n, Saved* saved, int directory, bool crt)
{
    std::fprintf(stderr, "n=%d: computing\n", n);
    std::size_t peak = 0;
    std::string value;
    if (crt || n == max_n) {
        value = compute_crt(n, peak);
    } else {
        Frontier<Count> first(0), second(0);
        value = compute(n, Count{{1}}, first, second, peak).decimal();
    }
    const auto term = std::to_string(n) + " " + value + "\n";
    if (saved != nullptr) save_term(*saved, directory, term);
    if (std::fputs(term.c_str(), stdout) == EOF || std::fflush(stdout) != 0)
        fail_io("write result");
    std::fprintf(stderr, "n=%d: peak frontier %zu states\n", n, peak);
}

void usage(FILE* stream, const char* program)
{
    std::fprintf(stream,
                 "Usage: %s [--crt] [--no-bfile] N (1 <= N <= %d)\n"
                 "  N: extend ./b358715.txt through N and print new terms.\n"
                 "  --no-bfile N: compute just a(N).\n"
                 "  --crt: use smaller counts and multiple passes.\n",
                 program, max_n);
}

} // namespace

int main(int argc, char** argv)
{
    try {
        if (argc == 2 && std::string_view(argv[1]) == "--help") {
            usage(stdout, argv[0]);
            return EXIT_SUCCESS;
        }
        int arg = 1;
        bool crt = false;
        bool single = false;
        while (arg < argc - 1) {
            const std::string_view option(argv[arg++]);
            if (option == "--crt" && !crt) crt = true;
            else if (option == "--no-bfile" && !single) single = true;
            else fail("invalid or repeated option");
        }
        if (arg != argc - 1) {
            usage(stderr, argv[0]);
            return EXIT_FAILURE;
        }
        const int n = parse_n(argv[arg]);
        if (single) {
            run_term(n, nullptr, -1, crt);
            return EXIT_SUCCESS;
        }
        const Descriptor lock(open(lock_path, O_RDWR | O_CREAT | O_NOFOLLOW | O_NONBLOCK, 0644));
        if (lock.get() < 0) fail_io("open b-file lock");
        struct stat info{};
        if (fstat(lock.get(), &info) != 0) fail_io("stat b-file lock");
        require_regular(info);
        if (flock(lock.get(), LOCK_EX | LOCK_NB) != 0) {
            if (errno == EWOULDBLOCK) fail("another process holds the b-file lock");
            fail_io("lock b-file");
        }
        Saved saved = load_saved();
        if (saved.last >= n) {
            std::fprintf(stderr, "%s already contains terms through n=%d\n", bfile_path, saved.last);
            return EXIT_SUCCESS;
        }
        const Descriptor directory(open(".", O_RDONLY | O_DIRECTORY));
        if (directory.get() < 0) fail_io("open output directory");
        for (int k = saved.last + 1; k <= n; ++k)
            run_term(k, &saved, directory.get(), crt);
        return EXIT_SUCCESS;
    } catch (const std::exception& error) {
        std::fprintf(stderr, "358715_01: %s\n", error.what());
        return EXIT_FAILURE;
    }
}
