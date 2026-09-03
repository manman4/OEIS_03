/*
 * A399568 -- endpoint split, dense modular DP, and exact CRT.
 *
 * Count permutations p of [n] such that
 *
 *     p(j) does not divide Sum_{i=1..j} p(i),   2 <= j < n.
 *
 * This is deliberately different from 399568_01.c.  Program 01 uses exact
 * unsigned-128-bit counts in two colexicographically ranked cardinality
 * layers.  This program uses the following endpoint decomposition, one flat
 * array indexed directly by an ordinary subset mask, modular arithmetic, and
 * Chinese remaindering.
 *
 * The value 1 divides every integer, so it cannot occur at an interior
 * position.  For n>=2 it is therefore either first or last, and these cases
 * are disjoint.
 *
 *   F(S): valid orders of S subseteq {2,...,n} following an initial 1.
 *         For a proper nonempty S, a possible last element x must not divide
 *         1+sum(S).  At S={2,...,n}, x is the unrestricted final term.
 *
 *   L(S): valid orders of S when the final 1 is not yet present.  Singletons
 *         have value 1; thereafter a possible last element x must not divide
 *         sum(S), including at S={2,...,n}.
 *
 * For each 31-bit prime, F(S) and L(S) are packed into the low and high halves
 * of one uint64_t.  Four sequential prime passes suffice because
 *
 *                  0 <= a(n) <= 2*(n-1)!  (n>=2),
 *
 * and their product exceeds this bound through n=30.  Incremental CRT in an
 * unsigned 128-bit integer then recovers the unique exact answer.  Every
 * modulus is verified for primality, the CRT product is checked against the
 * bound, and the reconstructed answer is replayed modulo every prime.
 *
 * There are 2^(n-1) packed states.  At n=30 the sole main array is exactly
 * 4 GiB; CRT passes reuse it, so peak explicit storage remains below 6 GiB.
 * Runtime is O(n*2^n), and working memory is O(2^(n-1)).
 *
 * A normal run writes b399568_02.txt atomically and also prints its lines.
 * --check compares this algorithm with a definition-level exhaustive DFS for
 * n<=10.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic \
 *       399568_02.c -o 399568_02
 *
 * Usage:
 *   ./399568_02                 # save a(0)..a(20)
 *   ./399568_02 25
 *   ./399568_02 --upto 30
 *   ./399568_02 --term 25
 *   ./399568_02 --check
 */

#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if !defined(__SIZEOF_INT128__)
#error "399568_02.c requires unsigned __int128"
#endif

#if SIZE_MAX < UINT64_MAX
#error "399568_02.c requires a 64-bit size_t"
#endif

__extension__ typedef unsigned __int128 U128;

#define DEFAULT_MAX_N 20U
#define MAX_N 30U
#define DIRECT_CHECK_MAX_N 10U
#define MODULUS_COUNT 4U
#define MASK_HALF_BITS 15U
#define MASK_HALF_SIZE (UINT32_C(1) << MASK_HALF_BITS)
#define MASK_HALF_MASK (MASK_HALF_SIZE - 1U)
#define MAX_TOTAL_SUM (MAX_N * (MAX_N + 1U) / 2U)
#define MEMORY_LIMIT_BYTES (UINT64_C(6) * 1024U * 1024U * 1024U)
#define REPORT_PARTS 8U
#define REPORT_MIN_STATES (UINT32_C(1) << 24U)
#define MERSENNE_BITS 31U
#define MERSENNE_BASE UINT64_C(2147483648)
#define MERSENNE_MASK UINT64_C(2147483647)
#define BFILE_PATH "b399568_02.txt"
#define PART_PATH "b399568_02.txt.part"

static const uint32_t available_moduli[MODULUS_COUNT] = {
    UINT32_C(2147483647),
    UINT32_C(2147483629),
    UINT32_C(2147483587),
    UINT32_C(2147483579)
};

