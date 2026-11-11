/*
 * A335547. Tile a staircase with staircases of the same orientation.
 *
 * Build on macOS in the directory containing this file (requires GMP):
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic \
 *     $(pkg-config --cflags gmp) 335547_02.c -o 335547_02 \
 *     $(pkg-config --libs gmp) -framework Accelerate
 *
 *   ./335547_02 43             # extend ./b335547_2.txt through n=43
 *   ./335547_02 --no-bfile 43  # compute only a(43)
 *
 * As in Rémy Sigrist's PARI row recurrence, a row of tiles shrinks one
 * cell per run and the remaining cells can be partitioned into new runs.
 * Put a 1 at each cut between tiles and a 0 within a tile. The adjacent
 * pairs 00, 01, 10 and 11 in one row permit 0, 1, 1 and either bit in the
 * next row, respectively. The local rule forbids exactly one 1 in each
 * triple (i,j), (i+1,j), (i,j+1) on the grid i,j>=0, i+j<=n-2.
 *
 * Cut at i=cut and j=cut. Each triple belongs to one of the four regions,
 * since each coordinate ranges over at most two consecutive integers.
 * The cuts share only their center. Fix its bit c and order each ray from
 * the center outward. Let D_c be the rectangle with the two inner rays
 * as indices, E_c the table for either trapezoid (inner ray, outer ray),
 * and F_c the triangle with the two outer rays as indices. The sum is
 *   sum_c sum_{l,h} D_c(l,h) (E_c F_c E_c^T)(l,h).
 * Reflection i<->j interchanges the trapezoids and makes D_c and F_c
 * symmetric. Thus the off-diagonal blocks of this sum occur in equal
 * pairs. Generate D_c by fixing the far-end ray bits of each row block.
 *
 * All tables are residues below 2^16. Each GEMM inner dimension is at
 * most 2^13, so products and nonnegative partial sums are below 2^45
 * and are exactly representable in binary64. Build without fast-math.
 * GMP reconstructs each integer by CRT before it is written to the b-file.
 */
#define _POSIX_C_SOURCE 200809L
#define _DARWIN_C_SOURCE
#define ACCELERATE_NEW_LAPACK

#include <errno.h>
#include <fcntl.h>
#include <float.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>

#include <gmp.h>
#include <vecLib/cblas_new.h>

enum {
    MAX_N = 45,
    MAX_CUT = 15,
    MAX_DEPTH = MAX_N - 2 - 2 * MAX_CUT,
    BLOCK_BITS = 8,
    BLOCK = 1 << BLOCK_BITS,
    BOUND_SIDE = 12,
    BOUND_DIAGONAL = 2 * (BOUND_SIDE - 1),
    PRIME_LIMIT = 1 << 16,
    PRIME_CAPACITY = MAX_N * (MAX_N - 1) / 30 + 2,
};

__extension__ typedef unsigned __int128 BoundWord;
typedef uint16_t Residue;

_Static_assert(FLT_RADIX == 2 && DBL_MANT_DIG == 53, "binary64 required");
_Static_assert(MAX_DEPTH + 32 <= DBL_MANT_DIG, "GEMM sums must be exact");
_Static_assert(SIZE_MAX >= UINT64_MAX && INT_MAX >= INT32_MAX, "64-bit target required");

static const char bfile_path[] = "b335547_2.txt";
static const char lock_path[] = "b335547_2.txt.lock";

_Noreturn static void fail(const char *message)
{
    fprintf(stderr, "335547_02: %s\n", message);
    exit(EXIT_FAILURE);
}

_Noreturn static void fail_io(const char *operation)
{
    perror(operation);
    exit(EXIT_FAILURE);
}

static void *allocate(size_t bytes)
{
    void *memory = malloc(bytes);
    if (memory == NULL) {
        fprintf(stderr, "335547_02: cannot allocate %.1f MiB\n",
                (double)bytes / 1048576.0);
        exit(EXIT_FAILURE);
    }
    return memory;
}

static int min_int(int a, int b)
{
    return a < b ? a : b;
}

static size_t states(int bits)
{
    if ((unsigned)bits >= sizeof(size_t) * CHAR_BIT)
        fail("invalid state bit count");
    return (size_t)1 << bits;
}

