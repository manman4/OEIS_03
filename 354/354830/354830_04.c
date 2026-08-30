/*
 * A354830 -- independent generalized-Ryser implementation (n <= 45).
 *
 * Count permutations p of {1,...,n} satisfying
 *
 *     gcd(i, p(i)) > 1  for 2 <= i <= n.
 *
 * This program does not read values, residues, or b-files produced by
 * 354830_03.  Its counting algorithm is also different.  As usual p(1)=1,
 * and every prime q>n/2 is a forced fixed point.  Among the remaining
 * vertices, group integers having the same squarefree radical.  Let type j
 * have multiplicity m[j], and let A[i][j] be 1 when the radicals of types i
 * and j have a common prime factor.
 *
 * Applying Ryser's formula after grouping identical rows and columns gives
 *
 *   permanent = (-1)^M sum_{0 <= s[j] <= m[j]} (-1)^sum(s[j])
 *       product_j binomial(m[j],s[j])
 *       product_i (sum_j A[i][j] s[j])^m[i],
 *
 * where M=sum m[j].  A reflected mixed-radix Gray code visits all vectors s,
 * changing one coordinate at a time.  Row sums, binomial weights, and their
 * modular products are updated incrementally, so the implementation uses
 * O(number of types) memory rather than the dense layer DP of 354830_03.
 *
 * Five or fewer disjoint ~2^31 primes are propagated simultaneously.  Fast
 * reduction uses p=2^31-c and is runtime-audited against ordinary remainder.
 * GMP reconstructs the exact integer only after the prime product exceeds
 * the rigorous M! bound; the result is then checked against every residue and
 * against the bound.
 *
 * Safety and independence checks:
 *   - the supported range is explicitly n=0..45;
 *   - every fixed modulus is verified by deterministic 64-bit Miller--Rabin;
 *   - all state-count, binomial-weight, and CRT bounds are checked;
 *   - --check compares reference terms through n=33 and independently brute
 *     forces the original, ungrouped permutation condition through n=10;
 *   - b354830_4.txt is written through a .part file and atomically renamed.
 *
 * Build on Apple Silicon with Homebrew GMP:
 *
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic \
 *       -I/opt/homebrew/opt/gmp/include -L/opt/homebrew/opt/gmp/lib \
 *       354830_04.c -lgmp -o 354830_04
 *
 * Examples:
 *
 *   ./354830_04 --check
 *   ./354830_04 --term 45
 *   ./354830_04 45
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
#error "354830_04 requires a platform with 64-bit unsigned long"
#endif

#define MAX_SUPPORTED_N 45
#define MAX_ACTIVE_VERTICES 44U
#define MAX_TYPES 32U
#define MAX_CRT_PASSES 6U
#define REFERENCE_MAX_N 33
#define DEFAULT_MAX_N 30
#define DEFAULT_CHECK_N 30
#define BRUTE_CHECK_MAX_N 10
#define PROGRESS_STEP UINT64_C(50000000)
#define REDUCTION_BASE UINT64_C(2147483648)
#define REDUCTION_MASK UINT64_C(2147483647)

__extension__ typedef unsigned __int128 uint128_t;

typedef struct {
    uint32_t modulus;
    uint32_t complement;
} PseudoMersennePrime;

/* Disjoint from every prime used by 354830_03.c. */
static const PseudoMersennePrime moduli[MAX_CRT_PASSES] = {
    {UINT32_C(2147483543), UINT32_C(105)},
    {UINT32_C(2147483497), UINT32_C(151)},
    {UINT32_C(2147483489), UINT32_C(159)},
    {UINT32_C(2147483477), UINT32_C(171)},
    {UINT32_C(2147483423), UINT32_C(225)},
    {UINT32_C(2147483399), UINT32_C(249)}
};

static const char *const reference_terms[REFERENCE_MAX_N + 1] = {
    "1", "1", "1", "1", "2", "2", "8", "8", "30", "72",
    "408", "408", "4104", "4104", "29640", "208704",
    "1437312", "1437312", "22653504", "22653504", "318695040",
    "2686493376", "27628410816", "27628410816", "575372874240",
    "1775480841216", "21115550048256", "132879856582656",
    "2321256928702464", "2321256928702464", "83095013944442880",
    "83095013944442880", "1101384298446336000",
    "12425377428266803200"
};

