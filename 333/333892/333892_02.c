/*
 * A333892 -- independent uint32 dense modular DP + CRT verifier.
 *
 * Count permutations sigma of [n] such that
 *
 *              i divides product_{k=1..i} sigma(k)
 *
 * for every 1 <= i <= n.
 *
 * This program is deliberately different from 333892_01.c.  Program 01
 * primarily uses fixed-width exact counters and switches to sparse layer
 * hash tables when its exact dense array does not fit.  This program always
 * uses one flat uint32_t array indexed directly by the complete subset mask.
 * It repeats that dense DP sequentially modulo distinct primes and reconstructs
 * the exact answer with CRT.  There are no hash keys, probes, or rehashes.
 *
 * For a finite set S, let D(S) be the number of valid orderings of S.  Then
 *
 *   D(empty) = 1,
 *   D(S) = [ |S| divides product(S) ] * sum_{x in S} D(S\{x}).
 *
 * As in 01, a divisibility test is necessary only when |S|=p^e is a prime
 * power.  All prime-power factors of a non-prime-power m occurred at earlier
 * positions, so a path counted by the recurrence already contains them.  At
 * m=p^e the overflow-free test is
 *
 *                    sum_{x in S} v_p(x) >= e.
 *
 * The four fixed moduli are distinct primes just below 2^31.  If p=2^31-c,
 * and a state sum is less than 29*p, reduction uses
 *
 *             x mod p = ((x mod 2^31) + c*floor(x/2^31)) mod p.
 *
 * Here c<=69, so the folded value is below 2*p and one subtraction suffices.
 * This avoids a hardware division in the inner subset loop.
 *
 * Safety:
 *
 *   0 <= a(n) <= n!.
 *
 * Moduli are selected until their product is strictly greater than n!.
 * Therefore the nonnegative CRT representative below the product is unique.
 * The reconstructed value is checked against n! and replayed modulo every
 * selected prime.  Each modulus is verified for primality at startup.  The
 * dense allocation is checked against A333892_02_MEMORY_MIB before malloc.
 * At n=29 one pass has 2^29 uint32_t entries, exactly 2 GiB, and four
 * sequential CRT passes are required; only one pass array exists at a time.
 *
 * Every a(n) is computed separately in its own universe [n].  Built-in OEIS
 * values n=0..23 are used only as checks.  For n<=20, an independent exact
 * dense recurrence tests the original condition |S| | product(S) without
 * using the prime-power reduction and without forming the product.
 *
 * Each completed term is written and flushed to b333892_1_part.txt beside
 * the executable.  Complete success renames it to b333892_1.txt.  The b-file
 * has OFFSET 0 and is overwritten on each run.
 *
 * Build:
 *
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic \
 *     -I/opt/homebrew/include -L/opt/homebrew/lib \
 *     333892_02.c -lgmp -o 333892_02
 *
 * Usage:
 *
 *   A333892_02_MEMORY_MIB=4096 ./333892_02 N
 *
 * N may be 0..29.
 */

#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <gmp.h>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#if ULONG_MAX < UINT64_MAX
#error "333892_02 requires a platform with 64-bit unsigned long"
#endif

#define MAX_N 29
#define DENSE_CHECK_MAX_N 20
#define VERIFIED_MAX_N 23
#define MAX_MODULUS_COUNT 4
#define DEFAULT_MEMORY_MIB UINT64_C(4096)
#define MIN_MEMORY_MIB UINT64_C(64)
#define MAX_MEMORY_MIB UINT64_C(65536)
#define MERSENNE_BITS 31
#define MERSENNE_BASE UINT64_C(2147483648)
#define MERSENNE_MASK UINT64_C(2147483647)
#define REPORT_PARTS 8

static const uint32_t available_moduli[MAX_MODULUS_COUNT] = {
    UINT32_C(2147483647),
    UINT32_C(2147483629),
    UINT32_C(2147483587),
    UINT32_C(2147483579)
};

