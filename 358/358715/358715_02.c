/* 358715_02.c  --  b-file for OEIS A358715, low-memory version (CRT)
 *
 *   a(n) = number of tilings of an equilateral triangle of side n by
 *          equilateral triangles of integer sides (any sizes, both
 *          orientations).   1, 2, 5, 26, 220, 3622, 105859, 5677789, ...
 *
 * Usage:  ./358715_02 N      computes a(1..N), writes "n a(n)" to b358715_02.txt
 *         ./358715_02 --help
 *
 * Same transfer DP as 358715_01.c (see the proof sketch there; repeated in
 * short here), but all counts are kept modulo primes p < 2^31 (uint32_t, so
 * a+b < 2^32), one prime at a time, and a(n) is rebuilt by CRT (Garner).
 *
 * ---------------------------------------------------------------------------
 * Row i has cells at positions 0..2i (up cells even, down cells odd).  An up
 * tile with apex s occupies [s, s+2t] in its t-th row; a down tile whose top
 * row is [s, s+2k-2] occupies [s+2t, s+2k-2] in its t-th row.  The slices of
 * a tiling in row i are segments U,D,U,...,U of odd lengths (U = starts at an
 * even position, D = starts at an odd position), encoded by cut bits.  The
 * number of possible rows is the number of compositions of 2i+1 into odd
 * parts, F(2i+1).
 * Row i -> i+1: every up slice ends or continues as [s,e+2]; every down slice
 * [s,e] with e > s continues as [s+2,e], one with e = s ends; every other new
 * cell is a new up apex (even) or belongs to the top row of a new down tile
 * (an odd..odd segment of free cells; free cells may form several tiles).  A tiling <-> its sequence of rows (bijection onto
 * admissible sequences whose last row has only length-1 down slices).
 *
 * Bound used for CRT:  a(n) <= B(n) := prod_{i=1}^{n-1} F(2i+1)
 *   (row 0 is fixed; row i takes at most F(2i+1) values).  B is
 *   non-decreasing.  We use the K largest primes below 2^31 with
 *   P = p_1...p_K > B(N), so 0 <= a(n) <= B(n) < P determines a(n), n <= N.
 *   One extra prime q is used only as a check (residue mod q must agree, and
 *   the rebuilt value must be <= B(n)); otherwise nothing is written.
 *
 * In-place sweep (positions q = 0..2i+2) with key = new cut bits (< q-1) |
 * old cut bits (>= q-1) | context (typ, cont, doff, ext, prev) -- see
 * 358715_01.c for the meaning and the correctness argument.
 *
 * Last row. Each old down slice of length d>1 must shrink to d-2.
 * Only singleton down slices are allowed at the bottom, so d>=5
 * contributes zero. An old up slice [s,e] may stop or occupy [s,e+2].
 * This extended interval never hits a mandatory down slice: the old
 * down slice to its right starts at e+1 and shrinks from that end.
 * Two up extensions intersect exactly when their old slices are
 * separated by a singleton down slice. Thus groups linked by singleton
 * down slices give independent sets on paths. A group of k up slices
 * contributes F(k+2). Remaining cells must be singleton tiles, uniquely.
 * The final row can therefore be summed directly from the previous map.
 *
 * Memory.  Hash table entries are 8 + 4*LANES bytes (key + residues; LANES
 * primes are done in one pass, default 1 -> 12 bytes);
 * capacity is not rounded to a power of two (index = (hash * cap) >> 64),
 * load factor <= 3/4.  Observed: row r has F(2r+1) states and the largest
 * intermediate table during the step to row r has F(2r+2) keys; tables are
 * pre-sized from that (and grow by 1.25x if ever needed).
 *   The largest constructed row now has side N-1. The steady table
 *   estimate for N>=3 is about 2 * F(2N-2) / 0.75 * (8+4*LANES)
 *   bytes, with additional transient memory during resizing.
 * ---------------------------------------------------------------------------
 *
 * Build:  gcc -O3 -march=native -std=c17 -Wall -Wextra -Wpedantic -o 358715_02 358715_02.c
 *         On macOS, use /usr/bin/clang in place of gcc.
 *         (options: -DMAXN=n   largest allowed N, default 27; at most 27
 *                   -DLANES=g  primes per pass, default 1: more memory, fewer passes)
 */

