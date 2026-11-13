/* 358715_01.c  --  b-file for OEIS A358715 (as understood here):
 *
 *   a(n) = number of tilings of an equilateral triangle of side n by
 *          equilateral triangles of integer sides (any sizes, both
 *          orientations).   1, 2, 5, 26, 220, 3622, 105859, 5677789, ...
 *
 * Usage:  ./358715_01 N      computes a(1..N), writes "n a(n)" to b358715_01.txt
 *         ./358715_01 --help
 *
 * ---------------------------------------------------------------------------
 * Model.  Row i (i = 0..n-1, from the top) of the triangular grid consists of
 * 2i+1 unit cells at positions 0..2i: up cells at even positions, down cells
 * at odd positions.  Position p of row i and position p of row i+1 are
 * directly above/below each other in the sense used below.
 *
 *  - An up tile of side k with apex at position s of row r occupies, in row
 *    r+t (t = 0..k-1), positions [s, s+2t].
 *  - A down tile of side k whose top row starts at position s of row r
 *    occupies, in row r+t, positions [s+2t, s+2k-2]  (always odd..odd).
 *
 * Row slices.  The restriction of a tiling to row i is a partition of
 * [0, 2i] into segments.  A segment starting at an even position is an up
 * slice (U...U, odd length), one starting at an odd position is a down slice
 * (D...D, odd length); so the segments alternate U,D,U,...,U, all of odd
 * length.  We encode a row by its cut bits (bit p = 1 iff positions p, p+1
 * are in different segments).  The number of such rows is the number of
 * compositions of 2i+1 into odd parts, F(2i+1) (Fibonacci).
 *
 * Transition row i -> row i+1 (row i+1 has positions [0, 2i+2]):
 *  (a) each up slice [s,e] either ends, or continues as [s, e+2];
 *  (b) each down slice [s,e] with e > s continues as [s+2, e]; a down slice
 *      with e = s (one cell) ends;  down tiles cannot end earlier;
 *  (c) the remaining (free) cells of row i+1 are split into new tiles: each
 *      odd..odd segment of free cells (its even cells inside) is the top row
 *      of a new down tile, and every other free even cell is the apex of a
 *      new up tile (slice of length 1).  Free cells may thus form several
 *      new tiles; the even cell between two odd free cells is either the
 *      apex of a new up tile or an interior cell of a new down tile.
 *  A tiling is determined by its sequence of row slices (a continued up slice
 *  [s,e+2] has length >= 3, so it cannot be a new up tile; a down slice can
 *  never be "restarted"), and every admissible sequence ending in a row whose
 *  down slices all have length 1 is a tiling.  Hence
 *      a(n) = #(admissible sequences of rows 0..n-1, last row has all down
 *              slices of length 1),
 *  and in particular  a(n) <= B(n) := prod_{i=1}^{n-1} F(2i+1).
 *
 * In-place sweep.  Row i -> i+1 is done position by position q = 0..2i+2,
 * keeping a key = (new cut bits for positions < q-1) | (old cut bits for
 * positions >= q-1) and a small context:
 *    typ  : type of the old segment containing q-1 (U/D)
 *    cont : that old up segment continues
 *    doff : offset of q-1 inside an old down segment (0, 1, >=2)
 *    ext  : cells still to be covered by the 2-cell extension of the last
 *           continued up slice (0..2)
 *    prev : owner class of new cell q-1: A (continued up tile), B (shrunk old
 *           down tile), C (new up apex), NMID (new down tile, even interior
 *           cell: must continue), NODD (new down tile, odd cell: may end).
 * Owner of cell q of the new row:
 *    A  if q lies in a continued old up segment, or ext > 0;
 *    B  if q lies at offset >= 2 of an old down segment;
 *    free otherwise -> C or new-down as in (c).
 * An old up segment may continue only if ext = 0 at its first cell (else its
 * first cell is already taken by the previous extension: this is exactly the
 * case "down slice of length 1 between two continuing up slices").
 * Two different A tiles (resp. B tiles) are never adjacent, so the new cut
 * bit between q-1 and q is 0 iff (A,A), (B,B) or a new down tile continues.
 * The old cut bit q-1 is read and the new cut bit q-1 is written at step q,
 * so the key never needs more than 2i+2 bits.
 *
 * Numbers: fixed width W limbs of 64 bits.  Every intermediate value is a
 * sum, over row-i states s, of count(s) times the number of partial choice
 * paths from s; sum_s count(s) <= B(i+1) <= B(N), and at each of the
 * 2i+3 <= 2N+1 positions an entry has at most 3 successors (at the start of
 * an up segment: continue, or stop and then C / new-down interior), so every
 * value is < B(N) * 4^(2N+1).  W is chosen with
 *   64W >= sum_i ceil(log2 F(2i+1)) + 2(2N+1) + 66   (66 = generous margin);
 * in addition, any carry out of the top limb aborts the program.
 * ---------------------------------------------------------------------------
 *
 * Build:  gcc -O2 -std=c17 -Wall -Wextra -Wpedantic -o 358715_01 358715_01.c
 *         (option: -DMAXN=n   largest allowed N, default 27; at most 27)
 * Memory: about (#states of the widest step) * (8 + 8W) * 2..4 bytes;
 *         the number of row states is F(2i+1) (~2.618^i).
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
#include <unistd.h>

#ifndef MAXN
#define MAXN 27
#endif
#if MAXN < 1 || MAXN > 27
#error "MAXN must be in 1..27 (key = 2N-1 mask bits + 9 context bits < 63 bits)"
#endif

#define OUTFILE "b358715_01.txt"
#define LOCKFILE "b358715_01.txt.lock"
#define MAX_SAVED_BYTES (16u << 20)
#define MAX_DIGITS 1000
#define MAX_LIMBS 16

__extension__ typedef unsigned __int128 u128;

static void die(const char *msg) {
    fprintf(stderr, "358715_01: %s\n", msg);
    exit(1);
}
static void die_errno(const char *msg) {
    fprintf(stderr, "358715_01: %s: %s\n", msg, strerror(errno));
    exit(1);
}

/* ---------------- fixed-width big integers (W limbs) ---------------- */
static int W;                 /* chosen at run time, 1..MAX_LIMBS */
static int g_overflow = 0;

