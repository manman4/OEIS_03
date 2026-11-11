// 335547_02.cpp  --  b-file for OEIS A335547, low-memory version (CRT)
//
// a(n) = number of ways to tile a size-n staircase polyomino (left-aligned rows
// of lengths 1..n) with staircase polyominoes in the same direction.
//
// Usage:  ./335547_02 N        computes a(1..N), writes "n a(n)" to b335547_02.txt
//         ./335547_02 --help
//
// ---------------------------------------------------------------------------
// Transfer DP (same as 335547_01.cpp).  Proof sketch:
//
// (1) A size-k staircase tile with apex at column c occupies, in its t-th row
//     (t = 0..k-1), exactly the columns [c, c+t].  So row i of a tiling is a
//     partition of {1..i} into intervals; we encode it by the cut bits
//     b_1..b_{i-1} (b_x = 1 iff columns x and x+1 are in different intervals).
// (2) From row i to row i+1 every interval [s,e] either ends or continues as
//     [s,e+1]; two adjacent intervals cannot both continue (the left one would
//     take the first column of the right one).  All other cells of row i+1 are
//     apexes of new tiles (singleton intervals).  Tilings <-> choice sequences
//     is a bijection.
// (3) New cut bits: c_x = 0 iff column x lies in a continued interval.
// (4) Sweep x = 1..i with flag d = "interval containing column x continues";
//     output c_x = !d, then if b_x = 1 a new interval starts and picks
//     d' in {0} u ({1} if d = 0).  b_x and c_x share bit position x-1, so with
//     A0[m] = A[m][d=0], A1[m] = A[m][d=1] and m0/m1 = bit x-1 equal to 0/1:
//        A0[m0] <- A1[m1]          A1[m0] <- A1[m0]
//        A0[m1] <- A0[m0]+A0[m1]   A1[m1] <- A0[m1]
// (5) a(1) = 1, a(i+1) = sum over all states after column i of row step i.
//     For the last row step (i = N-1) the new bit is never stored: we only
//     need the sum, so the arrays hold masks of at most N-2 bits.
//
// Low memory: everything is computed modulo primes p < 2^31 (so a+b < 2^32
// fits in uint32_t), one prime at a time, and a(n) is rebuilt by CRT (Garner).
// Memory: 2 arrays * 2^(N-2) * 4 bytes = 2^(N+1) bytes.
//
// Number of primes.  Upper bound:  a(n) <= B(n) := prod_{i=1}^{n-1} F_{i+2}
// (Fibonacci, F_1 = F_2 = 1).  Proof: by (2) a tiling is a sequence of choices;
// in row step i the state has m <= i intervals and the admissible choices are
// the independent sets of a path with m vertices, of which there are
// F_{m+2} <= F_{i+2}.  B is non-decreasing in n.  We take the K largest primes
// below 2^31 with P = p_1...p_K > B(N); then 0 <= a(n) <= B(n) < P determines
// a(n) for all n <= N.  One more prime q is used only as a check: the CRT value
// must agree with the residue mod q, and must be <= B(n).
// ---------------------------------------------------------------------------
//
// Build:  g++ -O3 -march=native -std=c++17 -Wall -Wextra -Wpedantic -o 335547_02 335547_02.cpp
//         (option: -DMAXN=n  largest allowed N, default 40)

#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>
#include <vector>

#include <fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef MAXN
#define MAXN 40
#endif
static_assert(MAXN >= 1 && MAXN <= 44, "MAXN out of range");   // F_{MAXN+1} < 2^32

static const char *OUTFILE = "b335547_02.txt";
static const char *LOCKFILE = "b335547_02.txt.lock";
static const size_t MAX_SAVED_BYTES = 16u << 20;
static const size_t MAX_DIGITS = 1000;

[[noreturn]] static void die(const char *msg) {
    std::fprintf(stderr, "335547_02: %s\n", msg);
    std::exit(1);
}
[[noreturn]] static void die_errno(const char *msg) {
    std::fprintf(stderr, "335547_02: %s: %s\n", msg, std::strerror(errno));
    std::exit(1);
}

