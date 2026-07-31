/*
 * A392408 -- fast exact implementation.
 *
 * Algorithm 1:
 *   parity decomposition + rook-polynomial subset DP + CRT.
 *
 * After odd and even indices are grouped, the odd-odd and even-even blocks
 * are all ones except for the entry (1,1).  Let B be the odd-row/even-column
 * block, let R_k be its k-rook number, and let U_k be the k-rook number after
 * deleting the row for odd value 1.  For
 *
 *     o = ceil(n/2), e = floor(n/2),
 *
 *   a(n) = sum_{k=0}^e (e-k)! *
 *          ((o-k)! R_k^2 - (o-k-1)! U_k^2),
 *
 * where the second term is absent for k=o.
 *
 * R_k and U_k are computed modulo pairwise distinct primes.  CRT reconstructs
 * the rook numbers, not a(n).  The rigorous bound
 *
 *     R_k <= binomial(o,k) binomial(e,k) k!
 *
 * determines how many primes are needed.  This substantially reduces the
 * number of moduli at n=60.
 *
 * Memory policy for the 8-GB target Mac:
 *   - up to floor(n/2)=28: 61-bit residues in uint64_t;
 *   - above that: residues modulo one-byte primes in uint8_t.
 * At most 2 GiB of DP arrays are active across all OpenMP workers.  In
 * particular, n=60 uses two 1-GiB arrays rather than an unsafe 8-GiB array.
 *
 * The b-file is written beside the executable, independently of the current
 * working directory.
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
#ifndef BYTE_RESIDUE_THRESHOLD
#define BYTE_RESIDUE_THRESHOLD 29
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

static char *path_beside_executable(const char *argv0, const char *filename)
{
    char executable[PATH_MAX];
    char resolved[PATH_MAX];
    bool have_path = false;

#ifdef __APPLE__
    uint32_t size = (uint32_t)sizeof(executable);
    if (_NSGetExecutablePath(executable, &size) == 0) {
        have_path = true;
    }
#elif defined(__linux__)
    ssize_t length = readlink("/proc/self/exe", executable,
                              sizeof(executable) - 1);
    if (length >= 0) {
        executable[length] = '\0';
        have_path = true;
    }
#endif

    if (!have_path) {
        if (strlen(argv0) >= sizeof(executable)) {
            die("executable path is too long");
        }
        strcpy(executable, argv0);
    }

    const char *base = realpath(executable, resolved);
    if (base == NULL) {
        base = executable;
    }
    const char *slash = strrchr(base, '/');
    size_t directory_length = slash == NULL ? 1 : (size_t)(slash - base);
    size_t filename_length = strlen(filename);
    char *path = malloc(directory_length + 1 + filename_length + 1);
    if (path == NULL) {
        die("could not allocate the output path");
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
    memcpy(path + directory_length + 1, filename, filename_length + 1);
    return path;
}

static bool *prime_table(int limit)
{
    bool *prime = calloc((size_t)limit + 1, sizeof(*prime));
    if (prime == NULL) {
        die("could not allocate the prime table");
    }
    for (int i = 2; i <= limit; ++i) {
        prime[i] = true;
    }
    for (int p = 2; p <= limit / p; ++p) {
        if (prime[p]) {
            for (int m = p * p; m <= limit; m += p) {
                prime[m] = false;
            }
        }
    }
    return prime;
}

static void build_cross_masks(uint64_t *row_masks, int odd_count,
                              int even_count, const bool *prime)
{
    for (int row = 0; row < odd_count; ++row) {
        uint64_t mask = 0;
        int odd_value = 2 * row + 1;
        for (int column = 0; column < even_count; ++column) {
            int even_value = 2 * (column + 1);
            int sum = odd_value + even_value;
            if (sum >= 4 && !prime[sum]) {
                mask |= UINT64_C(1) << column;
            }
        }
        row_masks[row] = mask;
    }
}

static uint64_t add_mod64(uint64_t a, uint64_t b, uint64_t modulus)
{
    uint64_t sum = a + b;
    return sum >= modulus ? sum - modulus : sum;
}

static uint64_t multiply_mod64(uint64_t a, uint64_t b, uint64_t modulus)
{
    if (modulus < 2) {
        die("invalid CRT modulus");
    }
    return (uint64_t)((__uint128_t)a * b % modulus);
}

static uint64_t power_mod64(uint64_t base, uint64_t exponent,
                            uint64_t modulus)
{
    uint64_t result = 1;
    while (exponent != 0) {
        if (exponent & 1) {
            result = multiply_mod64(result, base, modulus);
        }
        base = multiply_mod64(base, base, modulus);
        exponent >>= 1;
    }
    return result;
}

static void add_rook_row64(uint64_t *dp, size_t state_count,
                           uint64_t allowed, uint64_t modulus)
{
    for (size_t mask = state_count; mask-- > 0;) {
        uint64_t value = dp[mask];
        if (value == 0) {
            continue;
        }
        uint64_t available = allowed & ~(uint64_t)mask;
        while (available != 0) {
            uint64_t bit = available & (UINT64_C(0) - available);
            size_t target = mask | (size_t)bit;
            dp[target] = add_mod64(dp[target], value, modulus);
            available ^= bit;
        }
    }
}

static int rook_residues64(int odd_count, int even_count,
                           const uint64_t *row_masks, uint64_t modulus,
                           uint64_t *all_rooks, uint64_t *no_one_rooks)
{
    size_t state_count = (size_t)1 << even_count;
    uint64_t *dp = calloc(state_count, sizeof(*dp));
    if (dp == NULL) {
        return -1;
    }
    dp[0] = 1;

    for (int row = 1; row < odd_count; ++row) {
        add_rook_row64(dp, state_count, row_masks[row], modulus);
    }
    for (size_t mask = 0; mask < state_count; ++mask) {
        unsigned int k =
            (unsigned int)__builtin_popcountll((uint64_t)mask);
        no_one_rooks[k] =
            add_mod64(no_one_rooks[k], dp[mask], modulus);
    }

    add_rook_row64(dp, state_count, row_masks[0], modulus);
    for (size_t mask = 0; mask < state_count; ++mask) {
        unsigned int k =
            (unsigned int)__builtin_popcountll((uint64_t)mask);
        all_rooks[k] = add_mod64(all_rooks[k], dp[mask], modulus);
    }

    free(dp);
    return 0;
}

static void add_rook_row8(uint8_t *dp, size_t state_count,
                          uint64_t allowed, unsigned int modulus)
{
    for (size_t mask = state_count; mask-- > 0;) {
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

static int rook_residues8(int odd_count, int even_count,
                          const uint64_t *row_masks, unsigned int modulus,
                          uint64_t *all_rooks, uint64_t *no_one_rooks)
{
    size_t state_count = (size_t)1 << even_count;
    uint8_t *dp = calloc(state_count, sizeof(*dp));
    if (dp == NULL) {
        return -1;
    }
    dp[0] = 1;

    for (int row = 1; row < odd_count; ++row) {
        add_rook_row8(dp, state_count, row_masks[row], modulus);
    }
    for (size_t mask = 0; mask < state_count; ++mask) {
        unsigned int k =
            (unsigned int)__builtin_popcountll((uint64_t)mask);
        unsigned int sum = (unsigned int)no_one_rooks[k] + dp[mask];
        if (sum >= modulus) {
            sum -= modulus;
        }
        no_one_rooks[k] = sum;
    }

    add_rook_row8(dp, state_count, row_masks[0], modulus);
    for (size_t mask = 0; mask < state_count; ++mask) {
        unsigned int k =
            (unsigned int)__builtin_popcountll((uint64_t)mask);
        unsigned int sum = (unsigned int)all_rooks[k] + dp[mask];
        if (sum >= modulus) {
            sum -= modulus;
        }
        all_rooks[k] = sum;
    }

    free(dp);
    return 0;
}

static void make_rook_bounds(mpz_t *bounds, mpz_t maximum,
                             int odd_count, int even_count)
{
    mpz_t left;
    mpz_t right;
    mpz_t factorial;
    mpz_init(left);
    mpz_init(right);
    mpz_init(factorial);
    mpz_set_ui(maximum, 0);

    for (int k = 0; k <= even_count; ++k) {
        mpz_bin_uiui(left, (unsigned long)odd_count, (unsigned long)k);
        mpz_bin_uiui(right, (unsigned long)even_count, (unsigned long)k);
        mpz_fac_ui(factorial, (unsigned long)k);
        mpz_mul(bounds[k], left, right);
        mpz_mul(bounds[k], bounds[k], factorial);
        if (mpz_cmp(bounds[k], maximum) > 0) {
            mpz_set(maximum, bounds[k]);
        }
    }

    mpz_clear(factorial);
    mpz_clear(right);
    mpz_clear(left);
}

static int make_primes61(uint64_t *primes, const mpz_t required_bound)
{
    mpz_t candidate;
    mpz_t product;
    mpz_init(candidate);
    mpz_init_set_ui(product, 1);

    mpz_set_ui(candidate, 1);
    mpz_mul_2exp(candidate, candidate, 61);
    mpz_sub_ui(candidate, candidate, UINT64_C(10000000));

    int count = 0;
    while (mpz_cmp(product, required_bound) <= 0) {
        if (count >= MAX_CRT_PRIMES) {
            die("too many 61-bit CRT primes");
        }
        mpz_nextprime(candidate, candidate);
        if (mpz_sizeinbase(candidate, 2) > 61) {
            die("could not generate a 61-bit CRT prime");
        }
        primes[count] = (uint64_t)mpz_get_ui(candidate);
        mpz_mul_ui(product, product, (unsigned long)primes[count]);
        ++count;
    }

    mpz_clear(product);
    mpz_clear(candidate);
    return count;
}

static int make_primes8(uint64_t *primes, const mpz_t required_bound)
{
    bool prime[256] = { false };
    for (int i = 2; i < 256; ++i) {
        prime[i] = true;
    }
    for (int p = 2; p * p < 256; ++p) {
        if (prime[p]) {
            for (int m = p * p; m < 256; m += p) {
                prime[m] = false;
            }
        }
    }

    mpz_t product;
    mpz_init_set_ui(product, 1);
    int count = 0;
    for (int p = 251; p >= 2 && mpz_cmp(product, required_bound) <= 0;
         --p) {
        if (!prime[p]) {
            continue;
        }
        if (count >= MAX_CRT_PRIMES) {
            die("too many one-byte CRT primes");
        }
        primes[count++] = (uint64_t)p;
        mpz_mul_ui(product, product, (unsigned long)p);
    }
    if (mpz_cmp(product, required_bound) <= 0) {
        die("one-byte primes do not cover the rook-number bound");
    }
    mpz_clear(product);
    return count;
}

static void crt_reconstruct_strided(mpz_t result, const uint64_t *residues,
                                    size_t stride, int index,
                                    const uint64_t *primes, int prime_count)
{
    mpz_t modulus_product;
    mpz_init_set_ui(modulus_product, 1);
    mpz_set_ui(result, 0);

    for (int q = 0; q < prime_count; ++q) {
        uint64_t p = primes[q];
        uint64_t residue = residues[(size_t)q * stride + (size_t)index];
        uint64_t current = (uint64_t)mpz_fdiv_ui(result, (unsigned long)p);
        uint64_t product_mod =
            (uint64_t)mpz_fdiv_ui(modulus_product, (unsigned long)p);
        uint64_t delta =
            residue >= current ? residue - current : residue + p - current;
        uint64_t inverse = power_mod64(product_mod, p - 2, p);
        uint64_t multiplier = multiply_mod64(delta, inverse, p);

        mpz_addmul_ui(result, modulus_product, (unsigned long)multiplier);
        mpz_mul_ui(modulus_product, modulus_product, (unsigned long)p);
    }

    mpz_clear(modulus_product);
}

static int safe_thread_count(size_t bytes_per_worker, int jobs)
{
    uint64_t by_memory = bytes_per_worker == 0
                             ? 1
                             : DP_MEMORY_BUDGET / (uint64_t)bytes_per_worker;
    int threads = by_memory > (uint64_t)INT_MAX
                      ? INT_MAX
                      : (int)by_memory;
    if (threads < 1) {
        threads = 1;
    }
    if (threads > jobs) {
        threads = jobs;
    }
#ifdef _OPENMP
    if (threads > omp_get_max_threads()) {
        threads = omp_get_max_threads();
    }
#else
    threads = 1;
#endif
    return threads;
}

static void a392408(mpz_t result, int n, bool report)
{
    if (n == 0) {
        mpz_set_ui(result, 1);
        return;
    }

    int odd_count = n / 2 + n % 2;
    int even_count = n / 2;
    size_t state_count = (size_t)1 << even_count;
    bool use_byte_residues = even_count >= BYTE_RESIDUE_THRESHOLD;
    size_t bytes_per_worker =
        state_count * (use_byte_residues ? sizeof(uint8_t) : sizeof(uint64_t));

    bool *prime = prime_table(2 * n);
    uint64_t row_masks[32] = { 0 };
    build_cross_masks(row_masks, odd_count, even_count, prime);
    free(prime);

    mpz_t *bounds = malloc(((size_t)even_count + 1) * sizeof(*bounds));
    mpz_t *all_rooks = malloc(((size_t)even_count + 1) * sizeof(*all_rooks));
    mpz_t *no_one_rooks =
        malloc(((size_t)even_count + 1) * sizeof(*no_one_rooks));
    if (bounds == NULL || all_rooks == NULL || no_one_rooks == NULL) {
        free(no_one_rooks);
        free(all_rooks);
        free(bounds);
        die("could not allocate the rook-number arrays");
    }
    for (int k = 0; k <= even_count; ++k) {
        mpz_init(bounds[k]);
        mpz_init(all_rooks[k]);
        mpz_init(no_one_rooks[k]);
    }

    mpz_t maximum_bound;
    mpz_init(maximum_bound);
    make_rook_bounds(bounds, maximum_bound, odd_count, even_count);

    uint64_t primes[MAX_CRT_PRIMES];
    int prime_count = use_byte_residues
                          ? make_primes8(primes, maximum_bound)
                          : make_primes61(primes, maximum_bound);
    size_t stride = (size_t)even_count + 1;
    uint64_t *all_residues =
        calloc((size_t)prime_count * stride, sizeof(*all_residues));
    uint64_t *no_one_residues =
        calloc((size_t)prime_count * stride, sizeof(*no_one_residues));
    int *status = calloc((size_t)prime_count, sizeof(*status));
    if (all_residues == NULL || no_one_residues == NULL || status == NULL) {
        free(status);
        free(no_one_residues);
        free(all_residues);
        die("could not allocate the modular result arrays");
    }

    int threads = safe_thread_count(bytes_per_worker, prime_count);
    if (report) {
        fprintf(stderr,
                "392408_01: n=%d, states=%zu, %d CRT prime%s, "
                "%d OpenMP thread%s, %.2f GiB/worker\n",
                n, state_count, prime_count, prime_count == 1 ? "" : "s",
                threads, threads == 1 ? "" : "s",
                (double)bytes_per_worker / (double)(UINT64_C(1) << 30));
    }

#ifdef _OPENMP
#pragma omp parallel for schedule(static) num_threads(threads)
#endif
    for (int q = 0; q < prime_count; ++q) {
        uint64_t *r = all_residues + (size_t)q * stride;
        uint64_t *u = no_one_residues + (size_t)q * stride;
        if (use_byte_residues) {
            status[q] = rook_residues8(
                odd_count, even_count, row_masks, (unsigned int)primes[q],
                r, u);
        } else {
            status[q] = rook_residues64(
                odd_count, even_count, row_masks, primes[q], r, u);
        }
    }

    for (int q = 0; q < prime_count; ++q) {
        if (status[q] != 0) {
            die("could not allocate a thread-local subset DP");
        }
    }

    for (int k = 0; k <= even_count; ++k) {
        crt_reconstruct_strided(all_rooks[k], all_residues, stride, k,
                                primes, prime_count);
        crt_reconstruct_strided(no_one_rooks[k], no_one_residues, stride, k,
                                primes, prime_count);
        if (mpz_cmp(all_rooks[k], bounds[k]) > 0 ||
            mpz_cmp(no_one_rooks[k], bounds[k]) > 0) {
            die("CRT reconstruction exceeded the rigorous rook bound");
        }
        for (int q = 0; q < prime_count; ++q) {
            unsigned long p = (unsigned long)primes[q];
            if (mpz_fdiv_ui(all_rooks[k], p) !=
                    all_residues[(size_t)q * stride + (size_t)k] ||
                mpz_fdiv_ui(no_one_rooks[k], p) !=
                    no_one_residues[(size_t)q * stride + (size_t)k]) {
                die("CRT residue replay check failed");
            }
        }
    }

    mpz_t *factorial = malloc(((size_t)n + 1) * sizeof(*factorial));
    if (factorial == NULL) {
        die("could not allocate factorials");
    }
    for (int k = 0; k <= n; ++k) {
        mpz_init(factorial[k]);
    }
    mpz_set_ui(factorial[0], 1);
    for (int k = 1; k <= n; ++k) {
        mpz_mul_ui(factorial[k], factorial[k - 1], (unsigned long)k);
    }

    mpz_t term;
    mpz_t bad;
    mpz_init(term);
    mpz_init(bad);
    mpz_set_ui(result, 0);
    for (int k = 0; k <= even_count; ++k) {
        mpz_mul(term, all_rooks[k], all_rooks[k]);
        mpz_mul(term, term, factorial[odd_count - k]);
        mpz_mul(term, term, factorial[even_count - k]);
        if (k < odd_count) {
            mpz_mul(bad, no_one_rooks[k], no_one_rooks[k]);
            mpz_mul(bad, bad, factorial[odd_count - k - 1]);
            mpz_mul(bad, bad, factorial[even_count - k]);
            mpz_sub(term, term, bad);
        }
        mpz_add(result, result, term);
    }
    if (mpz_sgn(result) < 0 || mpz_cmp(result, factorial[n]) > 0) {
        die("final result is outside 0..n!");
    }

    mpz_clear(bad);
    mpz_clear(term);
    for (int k = 0; k <= n; ++k) {
        mpz_clear(factorial[k]);
    }
    free(factorial);
    free(status);
    free(no_one_residues);
    free(all_residues);
    mpz_clear(maximum_bound);
    for (int k = 0; k <= even_count; ++k) {
        mpz_clear(no_one_rooks[k]);
        mpz_clear(all_rooks[k]);
        mpz_clear(bounds[k]);
    }
    free(no_one_rooks);
    free(all_rooks);
    free(bounds);
}

static int check_known_terms(void)
{
    mpz_t actual;
    mpz_t expected;
    mpz_init(actual);
    mpz_init(expected);

    for (int n = 0; n <= KNOWN_MAX_N; ++n) {
        a392408(actual, n, false);
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

    printf("ok: algorithm 1 agrees with OEIS A392408 for n=0..%d\n",
           KNOWN_MAX_N);
    mpz_clear(expected);
    mpz_clear(actual);
    return EXIT_SUCCESS;
}

static void write_b_file(const char *argv0, int max_n)
{
    char *output_path = path_beside_executable(argv0, "b392408.txt");
    size_t temporary_length = strlen(output_path) + strlen(".part") + 1;
    char *temporary_path = malloc(temporary_length);
    if (temporary_path == NULL) {
        free(output_path);
        die("could not allocate the temporary output path");
    }
    snprintf(temporary_path, temporary_length, "%s.part", output_path);

    FILE *output = fopen(temporary_path, "w");
    if (output == NULL) {
        fprintf(stderr, "error: cannot open %s: %s\n",
                temporary_path, strerror(errno));
        free(temporary_path);
        free(output_path);
        exit(EXIT_FAILURE);
    }

    mpz_t value;
    mpz_init(value);
    for (int n = 0; n <= max_n; ++n) {
        a392408(value, n, true);
        if (gmp_fprintf(output, "%d %Zd\n", n, value) < 0 ||
            fflush(output) != 0) {
            fclose(output);
            free(temporary_path);
            free(output_path);
            die("could not write the b-file");
        }
    }
    mpz_clear(value);

    if (fclose(output) != 0) {
        die("could not close the b-file");
    }
    if (rename(temporary_path, output_path) != 0) {
        fprintf(stderr, "error: cannot rename %s to %s: %s\n",
                temporary_path, output_path, strerror(errno));
        free(temporary_path);
        free(output_path);
        exit(EXIT_FAILURE);
    }
    printf("wrote %s (n=0..%d)\n", output_path, max_n);
    free(temporary_path);
    free(output_path);
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
    write_b_file(argv[0], max_n);
    return EXIT_SUCCESS;
}
