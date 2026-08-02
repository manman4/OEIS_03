/*
 * A333892 -- fast exact subset DP.
 *
 * Count permutations sigma of [n] such that
 *
 *              i divides product_{k=1..i} sigma(k)
 *
 * for every 1 <= i <= n.
 *
 * For a finite set S of positive integers, let D(S) be the number of valid
 * orderings of S.  Deleting the final element gives
 *
 *   D(empty) = 1,
 *   D(S) = [ |S| divides product(S) ] * sum_{x in S} D(S\{x}).
 *
 * Only prime-power layers need a divisibility test.  If m is not a prime
 * power and p^e exactly divides m, then p^e < m.  Every ordering counted at
 * layer m has already satisfied the condition at position p^e, so p^e
 * divides its product at position m.  This holds for every prime divisor of
 * m, hence the condition at m is automatic.  At m=p^e the only test is
 *
 *                    sum_{x in S} v_p(x) >= e.
 *
 * The main implementation propagates D(S) forward from the empty set.  At a
 * prime-power target layer it computes the p-adic deficit of the parent once
 * and considers only unused elements capable of filling that deficit.  No
 * potentially overflowing subset product is formed.
 *
 * Each a(n) is computed in its own universe [n].  This deliberately repeats
 * the small D(S) values between terms, but it makes the time needed to reach
 * a(k) independent of the final requested N.  In particular, requesting a
 * large N does not force a(k) to be found among all k-subsets of [N].
 *
 * The default engine is selected automatically:
 *
 *   dense  -- one exact array indexed by the complete subset mask; fastest
 *             when it fits A333892_MEMORY_MIB;
 *   sparse -- exact current/next-layer hash tables; uses less memory when
 *             the dense array does not fit.
 *
 * A333892_ENGINE=dense or sparse overrides the automatic choice.  Every
 * count is stored in ceil(bit_length(N!)/64) uint64_t limbs.  Since every
 * D(S) is at most |S|! <= N!, this is an exact a priori bound, and a carry
 * beyond the allocated limbs is treated as an error.  There is no CRT
 * ambiguity in this fast version.
 *
 * An independent dense recurrence using the original condition
 * |S| | product(S), evaluated without multiplication, checks n=0..20.  The
 * computed values n=0..23 are also compared with the built-in OEIS prefix;
 * the prefix is never used as input.
 *
 * Each completed term is written and flushed to b333892_part.txt beside the
 * executable.  Complete success renames it to b333892.txt.  Both files have
 * OFFSET 0 and are overwritten on each run.
 *
 * Build:
 *
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic \
 *     -I/opt/homebrew/include -L/opt/homebrew/lib \
 *     333892_01.c -lgmp -o 333892_01
 *
 * Usage:
 *
 *   A333892_MEMORY_MIB=4096 ./333892_01 N
 *
 * N may be 0..63.  The exponential state count makes the practical limit
 * much smaller than the 64-bit mask limit.
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
#error "333892_01 requires a platform with 64-bit unsigned long"
#endif

#define MAX_N 63
#define MAX_LIMBS 5
#define DENSE_CHECK_MAX_N 20
#define VERIFIED_MAX_N 23
#define INITIAL_CAPACITY ((size_t)1024)
#define LOAD_NUMERATOR ((size_t)7)
#define LOAD_DENOMINATOR ((size_t)10)
#define DEFAULT_MEMORY_MIB UINT64_C(4096)
#define MIN_MEMORY_MIB UINT64_C(64)
#define MAX_MEMORY_MIB UINT64_C(65536)

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

typedef enum {
    ENGINE_AUTO,
    ENGINE_DENSE,
    ENGINE_SPARSE
} EngineChoice;

typedef struct {
    int prime;
    int exponent;
} PrimePowerInfo;

typedef struct {
    uint64_t limit;
    uint64_t current;
    uint64_t peak;
    int layer_size;
} MemoryBudget;

typedef struct {
    uint64_t *keys;
    uint64_t *values;
    size_t capacity;
    size_t size;
    int limb_count;
    uint64_t bytes;
    MemoryBudget *budget;
} LayerTable;

typedef struct {
    uint64_t states;
    uint64_t candidate_tests;
    uint64_t transitions;
    uint64_t insertions;
    uint64_t additions;
    uint64_t prime_power_parents;
    uint64_t peak_bytes;
    double seconds;
} RunStats;

typedef struct {
    int maximum_n;
    int limb_count;
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
    const char *text = getenv("A333892_MEMORY_MIB");
    uint64_t mib = DEFAULT_MEMORY_MIB;
    if (text != NULL && *text != '\0') {
        char *end = NULL;
        errno = 0;
        unsigned long long value = strtoull(text, &end, 10);
        if (errno != 0 || end == text || *end != '\0' ||
            value < MIN_MEMORY_MIB || value > MAX_MEMORY_MIB) {
            fprintf(stderr,
                    "error: A333892_MEMORY_MIB must be in %" PRIu64
                    "..%" PRIu64 ": %s\n",
                    MIN_MEMORY_MIB, MAX_MEMORY_MIB, text);
            exit(EXIT_FAILURE);
        }
        mib = (uint64_t)value;
    }
    return mib * UINT64_C(1024) * UINT64_C(1024);
}

static EngineChoice parse_engine(void)
{
    const char *text = getenv("A333892_ENGINE");
    if (text == NULL || *text == '\0' || strcmp(text, "auto") == 0) {
        return ENGINE_AUTO;
    }
    if (strcmp(text, "dense") == 0) {
        return ENGINE_DENSE;
    }
    if (strcmp(text, "sparse") == 0) {
        return ENGINE_SPARSE;
    }
    fprintf(stderr,
            "error: A333892_ENGINE must be auto, dense, or sparse: %s\n",
            text);
    exit(EXIT_FAILURE);
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

static uint64_t prefix_mask(int length)
{
    if (length == 0) {
        return 0;
    }
    return (UINT64_C(1) << length) - 1;
}

static uint64_t mix_mask(uint64_t value)
{
    value ^= value >> 30;
    value *= UINT64_C(0xbf58476d1ce4e5b9);
    value ^= value >> 27;
    value *= UINT64_C(0x94d049bb133111eb);
    value ^= value >> 31;
    return value;
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

static int integer_power(int base, int exponent)
{
    int result = 1;
    for (int index = 0; index < exponent; ++index) {
        if (result > MAX_N / base) {
            die("prime-power mask index overflow");
        }
        result *= base;
    }
    return result;
}

static int required_limb_count(int n)
{
    mpz_t factorial;
    mpz_init(factorial);
    mpz_fac_ui(factorial, (unsigned long)n);
    size_t bits = mpz_sizeinbase(factorial, 2);
    int limbs = (int)((bits + 63) / 64);
    mpz_clear(factorial);
    if (limbs < 1 || limbs > MAX_LIMBS) {
        die("factorial bound does not fit the configured exact limbs");
    }
    return limbs;
}

static void add_exact(uint64_t *sum, const uint64_t *addend,
                      int limb_count)
{
    uint64_t carry = 0;
    for (int limb = 0; limb < limb_count; ++limb) {
        __uint128_t total = (__uint128_t)sum[limb] + addend[limb] + carry;
        sum[limb] = (uint64_t)total;
        carry = (uint64_t)(total >> 64);
    }
    if (carry != 0) {
        die("exact counter exceeded the factorial bound");
    }
}

static bool is_zero_exact(const uint64_t *value, int limb_count)
{
    uint64_t combined = 0;
    for (int limb = 0; limb < limb_count; ++limb) {
        combined |= value[limb];
    }
    return combined == 0;
}

static void limbs_to_mpz(mpz_t result, const uint64_t *value,
                         int limb_count)
{
    mpz_import(result, (size_t)limb_count, -1, sizeof(uint64_t),
               0, 0, value);
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
            "333892_01: independent original-condition check prepared "
            "for n=0..%d\n", n);
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

static void verify_value(const Context *context, int n,
                         const uint64_t *value)
{
    if (is_zero_exact(value, context->limb_count)) {
        die("computed term is zero although the identity permutation works");
    }
    if (n <= context->dense_check_n) {
        if (value[0] != context->dense_expected[n]) {
            die("optimized DP differs from independent dense subset DP");
        }
        for (int limb = 1; limb < context->limb_count; ++limb) {
            if (value[limb] != 0) {
                die("dense-check value unexpectedly exceeds uint64_t");
            }
        }
    }
    if (n <= VERIFIED_MAX_N) {
        mpz_t actual;
        mpz_t expected;
        mpz_inits(actual, expected, NULL);
        limbs_to_mpz(actual, value, context->limb_count);
        if (mpz_set_str(expected, verified_terms[n], 10) != 0) {
            mpz_clears(actual, expected, NULL);
            die("invalid built-in verified term");
        }
        if (mpz_cmp(actual, expected) != 0) {
            mpz_clears(actual, expected, NULL);
            die("computed value differs from the verified OEIS prefix");
        }
        mpz_clears(actual, expected, NULL);
    }
}

static void write_value(const Context *context, int n,
                        const uint64_t *value)
{
    verify_value(context, n, value);
    mpz_t exact;
    mpz_init(exact);
    limbs_to_mpz(exact, value, context->limb_count);
    if (gmp_fprintf(context->output, "%d %Zd\n", n, exact) < 0) {
        mpz_clear(exact);
        die("cannot write b-file term");
    }
    flush_output(context);
    gmp_printf("%d %Zd\n", n, exact);
    fflush(stdout);
    mpz_clear(exact);
}

static bool dense_layout(int n, int limb_count, size_t *state_count,
                         uint64_t *bytes)
{
    if (n >= (int)(sizeof(size_t) * CHAR_BIT)) {
        return false;
    }
    size_t count = (size_t)1 << n;
    __uint128_t total = (__uint128_t)count * (unsigned)limb_count *
                        sizeof(uint64_t);
    if (total > UINT64_MAX || total > SIZE_MAX) {
        return false;
    }
    *state_count = count;
    *bytes = (uint64_t)total;
    return true;
}

static void compute_dense_term(uint64_t *result, const Context *context,
                               int n,
                               const PrimePowerInfo *prime_powers,
                               const uint64_t *divisible_masks,
                               RunStats *stats)
{
    memset(result, 0, MAX_LIMBS * sizeof(*result));
    if (n == 0) {
        result[0] = 1;
        return;
    }
    size_t state_count = 0;
    uint64_t bytes = 0;
    if (!dense_layout(n, context->limb_count,
                      &state_count, &bytes) ||
        bytes > context->memory_limit) {
        die("dense engine selected but its exact array exceeds the memory limit");
    }
    size_t word_count = state_count * (size_t)context->limb_count;
    uint64_t *values = calloc(word_count, sizeof(*values));
    if (values == NULL) {
        die("cannot allocate dense exact DP array");
    }
    stats->peak_bytes = bytes;
    values[0] = 1;

    double start = monotonic_seconds();
    for (size_t subset_index = 1; subset_index < state_count;
         ++subset_index) {
        uint64_t subset = (uint64_t)subset_index;
        int size = __builtin_popcountll(subset);
        ++stats->states;

        PrimePowerInfo info = prime_powers[size];
        if (info.exponent != 0) {
            ++stats->prime_power_parents;
            int valuation = subset_p_valuation(
                subset, info.prime, info.exponent, divisible_masks);
            if (valuation < info.exponent) {
                continue;
            }
        }

        uint64_t *target = values +
            subset_index * (size_t)context->limb_count;
        uint64_t remaining = subset;
        while (remaining != 0) {
            uint64_t bit = remaining & (0 - remaining);
            const uint64_t *child = values +
                (size_t)(subset ^ bit) * (size_t)context->limb_count;
            add_exact(target, child, context->limb_count);
            ++stats->candidate_tests;
            ++stats->transitions;
            remaining ^= bit;
        }
    }
    memcpy(result,
           values + (state_count - 1) * (size_t)context->limb_count,
           (size_t)context->limb_count * sizeof(*result));
    stats->seconds = monotonic_seconds() - start;
    free(values);
}

static uint64_t layer_table_bytes(size_t capacity, int limb_count)
{
    uint64_t words_per_slot = (uint64_t)limb_count + UINT64_C(1);
    if ((uint64_t)capacity > UINT64_MAX /
        (words_per_slot * (uint64_t)sizeof(uint64_t))) {
        die("layer table byte count overflow");
    }
    return (uint64_t)capacity * words_per_slot *
           (uint64_t)sizeof(uint64_t);
}

static void budget_reserve(MemoryBudget *budget, uint64_t bytes)
{
    if (bytes > budget->limit || budget->current > budget->limit - bytes) {
        double required_gib = (double)(budget->current + bytes) /
                              (1024.0 * 1024.0 * 1024.0);
        double configured_gib = (double)budget->limit /
                                (1024.0 * 1024.0 * 1024.0);
        fprintf(stderr,
                "error: A333892 layer m=%d memory limit exceeded\n"
                "required at the failed allocation: %.3f GiB; "
                "configured limit: %.3f GiB\n"
                "Increase A333892_MEMORY_MIB only if physical RAM permits.\n",
                budget->layer_size, required_gib, configured_gib);
        exit(EXIT_FAILURE);
    }
    budget->current += bytes;
    if (budget->current > budget->peak) {
        budget->peak = budget->current;
    }
}

static void budget_release(MemoryBudget *budget, uint64_t bytes)
{
    if (bytes > budget->current) {
        die("layer table memory accounting underflow");
    }
    budget->current -= bytes;
}

static void layer_table_initialize(LayerTable *table, size_t capacity,
                                   int limb_count, MemoryBudget *budget)
{
    if (capacity < INITIAL_CAPACITY) {
        capacity = INITIAL_CAPACITY;
    }
    if ((capacity & (capacity - 1)) != 0) {
        die("layer table capacity is not a power of two");
    }
    if (capacity > SIZE_MAX / (size_t)limb_count ||
        capacity * (size_t)limb_count >
            SIZE_MAX / sizeof(uint64_t)) {
        die("layer table allocation size overflow");
    }

    memset(table, 0, sizeof(*table));
    table->capacity = capacity;
    table->limb_count = limb_count;
    table->budget = budget;
    table->bytes = layer_table_bytes(capacity, limb_count);
    budget_reserve(budget, table->bytes);

    table->keys = calloc(capacity, sizeof(*table->keys));
    table->values = malloc(capacity * (size_t)limb_count *
                           sizeof(*table->values));
    if (table->keys == NULL || table->values == NULL) {
        free(table->values);
        free(table->keys);
        budget_release(budget, table->bytes);
        die("cannot allocate sparse layer table");
    }
}

static void layer_table_destroy(LayerTable *table)
{
    if (table->capacity == 0) {
        return;
    }
    free(table->values);
    free(table->keys);
    budget_release(table->budget, table->bytes);
    memset(table, 0, sizeof(*table));
}

static void layer_table_grow(LayerTable *table)
{
    if (table->capacity > SIZE_MAX / 2) {
        die("layer table capacity overflow");
    }
    LayerTable grown;
    layer_table_initialize(&grown, table->capacity * 2,
                           table->limb_count, table->budget);
    size_t mask = grown.capacity - 1;
    for (size_t old_slot = 0; old_slot < table->capacity; ++old_slot) {
        uint64_t key = table->keys[old_slot];
        if (key == 0) {
            continue;
        }
        size_t slot = (size_t)mix_mask(key) & mask;
        while (grown.keys[slot] != 0) {
            slot = (slot + 1) & mask;
        }
        memcpy(grown.values + slot * (size_t)grown.limb_count,
               table->values + old_slot * (size_t)table->limb_count,
               (size_t)table->limb_count * sizeof(uint64_t));
        grown.keys[slot] = key;
        ++grown.size;
    }
    layer_table_destroy(table);
    *table = grown;
}

static void layer_table_add(LayerTable *table, uint64_t key,
                            const uint64_t *value, RunStats *stats)
{
    if (key == 0) {
        die("empty subset must not be inserted into a layer table");
    }
    if (table->size + 1 >
        (table->capacity / LOAD_DENOMINATOR) * LOAD_NUMERATOR) {
        layer_table_grow(table);
    }
    size_t mask = table->capacity - 1;
    size_t slot = (size_t)mix_mask(key) & mask;
    while (table->keys[slot] != 0) {
        if (table->keys[slot] == key) {
            add_exact(table->values + slot * (size_t)table->limb_count,
                      value, table->limb_count);
            ++stats->additions;
            return;
        }
        slot = (slot + 1) & mask;
    }
    memcpy(table->values + slot * (size_t)table->limb_count, value,
           (size_t)table->limb_count * sizeof(uint64_t));
    table->keys[slot] = key;
    ++table->size;
    ++stats->insertions;
}

static const uint64_t *layer_table_lookup(const LayerTable *table,
                                          uint64_t key)
{
    if (key == 0 || table->capacity == 0) {
        return NULL;
    }
    size_t mask = table->capacity - 1;
    size_t slot = (size_t)mix_mask(key) & mask;
    while (table->keys[slot] != 0) {
        if (table->keys[slot] == key) {
            return table->values + slot * (size_t)table->limb_count;
        }
        slot = (slot + 1) & mask;
    }
    return NULL;
}

static size_t next_layer_initial_capacity(const LayerTable *current)
{
    size_t capacity = current->capacity;
    while (capacity > INITIAL_CAPACITY &&
           current->size < capacity / 5) {
        capacity /= 2;
    }
    return capacity;
}

static void compute_sparse_term(uint64_t *result, const Context *context,
                                int n,
                                const PrimePowerInfo *prime_powers,
                                const uint64_t *divisible_masks,
                                RunStats *stats)
{
    memset(result, 0, MAX_LIMBS * sizeof(*result));
    if (n == 0) {
        result[0] = 1;
        return;
    }

    double start = monotonic_seconds();
    MemoryBudget budget = {
        .limit = context->memory_limit,
        .layer_size = 1
    };
    LayerTable current;
    layer_table_initialize(&current, INITIAL_CAPACITY,
                           context->limb_count, &budget);
    uint64_t one[MAX_LIMBS] = { 1, 0, 0, 0, 0 };
    for (int element = 1; element <= n; ++element) {
        uint64_t key = UINT64_C(1) << (element - 1);
        layer_table_add(&current, key, one, stats);
    }

    uint64_t full = prefix_mask(n);
    for (int target_size = 2; target_size <= n;
         ++target_size) {
        budget.layer_size = target_size;
        LayerTable next;
        layer_table_initialize(&next,
                               next_layer_initial_capacity(&current),
                               context->limb_count, &budget);
        uint64_t transitions_before = stats->transitions;
        uint64_t tests_before = stats->candidate_tests;
        PrimePowerInfo info = prime_powers[target_size];

        for (size_t slot = 0; slot < current.capacity; ++slot) {
            uint64_t subset = current.keys[slot];
            if (subset == 0) {
                continue;
            }
            ++stats->states;
            if (__builtin_popcountll(subset) != target_size - 1) {
                die("subset stored in the wrong sparse layer");
            }
            const uint64_t *weight = current.values +
                slot * (size_t)context->limb_count;
            uint64_t unused = full ^ subset;
            uint64_t candidates = unused;
            stats->candidate_tests +=
                (uint64_t)__builtin_popcountll(unused);

            if (info.exponent != 0) {
                ++stats->prime_power_parents;
                int have = subset_p_valuation(
                    subset, info.prime, info.exponent,
                    divisible_masks);
                int missing = info.exponent - have;
                if (missing > 0) {
                    int required = integer_power(info.prime, missing);
                    candidates &= divisible_masks[required];
                }
            }

            while (candidates != 0) {
                uint64_t bit = candidates & (0 - candidates);
                layer_table_add(&next, subset | bit, weight, stats);
                ++stats->transitions;
                candidates ^= bit;
            }
        }

        uint64_t layer_transitions =
            stats->transitions - transitions_before;
        uint64_t layer_tests = stats->candidate_tests - tests_before;
        if (info.exponent != 0 || target_size == n ||
            target_size % 5 == 0 || next.size >= UINT64_C(1000000)) {
            fprintf(stderr,
                    "333892_01: n=%d sparse layer m=%d%s, "
                    "states=%zu -> %zu, "
                    "tests=%" PRIu64 ", transitions=%" PRIu64
                    ", workspace=%.3f GiB, %.3f s\n",
                    n, target_size,
                    info.exponent != 0 ? " (prime power)" : "",
                    current.size, next.size, layer_tests,
                    layer_transitions,
                    (double)budget.current /
                        (1024.0 * 1024.0 * 1024.0),
                    monotonic_seconds() - start);
        }

        layer_table_destroy(&current);
        current = next;
    }

    const uint64_t *term = layer_table_lookup(&current, full);
    if (term == NULL) {
        layer_table_destroy(&current);
        die("sparse DP lost the full-set state");
    }
    memcpy(result, term,
           (size_t)context->limb_count * sizeof(*result));
    layer_table_destroy(&current);
    if (budget.current != 0) {
        die("sparse memory accounting did not return to zero");
    }
    stats->peak_bytes = budget.peak;
    stats->seconds = monotonic_seconds() - start;
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s N\n", argv[0]);
        return EXIT_FAILURE;
    }
    int maximum_n = parse_n(argv[1]);

    Context context;
    memset(&context, 0, sizeof(context));
    context.maximum_n = maximum_n;
    context.memory_limit = parse_memory_limit();
    prepare_dense_check(&context);

    char *part_path = path_beside_executable(argv[0],
                                             "b333892_part.txt");
    char *final_path = path_beside_executable(argv[0], "b333892.txt");
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

    EngineChoice requested = parse_engine();
    fprintf(stderr,
            "333892_01: N=%d, independent universe for every term, "
            "engine=%s, memory limit=%.3f GiB\n",
            maximum_n,
            requested == ENGINE_AUTO ? "auto" :
                (requested == ENGINE_DENSE ? "dense" : "sparse"),
            (double)context.memory_limit /
                (1024.0 * 1024.0 * 1024.0));

    double all_start = monotonic_seconds();
    for (int n = 0; n <= maximum_n; ++n) {
        context.limb_count = required_limb_count(n);
        PrimePowerInfo prime_powers[MAX_N + 1];
        uint64_t divisible_masks[MAX_N + 1];
        prepare_layer_data(n, prime_powers, divisible_masks);

        size_t dense_states = 0;
        uint64_t dense_bytes = 0;
        bool dense_representable = dense_layout(
            n, context.limb_count, &dense_states, &dense_bytes);
        bool dense_fits = dense_representable &&
                          dense_bytes <= context.memory_limit;
        EngineChoice selected = requested;
        if (selected == ENGINE_AUTO) {
            selected = dense_fits ? ENGINE_DENSE : ENGINE_SPARSE;
        }
        if (selected == ENGINE_DENSE && !dense_fits) {
            fprintf(stderr,
                    "error: A333892 n=%d dense engine needs %s%.3f GiB; "
                    "configured limit is %.3f GiB\n",
                    n, dense_representable ? "" : "more than ",
                    dense_representable ?
                        (double)dense_bytes /
                            (1024.0 * 1024.0 * 1024.0) :
                        (double)context.memory_limit /
                            (1024.0 * 1024.0 * 1024.0),
                    (double)context.memory_limit /
                        (1024.0 * 1024.0 * 1024.0));
            exit(EXIT_FAILURE);
        }

        RunStats stats;
        memset(&stats, 0, sizeof(stats));
        uint64_t value[MAX_LIMBS] = { 0, 0, 0, 0, 0 };
        if (selected == ENGINE_DENSE) {
            compute_dense_term(value, &context, n,
                               prime_powers, divisible_masks, &stats);
        } else {
            compute_sparse_term(value, &context, n,
                                prime_powers, divisible_masks, &stats);
        }
        write_value(&context, n, value);

        fprintf(stderr,
                "333892_01: n=%d exact done, engine=%s, limbs=%d, "
                "states=%" PRIu64 ", tests=%" PRIu64
                ", transitions=%" PRIu64 ", insertions=%" PRIu64
                ", additions=%" PRIu64 ", peak=%.3f GiB, %.3f s\n",
                n, selected == ENGINE_DENSE ? "dense" : "sparse",
                context.limb_count, stats.states,
                stats.candidate_tests, stats.transitions,
                stats.insertions, stats.additions,
                (double)stats.peak_bytes /
                    (1024.0 * 1024.0 * 1024.0),
                stats.seconds);
    }
    fprintf(stderr, "333892_01: all terms completed in %.3f s\n",
            monotonic_seconds() - all_start);

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