/* Insert a free bit and retain all existing bits. The high-to-low copy
 * visits any source block that could be overwritten before the write. */
static void insert_free_bit(Residue *table, size_t count, int bit)
{
    const size_t step = states(bit);
    for (size_t from = count; from != 0;) {
        from -= step;
        memcpy(table + 2 * from + step, table + from, step * sizeof(*table));
        if (from != 0)
            memcpy(table + 2 * from, table + from, step * sizeof(*table));
    }
}

/* Replace the bit at position j by the new bit, summing out the old one.
 * The higher bit j+1 is known. For 0 the old and new bits must agree,
 * and for 1 the allowed pairs are 01, 10 and 11. */
static void transfer_bit(Residue *table, size_t count, int j, uint32_t prime)
{
    const size_t half = states(j);
    for (size_t base = 0; base < count; base += 4 * half) {
        Residue *zero = table + base + 2 * half;
        Residue *one = zero + half;
        for (size_t k = 0; k < half; ++k) {
            const uint32_t sum = (uint32_t)zero[k] + one[k];
            zero[k] = one[k];
            one[k] = (Residue)(sum >= prime ? sum - prime : sum);
        }
    }
}

/* Sweep the trapezoid from its tip toward the cut. The low bits are the
 * current row. When its length reaches cut+1, retain the last bit of each
 * visited row as an outer ray bit. The final order is [outer][c][inner],
 * with the inner ray indexed from its far end toward the center. */
static void fill_trapezoid(Residue *table, int depth, int cut, uint32_t prime)
{
    table[0] = 1;
    table[1] = 1;
    size_t count = 2;
    for (int old = 1; old <= depth + cut; ++old) {
        const int bit = min_int(old, cut);
        insert_free_bit(table, count, bit);
        count *= 2;
        for (int j = bit - 1; j >= 0; --j)
            transfer_bit(table, count, j, prime);
    }
}

/* Sweep the small triangle from the tip. Keep the first bit of every
 * visited row as a ray bit, even though its face still has to be tested.
 * Final indices are [first outer ray][second outer ray][center bit]. */
static void fill_triangle(Residue *table, int depth, uint32_t prime)
{
    table[0] = 1;
    table[1] = 1;
    size_t count = 2;
    for (int old = 1; old <= depth; ++old) {
        insert_free_bit(table, count, old);
        count *= 2;
        for (int j = old - 1; j >= 1; --j)
            transfer_bit(table, count, j, prime);

        const size_t column = states(old + 1);
        for (size_t base = count; base != 0;) {
            base -= column;
            for (size_t j = 0; j < column; j += 2) {
                const Residue old_zero = table[base + j];
                const Residue old_one = table[base + j + 1];
                const bool neighbor = (j & 2) != 0;
                table[2 * base + j] = neighbor ? 0 : old_zero;
                table[2 * base + j + 1] = neighbor ? old_zero : 0;
                table[2 * base + column + j] = neighbor ? old_one : 0;
                table[2 * base + column + j + 1] = old_one;
            }
        }
        count *= 2;
    }
}

/* Sweep across the rectangle. Expose its left ray in far-to-near order.
 * Hold the first 'fixed' of those bits at prefix and retain the rest.
 * Final indices are [free left ray bits][c][other ray from the far end]. */
static void fill_rectangle(Residue *table, int cut, int fixed, size_t prefix,
                           uint32_t prime)
{
    const size_t half = states(cut);
    size_t count = 2 * half;
    for (size_t j = 0; j < count; ++j) {
        const bool permitted = fixed == 0 || (j >= half) == (prefix >> (fixed - 1));
        table[j] = permitted ? 1 : 0;
    }
    for (int i = 0; i < cut; ++i) {
        for (int j = 0; j < cut; ++j)
            transfer_bit(table, count, j, prime);
        if (i < fixed) {
            const size_t source = ((prefix >> (fixed - 1 - i)) & 1u) * half;
            const bool next_fixed = i + 1 < fixed;
            const size_t next_bit = next_fixed ? (prefix >> (fixed - 2 - i)) & 1u : 0;
            for (size_t j = 0; j < half; ++j) {
                const Residue value = table[source + j];
                table[j] = !next_fixed || next_bit == 0 ? value : 0;
                table[half + j] = !next_fixed || next_bit == 1 ? value : 0;
            }
        } else {
            insert_free_bit(table, count, cut);
            count *= 2;
        }
    }
}

