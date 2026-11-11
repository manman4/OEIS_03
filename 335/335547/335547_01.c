/*
 * A335547: tilings of an n-step staircase by equally oriented staircases.
 *
 * Build on macOS from this directory (GMP is required):
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic \
 *     $(pkg-config --cflags gmp) 335547_01.c -o 335547_01 \
 *     $(pkg-config --libs gmp) -framework Accelerate
 *
 *   ./335547_01 38              # extend ./b335547.txt through n=38
 *   ./335547_01 --no-bfile 38   # independently compute just a(38)
 *
 * Between adjacent cells in a row, put 0 inside a tile and 1 between tiles.
 * An adjacent pair 00, 01, 10, or 11 forces the bit immediately above it to
 * be 0, 1, 1, or free, respectively. The shrinking runs belonging to tiles
 * already placed are forced, and cuts partition the remaining vacant runs
 * into bases of new tiles. Thus the bits determine the tiling bijectively.
 *
 * The bit vertices are (x,y,z) >= 0 with x+y+z=n-2. A face has vertices
 * q+e_x, q+e_y, q+e_z with sum(q)=n-3 and forbids exactly one 1. Split
 * around a balanced center (u,v,w). The three regions satisfy x>=u,z<=w;
 * y>=v,x<=u; or z>=w,y<=v. A face belongs to exactly one: the three tests
 * on q, q_x>=u, q_y>=v, q_z>=w, include both truth values, so exactly one
 * cyclic pair goes from true to false. The regions share only their rays
 * and center. For a fixed center bit their counts give matrices A,B,C;
 * summing over the three ray masks gives trace(A B C).
 *
 * Each residue uses exact binary64 GEMM on nonnegative integers. The
 * shared dimension in AB is at most 2^13 and p < 2^20, hence every dot
 * product and every partial sum is an integer below 2^53. Do not compile
 * with unsafe floating-point options. GMP reconstructs the final integer.
 *
 * b335547.txt is the sole saved prefix. Each new term is written to a
 * temporary file, flushed, synced and renamed under a stable file lock.
 */
#define _POSIX_C_SOURCE 200809L
#define _DARWIN_C_SOURCE
#define ACCELERATE_NEW_LAPACK

#include <Accelerate/Accelerate.h>
#include <gmp.h>

#include <errno.h>
#include <fcntl.h>
#include <float.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>

enum {
    MAX_N = 43,
    PRIME_LIMIT = 1 << 20,
    MAX_INNER_BITS = (MAX_N - 2) / 3,
    BLOCK = 512,
};
static const char bfile_path[] = "b335547.txt";
static const char lock_path[] = "b335547.txt.lock";

_Static_assert(FLT_RADIX == 2 && DBL_MANT_DIG == 53, "binary64 required");
_Static_assert(MAX_INNER_BITS + 40 <= DBL_MANT_DIG, "GEMM must be exact");
_Static_assert(SIZE_MAX >= UINT64_MAX, "64-bit size_t required");

static void fail(const char *message)
{
    fprintf(stderr, "335547_01: %s\n", message);
    exit(EXIT_FAILURE);
}

static void fail_io(const char *operation)
{
    perror(operation);
    exit(EXIT_FAILURE);
}

static void *allocate(size_t bytes)
{
    void *memory = malloc(bytes);
    if (memory == NULL) {
        fprintf(stderr, "335547_01: cannot allocate %.1f MiB\n",
                (double)bytes / 1048576.0);
        exit(EXIT_FAILURE);
    }
    return memory;
}

static int min_int(int a, int b)
{
    return a < b ? a : b;
}

static int ray_size(int bits)
{
    return 1 << bits;
}

static size_t region_size(int rows, int cols)
{
    return (size_t)2 * (size_t)rows * (size_t)cols;
}

/*
 * In the region x>=u,z<=w, write a=x-u and b=w-z. Its vertices satisfy
 * 0<=b<=w and 0<=a<=v+b. Sweep a downward. The state stores the current
 * column at low bits (in increasing b order), then the already visited
 * b=0 ray bits. The final index is [a-ray mask][b-ray mask][center bit].
 */
static void fill_region(uint32_t *table, int v, int w, uint32_t prime)
{
    table[0] = 1;
    table[1] = 1;
    size_t count = 2;
    const size_t column_size = (size_t)ray_size(w + 1);

    for (int a = v + w - 1; a >= 0; --a) {
        /* Introduce the first bit of the next column, with either value.
         * Once b=0 exists, keep the old b=0 bit on the exposed ray. Walk
         * backwards so expanding the table in place preserves the source. */
        if (a >= v) {
            for (size_t k = count; k != 0;) {
                --k;
                const uint32_t value = table[k];
                table[2 * k] = value;
                table[2 * k + 1] = value;
            }
        } else {
            for (size_t k = count; k != 0;) {
                k -= column_size;
                for (size_t j = 0; j < column_size; j += 2) {
                    const uint32_t zero = table[k + j];
                    const uint32_t one = table[k + j + 1];
                    table[2 * k + j] = zero;
                    table[2 * k + j + 1] = zero;
                    table[2 * k + column_size + j] = one;
                    table[2 * k + column_size + j + 1] = one;
                }
            }
        }
        count *= 2;

        /* For the face at b, the preceding new bit at b-1 controls the
         * update. For 0 the old/new bits at b must agree. For 1 their pair
         * may be 01, 10 or 11: (old0,old1) becomes (old1,old0+old1). */
        const int low = a > v ? a - v : 0;
        for (int b = low + 1; b <= w; ++b) {
            const size_t half = (size_t)ray_size(b - low - 1);
            for (size_t k = 0; k < count; k += 4 * half) {
                uint32_t *zero = table + k + half;
                uint32_t *one = zero + 2 * half;
                for (size_t j = 0; j < half; ++j) {
                    const uint32_t both = zero[j] + one[j];
                    zero[j] = one[j];
                    one[j] = both >= prime ? both - prime : both;
                }
            }
        }
    }
}

