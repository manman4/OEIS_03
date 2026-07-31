/*
 * A392408 -- second fast exact implementation.
 *
 * Algorithm 2:
 *   parity decomposition + column-oriented rook DP + CRT.
 *
 * This uses the same proven parity formula as 392408_01.c, but obtains the
 * rook numbers by a separate recurrence and data orientation.  Algorithm 1
 * processes odd rows and masks even columns.  This program processes even
 * columns and masks odd rows.  It computes the full board B and the board
 * with odd row 1 deleted in two separate DP passes.
 *
 * Let R_k be the k-rook number of B and U_k the k-rook number after deleting
 * odd row 1.  With o=ceil(n/2) and e=floor(n/2),
 *
 *   a(n) = sum_{k=0}^e (e-k)! *
 *          ((o-k)! R_k^2 - (o-k-1)! U_k^2),
 *
 * with the second term omitted for k=o.
 *
 * CRT reconstructs R_k and U_k using the rigorous bound
 *
 *   R_k <= binomial(o,k) binomial(e,k) k!.
 *
 * At most 2 GiB of DP storage is active across OpenMP workers.  Large cases
 * use uint8_t residues and one-byte primes, so n=60 needs 1 GiB per worker
 * and at most two concurrent workers on the target 8-GB Mac.
 *
 * The output b392408_1.txt is placed beside the executable, regardless of
 * the current working directory.
 */

#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <gmp.h>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#ifdef _OPENMP
#include <omp.h>
#endif

#define DEFAULT_MAX_N 24
#define MAX_SUPPORTED_N 60
#define KNOWN_MAX_N 24
#define DP_MEMORY_BUDGET (UINT64_C(2) << 30)
#define MAX_CRT_PRIMES 64
#ifndef COLUMN_BYTE_THRESHOLD
#define COLUMN_BYTE_THRESHOLD 29
#endif

static const char *const known_terms[KNOWN_MAX_N + 1] = {
    "1",
    "0",
    "0",
    "1",
    "2",
    "9",
    "32",
    "189",
    "1368",
    "8210",
    "44148",
    "389912",
    "3101440",
    "35058904",
    "443288708",
    "5108994142",
    "55028376940",
    "780766798385",
    "11860152133970",
    "196801719385449",
    "3515777759388072",
    "58688432620407329",
    "933262343169946392",
    "18278243997627725158",
    "344317636096990851008"
};

static void fail(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static int read_limit(const char *text)
{
    char *end = NULL;
    long value;

    errno = 0;
    value = strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' ||
        value < 0 || value > MAX_SUPPORTED_N) {
        fprintf(stderr, "error: N must be in 0..%d: %s\n",
                MAX_SUPPORTED_N, text);
        exit(EXIT_FAILURE);
    }
    return (int)value;
}

static char *make_output_path(const char *argv0, const char *name)
{
    char executable[PATH_MAX];
    char canonical[PATH_MAX];
    bool found = false;

#ifdef __APPLE__
    uint32_t capacity = (uint32_t)sizeof(executable);
    if (_NSGetExecutablePath(executable, &capacity) == 0) {
        found = true;
    }
#elif defined(__linux__)
    ssize_t length = readlink("/proc/self/exe", executable,
                              sizeof(executable) - 1);
    if (length >= 0) {
        executable[length] = '\0';
        found = true;
    }
#endif

    if (!found) {
        if (strlen(argv0) >= sizeof(executable)) {
            fail("executable path is too long");
        }
        strcpy(executable, argv0);
    }

    const char *base = realpath(executable, canonical);
    if (base == NULL) {
        base = executable;
    }
    const char *slash = strrchr(base, '/');
    size_t directory_length = slash == NULL ? 1 : (size_t)(slash - base);
    size_t name_length = strlen(name);
    char *path = malloc(directory_length + 1 + name_length + 1);
    if (path == NULL) {
        fail("could not allocate the output path");
    }

    if (slash == NULL) {
        path[0] = '.';
    } else if (directory_length == 0) {
        path[0] = '/';
        directory_length = 1;
    } else {
        memcpy(path, base, directory_length);
    }
    path[directory_length] = '/';
    memcpy(path + directory_length + 1, name, name_length + 1);
    return path;
}