typedef struct {
    int cut, depth, inner_size, outer_size, slice_size, fixed;
    int *reverse_mask;
    Residue *trapezoid, *folded, *scratch;
    double *panel_a, *panel_b, *product;
} Workspace;

static Workspace make_workspace(int n, bool report_memory)
{
    Workspace ws = {0};
    ws.cut = min_int(MAX_CUT, min_int((n + 4) / 3, (n - 2) / 2));
    ws.depth = n - 2 - 2 * ws.cut;
    if (ws.depth < 0 || ws.depth > MAX_DEPTH)
        fail("cut dimensions are outside the supported range");
    ws.inner_size = (int)states(ws.cut);
    ws.outer_size = (int)states(ws.depth);
    ws.slice_size = min_int(ws.inner_size, BLOCK);
    ws.fixed = ws.cut - min_int(ws.cut, BLOCK_BITS);

    const size_t inner = (size_t)ws.inner_size;
    const size_t outer = (size_t)ws.outer_size;
    const size_t side_count = 2 * inner * outer;
    const size_t triangle_count = 2 * outer * outer;
    const size_t rectangle_count = 2 * (size_t)ws.slice_size * inner;
    const size_t scratch_count = triangle_count > rectangle_count
                               ? triangle_count : rectangle_count;
    const size_t panel_count = BLOCK * outer;
    const size_t bytes = (2 * side_count + scratch_count)
                         * sizeof(Residue) + inner * sizeof(int)
                         + (2 * panel_count + BLOCK * BLOCK) * sizeof(double);
    if (report_memory)
        fprintf(stderr, "n=%d: %.1f MiB for tables and GEMM panels\n",
                n, (double)bytes / 1048576.0);

    ws.reverse_mask = allocate(inner * sizeof(*ws.reverse_mask));
    ws.reverse_mask[0] = 0;
    for (int i = 1; i < ws.inner_size; ++i)
        ws.reverse_mask[i] = (ws.reverse_mask[i >> 1] >> 1)
                            | ((i & 1) << (ws.cut - 1));
    ws.trapezoid = allocate(side_count * sizeof(*ws.trapezoid));
    ws.folded = allocate(side_count * sizeof(*ws.folded));
    ws.scratch = allocate(scratch_count * sizeof(*ws.scratch));
    ws.panel_a = allocate(panel_count * sizeof(*ws.panel_a));
    ws.panel_b = allocate(panel_count * sizeof(*ws.panel_b));
    ws.product = allocate((size_t)BLOCK * BLOCK * sizeof(*ws.product));
    return ws;
}

static void free_workspace(Workspace *ws)
{
    free(ws->reverse_mask);
    free(ws->trapezoid);
    free(ws->folded);
    free(ws->scratch);
    free(ws->panel_a);
    free(ws->panel_b);
    free(ws->product);
}

/* Pack one trapezoid panel as [outer][inner], reordering the inner ray
 * into the common center-to-end order used in the matrix products. */
static void pack_trapezoid(Workspace *ws, size_t center, int first, int rows)
{
    const size_t inner = (size_t)ws->inner_size;
    for (size_t k = 0; k < (size_t)ws->outer_size; ++k) {
        const Residue *source = ws->trapezoid + (2 * k + center) * inner;
        double *panel = ws->panel_a + k * (size_t)rows;
        for (int row = 0; row < rows; ++row)
            panel[row] = source[ws->reverse_mask[first + row]];
    }
}