typedef enum {
    COMPUTE_OK = 0,
    COMPUTE_SIZE_OVERFLOW,
    COMPUTE_ARITHMETIC_OVERFLOW,
    COMPUTE_INTERNAL_ERROR
} ComputeStatus;

typedef struct {
    unsigned active_count;
    unsigned forced_count;
    unsigned type_count;
    unsigned radical[MAX_TYPES];
    unsigned multiplicity[MAX_TYPES];
    uint32_t affected_rows[MAX_TYPES];
    uint64_t state_count;
    uint64_t maximum_weight;
} RyserPlan;

typedef struct {
    uint64_t states;
    uint64_t factor_updates;
    size_t bound_bits;
    unsigned passes;
    unsigned active_count;
    unsigned forced_count;
    unsigned type_count;
    double seconds;
} ComputeStats;

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

static bool checked_mul_u64(uint64_t left, uint64_t right,
                            uint64_t *result)
{
    if (left != 0U && right > UINT64_MAX / left) {
        return false;
    }
    *result = left * right;
    return true;
}

static uint64_t saturating_add_u64(uint64_t left, uint64_t right)
{
    return left > UINT64_MAX - right ? UINT64_MAX : left + right;
}

static unsigned gcd_positive(unsigned left, unsigned right)
{
    while (right != 0U) {
        unsigned remainder = left % right;
        left = right;
        right = remainder;
    }
    return left;
}

static bool is_prime_small(unsigned value)
{
    if (value < 2U) {
        return false;
    }
    if ((value & 1U) == 0U) {
        return value == 2U;
    }
    for (unsigned divisor = 3U;
         divisor <= value / divisor; divisor += 2U) {
        if (value % divisor == 0U) {
            return false;
        }
    }
    return true;
}

static unsigned squarefree_radical(unsigned value)
{
    unsigned radical = 1U;
    unsigned remaining = value;
    for (unsigned prime = 2U; prime <= remaining / prime; ++prime) {
        if (remaining % prime == 0U) {
            radical *= prime;
            do {
                remaining /= prime;
            } while (remaining % prime == 0U);
        }
    }
    if (remaining > 1U) {
        radical *= remaining;
    }
    return radical;
}

static int parse_n(const char *text, const char *label)
{
    char *end = NULL;
    errno = 0;
    long value = strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' ||
        value < 0 || value > MAX_SUPPORTED_N) {
        fprintf(stderr, "error: %s must be in 0..%d: %s\n",
                label, MAX_SUPPORTED_N, text);
        exit(EXIT_FAILURE);
    }
    return (int)value;
}

static uint64_t generic_multiply_mod(uint64_t left, uint64_t right,
                                     uint64_t modulus)
{
    return (uint64_t)((uint128_t)left * right % modulus);
}

static uint64_t generic_power_mod(uint64_t base, uint64_t exponent,
                                  uint64_t modulus)
{
    uint64_t result = 1U;
    while (exponent != 0U) {
        if ((exponent & 1U) != 0U) {
            result = generic_multiply_mod(result, base, modulus);
        }
        exponent >>= 1;
        if (exponent != 0U) {
            base = generic_multiply_mod(base, base, modulus);
        }
    }
    return result;
}

static bool is_prime_u64(uint64_t value)
{
    static const uint32_t small_primes[] = {
        2U, 3U, 5U, 7U, 11U, 13U, 17U, 19U, 23U, 29U, 31U, 37U
    };
    static const uint64_t bases[] = {
        UINT64_C(2), UINT64_C(325), UINT64_C(9375),
        UINT64_C(28178), UINT64_C(450775), UINT64_C(9780504),
        UINT64_C(1795265022)
    };
    if (value < 2U) {
        return false;
    }
    for (size_t index = 0U;
         index < sizeof(small_primes) / sizeof(small_primes[0]); ++index) {
        uint64_t prime = small_primes[index];
        if (value % prime == 0U) {
            return value == prime;
        }
    }
    uint64_t odd_part = value - 1U;
    unsigned twos = 0U;
    while ((odd_part & 1U) == 0U) {
        odd_part >>= 1;
        ++twos;
    }
    for (size_t index = 0U; index < sizeof(bases) / sizeof(bases[0]);
         ++index) {
        uint64_t base = bases[index] % value;
        if (base == 0U) {
            continue;
        }
        uint64_t witness = generic_power_mod(base, odd_part, value);
        if (witness == 1U || witness == value - 1U) {
            continue;
        }
        bool passed = false;
        for (unsigned round = 1U; round < twos; ++round) {
            witness = generic_multiply_mod(witness, witness, value);
            if (witness == value - 1U) {
                passed = true;
                break;
            }
        }
        if (!passed) {
            return false;
        }
    }
    return true;
}

