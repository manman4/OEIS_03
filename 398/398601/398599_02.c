/*
 * A398599 and b_{1,s}(n) -- q target-assignment convolution.
 *
 * Count permutations p of {1,...,n} satisfying
 *
 *        |p(i+1) - p(i)| != s,   1 <= i < n.
 *
 * The default s=3 gives A398599.  The command line accepts 1<=s<=128.
 *
 * Apply inclusion-exclusion to selected adjacency constraints in the
 * position path 1--2--...--n.  Let h_l count selected components with l
 * edges, r=sum l*h_l, and c=sum h_l.  Their source multiplicity is
 *
 *       W(H) = binomial(n-r,c) c! / product_l h_l!.
 *
 * To realize these components among the values, split {1,...,n} into its s
 * residue paths.  If x_l components of length l are assigned to a target
 * path with m vertices, their unlabeled placement coefficient is
 *
 *       (m-sum_l l*x_l)_(sum_l x_l) / product_l x_l!,
 *
 * where (u)_v is a falling factorial.  Multiplying these small target-path
 * polynomials gives Q(H).  Tauraso's q quantity is
 *
 *       q(H) = product_l h_l! * Q(H),
 *
 * and the required number is
 *
 *   b_{1,s}(n) = sum_H (-1)^r 2^c (n-r-c)! W(H) q(H).
 *
 * The 2^c factor selects the two orientations of every non-singleton value
 * chain.  This program evaluates the displayed finite convolution exactly.
 * It differs from 398599_03.c: 02 constructs target assignments Q(H), while
 * 03 constructs C_alpha(n,s) and applies the tiling-partition formula.
 *
 * GMP forms and exactly divides local multiplicities, stores target
 * coefficients, and holds the final alternating sum.  The divided local
 * values are checked before conversion to unsigned 128-bit storage.
 * Supported range: n<=60 and s<=128.
 *
 * References:
 *   R. Tauraso, "The Dinner Table Problem: The Rectangular Case",
 *   https://math.colgate.edu/~integers/g11/g11.pdf
 *   G. Spahn and D. Zeilberger,
 *   https://doi.org/10.54550/ECA2023V3S2R10
 *
 * Build:
 *
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic \
 *     -I/opt/homebrew/include -L/opt/homebrew/lib \
 *     398599_02.c -lgmp -o 398599_02
 *
 * A range run writes b398599_02_sS_part.txt and flushes every completed
 * term.  Only after all requested terms finish is it atomically renamed to
 * b398599_02_sS.txt.  The implementation number and s are both present, so
 * outputs from 02, 03, and different s values cannot overwrite one another.
 *
 * Usage:
 *   ./398599_02 --upto 30
 *   ./398599_02 --term 30
 *   ./398599_02 --s 4 --upto 30
 *   ./398599_02 --check
 */

#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <gmp.h>

#if ULONG_MAX < UINT64_MAX
#error "398599_02 requires a platform with 64-bit unsigned long"
#endif

#if !defined(__SIZEOF_INT128__)
#error "398599_02 requires unsigned __int128"
#endif

__extension__ typedef unsigned __int128 uint128_t;

#define DEFAULT_S 3
#define DEFAULT_MAX_N 20
#define DEFAULT_CHECK_N 30
#define MAX_SUPPORTED_N 60
#define MAX_SUPPORTED_S 128
#define MAX_COMPONENT_COUNT (MAX_SUPPORTED_N / 2)
#define MAX_HISTOGRAM_LENGTH (MAX_SUPPORTED_N - 1)
#define INITIAL_CAPACITY ((size_t)16)
#define LOAD_NUMERATOR ((size_t)7)
#define LOAD_DENOMINATOR ((size_t)10)
#define BRUTE_CHECK_MAX_N 11
#define GENERAL_CHECK_MAX_S 8
#define KNOWN_MAX_N 30

static const char *const known_s3[KNOWN_MAX_N + 1] = {
    "1", "1", "2", "6", "12", "48", "240", "1392", "9936",
    "82416", "771888", "8081808", "93273936", "1174442160",
    "16012371024", "234865643952", "3686260363152",
    "61627946597424", "1093150647044880", "20501844922196976",
    "405313119744707088", "8423466060115604208",
    "183582624178893950736", "4186520700374895202800",
    "99697398784352956533264", "2474726013785813664809712",
    "63922390767402268644275472", "1715494609619150973923837424",
    "47765601143125284027333699600",
    "1378019056802003925032691803376",
    "41140957260201936774910629270288"
};