/* Fold the triangle into the trapezoid: P_c = E_c F_c. */
static void fold_triangle(Workspace *ws, uint32_t prime)
{
    const size_t inner = (size_t)ws->inner_size;
    const size_t outer = (size_t)ws->outer_size;
    for (size_t center = 0; center < 2; ++center) {
        for (int j = 0; j < ws->outer_size; j += BLOCK) {
            const int cols = min_int(BLOCK, ws->outer_size - j);
            for (size_t k = 0; k < outer; ++k) {
                const Residue *source = ws->scratch + 2 * (k * outer + (size_t)j) + center;
                double *panel = ws->panel_b + k * (size_t)cols;
                for (int col = 0; col < cols; ++col)
                    panel[col] = source[2 * col];
            }
            for (int i = 0; i < ws->inner_size; i += BLOCK) {
                const int rows = min_int(BLOCK, ws->inner_size - i);
                pack_trapezoid(ws, center, i, rows);
                cblas_dgemm(CblasRowMajor, CblasTrans, CblasNoTrans,
                            rows, cols, ws->outer_size, 1.0,
                            ws->panel_a, rows, ws->panel_b, cols,
                            0.0, ws->product, cols);
                for (size_t row = 0; row < (size_t)rows; ++row) {
                    Residue *target = ws->folded + (center * inner + (size_t)i + row)
                                      * outer + (size_t)j;
                    const double *product = ws->product + row * (size_t)cols;
                    for (int col = 0; col < cols; ++col)
                        target[col] = (Residue)((uint64_t)product[col] % prime);
                }
            }
        }
    }
}

static uint32_t compute_residue(Workspace *ws, uint32_t prime)
{
    fill_trapezoid(ws->trapezoid, ws->depth, ws->cut, prime);
    fill_triangle(ws->scratch, ws->depth, prime);
    fold_triangle(ws, prime);

    const size_t inner = (size_t)ws->inner_size;
    const size_t outer = (size_t)ws->outer_size;
    const int slice = ws->slice_size;
    uint64_t total = 0;
    for (int i = 0; i < ws->inner_size; i += slice) {
        fill_rectangle(ws->scratch, ws->cut, ws->fixed, (size_t)(i / slice), prime);
        for (size_t center = 0; center < 2; ++center) {
            pack_trapezoid(ws, center, i, slice);
            for (int j = i; j < ws->inner_size; j += slice) {
                for (size_t col = 0; col < (size_t)slice; ++col) {
                    const Residue *source = ws->folded + (center * inner + (size_t)j + col) * outer;
                    double *panel = ws->panel_b + col * outer;
                    for (size_t k = 0; k < outer; ++k)
                        panel[k] = source[k];
                }
                cblas_dgemm(CblasRowMajor, CblasTrans, CblasTrans,
                            slice, slice, ws->outer_size, 1.0,
                            ws->panel_a, slice, ws->panel_b, ws->outer_size,
                            0.0, ws->product, slice);
                const uint64_t multiplicity = i == j ? 1 : 2;
                for (size_t row = 0; row < (size_t)slice; ++row) {
                    const Residue *rect = ws->scratch + (2 * row + center) * inner;
                    const double *product = ws->product + row * (size_t)slice;
                    uint64_t subtotal = 0;
                    for (int col = 0; col < slice; ++col)
                        subtotal += ((uint64_t)product[col] % prime)
                                    * rect[ws->reverse_mask[j + col]];
                    total = (total + multiplicity * subtotal) % prime;
                }
            }
        }
    }
    return (uint32_t)total;
}

/* Count a clipped square exactly using entire row masks. For a full 12x12
 * square, one of the three colors (i-j mod 3) contains at least 41 of its
 * 121 faces, and faces of a color share no vertices. Thus its count is
 * at most 5^41 * 2^21 < 2^117. Any valid clipped square extends to a valid
 * full square by setting the discarded vertices to 1. At a crossing face,
 * both forward neighbors lie outside, so this extension introduces no
 * forbidden triple. Every DP count also fits because a valid assignment
 * of the first rows extends one row at a time. Every pair has at least
 * one allowed output and any extra bit is free.
 */