static bool *make_sieve(int limit)
{
    bool *prime = calloc((size_t)limit + 1, sizeof(*prime));
    if (prime == NULL) {
        fail("could not allocate the prime sieve");
    }
    for (int value = 2; value <= limit; ++value) {
        prime[value] = true;
    }
    for (int divisor = 2; divisor <= limit / divisor; ++divisor) {
        if (prime[divisor]) {
            for (int value = divisor * divisor; value <= limit;
                 value += divisor) {
                prime[value] = false;
            }
        }
    }
    return prime;
}

static void build_column_masks(uint64_t *column_masks, int odd_count,
                               int even_count, const bool *prime)
{
    for (int column = 0; column < even_count; ++column) {
        uint64_t mask = 0;
        int even_value = 2 * (column + 1);
        for (int row = 0; row < odd_count; ++row) {
            int odd_value = 2 * row + 1;
            int sum = odd_value + even_value;
            if (sum >= 4 && !prime[sum]) {
                mask |= UINT64_C(1) << row;
            }
        }
        column_masks[column] = mask;
    }
}

static uint64_t add64(uint64_t a, uint64_t b, uint64_t modulus)
{
    uint64_t sum = a + b;
    return sum >= modulus ? sum - modulus : sum;
}

static void insert_column64(uint64_t *dp, size_t states, uint64_t allowed,
                            uint64_t modulus)
{
    for (size_t mask = states; mask-- > 0;) {
        uint64_t value = dp[mask];
        if (value == 0) {
            continue;
        }
        uint64_t available = allowed & ~(uint64_t)mask;
        while (available != 0) {
            uint64_t bit = available & (UINT64_C(0) - available);
            size_t target = mask | (size_t)bit;
            dp[target] = add64(dp[target], value, modulus);
            available ^= bit;
        }
    }
}

static void collect64(uint64_t *rook, const uint64_t *dp, size_t states,
                      uint64_t modulus)
{
    for (size_t mask = 0; mask < states; ++mask) {
        if (dp[mask] == 0) {
            continue;
        }
        unsigned int k =
            (unsigned int)__builtin_popcountll((uint64_t)mask);
        rook[k] = add64(rook[k], dp[mask], modulus);
    }
}

static int column_rooks64(int odd_count, int even_count,
                          const uint64_t *column_masks, uint64_t modulus,
                          uint64_t *all_rooks, uint64_t *no_one_rooks)
{
    size_t full_states = (size_t)1 << odd_count;
    size_t reduced_states = (size_t)1 << (odd_count - 1);
    uint64_t *dp = calloc(full_states, sizeof(*dp));
    if (dp == NULL) {
        return -1;
    }

    dp[0] = 1;
    for (int column = 0; column < even_count; ++column) {
        insert_column64(dp, full_states, column_masks[column], modulus);
    }
    collect64(all_rooks, dp, full_states, modulus);

    memset(dp, 0, reduced_states * sizeof(*dp));
    dp[0] = 1;
    for (int column = 0; column < even_count; ++column) {
        insert_column64(dp, reduced_states, column_masks[column] >> 1,
                        modulus);
    }
    collect64(no_one_rooks, dp, reduced_states, modulus);

    free(dp);
    return 0;
}

static void insert_column8(uint8_t *dp, size_t states, uint64_t allowed,
                           unsigned int modulus)
{
    for (size_t mask = states; mask-- > 0;) {
        unsigned int value = dp[mask];
        if (value == 0) {
            continue;
        }
        uint64_t available = allowed & ~(uint64_t)mask;
        while (available != 0) {
            uint64_t bit = available & (UINT64_C(0) - available);
            size_t target = mask | (size_t)bit;
            unsigned int sum = (unsigned int)dp[target] + value;
            if (sum >= modulus) {
                sum -= modulus;
            }
            dp[target] = (uint8_t)sum;
            available ^= bit;
        }
    }
}