#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#ifndef MAXN
#define MAXN 27
#endif
#if MAXN < 1 || MAXN > 27
#error "MAXN must be in 1..27 (key = 2N-1 mask bits + 9 context bits < 63 bits)"
#endif

#define OUTFILE "b358715_02.txt"
#define LOCKFILE "b358715_02.txt.lock"
#define MAX_SAVED_BYTES (16u << 20)
#define MAX_DIGITS 1000
#ifndef LANES
#define LANES 1          /* primes handled in one pass (memory 8+4*LANES bytes per slot) */
#endif
#if LANES < 1 || LANES > 64
#error "LANES must be in 1..64"
#endif

__extension__ typedef unsigned __int128 u128;

static void die(const char *msg) {
    fprintf(stderr, "358715_02: %s\n", msg);
    exit(1);
}
static void die_errno(const char *msg) {
    fprintf(stderr, "358715_02: %s: %s\n", msg, strerror(errno));
    exit(1);
}

/* ---------------- small unsigned big integer (base 2^32) ---------------- */
typedef struct { uint32_t *d; size_t n, cap; } Big;   /* n = used limbs, no leading zeros */
static void big_init(Big *x, uint32_t v) {
    x->cap = 8; x->d = malloc(x->cap * sizeof(uint32_t));
    if (!x->d) die("out of memory");
    x->n = 0; if (v) { x->d[0] = v; x->n = 1; }
}
static void big_push(Big *x, uint32_t v) {
    if (x->n == x->cap) {
        if (x->cap == 0 || x->cap > SIZE_MAX / (2 * sizeof(*x->d)))
            die("big integer capacity overflow");
        x->cap *= 2;
        uint32_t *t = realloc(x->d, x->cap * sizeof(uint32_t));
        if (!t) die("out of memory");
        x->d = t;
    }
    x->d[x->n++] = v;
}
static void big_mul_small(Big *x, uint64_t m) {   /* any m < 2^64 */
    u128 c = 0;                                     /* d*m + c < 2^97 */
    for (size_t k = 0; k < x->n; k++) { c += (u128)x->d[k] * m; x->d[k] = (uint32_t)c; c >>= 32; }
    while (c) { big_push(x, (uint32_t)c); c >>= 32; }
    while (x->n && x->d[x->n - 1] == 0) x->n--;
}
static void big_add_small(Big *x, uint32_t a) {
    uint64_t c = a;
    for (size_t k = 0; c && k < x->n; k++) { c += x->d[k]; x->d[k] = (uint32_t)c; c >>= 32; }
    if (c) big_push(x, (uint32_t)c);
}
static uint32_t big_mod_small(const Big *x, uint32_t m) {
    uint64_t r = 0;
    for (size_t k = x->n; k > 0; --k) r = ((r << 32) | x->d[k - 1]) % m;
    return (uint32_t)r;
}
static int big_cmp(const Big *a, const Big *b) {
    if (a->n != b->n) return a->n < b->n ? -1 : 1;
    for (size_t k = a->n; k > 0; --k)
        if (a->d[k - 1] != b->d[k - 1]) return a->d[k - 1] < b->d[k - 1] ? -1 : 1;
    return 0;
}
static void big_copy(Big *dst, const Big *src) {
    big_init(dst, 0);
    for (size_t k = 0; k < src->n; k++) big_push(dst, src->d[k]);
}
static char *big_to_dec(const Big *x0) {
    Big x; big_copy(&x, x0);
    size_t cap = x.n * 10 + 16, nch = 0;
    uint32_t *ch = malloc(cap * sizeof(uint32_t));
    if (!ch) die("out of memory");
    while (x.n) {
        uint64_t r = 0;
        for (size_t k = x.n; k > 0; --k) {
            uint64_t cur = (r << 32) | x.d[k - 1];
            x.d[k - 1] = (uint32_t)(cur / 1000000000u);
            r = cur % 1000000000u;
        }
        while (x.n && x.d[x.n - 1] == 0) x.n--;
        ch[nch++] = (uint32_t)r;
    }
    const size_t size = nch * 9 + 2;
    char *s = malloc(size);
    if (!s) die("out of memory");
    if (nch == 0) strcpy(s, "0");
    else {
        int written = snprintf(s, size, "%u", ch[nch - 1]);
        if (written <= 0 || (size_t)written >= size) die("cannot format result");
        size_t len = (size_t)written;
        for (size_t i = nch - 1; i > 0; --i) {
            written = snprintf(s + len, size - len, "%09u", ch[i - 1]);
            if (written != 9 || (size_t)written >= size - len) die("cannot format result");
            len += (size_t)written;
        }
    }
    free(ch); free(x.d);
    return s;
}