static BoundWord clipped_square_count(int diagonal)
{
    BoundWord rows[2][1u << BOUND_SIDE];
    BoundWord *prev = rows[0], *next = rows[1];
    const int height = min_int(BOUND_SIDE, diagonal + 1);
    int width = height;
    for (size_t x = 0; x < states(width); ++x)
        prev[x] = 1;
    for (int row = 1; row < height; ++row) {
        const int new_width = min_int(BOUND_SIDE, diagonal - row + 1);
        const size_t pair_mask = states(min_int(width - 1, new_width)) - 1;
        const size_t extra = new_width == width ? states(new_width - 1) : 0;
        memset(next, 0, states(new_width) * sizeof(*next));
        for (size_t x = 0; x < states(width); ++x) {
            const size_t forced = (x ^ (x >> 1)) & pair_mask;
            const size_t free_bits = ((x & (x >> 1)) & pair_mask) | extra;
            for (size_t subset = free_bits;; subset = (subset - 1) & free_bits) {
                next[forced | subset] += prev[x];
                if (subset == 0) break;
            }
        }
        BoundWord *temp = prev;
        prev = next;
        next = temp;
        width = new_width;
    }
    BoundWord count = 0;
    for (size_t x = 0; x < states(width); ++x)
        count += prev[x];
    return count;
}

/* Ignore faces crossing disjoint 12x12 grid blocks. A global assignment
 * then gives one allowed assignment per clipped block. Their product is
 * an upper bound, computed independently of any saved sequence values. */
static void count_bound(mpz_t bound, int n)
{
    static BoundWord cached[BOUND_DIAGONAL + 1];
    mpz_t factor;
    mpz_init(factor);
    mpz_set_ui(bound, 1);
    for (int i = 0; i <= n - 2; i += BOUND_SIDE) {
        for (int j = 0; i + j <= n - 2; j += BOUND_SIDE) {
            const int diagonal = min_int(BOUND_DIAGONAL, n - 2 - i - j);
            if (cached[diagonal] == 0)
                cached[diagonal] = clipped_square_count(diagonal);
            mpz_import(factor, 1, 1, sizeof(cached[diagonal]), 0, 0, &cached[diagonal]);
            mpz_mul(bound, bound, factor);
        }
    }
    mpz_clear(factor);
}

static uint32_t previous_prime(uint32_t limit)
{
    for (uint32_t p = (limit - 2) | 1u; p > PRIME_LIMIT / 2; p -= 2) {
        uint32_t divisor = 3;
        while (divisor * divisor <= p && p % divisor != 0)
            divisor += 2;
        if (divisor * divisor > p) return p;
    }
    fail("ran out of reconstruction primes");
    return 0;
}

static void compute_term(mpz_t value, int n)
{
    if (n == 1) {
        mpz_set_ui(value, 1);
        return;
    }
    mpz_t bound, modulus, prime_z, inverse;
    mpz_inits(bound, modulus, prime_z, inverse, NULL);
    count_bound(bound, n);

    /* Each prime exceeds 2^15 and the entire bit grid has at most
     * MAX_N*(MAX_N-1)/2 vertices, so PRIME_CAPACITY includes the check. */
    uint32_t primes[PRIME_CAPACITY];
    int used = 0;
    uint32_t prime = PRIME_LIMIT;
    mpz_set_ui(modulus, 1);
    while (mpz_cmp(modulus, bound) <= 0) {
        prime = previous_prime(prime);
        primes[used++] = prime;
        mpz_mul_ui(modulus, modulus, prime);
    }

    Workspace ws = make_workspace(n, true);
    mpz_set_ui(value, 0);
    mpz_set_ui(modulus, 1);
    for (int i = 0; i < used; ++i) {
        prime = primes[i];
        fprintf(stderr, "n=%d: prime %d/%d (%u)\n", n, i + 1, used + 1, prime);
        const uint32_t residue = compute_residue(&ws, prime);
        const unsigned long old = mpz_fdiv_ui(value, prime);
        mpz_set_ui(prime_z, prime);
        if (mpz_invert(inverse, modulus, prime_z) == 0)
            fail("CRT moduli are not coprime");
        const uint64_t delta = (residue + prime - old) % prime;
        const unsigned long factor = (unsigned long)
            ((delta * mpz_get_ui(inverse)) % prime);
        mpz_addmul_ui(value, modulus, factor);
        mpz_mul_ui(modulus, modulus, prime);
    }
    if (mpz_sgn(value) <= 0 || mpz_cmp(value, bound) > 0)
        fail("CRT result is outside the proved count bounds");
    prime = previous_prime(prime);
    fprintf(stderr, "n=%d: check prime %d/%d (%u)\n", n, used + 1, used + 1, prime);
    if (compute_residue(&ws, prime) != mpz_fdiv_ui(value, prime))
        fail("CRT result disagrees with the check prime");
    free_workspace(&ws);
    mpz_clears(bound, modulus, prime_z, inverse, NULL);
}