static const char *const verified_terms[VERIFIED_MAX_N + 1] = {
    "1",
    "1",
    "2",
    "4",
    "14",
    "36",
    "320",
    "1328",
    "7872",
    "51552",
    "756480",
    "5440752",
    "68999136",
    "584117952",
    "9632932800",
    "152699071104",
    "1881048314880",
    "21977611223040",
    "343998708042240",
    "4374197540536320",
    "77078374650869760",
    "1646804888482037760",
    "45052372505959096320",
    "727420047420178022400"
};

typedef struct {
    int prime;
    int exponent;
} PrimePowerInfo;

typedef struct {
    uint64_t states;
    uint64_t condition_tests;
    uint64_t transitions;
    double seconds;
} PassStats;

typedef struct {
    int maximum_n;
    int dense_check_n;
    uint64_t memory_limit;
    uint64_t dense_expected[DENSE_CHECK_MAX_N + 1];
    FILE *output;
    const char *part_path;
} Context;

static void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static double monotonic_seconds(void)
{
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) != 0) {
        die("clock_gettime failed");
    }
    return (double)now.tv_sec + (double)now.tv_nsec / 1000000000.0;
}

static int parse_n(const char *text)
{
    char *end = NULL;
    errno = 0;
    long value = strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' ||
        value < 0 || value > MAX_N) {
        fprintf(stderr, "error: N must be in 0..%d: %s\n", MAX_N, text);
        exit(EXIT_FAILURE);
    }
    return (int)value;
}

static uint64_t parse_memory_limit(void)
{
    const char *text = getenv("A333892_02_MEMORY_MIB");
    uint64_t mib = DEFAULT_MEMORY_MIB;
    if (text != NULL && *text != '\0') {
        char *end = NULL;
        errno = 0;
        unsigned long long value = strtoull(text, &end, 10);
        if (errno != 0 || end == text || *end != '\0' ||
            value < MIN_MEMORY_MIB || value > MAX_MEMORY_MIB) {
            fprintf(stderr,
                    "error: A333892_02_MEMORY_MIB must be in %" PRIu64
                    "..%" PRIu64 ": %s\n",
                    MIN_MEMORY_MIB, MAX_MEMORY_MIB, text);
            exit(EXIT_FAILURE);
        }
        mib = (uint64_t)value;
    }
    return mib * UINT64_C(1024) * UINT64_C(1024);
}

