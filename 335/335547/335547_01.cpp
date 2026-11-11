// 335547_01.cpp  --  b-file for OEIS A335547
//
// a(n) = number of ways to tile a size-n staircase polyomino (left-aligned rows
// of lengths 1..n) with staircase polyominoes in the same direction.
//
// Usage:  ./335547_01 N        computes a(1..N), writes "n a(n)" to b335547_01.txt
//         ./335547_01 --help
//
// ---------------------------------------------------------------------------
// Method (row-by-row transfer DP).  Proof sketch:
//
// (1) A size-k staircase tile with apex (top cell) at column c occupies, in its
//     t-th row (t = 0..k-1), exactly the columns [c, c+t].  So the restriction
//     of a tiling to row i is a partition of {1..i} into intervals, each the
//     slice of one tile.  We encode it by the cut bits b_1..b_{i-1}
//     (b_x = 1 iff columns x and x+1 lie in different intervals).
//
// (2) Going from row i to row i+1, each interval [s,e] either ends (its tile
//     ends at row i) or continues as [s,e+1].  A continued interval occupies
//     column e+1, which is the first column of the next interval of row i (or
//     the new column i+1 if e = i).  Hence two adjacent intervals cannot both
//     continue; any other choice is allowed.  Every cell of row i+1 not covered
//     by a continued interval must be the apex of a new tile (a tile's first
//     row has width 1), i.e. a singleton interval.  Conversely every such
//     choice sequence gives a tiling, and the correspondence is bijective.
//
// (3) Consequently the new cut bits c_1..c_i of row i+1 are
//        c_x = 0  <=>  column x (of row i) belongs to a continued interval.
//     (Inside [s,e+1] no cuts; at both ends of it the neighbouring interval is
//     not continued, so its columns are singletons -> cut = 1.)
//
// (4) Sweep x = 1..i keeping one flag d = "the interval containing column x is
//     continued".  At column x we output c_x = !d, then read b_x: if b_x = 1 a
//     new interval starts at x+1 and chooses d' in {0} u ({1} if d = 0).
//     Since b_x and c_x use the same bit position (x-1), the sweep is in place:
//       N[m0][0] = A[m1][1]     N[m0][1] = A[m0][1]
//       N[m1][0] = A[m0][0] + A[m1][0]     N[m1][1] = A[m1][0]
//     where m0/m1 have bit x-1 equal to 0/1.
//
// (5) a(1) = 1 (single state, row 1).  a(i+1) = sum of all states of row i+1
//     (in the last row every tile simply ends).
//
// Memory: 2^(N-1) masks * 2 flags * LIMBS*8 bytes.
// Numbers: fixed width LIMBS*64 bits; any carry out of the top limb aborts.
// Intermediate entries are partial counts of tilings of row i+1, hence
// <= a(i+1), so no entry exceeds the final value.
// ---------------------------------------------------------------------------
//
// Build:  g++ -O2 -std=c++17 -Wall -Wextra -Wpedantic -o 335547_01 335547_01.cpp
//         (options: -DLIMBS=k  number of 64-bit limbs, default 4 -> 256 bits;
//                   -DMAXN=n   largest allowed N, default 30)

#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include <fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef LIMBS
#define LIMBS 4
#endif
#ifndef MAXN
#define MAXN 30
#endif

static_assert(LIMBS >= 1 && LIMBS <= 64, "LIMBS out of range");
static_assert(MAXN >= 1 && MAXN <= 40, "MAXN out of range");

static const char *OUTFILE = "b335547_01.txt";
static const char *LOCKFILE = "b335547_01.txt.lock";
static const size_t MAX_SAVED_BYTES = 16u << 20;   // refuse absurdly large files
static const size_t MAX_DIGITS = 1000;

[[noreturn]] static void die(const char *msg) {
    std::fprintf(stderr, "335547_01: %s\n", msg);
    std::exit(1);
}
[[noreturn]] static void die_errno(const char *msg) {
    std::fprintf(stderr, "335547_01: %s: %s\n", msg, std::strerror(errno));
    std::exit(1);
}

// ---------------- fixed-width unsigned big integer ----------------
__extension__ typedef unsigned __int128 u128;   // GCC/Clang extension

struct Big {
    uint64_t w[LIMBS];
};

static bool g_overflow = false;

static inline void big_add(Big &r, const Big &a, const Big &b) {
    u128 c = 0;
    for (int k = 0; k < LIMBS; k++) {
        c += (u128)a.w[k] + b.w[k];
        r.w[k] = (uint64_t)c;
        c >>= 64;
    }
    if (c) g_overflow = true;
}