typedef enum {
    MODE_UPTO,
    MODE_TERM,
    MODE_CHECK
} Mode;

static bool quiet;
static uint16_t low_subset_sum[MASK_HALF_SIZE];
static uint16_t high_subset_sum[MASK_HALF_SIZE];
static bool subset_sums_ready;

static _Noreturn void die(const char *message)
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

static unsigned parse_unsigned(const char *text, unsigned maximum,
                               const char *label)
{
    char *end = NULL;
    errno = 0;
    const unsigned long value = strtoul(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' || value > maximum) {
        fprintf(stderr, "error: %s must be in 0..%u: %s\n",
                label, maximum, text);
        exit(EXIT_FAILURE);
    }
    return (unsigned)value;
}

static U128 checked_add(U128 left, U128 right)
{
    const U128 maximum = ~(U128)0;
    if (left > maximum - right) {
        die("unsigned 128-bit addition overflow");
    }
    return left + right;
}

static U128 checked_multiply(U128 left, uint32_t right)
{
    const U128 maximum = ~(U128)0;
    if (right != 0U && left > maximum / right) {
        die("unsigned 128-bit multiplication overflow");
    }
    return left * right;
}

static int print_u128(FILE *stream, U128 value)
{
    char reverse[40];
    char text[40];
    size_t length = 0U;
    do {
        reverse[length++] = (char)('0' + (unsigned)(value % 10U));
        value /= 10U;
    } while (value != 0U);
    for (size_t i = 0U; i < length; ++i) {
        text[i] = reverse[length - 1U - i];
    }
    text[length] = '\0';
    return fputs(text, stream) == EOF ? -1 : 0;
}

static unsigned u128_bit_length(U128 value)
{
    unsigned bits = 0U;
    while (value != 0U) {
        ++bits;
        value >>= 1U;
    }
    return bits;
}

static bool is_prime_u32(uint32_t value)
{
    if (value < 2U) {
        return false;
    }
    if ((value & 1U) == 0U) {
        return value == 2U;
    }
    for (uint32_t divisor = 3U;
         divisor <= value / divisor; divisor += 2U) {
        if (value % divisor == 0U) {
            return false;
        }
    }
    return true;
}

static void validate_moduli(void)
{
    for (unsigned i = 0U; i < MODULUS_COUNT; ++i) {
        const uint32_t modulus = available_moduli[i];
        if (!is_prime_u32(modulus)) {
            die("configured CRT modulus is not prime");
        }
        if ((uint64_t)modulus >= MERSENNE_BASE ||
            MERSENNE_BASE - modulus > 69U) {
            die("configured CRT modulus is outside reduction bounds");
        }
        for (unsigned j = 0U; j < i; ++j) {
            if (modulus == available_moduli[j]) {
                die("configured CRT moduli are not distinct");
            }
        }
    }
}

static U128 endpoint_bound(unsigned n)
{
    if (n < 2U) {
        return 1U;
    }
    U128 factorial = 1U;
    for (unsigned value = 2U; value < n; ++value) {
        factorial = checked_multiply(factorial, value);
    }
    return checked_multiply(factorial, 2U);
}

static unsigned select_moduli(unsigned n, uint32_t moduli[MODULUS_COUNT],
                              U128 *product_result)
{
    const U128 bound = endpoint_bound(n);
    U128 product = 1U;
    unsigned count = 0U;
    while (product <= bound) {
        if (count == MODULUS_COUNT) {
            die("configured CRT product does not exceed endpoint bound");
        }
        moduli[count] = available_moduli[count];
        product = checked_multiply(product, moduli[count]);
        ++count;
    }
    *product_result = product;
    return count;
}