static uint32_t fast_multiply_mod(uint32_t left, uint32_t right,
                                  unsigned pass)
{
    const PseudoMersennePrime *prime = &moduli[pass];
    uint64_t product = (uint64_t)left * right;
    uint64_t reduced = (product & REDUCTION_MASK) +
                       (product >> 31) * prime->complement;
    reduced = (reduced & REDUCTION_MASK) +
              (reduced >> 31) * prime->complement;
    while (reduced >= prime->modulus) {
        reduced -= prime->modulus;
    }
    return (uint32_t)reduced;
}

static uint32_t fast_power_mod(uint32_t base, uint32_t exponent,
                               unsigned pass)
{
    uint32_t result = 1U;
    while (exponent != 0U) {
        if ((exponent & 1U) != 0U) {
            result = fast_multiply_mod(result, base, pass);
        }
        exponent >>= 1;
        if (exponent != 0U) {
            base = fast_multiply_mod(base, base, pass);
        }
    }
    return result;
}

static bool validate_moduli(void)
{
    uint64_t random = UINT64_C(0x243f6a8885a308d3);
    for (unsigned pass = 0U; pass < MAX_CRT_PASSES; ++pass) {
        uint32_t prime = moduli[pass].modulus;
        if ((uint64_t)prime + moduli[pass].complement !=
                REDUCTION_BASE ||
            !is_prime_u64(prime)) {
            return false;
        }
        for (unsigned test = 0U; test < 1000U; ++test) {
            random ^= random << 13;
            random ^= random >> 7;
            random ^= random << 17;
            uint32_t left = (uint32_t)(random % prime);
            random ^= random << 13;
            random ^= random >> 7;
            random ^= random << 17;
            uint32_t right = (uint32_t)(random % prime);
            uint32_t expected =
                (uint32_t)((uint64_t)left * right % prime);
            if (fast_multiply_mod(left, right, pass) != expected) {
                return false;
            }
        }
    }
    return true;
}

static uint64_t binomial_small(unsigned n, unsigned k)
{
    if (k > n) {
        return 0U;
    }
    if (k > n - k) {
        k = n - k;
    }
    uint64_t result = 1U;
    for (unsigned index = 1U; index <= k; ++index) {
        result = result * (n - k + index) / index;
    }
    return result;
}