typedef struct {
    bool occupied;
    uint64_t hash;
    uint8_t count[MAX_HISTOGRAM_LENGTH];
    mpz_t coefficient;
} QSlot;

typedef struct {
    QSlot *slot;
    size_t capacity;
    size_t size;
    int key_length;
} QMap;

typedef struct {
    uint8_t count[MAX_HISTOGRAM_LENGTH];
    uint128_t coefficient;
} TargetPattern;

typedef struct {
    TargetPattern *item;
    size_t count;
    size_t capacity;
    int key_length;
} PatternSet;

typedef enum {
    MODE_UPTO,
    MODE_TERM
} OutputMode;

static void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static void *xcalloc(size_t count, size_t size)
{
    if (size != 0U && count > SIZE_MAX / size) {
        die("allocation size overflow");
    }
    void *pointer = calloc(count, size);
    if (pointer == NULL) {
        die("out of memory");
    }
    return pointer;
}

static void *xrealloc(void *old, size_t count, size_t size)
{
    if (size != 0U && count > SIZE_MAX / size) {
        die("allocation size overflow");
    }
    void *pointer = realloc(old, count * size);
    if (pointer == NULL) {
        die("out of memory");
    }
    return pointer;
}

static double monotonic_seconds(void)
{
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) != 0) {
        die("clock_gettime failed");
    }
    return (double)now.tv_sec + (double)now.tv_nsec / 1000000000.0;
}

static void mpz_set_u128(mpz_t result, uint128_t value)
{
    uint64_t high = (uint64_t)(value >> 64);
    uint64_t low = (uint64_t)value;
    mpz_set_ui(result, (unsigned long)high);
    mpz_mul_2exp(result, result, 64U);
    mpz_add_ui(result, result, (unsigned long)low);
}

static uint128_t multiply_u128(uint128_t left, uint128_t right)
{
    if (left != 0 && right > ~(uint128_t)0 / left) {
        die("unsigned 128-bit multiplication overflow");
    }
    return left * right;
}

static uint128_t mpz_get_u128_checked(const mpz_t value)
{
    if (mpz_sgn(value) < 0 || mpz_sizeinbase(value, 2) > 128U) {
        die("GMP value does not fit unsigned 128-bit storage");
    }
    uint64_t limb[2] = {0, 0};
    size_t count = 0;
    mpz_export(limb, &count, -1, sizeof(limb[0]), 0, 0, value);
    if (count > 2U) {
        die("GMP export exceeded unsigned 128-bit storage");
    }
    return (uint128_t)limb[0] | ((uint128_t)limb[1] << 64);
}

static void make_factorials(
    uint128_t factorial[MAX_COMPONENT_COUNT + 1])
{
    factorial[0] = 1;
    for (int n = 1; n <= MAX_COMPONENT_COUNT; ++n) {
        factorial[n] = multiply_u128(factorial[n - 1], (uint128_t)n);
    }
}

/* (available)_component_count / product h_l! */
static uint128_t target_multiplicity(
    int available, int component_count, uint128_t histogram_factor,
    const uint128_t factorial[MAX_COMPONENT_COUNT + 1],
    mpz_t numerator, mpz_t divisor)
{
    mpz_bin_uiui(numerator, (unsigned long)available,
                 (unsigned long)component_count);
    mpz_set_u128(divisor, factorial[component_count]);
    mpz_mul(numerator, numerator, divisor);
    mpz_set_u128(divisor, histogram_factor);
    if (!mpz_divisible_p(numerator, divisor)) {
        die("nonintegral target multiplicity");
    }
    mpz_divexact(numerator, numerator, divisor);
    return mpz_get_u128_checked(numerator);
}

