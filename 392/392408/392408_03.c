/*
 * A392408 -- independent exact permanent verification implementation.
 *
 * Algorithm 3:
 *   permanent of the original n by n 0-1 matrix, computed by the standard
 *   row/subset DP modulo pairwise distinct 61-bit primes, followed by CRT.
 *
 *     A[i,j] = 1  iff  i+j is composite,
 *
 *     dp[S] = sum dp[S \ {j}]
 *
 * where row |S| is matched to an allowed column j in S.  This implementation
 * deliberately does not use the parity decomposition or rook-number formula
 * from 392408_01.c, so agreement provides a genuinely separate check.
 *
 * Since 0 <= permanent(A) <= n!, CRT is continued until the product of the
 * moduli is greater than n!.  Each modulus is independent and is evaluated
 * in a separate OpenMP job with a private DP array.
 *
 * Complexity is O(n 2^n) time and O(2^n) memory per worker.  The deliberate
 * limit n<=28 keeps this verifier safe on the target 8-GB Mac.  The b-file is
 * written beside the executable, independently of the current directory.
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
#define MAX_SUPPORTED_N 28
#define KNOWN_MAX_N 24
#define DP_MEMORY_BUDGET (UINT64_C(2) << 30)
#define MAX_CRT_PRIMES 16

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

static void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static int parse_n(const char *text)
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

static char *output_path_from_executable(const char *argv0,
                                         const char *filename)
{
    char executable[PATH_MAX];
    char resolved[PATH_MAX];
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
        size_t length = strlen(argv0);
        if (length >= sizeof(executable)) {
            die("executable path is too long");
        }
        memcpy(executable, argv0, length + 1);
    }

    const char *absolute = realpath(executable, resolved);
    if (absolute == NULL) {
        absolute = executable;
    }
    const char *separator = strrchr(absolute, '/');
    size_t directory_length =
        separator == NULL ? 1 : (size_t)(separator - absolute);
    size_t name_length = strlen(filename);
    char *path = malloc(directory_length + 1 + name_length + 1);
    if (path == NULL) {
        die("could not allocate the output path");
    }

    if (separator == NULL) {
        path[0] = '.';
    } else if (directory_length == 0) {
        path[0] = '/';
        directory_length = 1;
    } else {
        memcpy(path, absolute, directory_length);
    }
    path[directory_length] = '/';
    memcpy(path + directory_length + 1, filename, name_length + 1);
    return path;
}

static bool *sieve(int limit)
{
    bool *is_prime = calloc((size_t)limit + 1, sizeof(*is_prime));
    if (is_prime == NULL) {
        die("could not allocate the sieve");
    }
    for (int value = 2; value <= limit; ++value) {
        is_prime[value] = true;
    }
    for (int divisor = 2; divisor <= limit / divisor; ++divisor) {
        if (!is_prime[divisor]) {
            continue;
        }
        for (int value = divisor * divisor; value <= limit;
             value += divisor) {
            is_prime[value] = false;
        }
    }
    return is_prime;
}

static void make_allowed_rows(uint64_t *allowed, int n)
{
    bool *is_prime = sieve(2 * n);
    for (int row = 1; row <= n; ++row) {
        uint64_t mask = 0;
        for (int column = 1; column <= n; ++column) {
            int sum = row + column;
            if (sum >= 4 && !is_prime[sum]) {
                mask |= UINT64_C(1) << (column - 1);
            }
        }
        allowed[row - 1] = mask;
    }
    free(is_prime);
}

static uint64_t add_residues(uint64_t a, uint64_t b, uint64_t modulus)
{
    uint64_t sum = a + b;
    if (sum >= modulus) {
        sum -= modulus;
    }
    return sum;
}

static int permanent_residue(int n, const uint64_t *allowed,
                             uint64_t modulus, uint64_t *answer)
{
    size_t state_count = (size_t)1 << n;
    uint64_t *dp = calloc(state_count, sizeof(*dp));
    if (dp == NULL) {
        return -1;
    }
    dp[0] = 1;

    for (size_t mask = 1; mask < state_count; ++mask) {
        int row =
            (int)__builtin_popcountll((uint64_t)mask) - 1;
        uint64_t columns = (uint64_t)mask & allowed[row];
        uint64_t value = 0;
        while (columns != 0) {
            uint64_t bit = columns & (UINT64_C(0) - columns);
            value = add_residues(value, dp[mask ^ (size_t)bit], modulus);
            columns ^= bit;
        }
        dp[mask] = value;
    }

    *answer = dp[state_count - 1];
    free(dp);
    return 0;
}

static int generate_moduli(uint64_t *moduli, const mpz_t upper_bound)
{
    mpz_t next;
    mpz_t product;
    mpz_init(next);
    mpz_init_set_ui(product, 1);

    /*
     * Start at a different point from algorithm 1.  This is not required by
     * CRT, but makes the verifier's modulus set independent as well.
     */
    mpz_set_ui(next, 1);
    mpz_mul_2exp(next, next, 61);
    mpz_sub_ui(next, next, UINT64_C(50000000));

    int count = 0;
    while (mpz_cmp(product, upper_bound) <= 0) {
        if (count >= MAX_CRT_PRIMES) {
            die("too many CRT moduli");
        }
        mpz_nextprime(next, next);
        if (mpz_sizeinbase(next, 2) > 61) {
            die("failed to generate a 61-bit modulus");
        }
        moduli[count] = (uint64_t)mpz_get_ui(next);
        mpz_mul_ui(product, product, (unsigned long)moduli[count]);
        ++count;
    }

    mpz_clear(product);
    mpz_clear(next);
    return count;
}