static inline void add_to(uint64_t *r, const uint64_t *a) {
    u128 c = 0;
    for (int k = 0; k < W; k++) {
        c += (u128)r[k] + a[k];
        r[k] = (uint64_t)c;
        c >>= 64;
    }
    if (c) g_overflow = 1;
}

static char *to_dec(const uint64_t *x) {
    uint64_t t[MAX_LIMBS];
    memcpy(t, x, sizeof(uint64_t) * (size_t)W);
    uint64_t ch[MAX_LIMBS * 2 + 2];
    int nch = 0;
    const uint64_t BASE = 10000000000000000000ULL;
    for (;;) {
        int zero = 1;
        for (int k = 0; k < W; k++) if (t[k]) { zero = 0; break; }
        if (zero) break;
        u128 rem = 0;
        for (int k = W - 1; k >= 0; k--) {
            u128 cur = (rem << 64) | t[k];
            t[k] = (uint64_t)(cur / BASE);
            rem = cur % BASE;
        }
        ch[nch++] = (uint64_t)rem;
    }
    char *s = malloc((size_t)nch * 19 + 2);
    if (!s) die("out of memory");
    if (nch == 0) { strcpy(s, "0"); return s; }
    int len = sprintf(s, "%llu", (unsigned long long)ch[nch - 1]);
    for (int i = nch - 2; i >= 0; i--) len += sprintf(s + len, "%019llu", (unsigned long long)ch[i]);
    return s;
}

/* ---------------- hash map: uint64 key -> W-limb value ---------------- */
#define EMPTY UINT64_MAX      /* keys are < 2^63, never equal to EMPTY */
typedef struct {
    size_t cap, cnt;          /* cap is a power of two, cnt <= cap/2 */
    uint64_t *keys;
    uint64_t *vals;           /* cap * W */
} Map;

static void map_init(Map *m, size_t cap) {
    m->cap = cap; m->cnt = 0;
    if (cap > SIZE_MAX / sizeof(uint64_t) / (size_t)(W + 1)) die("hash table too large");
    m->keys = malloc(cap * sizeof(uint64_t));
    m->vals = calloc(cap * (size_t)W, sizeof(uint64_t));
    if (!m->keys || !m->vals) die("out of memory (try a smaller N)");
    for (size_t i = 0; i < cap; i++) m->keys[i] = EMPTY;
}
static void map_free(Map *m) { free(m->keys); free(m->vals); m->keys = m->vals = NULL; m->cap = m->cnt = 0; }