static void collect8(uint64_t *rook, const uint8_t *dp, size_t states,
                     unsigned int modulus)
{
    for (size_t mask = 0; mask < states; ++mask) {
        if (dp[mask] == 0) {
            continue;
        }
        unsigned int k =
            (unsigned int)__builtin_popcountll((uint64_t)mask);
        unsigned int sum = (unsigned int)rook[k] + dp[mask];
        if (sum >= modulus) {
            sum -= modulus;
        }
        rook[k] = sum;
    }
}

static int column_rooks8(int odd_count, int even_count,
                         const uint64_t *column_masks,
                         unsigned int modulus, uint64_t *all_rooks,
                         uint64_t *no_one_rooks)
{
    size_t full_states = (size_t)1 << odd_count;
    size_t reduced_states = (size_t)1 << (odd_count - 1);
    uint8_t *dp = calloc(full_states, sizeof(*dp));
    if (dp == NULL) {
        return -1;
    }

    dp[0] = 1;
    for (int column = 0; column < even_count; ++column) {
        insert_column8(dp, full_states, column_masks[column], modulus);
    }
    collect8(all_rooks, dp, full_states, modulus);

    memset(dp, 0, reduced_states * sizeof(*dp));
    dp[0] = 1;
    for (int column = 0; column < even_count; ++column) {
        insert_column8(dp, reduced_states, column_masks[column] >> 1,
                       modulus);
    }
    collect8(no_one_rooks, dp, reduced_states, modulus);

    free(dp);
    return 0;
}

static void calculate_bounds(mpz_t *bounds, mpz_t maximum,
                             int odd_count, int even_count)
{
    mpz_t choose_rows;
    mpz_t choose_columns;
    mpz_t factorial;
    mpz_init(choose_rows);
    mpz_init(choose_columns);
    mpz_init(factorial);
    mpz_set_ui(maximum, 0);

    for (int k = 0; k <= even_count; ++k) {
        mpz_bin_uiui(choose_rows, (unsigned long)odd_count,
                     (unsigned long)k);
        mpz_bin_uiui(choose_columns, (unsigned long)even_count,
                     (unsigned long)k);
        mpz_fac_ui(factorial, (unsigned long)k);
        mpz_mul(bounds[k], choose_rows, choose_columns);
        mpz_mul(bounds[k], bounds[k], factorial);
        if (mpz_cmp(bounds[k], maximum) > 0) {
            mpz_set(maximum, bounds[k]);
        }
    }

    mpz_clear(factorial);
    mpz_clear(choose_columns);
    mpz_clear(choose_rows);
}

static int moduli61(uint64_t *moduli, const mpz_t bound)
{
    mpz_t candidate;
    mpz_t product;
    mpz_init(candidate);
    mpz_init_set_ui(product, 1);
    mpz_set_ui(candidate, 1);
    mpz_mul_2exp(candidate, candidate, 61);
    mpz_sub_ui(candidate, candidate, UINT64_C(30000000));

    int count = 0;
    while (mpz_cmp(product, bound) <= 0) {
        if (count >= MAX_CRT_PRIMES) {
            fail("too many 61-bit CRT moduli");
        }
        mpz_nextprime(candidate, candidate);
        if (mpz_sizeinbase(candidate, 2) > 61) {
            fail("could not generate a 61-bit CRT modulus");
        }
        moduli[count] = (uint64_t)mpz_get_ui(candidate);
        mpz_mul_ui(product, product, (unsigned long)moduli[count]);
        ++count;
    }

    mpz_clear(product);
    mpz_clear(candidate);
    return count;
}