static void prepare_subset_sums(void)
{
    if (subset_sums_ready) {
        return;
    }
    for (uint32_t mask = 1U; mask < MASK_HALF_SIZE; ++mask) {
        const unsigned bit = (unsigned)__builtin_ctz(mask);
        const uint32_t without = mask & (mask - 1U);
        low_subset_sum[mask] =
            (uint16_t)(low_subset_sum[without] + bit + 2U);
        high_subset_sum[mask] =
            (uint16_t)(high_subset_sum[without] + bit + 17U);
    }
    subset_sums_ready = true;
}

static inline unsigned subset_sum(uint32_t mask)
{
    return (unsigned)low_subset_sum[mask & MASK_HALF_MASK] +
           (unsigned)high_subset_sum[mask >> MASK_HALF_BITS];
}

static void prepare_nondivisor_masks(unsigned n,
                                     uint32_t masks[MAX_TOTAL_SUM + 2U])
{
    memset(masks, 0, (MAX_TOTAL_SUM + 2U) * sizeof(*masks));
    const unsigned maximum_sum = n * (n + 1U) / 2U;
    for (unsigned sum = 0U; sum <= maximum_sum + 1U; ++sum) {
        uint32_t mask = 0U;
        for (unsigned value = 2U; value <= n; ++value) {
            if (sum % value != 0U) {
                mask |= UINT32_C(1) << (value - 2U);
            }
        }
        masks[sum] = mask;
    }
}