// ---------------- simple unsigned big integer (base 2^32, little endian) ----
struct BigU {
    std::vector<uint32_t> d;   // no trailing zero limbs; empty = 0
    void trim() { while (!d.empty() && d.back() == 0) d.pop_back(); }
};
static void mul_small(BigU &x, uint32_t m) {
    uint64_t c = 0;
    for (auto &w : x.d) { c += (uint64_t)w * m; w = (uint32_t)c; c >>= 32; }
    if (c) x.d.push_back((uint32_t)c);
    x.trim();
}
static void add_small(BigU &x, uint32_t a) {
    uint64_t c = a;
    for (size_t k = 0; c && k < x.d.size(); k++) { c += x.d[k]; x.d[k] = (uint32_t)c; c >>= 32; }
    if (c) x.d.push_back((uint32_t)c);
}
static uint32_t mod_small(const BigU &x, uint32_t m) {
    uint64_t r = 0;
    for (size_t k = x.d.size(); k-- > 0;) r = ((r << 32) | x.d[k]) % m;
    return (uint32_t)r;
}
static int cmp(const BigU &a, const BigU &b) {
    if (a.d.size() != b.d.size()) return a.d.size() < b.d.size() ? -1 : 1;
    for (size_t k = a.d.size(); k-- > 0;)
        if (a.d[k] != b.d[k]) return a.d[k] < b.d[k] ? -1 : 1;
    return 0;
}
static std::string to_dec(BigU x) {
    if (x.d.empty()) return "0";
    std::vector<uint32_t> ch;   // base 1e9
    while (!x.d.empty()) {
        uint64_t r = 0;
        for (size_t k = x.d.size(); k-- > 0;) {
            uint64_t cur = (r << 32) | x.d[k];
            x.d[k] = (uint32_t)(cur / 1000000000u);
            r = cur % 1000000000u;
        }
        x.trim();
        ch.push_back((uint32_t)r);
    }
    std::string s = std::to_string(ch.back());
    char buf[16];
    for (size_t i = ch.size() - 1; i-- > 0;) { std::snprintf(buf, sizeof buf, "%09u", ch[i]); s += buf; }
    return s;
}

// ---------------- modular helpers ----------------
static uint32_t pow_mod(uint64_t a, uint64_t e, uint32_t m) {
    uint64_t r = 1 % m; a %= m;
    while (e) { if (e & 1) r = r * a % m; a = a * a % m; e >>= 1; }
    return (uint32_t)r;
}
static bool is_prime(uint32_t n) {
    if (n < 2) return false;
    for (uint32_t q = 2; (uint64_t)q * q <= n; q++) if (n % q == 0) return false;
    return true;
}