typedef struct {
    int u, v, w;
    int du, dv, dw;
    uint32_t *a, *b, *c;
    double *panel_a, *panel_b, *product;
} Workspace;

static Workspace make_workspace(int n)
{
    /* Put the shortest ray in the contracted dimension of AB. */
    Workspace ws = {.u = (n - 1) / 3, .v = n / 3, .w = (n - 2) / 3};
    ws.du = ray_size(ws.u);
    ws.dv = ray_size(ws.v);
    ws.dw = ray_size(ws.w);

    const size_t a_count = region_size(ws.dv, ws.dw);
    const size_t b_count = region_size(ws.dw, ws.du);
    const size_t c_count = region_size(ws.du, ws.dv);
    const bool symmetric = ws.u == ws.w && ws.v == ws.w;
    const size_t rows = (size_t)min_int(BLOCK, ws.dv);
    const size_t cols = (size_t)min_int(BLOCK, ws.du);
    const size_t inner = (size_t)ws.dw;
    const size_t bytes = (a_count + (symmetric ? 0 : b_count + c_count))
                       * sizeof(uint32_t)
                       + ((rows + cols) * inner + rows * cols) * sizeof(double);
    fprintf(stderr, "n=%d: %.1f MiB for DP tables and GEMM blocks\n",
            n, (double)bytes / 1048576.0);

    ws.a = allocate(a_count * sizeof(*ws.a));
    ws.b = symmetric ? ws.a : allocate(b_count * sizeof(*ws.b));
    ws.c = symmetric ? ws.a : allocate(c_count * sizeof(*ws.c));
    ws.panel_a = allocate(rows * inner * sizeof(*ws.panel_a));
    ws.panel_b = allocate(cols * inner * sizeof(*ws.panel_b));
    ws.product = allocate(rows * cols * sizeof(*ws.product));
    return ws;
}

static void free_workspace(Workspace *ws)
{
    if (ws->b != ws->a) {
        free(ws->b);
        free(ws->c);
    }
    free(ws->a);
    free(ws->panel_a);
    free(ws->panel_b);
    free(ws->product);
}

/* Form tiles of (A B)^T and consume them directly against C. Neither a full
 * double-precision copy of a region nor a full product matrix is needed. */
static uint32_t residue(Workspace *ws, uint32_t prime)
{
    fill_region(ws->a, ws->v, ws->w, prime);
    if (ws->b != ws->a) {
        fill_region(ws->b, ws->w, ws->u, prime);
        fill_region(ws->c, ws->u, ws->v, prime);
    }

    const int du = ws->du, dv = ws->dv, dw = ws->dw;
    const size_t stride_a = (size_t)dw;
    const size_t stride_b = (size_t)du;
    const size_t stride_c = (size_t)dv;
    uint64_t total = 0;
    for (size_t center = 0; center < 2; ++center) {
        for (int j = 0; j < du; j += BLOCK) {
            const int cols = min_int(BLOCK, du - j);
            for (size_t k = 0; k < (size_t)dw; ++k) {
                const uint32_t *source = ws->b + 2 * (k * stride_b + (size_t)j)
                                        + center;
                double *panel = ws->panel_b + k * (size_t)cols;
                for (size_t col = 0; col < (size_t)cols; ++col)
                    panel[col] = source[2 * col];
            }
            for (int i = 0; i < dv; i += BLOCK) {
                const int rows = min_int(BLOCK, dv - i);
                for (size_t row = 0; row < (size_t)rows; ++row) {
                    const uint32_t *source = ws->a + 2 * (((size_t)i + row)
                                              * stride_a) + center;
                    double *panel = ws->panel_a + row * stride_a;
                    for (size_t k = 0; k < (size_t)dw; ++k)
                        panel[k] = source[2 * k];
                }
                cblas_dgemm(CblasRowMajor, CblasTrans, CblasTrans,
                            cols, rows, dw, 1.0, ws->panel_b, cols,
                            ws->panel_a, dw, 0.0, ws->product, rows);

                for (size_t col = 0; col < (size_t)cols; ++col) {
                    const uint32_t *last = ws->c + 2 * (((size_t)j + col)
                                              * stride_c + (size_t)i) + center;
                    const double *dot = ws->product + col * (size_t)rows;
                    uint64_t subtotal = 0;
                    for (size_t row = 0; row < (size_t)rows; ++row)
                        subtotal += ((uint64_t)dot[row] % prime) * last[2 * row];
                    total = (total + subtotal) % prime;
                }
            }
        }
    }
    return (uint32_t)total;
}