/* ---------------- modular helpers ---------------- */
static uint32_t pow_mod(uint64_t a, uint64_t e, uint32_t m) {
    uint64_t r = 1 % m; a %= m;
    while (e) { if (e & 1) r = r * a % m; a = a * a % m; e >>= 1; }
    return (uint32_t)r;
}
static int is_prime(uint32_t n) {
    if (n < 2) return 0;
    for (uint32_t q = 2; (uint64_t)q * q <= n; q++) if (n % q == 0) return 0;
    return 1;
}

/* ---------------- hash map: uint64 key -> uint32 residue ---------------- */
#define EMPTY UINT64_MAX      /* keys are < 2^63 */
typedef struct {
    size_t cap, cnt;
    uint64_t *keys;
    uint32_t *vals;
} Map;
static uint32_t g_p[LANES];   /* primes of the current pass */

static void map_alloc(Map *m, size_t cap) {
    if (cap < 16) cap = 16;
    if (cap > SIZE_MAX / sizeof(uint64_t) / LANES) die("hash table too large");
    m->cap = cap; m->cnt = 0;
    m->keys = malloc(cap * sizeof(uint64_t));
    m->vals = malloc(cap * LANES * sizeof(uint32_t));
    if (!m->keys || !m->vals) die("out of memory (try a smaller N)");
    for (size_t i = 0; i < cap; i++) m->keys[i] = EMPTY;
}
static void map_free(Map *m) { free(m->keys); free(m->vals); m->keys = NULL; m->vals = NULL; m->cap = m->cnt = 0; }
static inline size_t slot0(uint64_t k, size_t cap) {
    /* The hash deliberately keeps the low 64 bits of each product. */
    k ^= k >> 33;
    (void)__builtin_mul_overflow(k, UINT64_C(0xff51afd7ed558ccd), &k);
    k ^= k >> 33;
    (void)__builtin_mul_overflow(k, UINT64_C(0xc4ceb9fe1a85ec53), &k);
    k ^= k >> 33;
    return (size_t)(((u128)k * cap) >> 64);
}
static void map_add(Map *m, uint64_t key, const uint32_t *v);
static void map_resize(Map *m, size_t newcap) {
    Map n;
    map_alloc(&n, newcap);
    for (size_t i = 0; i < m->cap; i++)
        if (m->keys[i] != EMPTY) map_add(&n, m->keys[i], m->vals + i * LANES);
    map_free(m);
    *m = n;
}
static void map_add(Map *m, uint64_t key, const uint32_t *v) {
    if ((m->cnt + 1) * 4 > m->cap * 3) map_resize(m, m->cap + m->cap / 4 + 16);
    size_t h = slot0(key, m->cap);
    while (m->keys[h] != EMPTY && m->keys[h] != key) { if (++h == m->cap) h = 0; }
    uint32_t *dst = m->vals + h * LANES;
    if (m->keys[h] == EMPTY) { m->keys[h] = key; memcpy(dst, v, LANES * sizeof(uint32_t)); m->cnt++; }
    else for (int l = 0; l < LANES; l++) { uint32_t s = dst[l] + v[l]; if (s >= g_p[l]) s -= g_p[l]; dst[l] = s; }
}
static void map_clear(Map *m) {
    for (size_t i = 0; i < m->cap; i++) m->keys[i] = EMPTY;
    m->cnt = 0;
}
/* make sure the table can hold `want` keys at load <= 3/4 without growing */
static void map_reserve(Map *m, size_t want) {
    size_t need = want / 3 * 4 + 64;
    if (m->cap < need) map_resize(m, need);      /* keeps the contents */
}