// ---------------- argument parsing ----------------
static void usage(FILE *f) {
    std::fprintf(f,
        "usage: 335547_02 N\n"
        "  computes A335547(1..N) modulo several primes and rebuilds them by CRT;\n"
        "  saves \"n a(n)\" lines to %s (written once, after all primes)\n"
        "  1 <= N <= %d   memory: 2^(N+1) bytes (N=30: 2 GiB, N=32: 8 GiB)\n"
        "  existing lines are re-verified; lines beyond N are kept\n",
        OUTFILE, MAXN);
}
static int parse_n(const char *s) {
    if (!*s) die("N must be a positive integer");
    for (const char *p = s; *p; p++)
        if (*p < '0' || *p > '9') die("N must be a positive integer (digits only)");
    errno = 0;
    char *end = nullptr;
    long v = std::strtol(s, &end, 10);
    if (errno || *end || v < 1 || v > MAXN) {
        std::fprintf(stderr, "335547_02: N must satisfy 1 <= N <= %d\n", MAXN);
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
    return fd;
}

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
            std::fprintf(stderr, "335547_02: note: ignoring incomplete last line of %s\n", OUTFILE);
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

// ---------------- the DP modulo p ----------------
// res[n] = a(n) mod p for n = 1..N.  A0, A1 have room for 2^(N-2) masks (N >= 2).
static void run_mod(int N, uint32_t p, uint32_t *A0, uint32_t *A1, std::vector<uint32_t> &res) {
    res.assign((size_t)N + 1, 0);
    res[1] = 1 % p;
    A0[0] = 1 % p; A1[0] = 1 % p;           // row 1: one state, both d
    for (int i = 1; i < N; i++) {           // row i -> row i+1
        const size_t m_i = (size_t)1 << (i - 1);
        for (int x = 1; x <= i - 1; x++) {
            const size_t bit = (size_t)1 << (x - 1);
            for (size_t base = 0; base < m_i; base += 2 * bit) {
                uint32_t *a0 = A0 + base, *a1 = A1 + base;
                for (size_t j = 0; j < bit; j++) {
                    uint32_t t0 = a0[j], t1 = a0[j + bit], u1 = a1[j + bit];
                    uint32_t s = t0 + t1;               // < 2^32 since p < 2^31
                    if (s >= p) s -= p;
                    a0[j] = u1;
                    a0[j + bit] = s;
                    a1[j + bit] = t1;
                }
            }
        }
        uint64_t sum = 0;   // <= 2 * 2^(N-2) * 2^31 < 2^64 for N <= 44
        if (i < N - 1) {
            const size_t hi = m_i;              // new bit i-1
            for (size_t m = 0; m < m_i; m++) {
                uint32_t v0 = A0[m], v1 = A1[m];
                A0[m | hi] = v0; A1[m | hi] = v0;   // d = 0 -> c_i = 1
                A0[m] = v1;      A1[m] = v1;        // d = 1 -> c_i = 0
                sum += (uint64_t)v0 + v1;
            }
        } else {
            for (size_t m = 0; m < m_i; m++) sum += (uint64_t)A0[m] + A1[m];
        }
        res[(size_t)i + 1] = (uint32_t)(sum % p);
    }
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
        std::fprintf(stderr, "335547_02: note: keeping saved lines beyond N=%d (not re-verified)\n", N);

    // bounds B(n) = prod_{i=1}^{n-1} F_{i+2}
    std::vector<BigU> B((size_t)N + 1);
    {
        std::vector<uint64_t> F((size_t)N + 3, 0);
        F[1] = F[2] = 1;
        for (size_t k = 3; k < F.size(); k++) F[k] = F[k - 1] + F[k - 2];
        BigU b; b.d = {1};
        B[1] = b;
        for (int n = 2; n <= N; n++) {
            if (F[n + 1] > 0xFFFFFFFFull) die("internal: Fibonacci factor too large");
            mul_small(b, (uint32_t)F[n + 1]);   // factor F_{(n-1)+2}
            B[n] = b;
        }
    }

    // primes: largest below 2^31, until product > B(N), then one check prime
    std::vector<uint32_t> primes;
    {
        BigU P; P.d = {1};
        uint32_t c = 0x7FFFFFFFu;
        while (cmp(P, B[N]) <= 0) {
            while (!is_prime(c)) c--;
            primes.push_back(c); mul_small(P, c); c--;
        }
        while (!is_prime(c)) c--;
        primes.push_back(c);                    // check prime (last)
    }
    const size_t K = primes.size() - 1;
    std::fprintf(stderr, "335547_02: N=%d, %zu primes + 1 check prime, memory %.3f GiB\n",
                 N, K, N >= 2 ? (double)((size_t)1 << (N - 2)) * 8 / (1u << 30) : 0.0);

    std::vector<std::vector<uint32_t>> R(primes.size());
    if (N >= 2) {
        const size_t nm = (size_t)1 << (N - 2);
        if (nm > SIZE_MAX / sizeof(uint32_t)) die("N too large for size_t");
        uint32_t *A0 = static_cast<uint32_t *>(std::malloc(nm * sizeof(uint32_t)));
        uint32_t *A1 = static_cast<uint32_t *>(std::malloc(nm * sizeof(uint32_t)));
        if (!A0 || !A1) die("out of memory (try a smaller N)");
        time_t t0 = std::time(nullptr);
        for (size_t k = 0; k < primes.size(); k++) {
            run_mod(N, primes[k], A0, A1, R[k]);
            std::fprintf(stderr, "335547_02: prime %zu/%zu (%u) done, %lds elapsed\n",
                         k + 1, primes.size(), primes[k], (long)(std::time(nullptr) - t0));
        }
        std::free(A0); std::free(A1);
    } else {
        for (size_t k = 0; k < primes.size(); k++) R[k] = {0, 1};
    }

    // Garner: x = c_0 + c_1 p_0 + c_2 p_0 p_1 + ...  (0 <= c_j < p_j)
    std::vector<std::string> computed((size_t)N + 1);
    for (int n = 1; n <= N; n++) {
        std::vector<uint32_t> c(K);
        for (size_t j = 0; j < K; j++) {
            const uint32_t pj = primes[j];
            // value of c_0 + c_1 p_0 + ... + c_{j-1} p_0..p_{j-2}  mod pj
            uint64_t v = 0, prod = 1;
            for (size_t t = 0; t < j; t++) {
                v = (v + (uint64_t)c[t] * prod) % pj;
                prod = prod * (primes[t] % pj) % pj;
            }
            uint64_t diff = ((uint64_t)R[j][n] + pj - v) % pj;
            c[j] = (uint32_t)(diff * pow_mod(prod, pj - 2, pj) % pj);
        }
        BigU x;   // Horner from the top: x = c_{K-1}; x = x*p_{j} + c_j
        for (size_t j = K; j-- > 0;) { mul_small(x, primes[j]); add_small(x, c[j]); }
        const uint32_t q = primes[K];
        if (mod_small(x, q) != R[K][n]) {
            std::fprintf(stderr, "335547_02: check prime mismatch at n=%d; nothing written\n", n);
            return 1;
        }
        if (cmp(x, B[n]) > 0) {
            std::fprintf(stderr, "335547_02: value exceeds proven bound at n=%d; nothing written\n", n);
            return 1;
        }
        computed[(size_t)n] = to_dec(x);
    }

    // compare with saved, then write once
    bool changed = false;
    for (int n = 1; n <= N; n++) {
        const std::string &v = computed[(size_t)n];
        if ((size_t)n <= vals.size()) {
            if (vals[(size_t)n - 1] != v) {
                std::fprintf(stderr, "335547_02: saved a(%d) = %s but computed %s; file left unchanged\n",
                             n, vals[(size_t)n - 1].c_str(), v.c_str());
                return 1;
            }
        } else {
            vals.push_back(v);
            changed = true;
        }
    }
    if (changed) write_all(vals);
    for (int n = 1; n <= N; n++) {
        if (std::printf("%d %s\n", n, computed[(size_t)n].c_str()) < 0) die("printf failed");
    }
    if (std::fflush(stdout) != 0) die("fflush stdout failed");
    return 0;
}