static uint64_t hash_key(const uint8_t *key, int key_length)
{
    uint64_t hash = UINT64_C(1469598103934665603);
    for (int i = 0; i < key_length; ++i) {
        hash ^= (uint64_t)key[i];
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}

static void map_init(QMap *map, int key_length)
{
    map->capacity = INITIAL_CAPACITY;
    map->size = 0;
    map->key_length = key_length;
    map->slot = xcalloc(map->capacity, sizeof(*map->slot));
}

static void map_destroy(QMap *map)
{
    for (size_t i = 0; i < map->capacity; ++i) {
        if (map->slot[i].occupied) {
            mpz_clear(map->slot[i].coefficient);
        }
    }
    free(map->slot);
    map->slot = NULL;
    map->capacity = 0;
    map->size = 0;
}

static QSlot *map_find_slot(QMap *map, const uint8_t *key, uint64_t hash)
{
    size_t index = (size_t)hash & (map->capacity - 1U);
    for (;;) {
        QSlot *slot = &map->slot[index];
        if (!slot->occupied ||
            (slot->hash == hash &&
             memcmp(slot->count, key, (size_t)map->key_length) == 0)) {
            return slot;
        }
        index = (index + 1U) & (map->capacity - 1U);
    }
}

static void map_grow(QMap *map)
{
    QSlot *old_slot = map->slot;
    size_t old_capacity = map->capacity;
    if (old_capacity > SIZE_MAX / 2U) {
        die("target histogram table is too large");
    }
    map->capacity *= 2U;
    map->size = 0;
    map->slot = xcalloc(map->capacity, sizeof(*map->slot));

    for (size_t i = 0; i < old_capacity; ++i) {
        if (!old_slot[i].occupied) {
            continue;
        }
        QSlot *target = map_find_slot(map, old_slot[i].count,
                                      old_slot[i].hash);
        target->occupied = true;
        target->hash = old_slot[i].hash;
        memcpy(target->count, old_slot[i].count, (size_t)map->key_length);
        mpz_init_set(target->coefficient, old_slot[i].coefficient);
        mpz_clear(old_slot[i].coefficient);
        ++map->size;
    }
    free(old_slot);
}

static QSlot *map_get_or_insert(QMap *map, const uint8_t *key)
{
    if ((map->size + 1U) * LOAD_DENOMINATOR >
        map->capacity * LOAD_NUMERATOR) {
        map_grow(map);
    }
    uint64_t hash = hash_key(key, map->key_length);
    QSlot *slot = map_find_slot(map, key, hash);
    if (!slot->occupied) {
        slot->occupied = true;
        slot->hash = hash;
        memcpy(slot->count, key, (size_t)map->key_length);
        mpz_init_set_ui(slot->coefficient, 0UL);
        ++map->size;
    }
    return slot;
}

static void patterns_init(PatternSet *patterns, int key_length)
{
    patterns->item = NULL;
    patterns->count = 0;
    patterns->capacity = 0;
    patterns->key_length = key_length;
}

static void patterns_add(PatternSet *patterns, const uint8_t *key,
                         uint128_t coefficient)
{
    if (patterns->count == patterns->capacity) {
        size_t next = patterns->capacity == 0U ? 16U :
                      patterns->capacity * 2U;
        patterns->item = xrealloc(patterns->item, next,
                                  sizeof(*patterns->item));
        patterns->capacity = next;
    }
    TargetPattern *pattern = &patterns->item[patterns->count++];
    memcpy(pattern->count, key, (size_t)patterns->key_length);
    pattern->coefficient = coefficient;
}

static void patterns_destroy(PatternSet *patterns)
{
    free(patterns->item);
    patterns->item = NULL;
    patterns->count = 0;
    patterns->capacity = 0;
}

static void generate_target_recursive(
    PatternSet *patterns, int path_size, int length, int edge_count,
    int component_count, uint8_t key[MAX_HISTOGRAM_LENGTH],
    const uint128_t factorial[MAX_COMPONENT_COUNT + 1],
    mpz_t numerator, mpz_t divisor)
{
    if (length >= path_size) {
        uint128_t denominator = 1;
        for (int i = 0; i < path_size - 1; ++i) {
            denominator = multiply_u128(denominator, factorial[key[i]]);
        }
        uint128_t coefficient = target_multiplicity(
            path_size - edge_count, component_count, denominator,
            factorial, numerator, divisor);
        patterns_add(patterns, key, coefficient);
        return;
    }

    int remaining_vertices = path_size - edge_count - component_count;
    int maximum = remaining_vertices / (length + 1);
    for (int count = 0; count <= maximum; ++count) {
        key[length - 1] = (uint8_t)count;
        generate_target_recursive(
            patterns, path_size, length + 1,
            edge_count + length * count, component_count + count,
            key, factorial, numerator, divisor);
    }
}

static void generate_target_patterns(
    PatternSet *patterns, int path_size,
    const uint128_t factorial[MAX_COMPONENT_COUNT + 1])
{
    uint8_t key[MAX_HISTOGRAM_LENGTH] = {0};
    mpz_t numerator;
    mpz_t divisor;
    mpz_init(numerator);
    mpz_init(divisor);
    generate_target_recursive(patterns, path_size, 1, 0, 0,
                              key, factorial, numerator, divisor);
    mpz_clear(divisor);
    mpz_clear(numerator);
}

static void convolve_target_path(QMap *current,
                                 const PatternSet *patterns)
{
    QMap following;
    map_init(&following, current->key_length);
    uint8_t sum[MAX_HISTOGRAM_LENGTH];
    mpz_t factor;
    mpz_init(factor);

    for (size_t slot_index = 0; slot_index < current->capacity;
         ++slot_index) {
        const QSlot *slot = &current->slot[slot_index];
        if (!slot->occupied) {
            continue;
        }
        for (size_t p = 0; p < patterns->count; ++p) {
            const TargetPattern *pattern = &patterns->item[p];
            for (int i = 0; i < current->key_length; ++i) {
                unsigned value =
                    (unsigned)slot->count[i] + pattern->count[i];
                if (value > UINT8_MAX) {
                    die("histogram count overflow");
                }
                sum[i] = (uint8_t)value;
            }
            QSlot *target = map_get_or_insert(&following, sum);
            mpz_set_u128(factor, pattern->coefficient);
            mpz_addmul(target->coefficient, slot->coefficient, factor);
        }
    }

    mpz_clear(factor);
    map_destroy(current);
    *current = following;
}

static void target_coefficients(
    QMap *result, int n, int step,
    const uint128_t factorial[MAX_COMPONENT_COUNT + 1])
{
    map_init(result, n - 1);
    uint8_t zero[MAX_HISTOGRAM_LENGTH] = {0};
    QSlot *origin = map_get_or_insert(result, zero);
    mpz_set_ui(origin->coefficient, 1UL);

    for (int residue = 1; residue <= step; ++residue) {
        int path_size = (n - residue) / step + 1;
        PatternSet patterns;
        patterns_init(&patterns, n - 1);
        generate_target_patterns(&patterns, path_size, factorial);
        convolve_target_path(result, &patterns);
        patterns_destroy(&patterns);
    }
}

static uint128_t histogram_factorial(
    const uint8_t *histogram, int key_length,
    const uint128_t factorial[MAX_COMPONENT_COUNT + 1],
    int *edge_count, int *component_count)
{
    uint128_t product = 1;
    *edge_count = 0;
    *component_count = 0;
    for (int i = 0; i < key_length; ++i) {
        int count = histogram[i];
        *edge_count += (i + 1) * count;
        *component_count += count;
        product = multiply_u128(product, factorial[count]);
    }
    return product;
}

static void multiply_by_u128(mpz_t value, uint128_t factor,
                             mpz_t temporary)
{
    mpz_set_u128(temporary, factor);
    mpz_mul(value, value, temporary);
}

static void compute_formula(mpz_t result, int n, int s,
                            size_t *histogram_count)
{
    uint128_t factorial[MAX_COMPONENT_COUNT + 1];
    make_factorials(factorial);
    if (n <= s) {
        mpz_fac_ui(result, (unsigned long)n);
        *histogram_count = 1U;
        return;
    }

    QMap target;
    target_coefficients(&target, n, s, factorial);
    *histogram_count = target.size;
    mpz_set_ui(result, 0UL);

    mpz_t term;
    mpz_t factor;
    mpz_init(term);
    mpz_init(factor);

    for (size_t i = 0; i < target.capacity; ++i) {
        const QSlot *slot = &target.slot[i];
        if (!slot->occupied) {
            continue;
        }
        int edge_count;
        int component_count;
        uint128_t hist_factor =
            histogram_factorial(slot->count, target.key_length, factorial,
                                &edge_count, &component_count);
        int singleton_count = n - edge_count - component_count;
        if (singleton_count < 0) {
            die("invalid target histogram");
        }

        uint128_t source = target_multiplicity(
            n - edge_count, component_count, hist_factor,
            factorial, term, factor);

        mpz_set(term, slot->coefficient);
        multiply_by_u128(term, hist_factor, factor);
        multiply_by_u128(term, source, factor);
        mpz_fac_ui(factor, (unsigned long)singleton_count);
        mpz_mul(term, term, factor);
        mpz_mul_2exp(term, term, (mp_bitcnt_t)component_count);

        if ((edge_count & 1) == 0) {
            mpz_add(result, result, term);
        } else {
            mpz_sub(result, result, term);
        }
    }

    mpz_clear(factor);
    mpz_clear(term);
    map_destroy(&target);

    mpz_t upper;
    mpz_init(upper);
    mpz_fac_ui(upper, (unsigned long)n);
    if (mpz_sgn(result) < 0 || mpz_cmp(result, upper) > 0) {
        mpz_clear(upper);
        die("formula result is outside 0..n!");
    }
    mpz_clear(upper);
}

static void verify_known(const mpz_t value, int n, int s)
{
    if (s != DEFAULT_S || n > KNOWN_MAX_N) {
        return;
    }
    mpz_t expected;
    mpz_init(expected);
    if (mpz_set_str(expected, known_s3[n], 10) != 0 ||
        mpz_cmp(value, expected) != 0) {
        gmp_fprintf(stderr,
                    "error: A398599 mismatch at n=%d: got %Zd, "
                    "expected %s\n",
                    n, value, known_s3[n]);
        mpz_clear(expected);
        exit(EXIT_FAILURE);
    }
    mpz_clear(expected);
}

static void compute_value(mpz_t result, int n, int s, bool verbose)
{
    double started = monotonic_seconds();
    size_t histograms = 0;
    compute_formula(result, n, s, &histograms);
    verify_known(result, n, s);
    if (verbose) {
        fprintf(stderr,
                "398599_02: s=%d, n=%d, target histograms=%zu, %.3f s\n",
                s, n, histograms, monotonic_seconds() - started);
    }
}

static uint64_t brute_recursive(int n, int s, uint32_t full,
                                uint64_t *memo, uint32_t used, int last)
{
    if (used == full) {
        return UINT64_C(1);
    }
    size_t index = (size_t)used * (size_t)(n + 1) + (size_t)last;
    if (memo[index] != UINT64_MAX) {
        return memo[index];
    }
    uint64_t total = 0;
    uint32_t available = full ^ used;
    while (available != 0U) {
        uint32_t bit = available & (0U - available);
        available ^= bit;
        int value = __builtin_ctz(bit);
        if (last < n) {
            int difference = value - last;
            if (difference == s || difference == -s) {
                continue;
            }
        }
        total += brute_recursive(n, s, full, memo, used | bit, value);
    }
    memo[index] = total;
    return total;
}

static uint64_t brute_count(int n, int s)
{
    size_t state_count = ((size_t)1U << n) * (size_t)(n + 1);
    uint64_t *memo = xrealloc(NULL, state_count, sizeof(*memo));
    for (size_t i = 0; i < state_count; ++i) {
        memo[i] = UINT64_MAX;
    }
    uint32_t full = ((uint32_t)1U << n) - 1U;
    uint64_t result = brute_recursive(n, s, full, memo, 0U, n);
    free(memo);
    return result;
}

static int check_implementation(int maximum_n)
{
    mpz_t value;
    mpz_init(value);
    for (int n = 0; n <= maximum_n; ++n) {
        compute_value(value, n, DEFAULT_S, false);
    }

    int brute_maximum = maximum_n < BRUTE_CHECK_MAX_N ?
                        maximum_n : BRUTE_CHECK_MAX_N;
    for (int s = 1; s <= GENERAL_CHECK_MAX_S; ++s) {
        for (int n = 0; n <= brute_maximum; ++n) {
            compute_formula(value, n, s, &(size_t){0});
            uint64_t direct = brute_count(n, s);
            if (mpz_cmp_ui(value, (unsigned long)direct) != 0) {
                gmp_fprintf(stderr,
                            "error: DP mismatch at s=%d, n=%d: "
                            "formula=%Zd, DP=%llu\n",
                            s, n, value, (unsigned long long)direct);
                mpz_clear(value);
                return EXIT_FAILURE;
            }
        }
    }
    mpz_clear(value);
    printf("ok: A398599 verified for n=0..%d; independent subset DP "
           "for s=1..%d, n=0..%d\n",
           maximum_n, GENERAL_CHECK_MAX_S, brute_maximum);
    return EXIT_SUCCESS;
}

static int parse_integer(const char *text, const char *label,
                         int minimum, int maximum)
{
    char *end = NULL;
    errno = 0;
    long value = strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' ||
        value < minimum || value > maximum) {
        fprintf(stderr, "error: %s must be in %d..%d: %s\n",
                label, minimum, maximum, text);
        exit(EXIT_FAILURE);
    }
    return (int)value;
}

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s [MAX_N]\n"
            "       %s --term N\n"
            "       %s --upto MAX_N\n"
            "       %s --s S --term N\n"
            "       %s --s S --upto MAX_N\n"
            "       %s --check [MAX_N]\n"
            "S defaults to %d; MAX_N defaults to %d.\n"
            "A range run writes b398599_02_sS.txt in the current directory.\n",
            program, program, program, program, program, program,
            DEFAULT_S, DEFAULT_MAX_N);
}