/* ---------------- context packing ---------------- */
enum { U = 0, D = 1 };
enum { P_NONE = 0, P_A, P_B, P_C, P_NMID, P_NODD };
#define CTX_SHIFT 54
static inline uint64_t pack(uint64_t mask, int typ, int cont, int doff, int ext, int prev) {
    uint64_t ctx = (uint64_t)typ | ((uint64_t)cont << 1) | ((uint64_t)doff << 2) |
                   ((uint64_t)ext << 4) | ((uint64_t)prev << 6);
    return mask | (ctx << CTX_SHIFT);
}

static uint64_t FIB[64];

/* row i -> row i+1 (result: cur holds plain row states) */
static void row_step(Map *cur, Map *nxt, int i) {
    const int L = 2 * i + 1, M = L + 2;
    /* expected peak F(2(i+1)+2) keys (observed); reserve for it */
    size_t want = (size_t)FIB[2 * (i + 1) + 2];
    const size_t need = want / 3 * 4 + 64;
    if (nxt->cap < need) {
        map_free(nxt);
        map_alloc(nxt, need);
    }
    map_reserve(cur, want);
    for (int q = 0; q < M; q++) {
        map_clear(nxt);
        for (size_t h = 0; h < cur->cap; h++) {
            uint64_t key = cur->keys[h];
            if (key == EMPTY) continue;
            const uint32_t *v = cur->vals + h * LANES;
            uint64_t mask = key & ((1ULL << CTX_SHIFT) - 1);
            uint64_t ctx = key >> CTX_SHIFT;
            int typ = (int)(ctx & 1), cont = (int)((ctx >> 1) & 1), doff = (int)((ctx >> 2) & 3);
            int ext = (int)((ctx >> 4) & 3), prev = (int)((ctx >> 6) & 7);

            int newseg;
            if (q == 0) newseg = 1;
            else if (q < L) newseg = (int)((mask >> (q - 1)) & 1);
            else newseg = (q == L);
            if (newseg) {
                if (q > 0 && typ == U && cont) ext = 2;
                if (q < L) { typ = (q & 1) ? D : U; doff = 0; cont = 0; }
            } else if (q < L && typ == D && doff < 2) {
                doff++;
            }
            int nc = 1, cl[2] = { cont, 0 };
            if (q < L && newseg && typ == U) {
                cl[0] = 0;
                if (ext == 0) { cl[1] = 1; nc = 2; }
            }
            for (int ci = 0; ci < nc; ci++) {
                int c = cl[ci], e = ext, own;
                if (q < L && typ == U && c) own = P_A;
                else if (e > 0) { own = P_A; e--; }
                else if (q < L && typ == D && doff >= 2) own = P_B;
                else own = 0;

                int no = 0, cuts[2], cls[2];
                if (own) {
                    if (prev == P_NMID) continue;
                    cuts[0] = (prev != own); cls[0] = own; no = 1;
                } else if ((q & 1) == 0) {
                    if (prev == P_NMID) die("internal: NMID before even cell");
                    cuts[0] = 1; cls[0] = P_C; no = 1;
                    if (prev == P_NODD) { cuts[1] = 0; cls[1] = P_NMID; no = 2; }
                } else {
                    cuts[0] = (prev == P_NMID) ? 0 : 1; cls[0] = P_NODD; no = 1;
                }
                for (int oi = 0; oi < no; oi++) {
                    uint64_t m2 = mask;
                    if (q >= 1) m2 = (m2 & ~(1ULL << (q - 1))) | ((uint64_t)cuts[oi] << (q - 1));
                    map_add(nxt, pack(m2, typ, c, doff, e, cls[oi]), v);
                }
            }
        }
        Map t = *cur; *cur = *nxt; *nxt = t;
    }
    map_clear(nxt);
    for (size_t h = 0; h < cur->cap; h++) {
        uint64_t key = cur->keys[h];
        if (key == EMPTY) continue;
        if ((int)((key >> (CTX_SHIFT + 6)) & 7) == P_NMID) continue;
        map_add(nxt, key & ((1ULL << CTX_SHIFT) - 1), cur->vals + h * LANES);
    }
    Map t = *cur; *cur = *nxt; *nxt = t;
}