static ComputeStatus build_ryser_plan(int n, RyserPlan *plan)
{
    memset(plan, 0, sizeof(*plan));
    if (n <= 1) {
        plan->forced_count = (unsigned)n;
        plan->state_count = 1U;
        plan->maximum_weight = 1U;
        return COMPUTE_OK;
    }
    unsigned raw_radical[MAX_TYPES] = {0};
    unsigned raw_multiplicity[MAX_TYPES] = {0};
    unsigned raw_count = 0U;
    plan->forced_count = 1U;
    for (unsigned value = 2U; value <= (unsigned)n; ++value) {
        if (is_prime_small(value) && 2U * value > (unsigned)n) {
            ++plan->forced_count;
            continue;
        }
        unsigned radical = squarefree_radical(value);
        unsigned type = 0U;
        while (type < raw_count && raw_radical[type] != radical) {
            ++type;
        }
        if (type == raw_count) {
            if (raw_count >= MAX_TYPES) {
                return COMPUTE_SIZE_OVERFLOW;
            }
            raw_radical[type] = radical;
            ++raw_count;
        }
        ++raw_multiplicity[type];
        ++plan->active_count;
    }
    if (plan->active_count > MAX_ACTIVE_VERTICES || raw_count > 32U) {
        return COMPUTE_SIZE_OVERFLOW;
    }

    bool selected[MAX_TYPES] = {false};
    for (unsigned position = 0U; position < raw_count; ++position) {
        int best = -1;
        unsigned best_degree = UINT_MAX;
        unsigned best_multiplicity = 0U;
        unsigned best_radical = 0U;
        for (unsigned candidate = 0U; candidate < raw_count; ++candidate) {
            if (selected[candidate]) {
                continue;
            }
            unsigned degree = 0U;
            for (unsigned row = 0U; row < raw_count; ++row) {
                if (gcd_positive(raw_radical[row],
                                 raw_radical[candidate]) > 1U) {
                    ++degree;
                }
            }
            if (best < 0 || degree < best_degree ||
                (degree == best_degree &&
                 raw_multiplicity[candidate] > best_multiplicity) ||
                (degree == best_degree &&
                 raw_multiplicity[candidate] == best_multiplicity &&
                 raw_radical[candidate] > best_radical)) {
                best = (int)candidate;
                best_degree = degree;
                best_multiplicity = raw_multiplicity[candidate];
                best_radical = raw_radical[candidate];
            }
        }
        if (best < 0) {
            return COMPUTE_INTERNAL_ERROR;
        }
        selected[(unsigned)best] = true;
        plan->radical[position] = raw_radical[(unsigned)best];
        plan->multiplicity[position] = raw_multiplicity[(unsigned)best];
    }
    plan->type_count = raw_count;

    uint64_t state_count = 1U;
    uint64_t maximum_weight = 1U;
    for (unsigned type = 0U; type < plan->type_count; ++type) {
        if (!checked_mul_u64(state_count,
                             (uint64_t)plan->multiplicity[type] + 1U,
                             &state_count)) {
            return COMPUTE_SIZE_OVERFLOW;
        }
        uint64_t largest_binomial = binomial_small(
            plan->multiplicity[type], plan->multiplicity[type] / 2U);
        if (!checked_mul_u64(maximum_weight, largest_binomial,
                             &maximum_weight)) {
            return COMPUTE_SIZE_OVERFLOW;
        }
    }
    plan->state_count = state_count;
    plan->maximum_weight = maximum_weight;

    for (unsigned column = 0U; column < plan->type_count; ++column) {
        uint32_t rows = 0U;
        for (unsigned row = 0U; row < plan->type_count; ++row) {
            if (gcd_positive(plan->radical[row],
                             plan->radical[column]) > 1U) {
                rows |= UINT32_C(1) << row;
            }
        }
        if ((rows & (UINT32_C(1) << column)) == 0U) {
            return COMPUTE_INTERNAL_ERROR;
        }
        plan->affected_rows[column] = rows;
    }
    return COMPUTE_OK;
}