static void print_value(const mpz_t value)
{
    if (mpz_out_str(stdout, 10, value) == 0U) {
        die("could not write result");
    }
}

static int write_b_file(int maximum_n, int s)
{
    char part_name[80];
    char final_name[80];
    int part_length = snprintf(part_name, sizeof(part_name),
                               "b398599_02_s%d_part.txt", s);
    int final_length = snprintf(final_name, sizeof(final_name),
                                "b398599_02_s%d.txt", s);
    if (part_length < 0 || (size_t)part_length >= sizeof(part_name) ||
        final_length < 0 || (size_t)final_length >= sizeof(final_name)) {
        die("b-file name is too long");
    }

    FILE *file = fopen(part_name, "w");
    if (file == NULL) {
        fprintf(stderr, "error: cannot open %s: %s\n",
                part_name, strerror(errno));
        return EXIT_FAILURE;
    }

    mpz_t value;
    mpz_init(value);
    for (int n = 0; n <= maximum_n; ++n) {
        compute_value(value, n, s, false);
        if (gmp_fprintf(file, "%d %Zd\n", n, value) < 0 ||
            fflush(file) != 0) {
            fprintf(stderr, "error: cannot write %s: %s\n",
                    part_name, strerror(errno));
            mpz_clear(value);
            (void)fclose(file);
            return EXIT_FAILURE;
        }
    }
    mpz_clear(value);

    if (fclose(file) != 0) {
        fprintf(stderr, "error: cannot close %s: %s\n",
                part_name, strerror(errno));
        return EXIT_FAILURE;
    }
    if (rename(part_name, final_name) != 0) {
        fprintf(stderr, "error: cannot rename %s to %s: %s\n",
                part_name, final_name, strerror(errno));
        return EXIT_FAILURE;
    }

    printf("wrote %s (n=0..%d, s=%d)\n", final_name, maximum_n, s);
    return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
    if (argc == 2 &&
        (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
        usage(argv[0]);
        return EXIT_SUCCESS;
    }
    if (argc >= 2 && strcmp(argv[1], "--check") == 0) {
        if (argc > 3) {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
        int maximum = argc == 3 ?
            parse_integer(argv[2], "MAX_N", 0, KNOWN_MAX_N) :
            DEFAULT_CHECK_N;
        return check_implementation(maximum);
    }

    int s = DEFAULT_S;
    int n = -1;
    bool have_s = false;
    OutputMode mode = MODE_UPTO;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--s") == 0) {
            if (have_s || i + 1 >= argc) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            s = parse_integer(argv[++i], "S", 1, MAX_SUPPORTED_S);
            have_s = true;
        } else if (strcmp(argv[i], "--term") == 0 ||
                   strcmp(argv[i], "--upto") == 0) {
            if (n >= 0 || i + 1 >= argc) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            mode = strcmp(argv[i], "--term") == 0 ? MODE_TERM : MODE_UPTO;
            n = parse_integer(argv[++i],
                              mode == MODE_TERM ? "N" : "MAX_N",
                              0, MAX_SUPPORTED_N);
        } else {
            if (argv[i][0] == '-' || n >= 0) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            n = parse_integer(argv[i], "MAX_N", 0, MAX_SUPPORTED_N);
            mode = MODE_UPTO;
        }
    }
    if (n < 0) {
        n = DEFAULT_MAX_N;
    }

    if (mode == MODE_UPTO) {
        return write_b_file(n, s);
    }

    mpz_t value;
    mpz_init(value);
    if (mode == MODE_TERM) {
        compute_value(value, n, s, true);
        printf("%d ", n);
        print_value(value);
        putchar('\n');
    }
    mpz_clear(value);
    return EXIT_SUCCESS;
}