/* The sentinel is the cut at the right edge, so every ctz has a
 * nonzero argument and the loop visits precisely the tile slices. */
static int final_ok(uint64_t mask, int length) {
    uint64_t cuts = mask | (UINT64_C(1) << (length - 1));
    int start = 0;
    while (cuts) {
        const int end = __builtin_ctzll(cuts);
        if ((start & 1) && end != start) return 0;
        start = end + 1;
        cuts &= cuts - 1;
    }
    return 1;
}

static uint64_t bottom_extensions(uint64_t mask, int length) {
    uint64_t cuts = mask | (UINT64_C(1) << (length - 1));
    uint64_t ways = 1;
    int start = 0, group = 0;
    while (cuts) {
        const int end = __builtin_ctzll(cuts);
        if ((start & 1) == 0) {
            ++group;
        } else if (end != start) {
            if (end - start != 2) return 0;
            ways *= FIB[group + 2];
            group = 0;
        }
        start = end + 1;
        cuts &= cuts - 1;
    }
    /* The product is at most 2^(number of up slices), hence <= 2^26
       for MAXN <= 27. Each residue product therefore fits in uint64_t. */
    return ways * FIB[group + 2];
}

/* res[l][n] = a(n) mod g_p[l], n = 1..N  (res is LANES x (N+1)) */
static void run_pass(int N, Map *cur, Map *nxt, uint32_t *res) {
    map_clear(cur);
    uint32_t one[LANES];
    for (int l = 0; l < LANES; l++) { one[l] = 1 % g_p[l]; res[(size_t)l * (N + 1) + 1] = one[l]; }
    map_add(cur, 0, one);
    for (int i = 0; i + 2 < N; i++) {
        row_step(cur, nxt, i);
        const int L1 = 2 * (i + 1) + 1;
        uint64_t s[LANES] = { 0 };
        for (size_t h = 0; h < cur->cap; h++)
            if (cur->keys[h] != EMPTY && final_ok(cur->keys[h], L1))
                for (int l = 0; l < LANES; l++) { s[l] += cur->vals[h * LANES + l]; if (s[l] >= g_p[l]) s[l] -= g_p[l]; }
        for (int l = 0; l < LANES; l++) res[(size_t)l * (N + 1) + i + 2] = (uint32_t)s[l];
    }

    if (N == 1) return;
    const int length = 2 * N - 3;
    uint64_t total[LANES] = {0};
    for (size_t h = 0; h < cur->cap; ++h) {
        if (cur->keys[h] == EMPTY) continue;
        const uint64_t ways = bottom_extensions(cur->keys[h], length);
        if (ways == 0) continue;
        for (int l = 0; l < LANES; ++l) {
            total[l] += (ways * cur->vals[h * LANES + l]) % g_p[l];
            if (total[l] >= g_p[l]) total[l] -= g_p[l];
        }
    }
    for (int l = 0; l < LANES; ++l)
        res[(size_t)l * (N + 1) + N] = (uint32_t)total[l];
}