static std::string big_to_dec(const Big &x) {
    Big t = x;
    std::vector<uint64_t> chunks;   // base 1e19
    const uint64_t BASE = 10000000000000000000ULL;
    for (;;) {
        bool zero = true;
        for (int k = 0; k < LIMBS; k++)
            if (t.w[k]) { zero = false; break; }
        if (zero) break;
        u128 rem = 0;
        for (int k = LIMBS - 1; k >= 0; k--) {
            u128 cur = (rem << 64) | t.w[k];
            t.w[k] = (uint64_t)(cur / BASE);
            rem = cur % BASE;
        }
        chunks.push_back((uint64_t)rem);
    }
    if (chunks.empty()) return "0";
    std::string s = std::to_string(chunks.back());
    char buf[32];
    for (size_t i = chunks.size() - 1; i-- > 0;) {
        std::snprintf(buf, sizeof buf, "%019llu", (unsigned long long)chunks[i]);
        s += buf;
    }
    return s;
}

// ---------------- argument parsing ----------------
static void usage(FILE *f) {
    std::fprintf(f,
        "usage: 335547_01 N\n"
        "  computes A335547(1..N) and saves \"n a(n)\" lines to %s\n"
        "  1 <= N <= %d (memory: 2^(N-1) * 2 * %d bytes)\n"
        "  existing lines are re-verified; lines beyond N are kept\n",
        OUTFILE, MAXN, LIMBS * 8);
}

static int parse_n(const char *s) {
    if (!*s) die("N must be a positive integer");
    for (const char *p = s; *p; p++)
        if (*p < '0' || *p > '9') die("N must be a positive integer (digits only)");
    errno = 0;
    char *end = nullptr;
    long v = std::strtol(s, &end, 10);
    if (errno || *end || v < 1 || v > MAXN) {
        std::fprintf(stderr, "335547_01: N must satisfy 1 <= N <= %d\n", MAXN);
        std::exit(1);
    }
    return (int)v;
}

// ---------------- file helpers ----------------
static int open_lock() {
    int fd = open(LOCKFILE, O_RDWR | O_CREAT | O_NOFOLLOW | O_CLOEXEC, 0644);
    if (fd < 0) die_errno("cannot open lock file");
    struct stat st;
    if (fstat(fd, &st) != 0) die_errno("fstat lock file");
    if (!S_ISREG(st.st_mode) || st.st_nlink != 1) die("lock file is not a plain regular file");
    if (flock(fd, LOCK_EX | LOCK_NB) != 0) {
        if (errno == EWOULDBLOCK) die("another instance is running (lock held)");
        die_errno("flock");
    }
    return fd;   // kept open until exit
}

// Reads saved values; returns them as decimal strings (index 0 -> n = 1).
static std::vector<std::string> read_saved() {
    std::vector<std::string> vals;
    int fd = open(OUTFILE, O_RDONLY | O_NOFOLLOW | O_CLOEXEC);
    if (fd < 0) {
        if (errno == ENOENT) return vals;
        die_errno("cannot open saved file (symlink?)");
    }
    struct stat st;
    if (fstat(fd, &st) != 0) die_errno("fstat saved file");
    if (!S_ISREG(st.st_mode) || st.st_nlink != 1) die("saved file is not a plain regular file");
    if ((size_t)st.st_size > MAX_SAVED_BYTES) die("saved file too large");
    std::string data;
    char buf[65536];
    for (;;) {
        ssize_t r = read(fd, buf, sizeof buf);
        if (r < 0) { if (errno == EINTR) continue; die_errno("read saved file"); }
        if (r == 0) break;
        data.append(buf, (size_t)r);
        if (data.size() > MAX_SAVED_BYTES) die("saved file too large");
    }
    if (close(fd) != 0) die_errno("close saved file");

    size_t pos = 0;
    long expect = 1;
    while (pos < data.size()) {
        size_t nl = data.find('\n', pos);
        if (nl == std::string::npos) {
            std::fprintf(stderr, "335547_01: note: ignoring incomplete last line of %s\n", OUTFILE);
            break;
        }
        std::string line = data.substr(pos, nl - pos);
        pos = nl + 1;
        size_t sp = line.find(' ');
        if (sp == std::string::npos || sp == 0 || sp + 1 >= line.size())
            die("saved file: malformed line (file left unchanged)");
        std::string ns = line.substr(0, sp), vs = line.substr(sp + 1);
        for (char ch : ns) if (ch < '0' || ch > '9') die("saved file: bad index (file left unchanged)");
        for (char ch : vs) if (ch < '0' || ch > '9') die("saved file: bad value (file left unchanged)");
        if (ns.size() > 6 || ns[0] == '0' || std::strtol(ns.c_str(), nullptr, 10) != expect)
            die("saved file: indices must be 1,2,3,... (file left unchanged)");
        if (vs.size() > MAX_DIGITS || vs[0] == '0')
            die("saved file: value out of range (must be positive, no leading zeros)");
        vals.push_back(vs);
        expect++;
    }
    return vals;
}