static int moduli8(uint64_t *moduli, const mpz_t bound)
{
    bool prime[256] = { false };
    for (int value = 2; value < 256; ++value) {
        prime[value] = true;
    }
    for (int divisor = 2; divisor * divisor < 256; ++divisor) {
        if (prime[divisor]) {
            for (int value = divisor * divisor; value < 256;
                 value += divisor) {
                prime[value] = false;
            }
        }
    }

    mpz_t product;
    mpz_init_set_ui(product, 1);
    int count = 0;
    /*
     * Algorithm 1 needs only the primes 167..251 through n=60.
     * Starting at 163 makes this verifier's one-byte CRT set disjoint.
     */
    for (int p = 163; p >= 2 && mpz_cmp(product, bound) <= 0; --p) {
        if (!prime[p]) {
            continue;
        }
        if (count >= MAX_CRT_PRIMES) {
            fail("too many one-byte CRT moduli");
        }
        moduli[count++] = (uint64_t)p;
        mpz_mul_ui(product, product, (unsigned long)p);
    }
    if (mpz_cmp(product, bound) <= 0) {
        fail("one-byte CRT moduli do not cover the rook bound");
    }
    mpz_clear(product);
    return count;
}

static int choose_workers(size_t bytes_per_worker, int jobs)
{
    uint64_t memory_limit =
        DP_MEMORY_BUDGET / (uint64_t)bytes_per_worker;
    int workers = memory_limit > (uint64_t)INT_MAX
                      ? INT_MAX
                      : (int)memory_limit;
    if (workers < 1) {
        workers = 1;
    }
    if (workers > jobs) {
        workers = jobs;
    }
#ifdef _OPENMP
    if (workers > omp_get_max_threads()) {
        workers = omp_get_max_threads();
    }
#else
    workers = 1;
#endif
    return workers;
}

static void reconstruct(mpz_t result, const uint64_t *residues,
                        size_t stride, int k, const uint64_t *moduli,
                        int count)
{
    mpz_t product;
    mpz_t p_integer;
    mpz_t product_residue;
    mpz_t inverse;
    mpz_init_set_ui(product, 1);
    mpz_init(p_integer);
    mpz_init(product_residue);
    mpz_init(inverse);
    mpz_set_ui(result, 0);

    for (int q = 0; q < count; ++q) {
        uint64_t p = moduli[q];
        if (p < 2) {
            fail("invalid CRT modulus");
        }
        uint64_t wanted =
            residues[(size_t)q * stride + (size_t)k];
        uint64_t current =
            (uint64_t)mpz_fdiv_ui(result, (unsigned long)p);
        uint64_t delta =
            wanted >= current ? wanted - current : wanted + p - current;

        mpz_set_ui(p_integer, (unsigned long)p);
        mpz_set_ui(product_residue,
                   mpz_fdiv_ui(product, (unsigned long)p));
        if (mpz_invert(inverse, product_residue, p_integer) == 0) {
            fail("CRT moduli are not pairwise coprime");
        }
        uint64_t multiplier =
            (uint64_t)((__uint128_t)delta * mpz_get_ui(inverse) % p);
        mpz_addmul_ui(result, product, (unsigned long)multiplier);
        mpz_mul_ui(product, product, (unsigned long)p);
    }

    mpz_clear(inverse);
    mpz_clear(product_residue);
    mpz_clear(p_integer);
    mpz_clear(product);
}