/* ---------------- arguments ---------------- */
static void usage(FILE *f) {
    fprintf(f,
        "usage: 358715_02 N\n"
        "  computes a(1..N) (tilings of a side-n triangle by integer triangles)\n"
        "  modulo several primes (CRT); saves \"n a(n)\" lines to %s once at the end\n"
        "  1 <= N <= %d; existing lines are re-verified, lines beyond N are kept\n",
        OUTFILE, MAXN);
}
static int parse_n(const char *s) {
    if (!*s) die("N must be a positive integer");
    for (const char *p = s; *p; p++)
        if (*p < '0' || *p > '9') die("N must be a positive integer (digits only)");
    errno = 0;
    char *end = NULL;
    long v = strtol(s, &end, 10);
    if (errno || *end || v < 1 || v > MAXN) {
        fprintf(stderr, "358715_02: N must satisfy 1 <= N <= %d\n", MAXN);
        exit(1);
    }
    return (int)v;
}

/* ---------------- files ---------------- */
static void open_lock(void) {
    int fd = open(LOCKFILE, O_RDWR | O_CREAT | O_NOFOLLOW | O_CLOEXEC | O_NONBLOCK, 0644);
    if (fd < 0) die_errno("cannot open lock file");
    struct stat st;
    if (fstat(fd, &st) != 0) die_errno("fstat lock file");
    if (!S_ISREG(st.st_mode) || st.st_nlink != 1) die("lock file is not a plain regular file");
    if (flock(fd, LOCK_EX | LOCK_NB) != 0) {
        if (errno == EWOULDBLOCK) die("another instance is running (lock held)");
        die_errno("flock");
    }
    /* fd intentionally kept open until exit */
}

static char **vals = NULL;
static size_t nvals = 0, capvals = 0;
static void push_val(const char *s) {
    if (nvals == capvals) {
        capvals = capvals ? capvals * 2 : 64;
        char **t = realloc(vals, capvals * sizeof(char *));
        if (!t) die("out of memory");
        vals = t;
    }
    vals[nvals] = strdup(s);
    if (!vals[nvals]) die("out of memory");
    nvals++;
}

static void read_saved(void) {
    int fd = open(OUTFILE, O_RDONLY | O_NOFOLLOW | O_CLOEXEC | O_NONBLOCK);
    if (fd < 0) {
        if (errno == ENOENT) return;
        die_errno("cannot open saved file (symlink?)");
    }
    struct stat st;
    if (fstat(fd, &st) != 0) die_errno("fstat saved file");
    if (!S_ISREG(st.st_mode) || st.st_nlink != 1) die("saved file is not a plain regular file");
    if ((uint64_t)st.st_size > MAX_SAVED_BYTES) die("saved file too large");
    size_t cap = (size_t)st.st_size + 1, len = 0;
    char *data = malloc(cap + 1);
    if (!data) die("out of memory");
    for (;;) {
        if (len == cap) die("saved file grew while reading");
        ssize_t r = read(fd, data + len, cap - len);
        if (r < 0) { if (errno == EINTR) continue; die_errno("read saved file"); }
        if (r == 0) break;
        len += (size_t)r;
    }
    if (close(fd) != 0) die_errno("close saved file");
    data[len] = '\0';
    if (memchr(data, '\0', len)) die("saved file contains NUL bytes (file left unchanged)");

    size_t pos = 0;
    long expect = 1;
    while (pos < len) {
        char *nl = memchr(data + pos, '\n', len - pos);
        if (!nl) die("saved file: incomplete last line (file left unchanged)");
        *nl = '\0';
        char *line = data + pos;
        pos = (size_t)(nl - data) + 1;
        char *sp = strchr(line, ' ');
        if (!sp || sp == line || sp[1] == '\0') die("saved file: malformed line (file left unchanged)");
        *sp = '\0';
        char *ns = line, *vs = sp + 1;
        for (char *p = ns; *p; p++) if (*p < '0' || *p > '9') die("saved file: bad index (file left unchanged)");
        for (char *p = vs; *p; p++) if (*p < '0' || *p > '9') die("saved file: bad value (file left unchanged)");
        if (strlen(ns) > 6 || ns[0] == '0' || strtol(ns, NULL, 10) != expect)
            die("saved file: indices must be 1,2,3,... (file left unchanged)");
        if (strlen(vs) > MAX_DIGITS || vs[0] == '0')
            die("saved file: value out of range (must be positive, no leading zeros)");
        push_val(vs);
        expect++;
    }
    free(data);
}