static ComputeStatus generalized_ryser(uint32_t *residues,
                                       unsigned passes,
                                       const RyserPlan *plan,
                                       int n, bool report,
                                       ComputeStats *stats)
{
    unsigned digits[MAX_TYPES] = {0};
    int direction[MAX_TYPES];
    unsigned row_sum[MAX_TYPES] = {0};
    uint64_t binomial[MAX_TYPES][MAX_ACTIVE_VERTICES + 1U] = {{0}};
    uint32_t up_ratio[MAX_CRT_PASSES][MAX_TYPES]
                     [MAX_ACTIVE_VERTICES + 1U] = {{{0}}};
    uint32_t down_ratio[MAX_CRT_PASSES][MAX_TYPES]
                       [MAX_ACTIVE_VERTICES + 1U] = {{{0}}};
    uint32_t products[MAX_CRT_PASSES];
    uint32_t accumulators[MAX_CRT_PASSES] = {0};

    for (unsigned type = 0U; type < plan->type_count; ++type) {
        direction[type] = 1;
        for (unsigned count = 0U;
             count <= plan->multiplicity[type]; ++count) {
            binomial[type][count] =
                binomial_small(plan->multiplicity[type], count);
        }
    }
    for (unsigned pass = 0U; pass < passes; ++pass) {
        products[pass] = 1U;
        if (plan->maximum_weight >= moduli[pass].modulus) {
            return COMPUTE_INTERNAL_ERROR;
        }
        for (unsigned row = 0U; row < plan->type_count; ++row) {
            uint32_t factors[MAX_ACTIVE_VERTICES + 1U] = {0};
            for (unsigned sum = 1U; sum <= plan->active_count; ++sum) {
                factors[sum] = fast_power_mod(
                    (uint32_t)sum, plan->multiplicity[row], pass);
            }
            for (unsigned sum = 1U; sum < plan->active_count; ++sum) {
                uint32_t inverse = fast_power_mod(
                    factors[sum], moduli[pass].modulus - 2U, pass);
                up_ratio[pass][row][sum] = fast_multiply_mod(
                    factors[sum + 1U], inverse, pass);
            }
            for (unsigned sum = 2U; sum <= plan->active_count; ++sum) {
                uint32_t inverse = fast_power_mod(
                    factors[sum], moduli[pass].modulus - 2U, pass);
                down_ratio[pass][row][sum] = fast_multiply_mod(
                    factors[sum - 1U], inverse, pass);
            }
        }
    }

    unsigned zero_rows = plan->type_count;
    uint64_t weight = 1U;
    bool odd_sum = false;
    uint64_t next_report = PROGRESS_STEP;
    uint64_t factor_updates = 0U;
    double start = monotonic_seconds();

    for (uint64_t visited = 1U; visited < plan->state_count; ++visited) {
        int changed = -1;
        unsigned old_digit = 0U;
        unsigned new_digit = 0U;
        for (unsigned type = 0U; type < plan->type_count; ++type) {
            int candidate = (int)digits[type] + direction[type];
            if (candidate >= 0 &&
                candidate <= (int)plan->multiplicity[type]) {
                changed = (int)type;
                old_digit = digits[type];
                new_digit = (unsigned)candidate;
                digits[type] = new_digit;
                break;
            }
            direction[type] = -direction[type];
        }
        if (changed < 0) {
            return COMPUTE_INTERNAL_ERROR;
        }
        unsigned column = (unsigned)changed;
        uint64_t old_binomial = binomial[column][old_digit];
        uint64_t new_binomial = binomial[column][new_digit];
        if (old_binomial == 0U || weight % old_binomial != 0U) {
            return COMPUTE_INTERNAL_ERROR;
        }
        weight /= old_binomial;
        if (!checked_mul_u64(weight, new_binomial, &weight) ||
            weight > plan->maximum_weight) {
            return COMPUTE_ARITHMETIC_OVERFLOW;
        }
        bool increasing = new_digit > old_digit;
        odd_sum = !odd_sum;

        uint32_t rows = plan->affected_rows[column];
        while (rows != 0U) {
            unsigned row = (unsigned)__builtin_ctz(rows);
            rows &= rows - 1U;
            unsigned old_sum = row_sum[row];
            unsigned new_sum = increasing ? old_sum + 1U : old_sum - 1U;
            if (increasing) {
                if (old_sum == 0U) {
                    if (zero_rows == 0U || new_sum != 1U) {
                        return COMPUTE_INTERNAL_ERROR;
                    }
                    --zero_rows;
                } else {
                    for (unsigned pass = 0U; pass < passes; ++pass) {
                        products[pass] = fast_multiply_mod(
                            products[pass], up_ratio[pass][row][old_sum],
                            pass);
                    }
                }
            } else {
                if (new_sum == 0U) {
                    if (old_sum != 1U || zero_rows >= plan->type_count) {
                        return COMPUTE_INTERNAL_ERROR;
                    }
                    ++zero_rows;
                } else {
                    for (unsigned pass = 0U; pass < passes; ++pass) {
                        products[pass] = fast_multiply_mod(
                            products[pass], down_ratio[pass][row][old_sum],
                            pass);
                    }
                }
            }
            row_sum[row] = new_sum;
            factor_updates = saturating_add_u64(factor_updates, 1U);
        }

        if (zero_rows == 0U) {
            bool positive = ((plan->active_count & 1U) != 0U) == odd_sum;
            for (unsigned pass = 0U; pass < passes; ++pass) {
                uint32_t term = fast_multiply_mod(
                    products[pass], (uint32_t)weight, pass);
                uint32_t prime = moduli[pass].modulus;
                if (positive) {
                    uint64_t sum = (uint64_t)accumulators[pass] + term;
                    accumulators[pass] =
                        (uint32_t)(sum >= prime ? sum - prime : sum);
                } else {
                    accumulators[pass] = accumulators[pass] >= term
                        ? accumulators[pass] - term
                        : prime - (term - accumulators[pass]);
                }
            }
        }
        uint64_t completed = visited + 1U;
        if (report && completed >= next_report) {
            fprintf(stderr,
                    "354830_04: n=%d states=%" PRIu64 "/%" PRIu64
                    " (%.1f%%), factor updates=%" PRIu64 ", %.3f s\n",
                    n, completed, plan->state_count,
                    100.0 * (double)completed / (double)plan->state_count,
                    factor_updates, monotonic_seconds() - start);
            next_report = next_report > UINT64_MAX - PROGRESS_STEP
                              ? UINT64_MAX
                              : next_report + PROGRESS_STEP;
        }
    }
    for (unsigned pass = 0U; pass < passes; ++pass) {
        residues[pass] = accumulators[pass];
    }
    stats->states = plan->state_count;
    stats->factor_updates = factor_updates;
    return COMPUTE_OK;
}