static void calculate_a392408(mpz_t result, int n, bool report)
{
    if (n == 0) {
        mpz_set_ui(result, 1);
        return;
    }

    int odd_count = n / 2 + n % 2;
    int even_count = n / 2;
    size_t state_count = (size_t)1 << odd_count;
    bool byte_mode = odd_count >= COLUMN_BYTE_THRESHOLD;
    size_t bytes_per_worker =
        state_count * (byte_mode ? sizeof(uint8_t) : sizeof(uint64_t));

    bool *prime = make_sieve(2 * n);
    uint64_t column_masks[32] = { 0 };
    build_column_masks(column_masks, odd_count, even_count, prime);
    free(prime);

    size_t stride = (size_t)even_count + 1;
    mpz_t *bounds = malloc(stride * sizeof(*bounds));
    mpz_t *all_rooks = malloc(stride * sizeof(*all_rooks));
    mpz_t *no_one_rooks = malloc(stride * sizeof(*no_one_rooks));
    if (bounds == NULL || all_rooks == NULL || no_one_rooks == NULL) {
        free(no_one_rooks);
        free(all_rooks);
        free(bounds);
        fail("could not allocate exact rook arrays");
    }
    for (int k = 0; k <= even_count; ++k) {
        mpz_init(bounds[k]);
        mpz_init(all_rooks[k]);
        mpz_init(no_one_rooks[k]);
    }

    mpz_t maximum;
    mpz_init(maximum);
    calculate_bounds(bounds, maximum, odd_count, even_count);

    uint64_t moduli[MAX_CRT_PRIMES];
    int modulus_count =
        byte_mode ? moduli8(moduli, maximum) : moduli61(moduli, maximum);
    uint64_t *all_residues =
        calloc((size_t)modulus_count * stride, sizeof(*all_residues));
    uint64_t *no_one_residues =
        calloc((size_t)modulus_count * stride, sizeof(*no_one_residues));
    int *status = calloc((size_t)modulus_count, sizeof(*status));
    if (all_residues == NULL || no_one_residues == NULL || status == NULL) {
        free(status);
        free(no_one_residues);
        free(all_residues);
        fail("could not allocate modular rook arrays");
    }

    int workers = choose_workers(bytes_per_worker, modulus_count);
    if (report) {
        fprintf(stderr,
                "392408_02: n=%d, states=%zu+%zu, %d CRT prime%s, "
                "%d OpenMP thread%s, %.2f GiB/worker\n",
                n, state_count, state_count >> 1, modulus_count,
                modulus_count == 1 ? "" : "s",
                workers, workers == 1 ? "" : "s",
                (double)bytes_per_worker /
                    (double)(UINT64_C(1) << 30));
    }

#ifdef _OPENMP
#pragma omp parallel for schedule(static) num_threads(workers)
#endif
    for (int q = 0; q < modulus_count; ++q) {
        uint64_t *r = all_residues + (size_t)q * stride;
        uint64_t *u = no_one_residues + (size_t)q * stride;
        if (byte_mode) {
            status[q] = column_rooks8(
                odd_count, even_count, column_masks,
                (unsigned int)moduli[q], r, u);
        } else {
            status[q] = column_rooks64(
                odd_count, even_count, column_masks, moduli[q], r, u);
        }
    }
    for (int q = 0; q < modulus_count; ++q) {
        if (status[q] != 0) {
            fail("could not allocate a column-DP array");
        }
    }

    for (int k = 0; k <= even_count; ++k) {
        reconstruct(all_rooks[k], all_residues, stride, k,
                    moduli, modulus_count);
        reconstruct(no_one_rooks[k], no_one_residues, stride, k,
                    moduli, modulus_count);
        if (mpz_cmp(all_rooks[k], bounds[k]) > 0 ||
            mpz_cmp(no_one_rooks[k], bounds[k]) > 0) {
            fail("CRT reconstruction exceeded the rook bound");
        }
        for (int q = 0; q < modulus_count; ++q) {
            unsigned long p = (unsigned long)moduli[q];
            if (mpz_fdiv_ui(all_rooks[k], p) !=
                    all_residues[(size_t)q * stride + (size_t)k] ||
                mpz_fdiv_ui(no_one_rooks[k], p) !=
                    no_one_residues[(size_t)q * stride + (size_t)k]) {
                fail("CRT residue replay check failed");
            }
        }
    }

    mpz_t *factorial = malloc(((size_t)n + 1) * sizeof(*factorial));
    if (factorial == NULL) {
        fail("could not allocate factorials");
    }
    for (int k = 0; k <= n; ++k) {
        mpz_init(factorial[k]);
    }
    mpz_set_ui(factorial[0], 1);
    for (int k = 1; k <= n; ++k) {
        mpz_mul_ui(factorial[k], factorial[k - 1], (unsigned long)k);
    }

    mpz_t term;
    mpz_t forbidden;
    mpz_init(term);
    mpz_init(forbidden);
    mpz_set_ui(result, 0);
    for (int k = 0; k <= even_count; ++k) {
        mpz_mul(term, all_rooks[k], all_rooks[k]);
        mpz_mul(term, term, factorial[odd_count - k]);
        mpz_mul(term, term, factorial[even_count - k]);
        if (k < odd_count) {
            mpz_mul(forbidden, no_one_rooks[k], no_one_rooks[k]);
            mpz_mul(forbidden, forbidden, factorial[odd_count - k - 1]);
            mpz_mul(forbidden, forbidden, factorial[even_count - k]);
            mpz_sub(term, term, forbidden);
        }
        mpz_add(result, result, term);
    }
    if (mpz_sgn(result) < 0 || mpz_cmp(result, factorial[n]) > 0) {
        fail("final result is outside 0..n!");
    }

    mpz_clear(forbidden);
    mpz_clear(term);
    for (int k = 0; k <= n; ++k) {
        mpz_clear(factorial[k]);
    }
    free(factorial);
    free(status);
    free(no_one_residues);
    free(all_residues);
    mpz_clear(maximum);
    for (int k = 0; k <= even_count; ++k) {
        mpz_clear(no_one_rooks[k]);
        mpz_clear(all_rooks[k]);
        mpz_clear(bounds[k]);
    }
    free(no_one_rooks);
    free(all_rooks);
    free(bounds);
}