static void write_all(void) {
    struct stat st;
    if (lstat(OUTFILE, &st) == 0) {
        if (!S_ISREG(st.st_mode) || st.st_nlink != 1)
            die("refusing to replace result file: not a plain regular file");
    } else if (errno != ENOENT) {
        die_errno("lstat result file");
    }
    char name[] = OUTFILE ".tmpXXXXXX";
    int fd = mkstemp(name);
    if (fd < 0) die_errno("mkstemp");
    if (fchmod(fd, 0644) != 0) { unlink(name); die_errno("fchmod"); }
    FILE *f = fdopen(fd, "w");
    if (!f) { close(fd); unlink(name); die_errno("fdopen"); }
    for (size_t i = 0; i < nvals; i++)
        if (fprintf(f, "%zu %s\n", i + 1, vals[i]) < 0) { fclose(f); unlink(name); die("write to temp file failed"); }
    if (fflush(f) != 0) { fclose(f); unlink(name); die_errno("fflush temp"); }
    if (fsync(fileno(f)) != 0) { fclose(f); unlink(name); die_errno("fsync temp"); }
    if (fclose(f) != 0) { unlink(name); die_errno("fclose temp"); }
    if (rename(name, OUTFILE) != 0) { unlink(name); die_errno("rename"); }
    {   /* make the rename durable */
        int dfd = open(".", O_RDONLY | O_DIRECTORY | O_CLOEXEC);
        if (dfd < 0) die_errno("open directory");
        if (fsync(dfd) != 0) die_errno("fsync directory");
        if (close(dfd) != 0) die_errno("close directory");
    }
}