/* Color the faces q by q_x-q_y modulo 3. Faces sharing a vertex differ by
 * e_i-e_j, so each color class consists of vertex-disjoint faces. A class
 * of k faces replaces 2^(3k) choices of their bits by at most 5^k. */
static void count_bound(mpz_t bound, int n)
{
    const int m = n - 1;
    int classes[3] = {0};
    for (int x = 0; x < m - 1; ++x)
        for (int y = 0; y < m - 1 - x; ++y)
            ++classes[(x - y + 3 * m) % 3];

    int disjoint = classes[0];
    for (int i = 1; i < 3; ++i)
        if (classes[i] > disjoint) disjoint = classes[i];
    mpz_ui_pow_ui(bound, 5, (unsigned long)disjoint);
    mpz_mul_2exp(bound, bound, (mp_bitcnt_t)(m * (m + 1) / 2 - 3 * disjoint));
}

/* Trial division is cheap for 20-bit primes, and gives a definite test. */
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
    Workspace ws = make_workspace(n);
    mpz_t bound, modulus, prime_z, inverse;
    mpz_inits(bound, modulus, prime_z, inverse, NULL);
    count_bound(bound, n);
    mpz_set_ui(value, 0);
    mpz_set_ui(modulus, 1);
    uint32_t prime = PRIME_LIMIT;
    int used = 0;
    while (mpz_cmp(modulus, bound) <= 0) {
        prime = previous_prime(prime);
        const uint32_t r = residue(&ws, prime);
        const unsigned long old = mpz_fdiv_ui(value, prime);
        mpz_set_ui(prime_z, prime);
        if (mpz_invert(inverse, modulus, prime_z) == 0)
            fail("CRT moduli are not coprime");
        const uint64_t delta = (r + prime - old) % prime;
        const unsigned long factor = (unsigned long)
            ((delta * mpz_get_ui(inverse)) % prime);
        mpz_addmul_ui(value, modulus, factor);
        mpz_mul_ui(modulus, modulus, prime);
        ++used;
    }
    if (mpz_sgn(value) <= 0 || mpz_cmp(value, bound) > 0)
        fail("CRT result is outside the proved count bounds");
    prime = previous_prime(prime);
    if (residue(&ws, prime) != mpz_fdiv_ui(value, prime))
        fail("CRT result disagrees with the check prime");
    fprintf(stderr, "n=%d: reconstructed from %d primes and checked with one more\n",
            n, used);
    mpz_clears(bound, modulus, prime_z, inverse, NULL);
    free_workspace(&ws);
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

static FILE *open_saved(void)
{
    const int fd = open(bfile_path, O_RDONLY | O_NOFOLLOW | O_NONBLOCK);
    if (fd < 0) {
        if (errno == ENOENT) return NULL;
        fail_io(bfile_path);
    }
    struct stat info;
    if (fstat(fd, &info) != 0) fail_io("stat b-file");
    if (!S_ISREG(info.st_mode) || info.st_nlink != 1)
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

static FILE *save_term(FILE *input, int directory, int n, const mpz_t value)
{
    struct stat info;
    if (input != NULL && fstat(fileno(input), &info) != 0)
        fail_io("stat previous b-file");
    char temporary[] = "b335547.txt.tmp.XXXXXX";
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
    if ((input != NULL && fchmod(fd, info.st_mode & 0777) != 0)
        || !copy_saved(output, input) || gmp_fprintf(output, "%d %Zd\n", n, value) < 0
        || fflush(output) != 0 || fsync(fd) != 0 || rename(temporary, bfile_path) != 0) {
        const int error = errno;
        fclose(output);
        unlink(temporary);
        errno = error;
        fail_io("save b-file");
    }
    if (fsync(directory) != 0) fail_io("sync b-file directory");
    if (input != NULL && fclose(input) != 0) fail_io("close previous b-file");
    return output;
}

static void usage(FILE *stream, const char *program)
{
    fprintf(stream, "Usage: %s N | %s --no-bfile N\n"
            "  1 <= N <= %d\n"
            "  N: extend ./b335547.txt through N and print newly computed terms.\n"
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
    int lock = -1;
    int directory = -1;
    int first = n;
    if (!single) {
        lock = lock_output();
        saved = open_saved();
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
        if (!single) saved = save_term(saved, directory, k, value);
        if (gmp_printf("%d %Zd\n", k, value) < 0 || fflush(stdout) != 0)
            fail_io("write result");
    }
    mpz_clear(value);
    if (saved != NULL && fclose(saved) != 0) fail_io("close b-file");
    if (directory >= 0 && close(directory) != 0) fail_io("close b-file directory");
    if (lock >= 0 && close(lock) != 0) fail_io("close b-file lock");
    return EXIT_SUCCESS;
}