static ComputeStatus reconstruct_crt(mpz_t result,
                                     const uint32_t *residues,
                                     unsigned passes,
                                     const mpz_t bound)
{
    mpz_t product;
    mpz_init_set_ui(product, 1U);
    mpz_set_ui(result, 0U);
    for (unsigned pass = 0U; pass < passes; ++pass) {
        uint32_t prime = moduli[pass].modulus;
        uint32_t result_mod =
            (uint32_t)mpz_fdiv_ui(result, (unsigned long)prime);
        uint32_t product_mod =
            (uint32_t)mpz_fdiv_ui(product, (unsigned long)prime);
        if (product_mod == 0U) {
            mpz_clear(product);
            return COMPUTE_INTERNAL_ERROR;
        }
        uint32_t inverse = fast_power_mod(
            product_mod, prime - 2U, pass);
        if (fast_multiply_mod(product_mod, inverse, pass) != 1U) {
            mpz_clear(product);
            return COMPUTE_INTERNAL_ERROR;
        }
        uint32_t difference = residues[pass] >= result_mod
                                  ? residues[pass] - result_mod
                                  : prime - (result_mod - residues[pass]);
        uint32_t multiplier = fast_multiply_mod(
            difference, inverse, pass);
        mpz_addmul_ui(result, product, (unsigned long)multiplier);
        mpz_mul_ui(product, product, (unsigned long)prime);
    }
    if (mpz_cmp(product, bound) <= 0 || mpz_sgn(result) < 0 ||
        mpz_cmp(result, bound) > 0) {
        mpz_clear(product);
        return COMPUTE_INTERNAL_ERROR;
    }
    for (unsigned pass = 0U; pass < passes; ++pass) {
        if (mpz_fdiv_ui(result, (unsigned long)moduli[pass].modulus) !=
            residues[pass]) {
            mpz_clear(product);
            return COMPUTE_INTERNAL_ERROR;
        }
    }
    mpz_clear(product);
    return COMPUTE_OK;
}