int main(int argc, char **argv) {
    if (argc == 2 && (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h"))) { usage(stdout); return 0; }
    if (argc != 2) { usage(stderr); return 1; }
    const int N = parse_n(argv[1]);

    open_lock();
    read_saved();
    if (nvals > (size_t)N)
        fprintf(stderr, "358715_02: note: keeping saved lines beyond N=%d (not re-verified)\n", N);

    FIB[0] = 0; FIB[1] = 1;
    for (int k = 2; k < 64; k++) FIB[k] = FIB[k - 1] + FIB[k - 2];   /* F(63) < 2^64 */

    /* B(n) = prod_{i=1}^{n-1} F(2i+1); F(2i+1) <= F(53) < 2^64 for n <= 27 */
    Big *Bn = malloc(((size_t)N + 1) * sizeof(Big));
    if (!Bn) die("out of memory");
    {
        Big b; big_init(&b, 1);
        big_copy(&Bn[1], &b);
        for (int n = 2; n <= N; n++) {
            big_mul_small(&b, FIB[2 * (n - 1) + 1]);
            big_copy(&Bn[n], &b);
        }
        free(b.d);
    }

    /* primes: largest below 2^31 until product > B(N), plus one check prime */
    uint32_t primes[256];
    size_t np = 0;
    {
        Big P; big_init(&P, 1);
        uint32_t c = 0x7FFFFFFFu;
        while (big_cmp(&P, &Bn[N]) <= 0) {
            while (!is_prime(c)) c--;
            if (np >= 255) die("internal: too many primes");
            primes[np++] = c; big_mul_small(&P, c); c--;
        }
        while (!is_prime(c)) c--;
        primes[np++] = c;
        free(P.d);
    }
    const size_t K = np - 1;
    fprintf(stderr, "358715_02: N=%d, %zu primes + 1 check prime, LANES=%d, est. memory %.2f GiB\n", N, K, LANES,
            2.0 * ((double)FIB[2 * (N - 1)] / 0.75 + 64) * (8.0 + 4.0 * LANES) / (1u << 30));

    uint32_t *R = calloc(np * ((size_t)N + 1), sizeof(uint32_t));
    if (!R) die("out of memory");
    Map cur, nxt;
    map_alloc(&cur, 64);
    map_alloc(&nxt, 64);
    time_t t0 = time(NULL);
    uint32_t *Rp = malloc((size_t)LANES * ((size_t)N + 1) * sizeof(uint32_t));
    if (!Rp) die("out of memory");
    for (size_t k0 = 0; k0 < np; k0 += LANES) {
        /* unused lanes repeat the last prime */
        for (int l = 0; l < LANES; l++) g_p[l] = primes[k0 + (size_t)l < np ? k0 + (size_t)l : np - 1];
        run_pass(N, &cur, &nxt, Rp);
        for (int l = 0; l < LANES && k0 + (size_t)l < np; l++)
            memcpy(R + (k0 + (size_t)l) * ((size_t)N + 1), Rp + (size_t)l * ((size_t)N + 1),
                   ((size_t)N + 1) * sizeof(uint32_t));
        size_t done = k0 + LANES < np ? k0 + LANES : np;
        fprintf(stderr, "358715_02: primes %zu/%zu done, %lds elapsed\n", done, np, (long)(time(NULL) - t0));
    }
    free(Rp);
    map_free(&cur); map_free(&nxt);

    /* Garner and checks */
    char **computed = calloc((size_t)N + 1, sizeof(char *));
    uint32_t *cc = malloc((K ? K : 1) * sizeof(uint32_t));
    if (!computed || !cc) die("out of memory");
    for (int n = 1; n <= N; n++) {
        for (size_t j = 0; j < K; j++) {
            const uint32_t pj = primes[j];
            uint64_t v = 0, prod = 1;
            for (size_t t = 0; t < j; t++) {
                v = (v + (uint64_t)cc[t] * prod) % pj;
                prod = prod * (primes[t] % pj) % pj;
            }
            uint64_t diff = ((uint64_t)R[j * ((size_t)N + 1) + (size_t)n] + pj - v) % pj;
            cc[j] = (uint32_t)(diff * pow_mod(prod, pj - 2, pj) % pj);
        }
        Big x; big_init(&x, 0);
        for (size_t j = K; j > 0; --j) {
            big_mul_small(&x, primes[j - 1]);
            big_add_small(&x, cc[j - 1]);
        }
        if (big_mod_small(&x, primes[K]) != R[K * ((size_t)N + 1) + (size_t)n]) {
            fprintf(stderr, "358715_02: check prime mismatch at n=%d; nothing written\n", n);
            return 1;
        }
        if (big_cmp(&x, &Bn[n]) > 0) {
            fprintf(stderr, "358715_02: value exceeds proven bound at n=%d; nothing written\n", n);
            return 1;
        }
        computed[n] = big_to_dec(&x);
        free(x.d);
    }

    int changed = 0;
    for (int n = 1; n <= N; n++) {
        if ((size_t)n <= nvals) {
            if (strcmp(vals[n - 1], computed[n]) != 0) {
                fprintf(stderr, "358715_02: saved a(%d) = %s but computed %s; file left unchanged\n",
                        n, vals[n - 1], computed[n]);
                return 1;
            }
        } else {
            push_val(computed[n]);
            changed = 1;
        }
    }
    if (changed) write_all();
    for (int n = 1; n <= N; n++)
        if (printf("%d %s\n", n, computed[n]) < 0) die("printf failed");
    if (fflush(stdout) != 0) die("fflush stdout failed");
    for (int n = 1; n <= N; ++n) {
        free(Bn[n].d);
        free(computed[n]);
    }
    free(Bn);
    free(computed);
    free(R);
    free(cc);
    for (size_t i = 0; i < nvals; ++i) free(vals[i]);
    free(vals);
    return 0;
}