static inline size_t hash64(uint64_t k) {
    k ^= k >> 33; k *= 0xff51afd7ed558ccdULL;
    k ^= k >> 33; k *= 0xc4ceb9fe1a85ec53ULL;
    k ^= k >> 33;
    return (size_t)k;
}
static void map_add(Map *m, uint64_t key, const uint64_t *val);
static void map_grow(Map *m) {
    Map n;
    if (m->cap > SIZE_MAX / 2) die("hash table too large");
    map_init(&n, m->cap * 2);
    for (size_t i = 0; i < m->cap; i++)
        if (m->keys[i] != EMPTY) map_add(&n, m->keys[i], m->vals + i * (size_t)W);
    map_free(m);
    *m = n;
}
static void map_add(Map *m, uint64_t key, const uint64_t *val) {
    if ((m->cnt + 1) * 2 > m->cap) map_grow(m);
    size_t mask = m->cap - 1, h = hash64(key) & mask;
    while (m->keys[h] != EMPTY && m->keys[h] != key) h = (h + 1) & mask;
    if (m->keys[h] == EMPTY) { m->keys[h] = key; m->cnt++; }
    add_to(m->vals + h * (size_t)W, val);
}
static void map_clear(Map *m) {
    for (size_t i = 0; i < m->cap; i++) m->keys[i] = EMPTY;
    memset(m->vals, 0, m->cap * (size_t)W * sizeof(uint64_t));
    m->cnt = 0;
}

/* ---------------- context packing ---------------- */
enum { U = 0, D = 1 };
enum { P_NONE = 0, P_A, P_B, P_C, P_NMID, P_NODD };
#define CTX_SHIFT 54          /* mask bits < 54 (2N-1 <= 53) */
static inline uint64_t pack(uint64_t mask, int typ, int cont, int doff, int ext, int prev) {
    uint64_t ctx = (uint64_t)typ | ((uint64_t)cont << 1) | ((uint64_t)doff << 2) |
                   ((uint64_t)ext << 4) | ((uint64_t)prev << 6);
    return mask | (ctx << CTX_SHIFT);
}