static ComputeStatus compute_exact(mpz_t result, int n, bool report,
                                   ComputeStats *stats)
{
    memset(stats, 0, sizeof(*stats));
    double start = monotonic_seconds();
    if (!validate_moduli()) {
        return COMPUTE_INTERNAL_ERROR;
    }
    RyserPlan plan;
    ComputeStatus status = build_ryser_plan(n, &plan);
    stats->active_count = plan.active_count;
    stats->forced_count = plan.forced_count;
    stats->type_count = plan.type_count;
    if (status != COMPUTE_OK) {
        stats->seconds = monotonic_seconds() - start;
        return status;
    }
    mpz_t bound;
    mpz_init(bound);
    mpz_fac_ui(bound, (unsigned long)plan.active_count);
    stats->bound_bits = mpz_sizeinbase(bound, 2);
    if (plan.active_count == 0U) {
        mpz_set_ui(result, 1U);
        stats->states = 1U;
        stats->seconds = monotonic_seconds() - start;
        mpz_clear(bound);
        return COMPUTE_OK;
    }

    mpz_t prime_product;
    mpz_init_set_ui(prime_product, 1U);
    unsigned passes = 0U;
    while (mpz_cmp(prime_product, bound) <= 0) {
        if (passes >= MAX_CRT_PASSES) {
            mpz_clear(prime_product);
            mpz_clear(bound);
            return COMPUTE_INTERNAL_ERROR;
        }
        mpz_mul_ui(prime_product, prime_product,
                   (unsigned long)moduli[passes].modulus);
        ++passes;
    }
    mpz_clear(prime_product);
    stats->passes = passes;
    if (report) {
        fprintf(stderr,
                "354830_04: n=%d independent generalized Ryser, "
                "active=%u forced=%u types=%u, states=%" PRIu64
                ", exact bound=%zu bits, %u simultaneous prime%s\n",
                n, plan.active_count, plan.forced_count, plan.type_count,
                plan.state_count, stats->bound_bits, passes,
                passes == 1U ? "" : "s");
    }
    uint32_t residues[MAX_CRT_PASSES] = {0};
    status = generalized_ryser(residues, passes, &plan, n, report, stats);
    if (status == COMPUTE_OK) {
        status = reconstruct_crt(result, residues, passes, bound);
    }
    mpz_clear(bound);
    stats->seconds = monotonic_seconds() - start;
    return status;
}

static uint64_t brute_force_rec(unsigned row, unsigned n,
                                uint64_t used_columns)
{
    if (row > n) {
        return 1U;
    }
    uint64_t total = 0U;
    for (unsigned column = 1U; column <= n; ++column) {
        uint64_t bit = UINT64_C(1) << (column - 1U);
        if ((used_columns & bit) == 0U &&
            (row == 1U || gcd_positive(row, column) > 1U)) {
            total += brute_force_rec(row + 1U, n,
                                     used_columns | bit);
        }
    }
    return total;
}

static const char *status_text(ComputeStatus status)
{
    switch (status) {
    case COMPUTE_OK:
        return "ok";
    case COMPUTE_SIZE_OVERFLOW:
        return "state-count or size calculation overflowed";
    case COMPUTE_ARITHMETIC_OVERFLOW:
        return "incremental exact weight overflowed its bound";
    case COMPUTE_INTERNAL_ERROR:
        return "internal consistency check failed";
    }
    return "unknown error";
}

static void verify_reference_value(const mpz_t value, int n)
{
    if (n < 0 || n > REFERENCE_MAX_N) {
        return;
    }
    mpz_t expected;
    mpz_init(expected);
    if (mpz_set_str(expected, reference_terms[n], 10) != 0) {
        mpz_clear(expected);
        die("invalid built-in A354830 reference term");
    }
    if (mpz_cmp(value, expected) != 0) {
        gmp_fprintf(stderr,
                    "error: A354830 reference check failed at n=%d: "
                    "got %Zd, expected %Zd\n",
                    n, value, expected);
        mpz_clear(expected);
        exit(EXIT_FAILURE);
    }
    mpz_clear(expected);
}

static void compute_checked(mpz_t value, int n, bool report)
{
    ComputeStats stats;
    ComputeStatus status = compute_exact(value, n, report, &stats);
    if (status != COMPUTE_OK) {
        fprintf(stderr, "error: A354830 n=%d: %s\n",
                n, status_text(status));
        exit(EXIT_FAILURE);
    }
    verify_reference_value(value, n);
    if (report) {
        fprintf(stderr,
                "354830_04: n=%d done, states=%" PRIu64
                ", factor updates=%" PRIu64 ", %.3f s\n",
                n, stats.states, stats.factor_updates, stats.seconds);
    }
}