static int lock_output(void)
{
    const int fd = open(lock_path, O_RDWR | O_CREAT | O_NOFOLLOW | O_NONBLOCK, 0644);
    if (fd < 0) fail_io(lock_path);
    struct stat info;
    if (fstat(fd, &info) != 0) fail_io("stat b-file lock");
    if (!S_ISREG(info.st_mode) || info.st_nlink != 1)
        fail("b-file lock must be a regular file with one name");
    if (flock(fd, LOCK_EX | LOCK_NB) != 0) {
        if (errno == EWOULDBLOCK) fail("another process holds the b-file lock");
        fail_io("lock b-file");
    }
    return fd;
}

static FILE *open_saved(struct stat *snapshot)
{
    const int fd = open(bfile_path, O_RDONLY | O_NOFOLLOW | O_NONBLOCK);
    if (fd < 0) {
        if (errno == ENOENT) return NULL;
        fail_io(bfile_path);
    }
    if (fstat(fd, snapshot) != 0) fail_io("stat b-file");
    if (!S_ISREG(snapshot->st_mode) || snapshot->st_nlink != 1)
        fail("b-file must be a regular file with one name");
    FILE *input = fdopen(fd, "r");
    if (input == NULL) fail_io("open b-file stream");
    return input;
}

/* The existing prefix must contain contiguous indices starting at 1.
 * Preserve comments and whitespace by copying the original bytes on save. */
static int validate_saved(FILE *input)
{
    if (input == NULL) return 0;

    int n = 0;
    mpz_t value, bound;
    mpz_inits(value, bound, NULL);
    char line[1024];
    const char blanks[] = " \t\r\n";
    const char digits[] = "0123456789";
    while (fgets(line, sizeof(line), input) != NULL) {
        if (strchr(line, '\n') == NULL)
            fail("b-file contains an incomplete or oversized line");
        char *index = line + strspn(line, blanks);
        if (*index == '\0' || *index == '#') continue;
        char *sep = index + strspn(index, digits);
        if (sep == index || (*sep != ' ' && *sep != '\t'))
            fail("b-file contains a malformed index");
        *sep++ = '\0';
        char *end;
        errno = 0;
        const long saved_n = strtol(index, &end, 10);
        if (errno != 0 || *end != '\0' || saved_n != n + 1 || saved_n > MAX_N)
            fail("b-file indices are out of range or are not contiguous");
        char *number = sep + strspn(sep, blanks);
        end = number + strspn(number, digits);
        if (end == number || end[strspn(end, blanks)] != '\0')
            fail("b-file contains a malformed value");
        *end = '\0';
        if (mpz_set_str(value, number, 10) != 0 || mpz_sgn(value) <= 0)
            fail("b-file values must be positive integers");
        count_bound(bound, (int)saved_n);
        if (mpz_cmp(value, bound) > 0)
            fail("b-file value exceeds the proved count bound");
        n = (int)saved_n;
    }
    if (ferror(input)) fail_io("read b-file");
    mpz_clears(value, bound, NULL);
    return n;
}

static bool copy_saved(FILE *output, FILE *input)
{
    if (input == NULL) return true;
    if (fseek(input, 0, SEEK_SET) != 0) return false;
    char data[4096];
    size_t count;
    while ((count = fread(data, 1, sizeof(data), input)) != 0)
        if (fwrite(data, 1, count, output) != count) return false;
    return !ferror(input);
}