static void write_all(const std::vector<std::string> &vals) {
    struct stat st;
    if (lstat(OUTFILE, &st) == 0) {
        if (!S_ISREG(st.st_mode) || st.st_nlink != 1)
            die("refusing to replace result file: not a plain regular file");
    } else if (errno != ENOENT) {
        die_errno("lstat result file");
    }
    std::string tmpl = std::string(OUTFILE) + ".tmpXXXXXX";
    std::vector<char> name(tmpl.begin(), tmpl.end());
    name.push_back('\0');
    int fd = mkstemp(name.data());
    if (fd < 0) die_errno("mkstemp");
    if (fchmod(fd, 0644) != 0) { unlink(name.data()); die_errno("fchmod"); }
    FILE *f = fdopen(fd, "w");
    if (!f) { close(fd); unlink(name.data()); die_errno("fdopen"); }
    for (size_t i = 0; i < vals.size(); i++) {
        if (std::fprintf(f, "%zu %s\n", i + 1, vals[i].c_str()) < 0) {
            std::fclose(f); unlink(name.data()); die("write to temp file failed");
        }
    }
    if (std::fflush(f) != 0) { std::fclose(f); unlink(name.data()); die_errno("fflush temp"); }
    if (fsync(fileno(f)) != 0) { std::fclose(f); unlink(name.data()); die_errno("fsync temp"); }
    if (std::fclose(f) != 0) { unlink(name.data()); die_errno("fclose temp"); }
    if (rename(name.data(), OUTFILE) != 0) { unlink(name.data()); die_errno("rename"); }
}

// Called for every computed term n (1-based).  Verifies against saved values or
// appends and rewrites the file.
static void record(std::vector<std::string> &vals, size_t n, const std::string &v) {
    if (n <= vals.size()) {
        if (vals[n - 1] != v) {
            std::fprintf(stderr, "335547_01: saved a(%zu) = %s but computed %s; file left unchanged\n",
                         n, vals[n - 1].c_str(), v.c_str());
            std::exit(1);
        }
        if (std::printf("%zu %s (verified)\n", n, v.c_str()) < 0) die("printf failed");
    } else {
        // n == vals.size()+1 always, since terms are produced in order
        vals.push_back(v);
        write_all(vals);
        if (std::printf("%zu %s\n", n, v.c_str()) < 0) die("printf failed");
    }
    if (std::fflush(stdout) != 0) die("fflush stdout failed");
}

int main(int argc, char **argv) {
    if (argc == 2 && (!std::strcmp(argv[1], "--help") || !std::strcmp(argv[1], "-h"))) {
        usage(stdout);
        return 0;
    }
    if (argc != 2) { usage(stderr); return 1; }
    const int N = parse_n(argv[1]);

    open_lock();
    std::vector<std::string> vals = read_saved();
    if (vals.size() > (size_t)N)
        std::fprintf(stderr, "335547_01: note: keeping saved lines beyond N=%d (not re-verified)\n", N);

    // a(1) = 1
    record(vals, 1, "1");
    if (N == 1) return 0;

    // A[mask*2 + d], masks of up to N-1 bits
    const size_t nmask = (size_t)1 << (N - 1);
    if (nmask > SIZE_MAX / 2 / sizeof(Big)) die("N too large for size_t");
    const size_t total = nmask * 2;
    Big *A = static_cast<Big *>(std::calloc(total, sizeof(Big)));
    if (!A) die("out of memory (try a smaller N)");

    // row 1: single state (mask 0, no bits), value 1, both choices of d
    A[0].w[0] = 1;
    A[1].w[0] = 1;

    for (int i = 1; i < N; i++) {          // row i (length i) -> row i+1
        const size_t m_i = (size_t)1 << (i - 1);   // number of masks of i-1 bits
        // columns x = 1..i-1: bit position x-1 holds b_x, replaced by c_x
        for (int x = 1; x <= i - 1; x++) {
            const size_t bit = (size_t)1 << (x - 1);
            for (size_t m0 = 0; m0 < m_i; m0++) {
                if (m0 & bit) continue;
                const size_t m1 = m0 | bit;
                Big a00 = A[m0 * 2], a01 = A[m0 * 2 + 1];
                Big a10 = A[m1 * 2], a11 = A[m1 * 2 + 1];
                Big s;
                big_add(s, a00, a10);
                A[m0 * 2]     = a11;   // from (m1, d=1): c=0, new interval d'=0
                A[m0 * 2 + 1] = a01;   // from (m0, d=1): c=0, same interval
                A[m1 * 2]     = s;     // from (m0,d=0) same interval; (m1,d=0) d'=0
                A[m1 * 2 + 1] = a10;   // from (m1, d=0): new interval d'=1
            }
        }
        // column x = i: write c_i = !d into new bit i-1, sum over d
        const size_t hi = (size_t)1 << (i - 1);
        Big sum{};
        for (size_t m = 0; m < m_i; m++) {
            Big v0 = A[m * 2], v1 = A[m * 2 + 1];
            A[(m | hi) * 2] = v0; A[(m | hi) * 2 + 1] = v0;   // d=0 -> c_i = 1
            A[m * 2] = v1;        A[m * 2 + 1] = v1;          // d=1 -> c_i = 0
            big_add(sum, sum, v0);
            big_add(sum, sum, v1);
        }
        if (g_overflow) {
            std::free(A);
            std::fprintf(stderr, "335547_01: overflow at a(%d); rebuild with larger -DLIMBS\n", i + 1);
            return 1;
        }
        record(vals, (size_t)i + 1, big_to_dec(sum));
    }
    std::free(A);
    return 0;
}