/* row i (cur holds keys = masks, ctx 0) -> row i+1 (result in cur) */
static size_t g_peak = 0;
static void row_step(Map *cur, Map *nxt, int i) {
    const int L = 2 * i + 1, M = L + 2;
    for (int q = 0; q < M; q++) {
        map_clear(nxt);
        for (size_t h = 0; h < cur->cap; h++) {
            uint64_t key = cur->keys[h];
            if (key == EMPTY) continue;
            const uint64_t *v = cur->vals + h * (size_t)W;
            uint64_t mask = key & ((1ULL << CTX_SHIFT) - 1);
            uint64_t ctx = key >> CTX_SHIFT;
            int typ = (int)(ctx & 1), cont = (int)((ctx >> 1) & 1), doff = (int)((ctx >> 2) & 3);
            int ext = (int)((ctx >> 4) & 3), prev = (int)((ctx >> 6) & 7);

            int newseg;
            if (q == 0) newseg = 1;
            else if (q < L) newseg = (int)((mask >> (q - 1)) & 1);
            else newseg = (q == L);
            if (newseg) {
                if (q > 0 && typ == U && cont) ext = 2;     /* continued up slice just ended */
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
                int c = cl[ci], e = ext, own;   /* own: 1 = A, 2 = B, 0 = free */
                if (q < L && typ == U && c) own = P_A;
                else if (e > 0) { own = P_A; e--; }
                else if (q < L && typ == D && doff >= 2) own = P_B;
                else own = 0;

                int no = 0, cuts[2], cls[2];
                if (own) {
                    if (prev == P_NMID) continue;           /* open new down tile blocked */
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
        if (nxt->cnt > g_peak) g_peak = nxt->cnt;
        Map t = *cur; *cur = *nxt; *nxt = t;
    }
    /* project to plain row states */
    map_clear(nxt);
    for (size_t h = 0; h < cur->cap; h++) {
        uint64_t key = cur->keys[h];
        if (key == EMPTY) continue;
        int prev = (int)((key >> (CTX_SHIFT + 6)) & 7);
        if (prev == P_NMID) continue;                 /* new down tile not closed */
        map_add(nxt, key & ((1ULL << CTX_SHIFT) - 1), cur->vals + h * (size_t)W);
    }
    Map t = *cur; *cur = *nxt; *nxt = t;
}

/* row with L cells: every down slice (segment starting at odd position) has length 1 */
static int final_ok(uint64_t mask, int L) {
    int s = 0;
    for (int p = 0; p < L; p++) {
        if (p == L - 1 || ((mask >> p) & 1)) {
            if ((s & 1) && p != s) return 0;
            s = p + 1;
        }
    }
    return 1;
}

/* ---------------- arguments ---------------- */
static void usage(FILE *f) {
    fprintf(f,
        "usage: 358715_01 N\n"
        "  computes a(1..N) (tilings of a side-n triangle by integer triangles)\n"
        "  and saves \"n a(n)\" lines to %s after each new term\n"
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
        fprintf(stderr, "358715_01: N must satisfy 1 <= N <= %d\n", MAXN);
        exit(1);
    }
    return (int)v;
}

/* ---------------- files ---------------- */
static void open_lock(void) {
    int fd = open(LOCKFILE, O_RDWR | O_CREAT | O_NOFOLLOW | O_CLOEXEC, 0644);
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
    int fd = open(OUTFILE, O_RDONLY | O_NOFOLLOW | O_CLOEXEC);
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
        if (!nl) {
            fprintf(stderr, "358715_01: note: ignoring incomplete last line of %s\n", OUTFILE);
            break;
        }
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

static void record(size_t n, const char *v) {
    if (n <= nvals) {
        if (strcmp(vals[n - 1], v) != 0) {
            fprintf(stderr, "358715_01: saved a(%zu) = %s but computed %s; file left unchanged\n",
                    n, vals[n - 1], v);
            exit(1);
        }
        if (printf("%zu %s (verified)\n", n, v) < 0) die("printf failed");
    } else {
        push_val(v);                 /* n == nvals here: terms come in order */
        write_all();
        if (printf("%zu %s\n", n, v) < 0) die("printf failed");
    }
    if (fflush(stdout) != 0) die("fflush stdout failed");
}

/* ---------------- main ---------------- */
int main(int argc, char **argv) {
    if (argc == 2 && (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h"))) { usage(stdout); return 0; }
    if (argc != 2) { usage(stderr); return 1; }
    const int N = parse_n(argv[1]);

    open_lock();
    read_saved();
    if (nvals > (size_t)N)
        fprintf(stderr, "358715_01: note: keeping saved lines beyond N=%d (not re-verified)\n", N);

    /* W from bits(B(N)) + bits(F(2N-1)) + (2N+1) + margin, using exact
       Fibonacci numbers (F(53) < 2^64) and log2 rounded up per factor. */
    {
        uint64_t F[64] = { 0, 1 };
        for (int k = 2; k < 64; k++) F[k] = F[k - 1] + F[k - 2];   /* F(63) < 2^64 */
        int bits = 0;
        for (int i = 1; i <= N - 1; i++) {                        /* ceil(log2 F(2i+1)) */
            uint64_t f = F[2 * i + 1]; int b = 0;
            while (b < 63 && (1ULL << b) < f) b++;
            if ((1ULL << b) < f) b = 64;
            bits += b;
        }
        bits += 2 * (2 * N + 1);  /* <= 3 < 2^2 successors per position */
        bits += 66;               /* margin */
        W = (bits + 63) / 64;
        if (W < 1) W = 1;
        if (W > MAX_LIMBS) die("internal: too many limbs");
    }

    Map cur, nxt;
    map_init(&cur, 1024);
    map_init(&nxt, 1024);
    uint64_t one[MAX_LIMBS] = { 1 };
    map_add(&cur, 0, one);                                  /* row 0: single up cell */
    record(1, "1");

    for (int i = 0; i + 1 < N; i++) {                       /* row i -> row i+1 */
        row_step(&cur, &nxt, i);
        if (g_overflow) die("overflow (internal bound violated)");
        const int L1 = 2 * (i + 1) + 1;
        uint64_t sum[MAX_LIMBS] = { 0 };
        for (size_t h = 0; h < cur.cap; h++)
            if (cur.keys[h] != EMPTY && final_ok(cur.keys[h], L1)) add_to(sum, cur.vals + h * (size_t)W);
        if (g_overflow) die("overflow (internal bound violated)");
        char *s = to_dec(sum);
        fprintf(stderr, "358715_01: row %d: %zu states, peak %zu keys\n", i + 1, cur.cnt, g_peak);
        record((size_t)i + 2, s);
        free(s);
    }
    map_free(&cur); map_free(&nxt);
    return 0;
}