static bool saved_is_current(FILE *input, const struct stat *snapshot)
{
    struct stat current;
    if (lstat(bfile_path, &current) != 0)
        return input == NULL && errno == ENOENT;
    return input != NULL && current.st_nlink == 1
           && current.st_dev == snapshot->st_dev
           && current.st_ino == snapshot->st_ino
           && current.st_size == snapshot->st_size
           && current.st_mtimespec.tv_sec == snapshot->st_mtimespec.tv_sec
           && current.st_mtimespec.tv_nsec == snapshot->st_mtimespec.tv_nsec
           && current.st_ctimespec.tv_sec == snapshot->st_ctimespec.tv_sec
           && current.st_ctimespec.tv_nsec == snapshot->st_ctimespec.tv_nsec;
}

static FILE *save_term(FILE *input, struct stat *snapshot, int directory, int n,
                       const mpz_t value)
{
    if (!saved_is_current(input, snapshot))
        fail("b-file changed during computation");
    char temporary[] = "b335547_2.txt.tmp.XXXXXX";
    const int fd = mkstemp(temporary);
    if (fd < 0) fail_io("create b-file temporary");
    FILE *output = fdopen(fd, "w+");
    if (output == NULL) {
        const int error = errno;
        close(fd);
        unlink(temporary);
        errno = error;
        fail_io("open b-file temporary");
    }
    if ((input != NULL && fchmod(fd, snapshot->st_mode & 0777) != 0)
        || !copy_saved(output, input) || gmp_fprintf(output, "%d %Zd\n", n, value) < 0
        || fflush(output) != 0 || fsync(fd) != 0) {
        const int error = errno;
        fclose(output);
        unlink(temporary);
        errno = error;
        fail_io("save b-file");
    }
    if (!saved_is_current(input, snapshot)) {
        fclose(output);
        unlink(temporary);
        fail("b-file changed before saving");
    }
    if (rename(temporary, bfile_path) != 0) {
        const int error = errno;
        fclose(output);
        unlink(temporary);
        errno = error;
        fail_io("replace b-file");
    }
    if (fsync(directory) != 0) fail_io("sync b-file directory");
    if (fstat(fd, snapshot) != 0) fail_io("stat updated b-file");
    if (input != NULL && fclose(input) != 0) fail_io("close previous b-file");
    return output;
}

static void usage(FILE *stream, const char *program)
{
    fprintf(stream, "Usage: %s N | %s --no-bfile N\n"
            "  1 <= N <= %d\n"
            "  N: extend ./b335547_2.txt through N and print newly computed terms.\n"
            "  --no-bfile N: compute and print only a(N), without opening the b-file.\n",
            program, program, MAX_N);
}

static int parse_n(const char *argument)
{
    errno = 0;
    char *end;
    const long n = strtol(argument, &end, 10);
    if (errno != 0 || end == argument || *end != '\0' || n < 1 || n > MAX_N)
        return 0;
    return (int)n;
}

int main(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        usage(stdout, argv[0]);
        return EXIT_SUCCESS;
    }
    const bool single = argc == 3 && strcmp(argv[1], "--no-bfile") == 0;
    const int n = argc == 2 || single ? parse_n(argv[argc - 1]) : 0;
    if (n == 0) {
        usage(stderr, argv[0]);
        return EXIT_FAILURE;
    }

    FILE *saved = NULL;
    struct stat snapshot;
    int lock = -1;
    int directory = -1;
    int first = n;
    if (!single) {
        lock = lock_output();
        saved = open_saved(&snapshot);
        first = validate_saved(saved) + 1;
        if (first > n)
            fprintf(stderr, "%s already contains terms through n=%d\n",
                    bfile_path, first - 1);
        else if ((directory = open(".", O_RDONLY)) < 0)
            fail_io("open b-file directory");
    }

    mpz_t value;
    mpz_init(value);
    for (int k = first; k <= n; ++k) {
        compute_term(value, k);
        if (!single) saved = save_term(saved, &snapshot, directory, k, value);
        if (gmp_printf("%d %Zd\n", k, value) < 0 || fflush(stdout) != 0)
            fail_io("write result");
    }
    mpz_clear(value);
    if (saved != NULL && fclose(saved) != 0) fail_io("close b-file");
    if (directory >= 0 && close(directory) != 0) fail_io("close b-file directory");
    if (lock >= 0 && close(lock) != 0) fail_io("close b-file lock");
    return EXIT_SUCCESS;
}