static inline uint32_t reduce_pseudo_mersenne(uint64_t value,
                                               uint32_t modulus)
{
    const uint64_t gap = MERSENNE_BASE - modulus;
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

static inline uint64_t pack_pair(uint32_t first_case, uint32_t last_case)
{
    return (uint64_t)first_case | ((uint64_t)last_case << 32U);
}

static uint32_t modular_pass(uint64_t *dp, unsigned n, uint32_t modulus,
                             unsigned pass, unsigned pass_count)
{
    const unsigned mask_bits = n - 1U;
    const uint32_t state_count = UINT32_C(1) << mask_bits;
    const uint32_t full = state_count - 1U;
    uint32_t nondivisor_masks[MAX_TOTAL_SUM + 2U];
    prepare_nondivisor_masks(n, nondivisor_masks);

    dp[0] = pack_pair(1U, 0U);
    const uint32_t report_step = state_count / REPORT_PARTS;
    uint32_t next_report = report_step;
    const double started = monotonic_seconds();

    for (uint32_t mask = 1U; mask <= full; ++mask) {
        if ((mask & (mask - 1U)) == 0U) {
            dp[mask] = pack_pair(1U, 1U);
        } else {
            const unsigned sum = subset_sum(mask);
            const uint32_t first_candidates = mask == full ? mask :
                mask & nondivisor_masks[sum + 1U];
            const uint32_t last_candidates =
                mask & nondivisor_masks[sum];
            uint32_t candidates = first_candidates | last_candidates;
            uint64_t first_sum = 0U;
            uint64_t last_sum = 0U;
            while (candidates != 0U) {
                const uint32_t bit = candidates & (0U - candidates);
                candidates ^= bit;
                const uint64_t predecessor = dp[mask ^ bit];
                if ((first_candidates & bit) != 0U) {
                    first_sum += (uint32_t)predecessor;
                }
                if ((last_candidates & bit) != 0U) {
                    last_sum += (uint32_t)(predecessor >> 32U);
                }
            }
            dp[mask] = pack_pair(
                reduce_pseudo_mersenne(first_sum, modulus),
                reduce_pseudo_mersenne(last_sum, modulus));
        }

        if (!quiet && state_count >= REPORT_MIN_STATES &&
            mask == next_report && mask < full) {
            fprintf(stderr,
                    "399568_02: n=%u CRT pass %u/%u %.1f%% time=%.1fs\n",
                    n, pass + 1U, pass_count,
                    100.0 * (double)mask / (double)state_count,
                    monotonic_seconds() - started);
            next_report += report_step;
        }
    }

    const uint32_t first_residue = (uint32_t)dp[full];
    const uint32_t last_residue = (uint32_t)(dp[full] >> 32U);
    const uint64_t total = (uint64_t)first_residue + last_residue;
    const uint32_t residue = total >= modulus ?
        (uint32_t)(total - modulus) : (uint32_t)total;
    if (!quiet) {
        fprintf(stderr,
                "399568_02: n=%u CRT pass %u/%u p=%" PRIu32
                " done time=%.3fs\n",
                n, pass + 1U, pass_count, modulus,
                monotonic_seconds() - started);
    }
    return residue;
}

static uint32_t multiply_mod(uint32_t left, uint32_t right,
                             uint32_t modulus)
{
    return (uint32_t)(((uint64_t)left * right) % modulus);
}

static uint32_t power_mod(uint32_t base, uint32_t exponent,
                          uint32_t modulus)
{
    uint32_t result = 1U;
    while (exponent != 0U) {
        if ((exponent & 1U) != 0U) {
            result = multiply_mod(result, base, modulus);
        }
        base = multiply_mod(base, base, modulus);
        exponent >>= 1U;
    }
    return result;
}

static U128 reconstruct_crt(const uint32_t *residues,
                            const uint32_t *moduli, unsigned count)
{
    U128 result = 0U;
    U128 product = 1U;
    for (unsigned i = 0U; i < count; ++i) {
        const uint32_t modulus = moduli[i];
        const uint32_t product_mod = (uint32_t)(product % modulus);
        const uint32_t inverse =
            power_mod(product_mod, modulus - 2U, modulus);
        const uint32_t result_mod = (uint32_t)(result % modulus);
        const uint32_t difference = residues[i] >= result_mod ?
            residues[i] - result_mod :
            modulus - (result_mod - residues[i]);
        const uint32_t multiplier =
            multiply_mod(difference, inverse, modulus);
        result = checked_add(result, checked_multiply(product, multiplier));
        product = checked_multiply(product, modulus);
    }
    return result;
}

static void check_modular_arithmetic(void)
{
    for (unsigned i = 0U; i < MODULUS_COUNT; ++i) {
        const uint32_t modulus = available_moduli[i];
        const uint64_t samples[] = {
            0U,
            1U,
            (uint64_t)modulus - 1U,
            modulus,
            (uint64_t)modulus + 1U,
            UINT64_C(29) * ((uint64_t)modulus - 1U)
        };
        for (size_t j = 0U; j < sizeof(samples) / sizeof(samples[0]); ++j) {
            if (reduce_pseudo_mersenne(samples[j], modulus) !=
                samples[j] % modulus) {
                die("pseudo-Mersenne reduction self-test failed");
            }
        }
    }

    uint32_t moduli[MODULUS_COUNT] = {0U, 0U, 0U, 0U};
    uint32_t residues[MODULUS_COUNT] = {0U, 0U, 0U, 0U};
    U128 product;
    const unsigned count = select_moduli(MAX_N, moduli, &product);
    if (count != MODULUS_COUNT || product <= endpoint_bound(MAX_N)) {
        die("four-modulus CRT coverage self-test failed");
    }
    const U128 sample = endpoint_bound(MAX_N) - UINT32_C(12345);
    for (unsigned i = 0U; i < count; ++i) {
        residues[i] = (uint32_t)(sample % moduli[i]);
    }
    if (reconstruct_crt(residues, moduli, count) != sample) {
        die("four-modulus CRT reconstruction self-test failed");
    }
}

static U128 count_exact(unsigned n)
{
    if (n < 2U) {
        return 1U;
    }

    prepare_subset_sums();
    uint32_t moduli[MODULUS_COUNT] = {0U, 0U, 0U, 0U};
    uint32_t residues[MODULUS_COUNT] = {0U, 0U, 0U, 0U};
    U128 crt_product;
    const unsigned count = select_moduli(n, moduli, &crt_product);
    const uint64_t state_count = UINT64_C(1) << (n - 1U);
    const uint64_t bytes = state_count * sizeof(uint64_t);
    if (bytes > MEMORY_LIMIT_BYTES || bytes > SIZE_MAX) {
        die("dense DP array exceeds the 6 GiB memory limit");
    }
    uint64_t *dp = malloc((size_t)bytes);
    if (dp == NULL) {
        fprintf(stderr,
                "error: cannot allocate dense DP array for n=%u (%.3f GiB)\n",
                n, (double)bytes / (1024.0 * 1024.0 * 1024.0));
        exit(EXIT_FAILURE);
    }

    const double started = monotonic_seconds();
    if (!quiet) {
        fprintf(stderr,
                "399568_02: n=%u states=%" PRIu64 " memory=%.3f GiB"
                " bound=%u bits CRT_passes=%u\n",
                n, state_count,
                (double)bytes / (1024.0 * 1024.0 * 1024.0),
                u128_bit_length(endpoint_bound(n)), count);
    }
    for (unsigned pass = 0U; pass < count; ++pass) {
        residues[pass] = modular_pass(
            dp, n, moduli[pass], pass, count);
    }
    free(dp);

    const U128 answer = reconstruct_crt(residues, moduli, count);
    const U128 bound = endpoint_bound(n);
    if (crt_product <= bound) {
        die("CRT product does not prove unique reconstruction");
    }
    if (answer > bound) {
        die("CRT answer exceeds the rigorous endpoint bound");
    }
    for (unsigned pass = 0U; pass < count; ++pass) {
        if ((uint32_t)(answer % moduli[pass]) != residues[pass]) {
            die("CRT residue replay failed");
        }
    }
    if (!quiet) {
        fprintf(stderr, "399568_02: n=%u exact answer done time=%.3fs\n",
                n, monotonic_seconds() - started);
    }
    return answer;
}

/* Independent definition-level DFS used only by --check. */
static U128 direct_search(unsigned n, unsigned position, uint32_t used,
                          unsigned preceding_sum)
{
    if (position == n) {
        return 1U;
    }
    U128 count = 0U;
    for (unsigned value = 1U; value <= n; ++value) {
        const uint32_t bit = UINT32_C(1) << (value - 1U);
        if ((used & bit) != 0U) {
            continue;
        }
        const bool interior = position >= 1U && position + 1U < n;
        if (interior && preceding_sum % value == 0U) {
            continue;
        }
        count = checked_add(count, direct_search(
            n, position + 1U, used | bit, preceding_sum + value));
    }
    return count;
}

static int write_bfile_line(FILE *stream, unsigned n, U128 value)
{
    if (fprintf(stream, "%u ", n) < 0 || print_u128(stream, value) != 0 ||
        fputc('\n', stream) == EOF) {
        return -1;
    }
    return 0;
}

static void write_sequence(unsigned maximum)
{
    FILE *output = fopen(PART_PATH, "w");
    if (output == NULL) {
        fprintf(stderr, "error: cannot open %s: %s\n",
                PART_PATH, strerror(errno));
        exit(EXIT_FAILURE);
    }
    for (unsigned n = 0U; n <= maximum; ++n) {
        const U128 value = count_exact(n);
        if (write_bfile_line(stdout, n, value) != 0 ||
            write_bfile_line(output, n, value) != 0 ||
            fflush(stdout) != 0 || fflush(output) != 0) {
            (void)fclose(output);
            fprintf(stderr, "error: cannot write output: %s\n",
                    strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    if (fclose(output) != 0) {
        fprintf(stderr, "error: cannot close %s: %s\n",
                PART_PATH, strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (rename(PART_PATH, BFILE_PATH) != 0) {
        fprintf(stderr, "error: cannot rename %s to %s: %s\n",
                PART_PATH, BFILE_PATH, strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (!quiet) {
        fprintf(stderr, "saved: %s; n=0..%u\n", BFILE_PATH, maximum);
    }
}

static void check_implementation(unsigned maximum)
{
    check_modular_arithmetic();
    const bool saved_quiet = quiet;
    quiet = true;
    for (unsigned n = 0U; n <= maximum; ++n) {
        const U128 modular = count_exact(n);
        const U128 direct = direct_search(n, 0U, 0U, 0U);
        if (modular != direct) {
            fprintf(stderr, "error: modular/direct mismatch at n=%u: ", n);
            (void)print_u128(stderr, modular);
            fputs(" != ", stderr);
            (void)print_u128(stderr, direct);
            fputc('\n', stderr);
            exit(EXIT_FAILURE);
        }
    }
    quiet = saved_quiet;
    printf("ok: endpoint/CRT DP agrees with direct enumeration for n=0..%u\n",
           maximum);
}

static void usage(const char *program, FILE *stream)
{
    fprintf(stream,
            "Usage:\n"
            "  %s [MAX_N] [--quiet]\n"
            "  %s --upto MAX_N [--quiet]\n"
            "  %s --term N [--quiet]\n"
            "  %s --check [CHECK_N]\n"
            "\n"
            "MAX_N and N may be 0..%u; the default MAX_N is %u.\n"
            "CHECK_N may be 0..%u and defaults to %u.\n"
            "A normal run saves a(0)..a(MAX_N) as %s.\n"
            "At n=30 the main array is 4 GiB.\n",
            program, program, program, program,
            MAX_N, DEFAULT_MAX_N,
            DIRECT_CHECK_MAX_N, DIRECT_CHECK_MAX_N, BFILE_PATH);
}

int main(int argc, char **argv)
{
    validate_moduli();
    Mode mode = MODE_UPTO;
    bool mode_seen = false;
    bool n_seen = false;
    unsigned n = DEFAULT_MAX_N;

    for (int i = 1; i < argc; ++i) {
        const char *argument = argv[i];
        if (strcmp(argument, "--help") == 0 || strcmp(argument, "-h") == 0) {
            usage(argv[0], stdout);
            return EXIT_SUCCESS;
        }
        if (strcmp(argument, "--quiet") == 0 || strcmp(argument, "-q") == 0) {
            quiet = true;
            continue;
        }
        if (strcmp(argument, "--term") == 0 ||
            strcmp(argument, "--upto") == 0) {
            if (mode_seen || n_seen || i + 1 >= argc) {
                usage(argv[0], stderr);
                return EXIT_FAILURE;
            }
            mode = strcmp(argument, "--term") == 0 ? MODE_TERM : MODE_UPTO;
            mode_seen = true;
            n = parse_unsigned(argv[++i], MAX_N,
                               mode == MODE_TERM ? "N" : "MAX_N");
            n_seen = true;
            continue;
        }
        if (strcmp(argument, "--check") == 0) {
            if (mode_seen || n_seen) {
                usage(argv[0], stderr);
                return EXIT_FAILURE;
            }
            mode = MODE_CHECK;
            mode_seen = true;
            n = DIRECT_CHECK_MAX_N;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                n = parse_unsigned(argv[++i], DIRECT_CHECK_MAX_N, "CHECK_N");
                n_seen = true;
            }
            continue;
        }
        if (argument[0] == '-' || mode_seen || n_seen) {
            usage(argv[0], stderr);
            return EXIT_FAILURE;
        }
        n = parse_unsigned(argument, MAX_N, "MAX_N");
        n_seen = true;
    }

    if (mode == MODE_CHECK) {
        check_implementation(n);
    } else if (mode == MODE_TERM) {
        const U128 value = count_exact(n);
        if (print_u128(stdout, value) != 0 || putchar('\n') == EOF ||
            fflush(stdout) != 0) {
            die("cannot write standard output");
        }
    } else {
        write_sequence(n);
    }
    return EXIT_SUCCESS;
}