static char *path_beside_executable(const char *argv0,
                                    const char *filename)
{
    char executable[PATH_MAX];
    char resolved[PATH_MAX];
    bool found = false;

#ifdef __APPLE__
    uint32_t size = (uint32_t)sizeof(executable);
    if (_NSGetExecutablePath(executable, &size) == 0) {
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

    const char *base = realpath(executable, resolved);
    if (base == NULL) {
        base = executable;
    }
    const char *slash = strrchr(base, '/');
    size_t directory_length = slash == NULL ? 1 : (size_t)(slash - base);
    const char *directory = slash == NULL ? "." : base;
    size_t filename_length = strlen(filename);
    if (directory_length > SIZE_MAX - filename_length - 2) {
        die("output path length overflow");
    }
    char *path = malloc(directory_length + filename_length + 2);
    if (path == NULL) {
        die("cannot allocate output path");
    }
    memcpy(path, directory, directory_length);
    path[directory_length] = '/';
    memcpy(path + directory_length + 1, filename, filename_length + 1);
    return path;
}

static int gcd_int(int left, int right)
{
    while (right != 0) {
        int remainder = left % right;
        left = right;
        right = remainder;
    }
    return left;
}

static bool is_prime_u32(uint32_t value)
{
    if (value < 2) {
        return false;
    }
    if ((value & 1U) == 0) {
        return value == 2;
    }
    for (uint32_t divisor = 3;
         divisor <= value / divisor; divisor += 2) {
        if (value % divisor == 0) {
            return false;
        }
    }
    return true;
}

static void validate_moduli(void)
{
    for (size_t index = 0; index < MAX_MODULUS_COUNT; ++index) {
        uint32_t modulus = available_moduli[index];
        if (!is_prime_u32(modulus)) {
            die("configured CRT modulus is not prime");
        }
        if ((uint64_t)modulus >= MERSENNE_BASE) {
            die("configured CRT modulus is not below 2^31");
        }
        uint64_t gap = MERSENNE_BASE - modulus;
        if (gap > 69) {
            die("configured CRT modulus has an unsupported reduction gap");
        }
        for (size_t previous = 0; previous < index; ++previous) {
            if (modulus == available_moduli[previous]) {
                die("configured CRT moduli are not distinct");
            }
        }
    }
}

static PrimePowerInfo prime_power_info(int value)
{
    PrimePowerInfo result = { 0, 0 };
    if (value < 2) {
        return result;
    }
    int prime = 0;
    for (int divisor = 2; divisor <= value / divisor; ++divisor) {
        if (value % divisor == 0) {
            prime = divisor;
            break;
        }
    }
    if (prime == 0) {
        result.prime = value;
        result.exponent = 1;
        return result;
    }
    int remaining = value;
    int exponent = 0;
    while (remaining % prime == 0) {
        remaining /= prime;
        ++exponent;
    }
    if (remaining == 1) {
        result.prime = prime;
        result.exponent = exponent;
    }
    return result;
}

static void prepare_layer_data(int n, PrimePowerInfo *prime_powers,
                               uint64_t *divisible_masks)
{
    memset(prime_powers, 0,
           ((size_t)n + 1) * sizeof(*prime_powers));
    memset(divisible_masks, 0,
           ((size_t)n + 1) * sizeof(*divisible_masks));
    for (int value = 1; value <= n; ++value) {
        prime_powers[value] = prime_power_info(value);
        uint64_t mask = 0;
        for (int multiple = value; multiple <= n; multiple += value) {
            mask |= UINT64_C(1) << (multiple - 1);
        }
        divisible_masks[value] = mask;
    }
}

static int subset_p_valuation(uint64_t subset, int prime, int exponent,
                              const uint64_t *divisible_masks)
{
    int valuation = 0;
    int power = 1;
    for (int index = 0; index < exponent; ++index) {
        power *= prime;
        valuation += __builtin_popcountll(subset & divisible_masks[power]);
    }
    return valuation;
}

static bool original_condition(uint64_t subset, int size)
{
    int need = size;
    uint64_t remaining = subset;
    while (remaining != 0 && need != 1) {
        int bit_index = __builtin_ctzll(remaining);
        int element = bit_index + 1;
        need /= gcd_int(need, element);
        remaining &= remaining - 1;
    }
    return need == 1;
}

static void prepare_dense_check(Context *context)
{
    int n = context->maximum_n;
    if (n > DENSE_CHECK_MAX_N) {
        n = DENSE_CHECK_MAX_N;
    }
    context->dense_check_n = n;
    context->dense_expected[0] = 1;
    if (n == 0) {
        return;
    }

    size_t state_count = (size_t)1 << n;
    uint64_t *counts = calloc(state_count, sizeof(*counts));
    if (counts == NULL) {
        die("cannot allocate independent dense-check workspace");
    }
    counts[0] = 1;
    for (size_t subset = 1; subset < state_count; ++subset) {
        int size = __builtin_popcountll((uint64_t)subset);
        if (!original_condition((uint64_t)subset, size)) {
            continue;
        }
        uint64_t sum = 0;
        size_t remaining = subset;
        while (remaining != 0) {
            size_t bit = remaining & (0 - remaining);
            if (UINT64_MAX - sum < counts[subset ^ bit]) {
                free(counts);
                die("independent dense-check counter overflow");
            }
            sum += counts[subset ^ bit];
            remaining ^= bit;
        }
        counts[subset] = sum;
    }
    for (int index = 1; index <= n; ++index) {
        context->dense_expected[index] =
            counts[((size_t)1 << index) - 1];
    }
    free(counts);
    fprintf(stderr,
            "333892_02: independent original-condition check prepared "
            "for n=0..%d\n", n);
}

static size_t choose_moduli(uint32_t *moduli, mpz_t modulus_product,
                            const mpz_t bound)
{
    mpz_set_ui(modulus_product, 1);
    size_t count = 0;
    while (mpz_cmp(modulus_product, bound) <= 0) {
        if (count == MAX_MODULUS_COUNT) {
            die("factorial bound needs more configured CRT primes");
        }
        moduli[count] = available_moduli[count];
        mpz_mul_ui(modulus_product, modulus_product,
                   (unsigned long)moduli[count]);
        ++count;
    }
    return count;
}

static uint32_t reduce_pseudo_mersenne(uint64_t value,
                                       uint32_t modulus)
{
    uint64_t gap = MERSENNE_BASE - modulus;
    uint64_t folded = (value & MERSENNE_MASK) +
                      gap * (value >> MERSENNE_BITS);
    if (folded >= modulus) {
        folded -= modulus;
    }
    if (folded >= modulus) {
        die("pseudo-Mersenne reduction bound was violated");
    }
    return (uint32_t)folded;
}

static uint32_t multiply_mod(uint32_t left, uint32_t right,
                             uint32_t modulus)
{
    return (uint32_t)(((uint64_t)left * right) % modulus);
}

static uint32_t power_mod(uint32_t base, uint32_t exponent,
                          uint32_t modulus)
{
    uint32_t result = 1;
    while (exponent != 0) {
        if ((exponent & 1U) != 0) {
            result = multiply_mod(result, base, modulus);
        }
        base = multiply_mod(base, base, modulus);
        exponent >>= 1;
    }
    return result;
}

static void reconstruct_crt(mpz_t result, const uint32_t *residues,
                            const uint32_t *moduli, size_t count)
{
    mpz_t product;
    mpz_init_set_ui(product, 1);
    mpz_set_ui(result, 0);
    for (size_t index = 0; index < count; ++index) {
        uint32_t modulus = moduli[index];
        uint32_t product_mod =
            (uint32_t)mpz_fdiv_ui(product, (unsigned long)modulus);
        uint32_t inverse = power_mod(product_mod, modulus - 2, modulus);
        uint32_t result_mod =
            (uint32_t)mpz_fdiv_ui(result, (unsigned long)modulus);
        uint32_t difference = residues[index] >= result_mod
            ? residues[index] - result_mod
            : modulus - (result_mod - residues[index]);
        uint32_t multiplier = multiply_mod(difference, inverse, modulus);
        mpz_addmul_ui(result, product, (unsigned long)multiplier);
        mpz_mul_ui(product, product, (unsigned long)modulus);
    }
    mpz_clear(product);
}

static uint32_t compute_modular_pass(uint32_t *dp, int n,
                                     uint32_t modulus,
                                     const PrimePowerInfo *prime_powers,
                                     const uint64_t *divisible_masks,
                                     size_t pass_index,
                                     size_t pass_count,
                                     PassStats *stats)
{
    size_t state_count = (size_t)1 << n;
    dp[0] = 1;
    double start = monotonic_seconds();
    size_t report_step = state_count / REPORT_PARTS;
    size_t next_report = report_step;

    for (size_t subset_index = 1; subset_index < state_count;
         ++subset_index) {
        uint64_t subset = (uint64_t)subset_index;
        int size = __builtin_popcountll(subset);
        ++stats->states;

        PrimePowerInfo info = prime_powers[size];
        if (info.exponent != 0) {
            ++stats->condition_tests;
            int valuation = subset_p_valuation(
                subset, info.prime, info.exponent, divisible_masks);
            if (valuation < info.exponent) {
                dp[subset_index] = 0;
                goto maybe_report;
            }
        }

        uint64_t sum = 0;
        uint64_t remaining = subset;
        while (remaining != 0) {
            uint64_t bit = remaining & (0 - remaining);
            sum += dp[(size_t)(subset ^ bit)];
            ++stats->transitions;
            remaining ^= bit;
        }
        dp[subset_index] = reduce_pseudo_mersenne(sum, modulus);

maybe_report:
        if (state_count >= ((size_t)1 << 24) &&
            report_step != 0 && subset_index == next_report &&
            subset_index + 1 < state_count) {
            fprintf(stderr,
                    "333892_02: n=%d pass %zu/%zu %.1f%%, %.3f s\n",
                    n, pass_index + 1, pass_count,
                    100.0 * (double)subset_index / (double)state_count,
                    monotonic_seconds() - start);
            next_report += report_step;
        }
    }
    stats->seconds = monotonic_seconds() - start;
    return dp[state_count - 1];
}

static void compute_exact_term(mpz_t result, int n,
                               uint64_t memory_limit)
{
    if (n == 0) {
        mpz_set_ui(result, 1);
        return;
    }

    size_t state_count = (size_t)1 << n;
    uint64_t bytes = (uint64_t)state_count * sizeof(uint32_t);
    if (bytes > memory_limit) {
        fprintf(stderr,
                "error: A333892_02 n=%d dense array needs %.3f GiB; "
                "configured limit is %.3f GiB\n",
                n,
                (double)bytes / (1024.0 * 1024.0 * 1024.0),
                (double)memory_limit / (1024.0 * 1024.0 * 1024.0));
        exit(EXIT_FAILURE);
    }

    mpz_t bound;
    mpz_t modulus_product;
    mpz_inits(bound, modulus_product, NULL);
    mpz_fac_ui(bound, (unsigned long)n);
    uint32_t moduli[MAX_MODULUS_COUNT] = { 0, 0, 0, 0 };
    uint32_t residues[MAX_MODULUS_COUNT] = { 0, 0, 0, 0 };
    size_t modulus_count = choose_moduli(moduli, modulus_product, bound);

    PrimePowerInfo prime_powers[MAX_N + 1];
    uint64_t divisible_masks[MAX_N + 1];
    prepare_layer_data(n, prime_powers, divisible_masks);

    uint32_t *dp = malloc((size_t)bytes);
    if (dp == NULL) {
        mpz_clears(bound, modulus_product, NULL);
        die("cannot allocate uint32 dense DP array");
    }
    fprintf(stderr,
            "333892_02: n=%d, states=%zu, array=%.3f GiB, "
            "bound=%zu bits, %zu CRT pass%s\n",
            n, state_count,
            (double)bytes / (1024.0 * 1024.0 * 1024.0),
            mpz_sizeinbase(bound, 2), modulus_count,
            modulus_count == 1 ? "" : "es");

    double all_start = monotonic_seconds();
    for (size_t pass = 0; pass < modulus_count; ++pass) {
        PassStats stats;
        memset(&stats, 0, sizeof(stats));
        residues[pass] = compute_modular_pass(
            dp, n, moduli[pass], prime_powers, divisible_masks,
            pass, modulus_count, &stats);
        fprintf(stderr,
                "333892_02: n=%d pass %zu/%zu done, p=%" PRIu32
                ", states=%" PRIu64 ", condition tests=%" PRIu64
                ", transitions=%" PRIu64 ", %.3f s\n",
                n, pass + 1, modulus_count, moduli[pass],
                stats.states, stats.condition_tests,
                stats.transitions, stats.seconds);
    }
    free(dp);

    reconstruct_crt(result, residues, moduli, modulus_count);
    if (mpz_sgn(result) < 0 || mpz_cmp(result, bound) > 0) {
        mpz_clears(bound, modulus_product, NULL);
        die("CRT result lies outside 0..n!");
    }
    for (size_t pass = 0; pass < modulus_count; ++pass) {
        if ((uint32_t)mpz_fdiv_ui(
                result, (unsigned long)moduli[pass]) != residues[pass]) {
            mpz_clears(bound, modulus_product, NULL);
            die("CRT residue replay failed");
        }
    }
    fprintf(stderr,
            "333892_02: n=%d exact CRT reconstruction done, %.3f s\n",
            n, monotonic_seconds() - all_start);
    mpz_clears(bound, modulus_product, NULL);
}

static void verify_value(const Context *context, int n,
                         const mpz_t value)
{
    if (mpz_sgn(value) <= 0) {
        die("computed term is nonpositive although the identity works");
    }
    if (n <= context->dense_check_n &&
        mpz_cmp_ui(value, (unsigned long)context->dense_expected[n]) != 0) {
        die("modular DP differs from independent dense subset DP");
    }
    if (n <= VERIFIED_MAX_N) {
        mpz_t expected;
        mpz_init(expected);
        if (mpz_set_str(expected, verified_terms[n], 10) != 0) {
            mpz_clear(expected);
            die("invalid built-in verified term");
        }
        if (mpz_cmp(value, expected) != 0) {
            mpz_clear(expected);
            die("computed value differs from the verified OEIS prefix");
        }
        mpz_clear(expected);
    }
}

static void flush_output(const Context *context)
{
    if (fflush(context->output) != 0 ||
        fsync(fileno(context->output)) != 0) {
        fprintf(stderr, "error: cannot flush %s: %s\n",
                context->part_path, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

static void write_value(const Context *context, int n,
                        const mpz_t value)
{
    verify_value(context, n, value);
    if (gmp_fprintf(context->output, "%d %Zd\n", n, value) < 0) {
        die("cannot write b-file term");
    }
    flush_output(context);
    gmp_printf("%d %Zd\n", n, value);
    fflush(stdout);
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s N\n", argv[0]);
        return EXIT_FAILURE;
    }
    int maximum_n = parse_n(argv[1]);
    validate_moduli();

    Context context;
    memset(&context, 0, sizeof(context));
    context.maximum_n = maximum_n;
    context.memory_limit = parse_memory_limit();
    prepare_dense_check(&context);

    char *part_path = path_beside_executable(argv[0],
                                             "b333892_1_part.txt");
    char *final_path = path_beside_executable(argv[0], "b333892_1.txt");
    FILE *output = fopen(part_path, "w");
    if (output == NULL) {
        fprintf(stderr, "error: cannot open %s: %s\n",
                part_path, strerror(errno));
        free(final_path);
        free(part_path);
        return EXIT_FAILURE;
    }
    context.output = output;
    context.part_path = part_path;

    fprintf(stderr,
            "333892_02: N=%d, independent uint32 dense modular DP + CRT, "
            "memory limit=%.3f GiB\n",
            maximum_n,
            (double)context.memory_limit /
                (1024.0 * 1024.0 * 1024.0));
    double start = monotonic_seconds();
    mpz_t value;
    mpz_init(value);
    for (int n = 0; n <= maximum_n; ++n) {
        compute_exact_term(value, n, context.memory_limit);
        write_value(&context, n, value);
    }
    mpz_clear(value);
    fprintf(stderr, "333892_02: all terms completed in %.3f s\n",
            monotonic_seconds() - start);

    if (fclose(output) != 0) {
        fprintf(stderr, "error: cannot close %s: %s\n",
                part_path, strerror(errno));
        free(final_path);
        free(part_path);
        return EXIT_FAILURE;
    }
    if (rename(part_path, final_path) != 0) {
        fprintf(stderr, "error: cannot rename %s to %s: %s\n",
                part_path, final_path, strerror(errno));
        free(final_path);
        free(part_path);
        return EXIT_FAILURE;
    }
    fprintf(stderr, "wrote %s (n=0..%d)\n", final_path, maximum_n);
    free(final_path);
    free(part_path);
    return EXIT_SUCCESS;
}