static int check_terms(int max_n)
{
    if (max_n > REFERENCE_MAX_N) {
        fprintf(stderr,
                "error: --check maximum is %d because later reference "
                "terms are not built in\n",
                REFERENCE_MAX_N);
        return EXIT_FAILURE;
    }
    mpz_t value;
    mpz_init(value);
    for (int n = 0; n <= max_n; ++n) {
        compute_checked(value, n, false);
        if (n <= BRUTE_CHECK_MAX_N) {
            uint64_t brute = brute_force_rec(1U, (unsigned)n, 0U);
            if (mpz_cmp_ui(value, (unsigned long)brute) != 0) {
                gmp_fprintf(stderr,
                            "error: generalized Ryser and original brute "
                            "force disagree at n=%d: %Zd versus %" PRIu64
                            "\n",
                            n, value, brute);
                mpz_clear(value);
                return EXIT_FAILURE;
            }
        }
    }
    mpz_clear(value);
    printf("ok: independent generalized-Ryser computation agrees with "
           "A354830 reference terms for n=0..%d; original permutation "
           "brute force agrees for n=0..%d\n",
           max_n, max_n < BRUTE_CHECK_MAX_N
                      ? max_n : BRUTE_CHECK_MAX_N);
    return EXIT_SUCCESS;
}

static char *path_beside_executable(const char *argv0,
                                    const char *filename)
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
                              sizeof(executable) - 1U);
    if (length >= 0) {
        executable[length] = '\0';
        have_path = true;
    }
#endif
    if (!have_path) {
        size_t length = strlen(argv0);
        if (length >= sizeof(executable)) {
            die("executable path is too long");
        }
        memcpy(executable, argv0, length + 1U);
    }
    const char *base = realpath(executable, resolved);
    if (base == NULL) {
        base = executable;
    }
    const char *slash = strrchr(base, '/');
    size_t directory_length = slash == NULL ? 1U : (size_t)(slash - base);
    size_t filename_length = strlen(filename);
    if (directory_length > SIZE_MAX - filename_length - 2U) {
        die("output path length overflow");
    }
    size_t path_size = directory_length + filename_length + 2U;
    char *path = malloc(path_size);
    if (path == NULL) {
        die("could not allocate output path");
    }
    if (slash == NULL) {
        path[0] = '.';
    } else if (directory_length == 0U) {
        path[0] = '/';
        directory_length = 1U;
    } else {
        memcpy(path, base, directory_length);
    }
    path[directory_length] = '/';
    memcpy(path + directory_length + 1U, filename,
           filename_length + 1U);
    return path;
}

static void produce_b_file(const char *argv0, int max_n)
{
    char *path = path_beside_executable(argv0, "b354830_4.txt");
    size_t path_length = strlen(path);
    if (path_length > SIZE_MAX - strlen(".part") - 1U) {
        free(path);
        die("temporary output path length overflow");
    }
    size_t part_size = path_length + strlen(".part") + 1U;
    char *part = malloc(part_size);
    if (part == NULL) {
        free(path);
        die("could not allocate temporary output path");
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
        compute_checked(value, n, true);
        if (gmp_fprintf(stream, "%d %Zd\n", n, value) < 0 ||
            fflush(stream) != 0) {
            mpz_clear(value);
            fclose(stream);
            free(part);
            free(path);
            die("could not write the A354830 b-file");
        }
    }
    mpz_clear(value);
    if (fclose(stream) != 0) {
        free(part);
        free(path);
        die("could not close the A354830 b-file");
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
            "       %s --term N\n"
            "       %s --check [MAX_N]\n"
            "\n"
            "MAX_N defaults to %d and is limited to %d.\n"
            "All values are independently computed by 354830_04; "
            "354830_03 output is never read.\n",
            program, program, program, DEFAULT_MAX_N, MAX_SUPPORTED_N);
}

int main(int argc, char **argv)
{
    if (argc >= 2 && strcmp(argv[1], "--check") == 0) {
        if (argc > 3) {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
        int max_n = argc == 3
                        ? parse_n(argv[2], "check MAX_N")
                        : DEFAULT_CHECK_N;
        return check_terms(max_n);
    }
    if (argc >= 2 && strcmp(argv[1], "--term") == 0) {
        if (argc != 3) {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
        int n = parse_n(argv[2], "N");
        mpz_t value;
        mpz_init(value);
        compute_checked(value, n, true);
        gmp_printf("%d %Zd\n", n, value);
        mpz_clear(value);
        return EXIT_SUCCESS;
    }
    if (argc > 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    int max_n = argc == 2 ? parse_n(argv[1], "MAX_N")
                          : DEFAULT_MAX_N;
    produce_b_file(argv[0], max_n);
    return EXIT_SUCCESS;
}