static int check_terms(void)
{
    mpz_t actual;
    mpz_t expected;
    mpz_init(actual);
    mpz_init(expected);

    for (int n = 0; n <= KNOWN_MAX_N; ++n) {
        calculate_a392408(actual, n, false);
        if (mpz_set_str(expected, known_terms[n], 10) != 0) {
            fail("invalid built-in test term");
        }
        if (mpz_cmp(actual, expected) != 0) {
            gmp_fprintf(stderr,
                        "check failed at n=%d: got %Zd, expected %Zd\n",
                        n, actual, expected);
            mpz_clear(expected);
            mpz_clear(actual);
            return EXIT_FAILURE;
        }
    }

    printf("ok: algorithm 2 agrees with OEIS A392408 for n=0..%d\n",
           KNOWN_MAX_N);
    mpz_clear(expected);
    mpz_clear(actual);
    return EXIT_SUCCESS;
}

static void write_output(const char *argv0, int max_n)
{
    char *path = make_output_path(argv0, "b392408_1.txt");
    size_t part_length = strlen(path) + strlen(".part") + 1;
    char *part = malloc(part_length);
    if (part == NULL) {
        free(path);
        fail("could not allocate the temporary output name");
    }
    snprintf(part, part_length, "%s.part", path);

    FILE *stream = fopen(part, "w");
    if (stream == NULL) {
        fprintf(stderr, "error: cannot open %s: %s\n",
                part, strerror(errno));
        free(part);
        free(path);
        exit(EXIT_FAILURE);
    }

    mpz_t value;
    mpz_init(value);
    for (int n = 0; n <= max_n; ++n) {
        calculate_a392408(value, n, true);
        if (gmp_fprintf(stream, "%d %Zd\n", n, value) < 0 ||
            fflush(stream) != 0) {
            fclose(stream);
            free(part);
            free(path);
            fail("could not write b392408_1.txt");
        }
    }
    mpz_clear(value);

    if (fclose(stream) != 0) {
        fail("could not close b392408_1.txt");
    }
    if (rename(part, path) != 0) {
        fprintf(stderr, "error: cannot rename %s to %s: %s\n",
                part, path, strerror(errno));
        free(part);
        free(path);
        exit(EXIT_FAILURE);
    }
    printf("wrote %s (n=0..%d)\n", path, max_n);
    free(part);
    free(path);
}

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s [MAX_N]\n"
            "       %s --check\n"
            "MAX_N defaults to %d and may be at most %d.\n",
            program, program, DEFAULT_MAX_N, MAX_SUPPORTED_N);
}

int main(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "--check") == 0) {
        return check_terms();
    }
    if (argc > 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    int max_n = argc == 2 ? read_limit(argv[1]) : DEFAULT_MAX_N;
    write_output(argv[0], max_n);
    return EXIT_SUCCESS;
}