static int worker_count(size_t bytes_per_worker, int jobs)
{
    uint64_t memory_workers =
        DP_MEMORY_BUDGET / (uint64_t)bytes_per_worker;
    int workers = memory_workers > (uint64_t)INT_MAX
                      ? INT_MAX
                      : (int)memory_workers;
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

static void reconstruct_permanent(mpz_t result, const uint64_t *residues,
                                  const uint64_t *moduli, int count)
{
    mpz_t product;
    mpz_t p_as_integer;
    mpz_t product_mod_p;
    mpz_t inverse;
    mpz_init_set_ui(product, 1);
    mpz_init(p_as_integer);
    mpz_init(product_mod_p);
    mpz_init(inverse);
    mpz_set_ui(result, 0);

    for (int q = 0; q < count; ++q) {
        if (moduli[q] < 2) {
            die("invalid CRT modulus");
        }
        unsigned long p = (unsigned long)moduli[q];
        unsigned long current = mpz_fdiv_ui(result, p);
        unsigned long delta =
            residues[q] >= current
                ? (unsigned long)(residues[q] - current)
                : (unsigned long)(residues[q] + moduli[q] - current);

        mpz_set_ui(p_as_integer, p);
        mpz_set_ui(product_mod_p, mpz_fdiv_ui(product, p));
        if (mpz_invert(inverse, product_mod_p, p_as_integer) == 0) {
            die("CRT moduli are not pairwise coprime");
        }
        unsigned long multiplier =
            (unsigned long)((__uint128_t)delta *
                            (uint64_t)mpz_get_ui(inverse) %
                            moduli[q]);
        mpz_addmul_ui(result, product, multiplier);
        mpz_mul_ui(product, product, p);
    }

    mpz_clear(inverse);
    mpz_clear(product_mod_p);
    mpz_clear(p_as_integer);
    mpz_clear(product);
}

static void a392408_permanent(mpz_t result, int n, bool report)
{
    if (n == 0) {
        mpz_set_ui(result, 1);
        return;
    }

    uint64_t allowed[32] = { 0 };
    make_allowed_rows(allowed, n);

    mpz_t upper_bound;
    mpz_init(upper_bound);
    mpz_fac_ui(upper_bound, (unsigned long)n);

    uint64_t moduli[MAX_CRT_PRIMES];
    int modulus_count = generate_moduli(moduli, upper_bound);
    uint64_t *residues =
        calloc((size_t)modulus_count, sizeof(*residues));
    int *status = calloc((size_t)modulus_count, sizeof(*status));
    if (residues == NULL || status == NULL) {
        free(status);
        free(residues);
        mpz_clear(upper_bound);
        die("could not allocate modular results");
    }

    size_t state_count = (size_t)1 << n;
    size_t bytes_per_worker = state_count * sizeof(uint64_t);
    int workers = worker_count(bytes_per_worker, modulus_count);
    if (report) {
        fprintf(stderr,
                "392408_03: n=%d, states=%zu, %d CRT prime%s, "
                "%d OpenMP thread%s, %.2f GiB/worker\n",
                n, state_count, modulus_count,
                modulus_count == 1 ? "" : "s",
                workers, workers == 1 ? "" : "s",
                (double)bytes_per_worker /
                    (double)(UINT64_C(1) << 30));
    }

#ifdef _OPENMP
#pragma omp parallel for schedule(static) num_threads(workers)
#endif
    for (int q = 0; q < modulus_count; ++q) {
        status[q] =
            permanent_residue(n, allowed, moduli[q], &residues[q]);
    }
    for (int q = 0; q < modulus_count; ++q) {
        if (status[q] != 0) {
            die("could not allocate a verifier DP array");
        }
    }

    reconstruct_permanent(result, residues, moduli, modulus_count);
    if (mpz_sgn(result) < 0 || mpz_cmp(result, upper_bound) > 0) {
        die("reconstructed permanent is outside 0..n!");
    }
    for (int q = 0; q < modulus_count; ++q) {
        if (mpz_fdiv_ui(result, (unsigned long)moduli[q]) != residues[q]) {
            die("CRT residue replay check failed");
        }
    }

    free(status);
    free(residues);
    mpz_clear(upper_bound);
}

static int check_known_terms(void)
{
    mpz_t actual;
    mpz_t expected;
    mpz_init(actual);
    mpz_init(expected);

    for (int n = 0; n <= KNOWN_MAX_N; ++n) {
        a392408_permanent(actual, n, false);
        if (mpz_set_str(expected, known_terms[n], 10) != 0) {
            die("invalid built-in test term");
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

    printf("ok: algorithm 3 agrees with OEIS A392408 for n=0..%d\n",
           KNOWN_MAX_N);
    mpz_clear(expected);
    mpz_clear(actual);
    return EXIT_SUCCESS;
}

static void produce_b_file(const char *argv0, int max_n)
{
    char *path =
        output_path_from_executable(argv0, "b392408_2.txt");
    size_t part_size = strlen(path) + strlen(".part") + 1;
    char *part = malloc(part_size);
    if (part == NULL) {
        free(path);
        die("could not allocate the temporary filename");
    }
    snprintf(part, part_size, "%s.part", path);

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
        a392408_permanent(value, n, true);
        if (gmp_fprintf(stream, "%d %Zd\n", n, value) < 0 ||
            fflush(stream) != 0) {
            fclose(stream);
            free(part);
            free(path);
            die("could not write the verifier b-file");
        }
    }
    mpz_clear(value);

    if (fclose(stream) != 0) {
        die("could not close the verifier b-file");
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
        return check_known_terms();
    }
    if (argc > 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    int max_n = argc == 2 ? parse_n(argv[1]) : DEFAULT_MAX_N;
    produce_b_file(argv[0], max_n);
    return EXIT_SUCCESS;
}
