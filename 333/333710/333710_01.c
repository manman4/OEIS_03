/*
 * A333710 -- exact sparse layer DP.
 *
 * Count permutations sigma of [n] such that
 *
 *             i! divides product_{k=1..i} sigma(k)
 *
 * for every 1 <= i <= n.
 *
 * If S is a valid prefix set, m=|S| and
 *
 *                     q(S) = product(S) / m!,
 *
 * then j can be the last element of that prefix exactly when
 *
 *                     j divides q(S)*m.
 *
 * The preceding prefix has quotient q(S\{j})=q(S)*m/j.  Starting with
 * S=[n], q=1, the program propagates the number of suffix-removal paths from
 * one subset-size layer to the next.  When the empty set is reached, the
 * accumulated weight is a(n).
 *
 * Only the current and next layers are kept.  This avoids the large all-layer
 * memo table and its 3x rehash peak.  Each hash entry stores the complete
 * subset mask, its uniquely determined quotient, and an exact fixed-width
 * nonnegative count.  Since every partial count is at most n!,
 * ceil(bit_length(n!)/64) limbs suffice; there is no CRT ambiguity.
 * For n<=63, q(S)<=C(n,|S|)<2^64.  q*m is formed in unsigned 128-bit
 * arithmetic when needed.
 *
 * The built-in verified prefix is n=0..29.  With one argument, every
 * nonprime term from 0 through N is computed by the layer DP; prime terms use
 * the proved identity a(p)=2*a(p-1).  An optional FROM argument explicitly
 * requests use of the verified prefix below FROM.  Setting
 * A333710_NO_PRIME_SHORTCUT also computes prime terms by the layer DP.
 *
 * For n<=20, an independent dense forward subset DP checks the result.  All
 * computed terms through n=29 are also compared with the built-in prefix.
 * Each completed term is written and flushed to b333710_part.txt beside the
 * executable.  Complete success renames it to b333710.txt.  The b-file has
 * OFFSET 0 and is overwritten on every run.
 *
 * Build:
 *
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic \
 *     -I/opt/homebrew/include -L/opt/homebrew/lib \
 *     333710_01.c -lgmp -o 333710_01
 *
 * Usage:
 *
 *   A333710_MEMORY_MIB=4096 ./333710_01 N [FROM]
 *
 * N may be 0..63.  FROM may be 0..min(N,30).  The default workspace limit
 * is 4096 MiB.  The exponential state count makes the practical limit much
 * smaller than the mask limit.
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
#error "333710_01 requires a platform with 64-bit unsigned long"
#endif

#define MAX_N 63
#define MAX_LIMBS 5
#define DENSE_CHECK_MAX_N 20
#define VERIFIED_MAX_N 29
#define FIRST_UNKNOWN_N 30
#define INITIAL_CAPACITY ((size_t)1024)
#define DEFAULT_MEMORY_MIB UINT64_C(4096)
#define MIN_MEMORY_MIB UINT64_C(64)
#define MAX_MEMORY_MIB UINT64_C(65536)

static const char *const verified_terms[VERIFIED_MAX_N + 1] = {
    "1",
    "1",
    "2",
    "4",
    "14",
    "28",
    "212",
    "424",
    "3060",
    "13488",
    "131212",
    "262424",
    "6444376",
    "12888752",
    "145241952",
    "2146993212",
    "40313750564",
    "80627501128",
    "2265599072684",
    "4531198145368",
    "173216179971224",
    "3202520631881824",
    "42018513097187068",
    "84037026194374136",
    "7051753589203676704",
    "50056536119264986708",
    "688439474374116497524",
    "12007703969158689627640",
    "711014727699030384945868",
    "1422029455398060769891736"
};

typedef struct {
    uint64_t limit;
    uint64_t current;
    uint64_t peak;
    int term_n;
    int layer_size;
} MemoryBudget;

typedef struct {
    uint64_t *keys;
    uint64_t *quotients;
    uint64_t *values;
    size_t capacity;
    size_t size;
    int limb_count;
    uint64_t bytes;
    MemoryBudget *budget;
} LayerTable;

typedef struct {
    uint64_t states;
    uint64_t divisibility_tests;
    uint64_t transitions;
    uint64_t insertions;
    uint64_t additions;
    double seconds;
    uint64_t peak_bytes;
} TermStats;

typedef struct {
    int maximum_n;
    int dense_check_n;
    int maximum_limb_count;
    uint64_t memory_limit;
    uint64_t dense_expected[DENSE_CHECK_MAX_N + 1];
    bool use_prime_shortcut;
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

static long parse_long_range(const char *text, long minimum,
                             long maximum, const char *name)
{
    char *end = NULL;
    errno = 0;
    long value = strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' ||
        value < minimum || value > maximum) {
        fprintf(stderr, "error: %s must be in %ld..%ld: %s\n",
                name, minimum, maximum, text);
        exit(EXIT_FAILURE);
    }
    return value;
}

static uint64_t parse_memory_limit(void)
{
    const char *text = getenv("A333710_MEMORY_MIB");
    uint64_t mib = DEFAULT_MEMORY_MIB;
    if (text != NULL && *text != '\0') {
        char *end = NULL;
        errno = 0;
        unsigned long long value = strtoull(text, &end, 10);
        if (errno != 0 || end == text || *end != '\0' ||
            value < MIN_MEMORY_MIB || value > MAX_MEMORY_MIB) {
            fprintf(stderr,
                    "error: A333710_MEMORY_MIB must be in %" PRIu64
                    "..%" PRIu64 ": %s\n",
                    MIN_MEMORY_MIB, MAX_MEMORY_MIB, text);
            exit(EXIT_FAILURE);
        }
        mib = (uint64_t)value;
    }
    return mib * UINT64_C(1024) * UINT64_C(1024);
}

static bool environment_flag(const char *name)
{
    const char *text = getenv(name);
    return text != NULL && *text != '\0' && strcmp(text, "0") != 0;
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

static uint64_t mix_mask(uint64_t value)
{
    value ^= value >> 30;
    value *= UINT64_C(0xbf58476d1ce4e5b9);
    value ^= value >> 27;
    value *= UINT64_C(0x94d049bb133111eb);
    value ^= value >> 31;
    return value;
}

static uint64_t layer_table_bytes(size_t capacity, int limb_count)
{
    uint64_t words_per_slot = (uint64_t)limb_count + UINT64_C(2);
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
                "error: A333710 n=%d layer m=%d memory limit exceeded\n"
                "required at the failed allocation: %.3f GiB; "
                "configured limit: %.3f GiB\n"
                "Increase A333710_MEMORY_MIB only if physical RAM permits.\n",
                budget->term_n, budget->layer_size,
                required_gib, configured_gib);
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
    table->quotients = malloc(capacity * sizeof(*table->quotients));
    table->values = malloc(capacity * (size_t)limb_count *
                           sizeof(*table->values));
    if (table->keys == NULL || table->quotients == NULL ||
        table->values == NULL) {
        free(table->values);
        free(table->quotients);
        free(table->keys);
        budget_release(budget, table->bytes);
        die("cannot allocate layer table");
    }
}

static void layer_table_destroy(LayerTable *table)
{
    if (table->capacity == 0) {
        return;
    }
    free(table->values);
    free(table->quotients);
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
        grown.quotients[slot] = table->quotients[old_slot];
        memcpy(grown.values + slot * (size_t)grown.limb_count,
               table->values + old_slot * (size_t)table->limb_count,
               (size_t)table->limb_count * sizeof(uint64_t));
        grown.keys[slot] = key;
        ++grown.size;
    }
    layer_table_destroy(table);
    *table = grown;
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

static bool layer_table_add(LayerTable *table, uint64_t key,
                            uint64_t quotient, const uint64_t *value,
                            TermStats *stats)
{
    if (key == 0) {
        die("empty subset must not be inserted into a layer table");
    }
    if (table->size + 1 > (table->capacity / 10) * 7) {
        layer_table_grow(table);
    }
    size_t mask = table->capacity - 1;
    size_t slot = (size_t)mix_mask(key) & mask;
    while (table->keys[slot] != 0) {
        if (table->keys[slot] == key) {
            if (table->quotients[slot] != quotient) {
                die("the same subset was reached with two quotients");
            }
            add_exact(table->values + slot * (size_t)table->limb_count,
                      value, table->limb_count);
            ++stats->additions;
            return false;
        }
        slot = (slot + 1) & mask;
    }
    table->quotients[slot] = quotient;
    memcpy(table->values + slot * (size_t)table->limb_count, value,
           (size_t)table->limb_count * sizeof(uint64_t));
    table->keys[slot] = key;
    ++table->size;
    ++stats->insertions;
    return true;
}

static void limbs_to_mpz(mpz_t result, const uint64_t *value,
                         int limb_count)
{
    mpz_import(result, (size_t)limb_count, -1, sizeof(uint64_t),
               0, 0, value);
}

static void mpz_to_limbs(uint64_t *value, int limb_count,
                         const mpz_t exact)
{
    memset(value, 0, (size_t)limb_count * sizeof(uint64_t));
    size_t count = 0;
    mpz_export(value, &count, -1, sizeof(uint64_t), 0, 0, exact);
    if (count > (size_t)limb_count) {
        die("integer does not fit the configured exact limbs");
    }
}

static int required_limb_count(int n)
{
    mpz_t factorial;
    mpz_t central_binomial;
    mpz_inits(factorial, central_binomial, NULL);
    mpz_fac_ui(factorial, (unsigned long)n);
    size_t bits = mpz_sizeinbase(factorial, 2);
    int limbs = (int)((bits + 63) / 64);
    if (limbs < 1 || limbs > MAX_LIMBS) {
        mpz_clears(factorial, central_binomial, NULL);
        die("factorial bound does not fit the configured exact limbs");
    }
    mpz_bin_uiui(central_binomial, (unsigned long)n,
                 (unsigned long)(n / 2));
    if (!mpz_fits_ulong_p(central_binomial)) {
        mpz_clears(factorial, central_binomial, NULL);
        die("subset quotient bound does not fit uint64_t");
    }
    mpz_clears(factorial, central_binomial, NULL);
    return limbs;
}

static bool is_prime_int(int value)
{
    if (value < 2) {
        return false;
    }
    if ((value & 1) == 0) {
        return value == 2;
    }
    for (int divisor = 3; divisor <= value / divisor; divisor += 2) {
        if (value % divisor == 0) {
            return false;
        }
    }
    return true;
}

static uint64_t prefix_mask(int length)
{
    if (length == 0) {
        return 0;
    }
    return (UINT64_C(1) << length) - 1;
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

static void compute_term_layer(uint64_t *result, int n,
                               int limb_count, uint64_t memory_limit,
                               TermStats *stats)
{
    memset(result, 0, (size_t)limb_count * sizeof(uint64_t));
    memset(stats, 0, sizeof(*stats));
    if (n == 0) {
        result[0] = 1;
        return;
    }

    MemoryBudget budget = {
        .limit = memory_limit,
        .term_n = n,
        .layer_size = n
    };
    LayerTable current;
    layer_table_initialize(&current, INITIAL_CAPACITY,
                           limb_count, &budget);
    uint64_t one[MAX_LIMBS] = { 1, 0, 0, 0, 0 };
    (void)layer_table_add(&current, prefix_mask(n), 1, one, stats);

    double start = monotonic_seconds();
    for (int size = n; size >= 1; --size) {
        budget.layer_size = size;
        LayerTable next;
        memset(&next, 0, sizeof(next));
        if (size > 1) {
            layer_table_initialize(
                &next, next_layer_initial_capacity(&current),
                limb_count, &budget);
        }

        uint64_t layer_transitions_before = stats->transitions;
        for (size_t slot = 0; slot < current.capacity; ++slot) {
            uint64_t subset = current.keys[slot];
            if (subset == 0) {
                continue;
            }
            ++stats->states;
            if (__builtin_popcountll(subset) != size) {
                die("subset stored in the wrong layer");
            }
            uint64_t quotient = current.quotients[slot];
            const uint64_t *weight =
                current.values + slot * (size_t)limb_count;
            bool numerator_fits =
                quotient <= UINT64_MAX / (uint64_t)size;
            uint64_t numerator64 = numerator_fits
                ? quotient * (uint64_t)size : 0;
            __uint128_t numerator128 = numerator_fits
                ? 0 : (__uint128_t)quotient * (uint64_t)size;

            uint64_t remaining = subset;
            while (remaining != 0) {
                int bit_index = 63 - __builtin_clzll(remaining);
                uint64_t bit = UINT64_C(1) << bit_index;
                uint64_t element = (uint64_t)bit_index + 1;
                remaining ^= bit;
                ++stats->divisibility_tests;

                bool divisible;
                uint64_t child_quotient = 0;
                if (numerator_fits) {
                    divisible = numerator64 % element == 0;
                    if (divisible) {
                        child_quotient = numerator64 / element;
                    }
                } else {
                    divisible = numerator128 % element == 0;
                    if (divisible) {
                        __uint128_t next_quotient =
                            numerator128 / element;
                        if (next_quotient > UINT64_MAX) {
                            die("subset quotient exceeded uint64_t");
                        }
                        child_quotient = (uint64_t)next_quotient;
                    }
                }
                if (!divisible) {
                    continue;
                }

                ++stats->transitions;
                if (size == 1) {
                    if (child_quotient != 1) {
                        die("empty subset has a quotient other than one");
                    }
                    add_exact(result, weight, limb_count);
                } else {
                    (void)layer_table_add(&next, subset ^ bit,
                                          child_quotient, weight, stats);
                }
            }
        }

        uint64_t current_states = (uint64_t)current.size;
        uint64_t next_states = size > 1 ? (uint64_t)next.size : 1;
        uint64_t layer_transitions =
            stats->transitions - layer_transitions_before;
        if (size == n || size == 1 || size % 5 == 0 ||
            current_states >= UINT64_C(1000000)) {
            fprintf(stderr,
                    "333710_01: n=%d layer m=%d, states=%" PRIu64
                    " -> %" PRIu64 ", transitions=%" PRIu64
                    ", workspace=%.3f GiB, %.3f s\n",
                    n, size, current_states, next_states,
                    layer_transitions,
                    (double)budget.current /
                        (1024.0 * 1024.0 * 1024.0),
                    monotonic_seconds() - start);
        }

        layer_table_destroy(&current);
        if (size > 1) {
            current = next;
        }
    }
    if (budget.current != 0) {
        die("layer table memory accounting did not return to zero");
    }
    stats->seconds = monotonic_seconds() - start;
    stats->peak_bytes = budget.peak;
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
    uint64_t *products = malloc(state_count * sizeof(*products));
    uint64_t *counts = calloc(state_count, sizeof(*counts));
    if (products == NULL || counts == NULL) {
        free(counts);
        free(products);
        die("cannot allocate independent dense-check workspace");
    }
    uint64_t factorial[DENSE_CHECK_MAX_N + 1];
    factorial[0] = 1;
    for (int index = 1; index <= n; ++index) {
        factorial[index] = factorial[index - 1] * (uint64_t)index;
    }
    products[0] = 1;
    counts[0] = 1;
    for (size_t subset = 1; subset < state_count; ++subset) {
        size_t bit = subset & (0 - subset);
        size_t previous = subset ^ bit;
        int element = __builtin_ctzll((uint64_t)bit) + 1;
        int size = __builtin_popcountll((uint64_t)subset);
        products[subset] = products[previous] * (uint64_t)element;
        if (products[subset] % factorial[size] != 0) {
            continue;
        }
        uint64_t sum = 0;
        size_t remaining = subset;
        while (remaining != 0) {
            size_t last = remaining & (0 - remaining);
            if (UINT64_MAX - sum < counts[subset ^ last]) {
                die("dense-check counter overflow");
            }
            sum += counts[subset ^ last];
            remaining ^= last;
        }
        counts[subset] = sum;
    }
    for (int index = 1; index <= n; ++index) {
        context->dense_expected[index] =
            counts[((size_t)1 << index) - 1];
    }
    free(counts);
    free(products);
    fprintf(stderr,
            "333710_01: independent dense subset-DP check prepared "
            "for n=0..%d\n", n);
}

static void verify_value(const Context *context, int n,
                         const uint64_t *value, int limb_count)
{
    if (n <= context->dense_check_n) {
        if (value[0] != context->dense_expected[n]) {
            die("sparse layer DP differs from independent dense subset DP");
        }
        for (int limb = 1; limb < limb_count; ++limb) {
            if (value[limb] != 0) {
                die("dense-check value unexpectedly exceeds uint64_t");
            }
        }
    }
    if (n <= VERIFIED_MAX_N) {
        mpz_t actual;
        mpz_t expected;
        mpz_inits(actual, expected, NULL);
        limbs_to_mpz(actual, value, limb_count);
        if (mpz_set_str(expected, verified_terms[n], 10) != 0) {
            die("invalid built-in verified term");
        }
        if (mpz_cmp(actual, expected) != 0) {
            mpz_clears(actual, expected, NULL);
            die("computed value differs from the verified prefix");
        }
        mpz_clears(actual, expected, NULL);
    }
}

static void load_verified_value(uint64_t *value, int limb_count, int n)
{
    if (n < 0 || n > VERIFIED_MAX_N) {
        die("requested term is not in the built-in verified prefix");
    }
    mpz_t exact;
    mpz_init(exact);
    if (mpz_set_str(exact, verified_terms[n], 10) != 0) {
        mpz_clear(exact);
        die("invalid built-in verified term");
    }
    mpz_to_limbs(value, limb_count, exact);
    mpz_clear(exact);
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
                        const uint64_t *value, int limb_count)
{
    mpz_t exact;
    mpz_init(exact);
    limbs_to_mpz(exact, value, limb_count);
    if (gmp_fprintf(context->output, "%d %Zd\n", n, exact) < 0) {
        mpz_clear(exact);
        die("cannot write b-file term");
    }
    flush_output(context);
    gmp_printf("%d %Zd\n", n, exact);
    fflush(stdout);
    mpz_clear(exact);
}

static void double_exact(uint64_t *result, const uint64_t *previous,
                         int limb_count)
{
    memcpy(result, previous, (size_t)limb_count * sizeof(uint64_t));
    add_exact(result, previous, limb_count);
}

int main(int argc, char **argv)
{
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "usage: %s N [FROM]\n", argv[0]);
        return EXIT_FAILURE;
    }
    int maximum_n = (int)parse_long_range(argv[1], 0, MAX_N, "N");
    int from = 0;
    if (argc == 3) {
        long maximum_from = maximum_n < FIRST_UNKNOWN_N
            ? maximum_n : FIRST_UNKNOWN_N;
        from = (int)parse_long_range(argv[2], 0, maximum_from, "FROM");
    }

    char *part_path = path_beside_executable(argv[0],
                                             "b333710_part.txt");
    char *final_path = path_beside_executable(argv[0], "b333710.txt");
    FILE *output = fopen(part_path, "w");
    if (output == NULL) {
        fprintf(stderr, "error: cannot open %s: %s\n",
                part_path, strerror(errno));
        free(final_path);
        free(part_path);
        return EXIT_FAILURE;
    }

    Context context;
    memset(&context, 0, sizeof(context));
    context.maximum_n = maximum_n;
    context.maximum_limb_count = required_limb_count(maximum_n);
    context.memory_limit = parse_memory_limit();
    context.use_prime_shortcut =
        !environment_flag("A333710_NO_PRIME_SHORTCUT");
    context.output = output;
    context.part_path = part_path;
    prepare_dense_check(&context);

    fprintf(stderr,
            "333710_01: N=%d, FROM=%d, sparse two-layer exact DP, "
            "memory limit=%.3f GiB, prime shortcut=%s\n",
            maximum_n, from,
            (double)context.memory_limit /
                (1024.0 * 1024.0 * 1024.0),
            context.use_prime_shortcut ? "on" : "off");

    uint64_t previous[MAX_LIMBS] = { 0 };
    uint64_t value[MAX_LIMBS] = { 0 };
    for (int n = 0; n < from; ++n) {
        load_verified_value(value, context.maximum_limb_count, n);
        verify_value(&context, n, value, context.maximum_limb_count);
        write_value(&context, n, value, context.maximum_limb_count);
        memcpy(previous, value, sizeof(previous));
    }
    if (from > 0) {
        load_verified_value(previous, context.maximum_limb_count,
                            from - 1);
    }

    for (int n = from; n <= maximum_n; ++n) {
        memset(value, 0, sizeof(value));
        int term_limb_count = required_limb_count(n);
        if (n == 0) {
            value[0] = 1;
        } else if (context.use_prime_shortcut && is_prime_int(n)) {
            double_exact(value, previous, term_limb_count);
            fprintf(stderr,
                    "333710_01: n=%d is prime; used a(n)=2*a(n-1)\n",
                    n);
        } else {
            TermStats stats;
            compute_term_layer(value, n, term_limb_count,
                               context.memory_limit, &stats);
            fprintf(stderr,
                    "333710_01: n=%d exact layer DP done, "
                    "states=%" PRIu64 ", tests=%" PRIu64
                    ", transitions=%" PRIu64 ", peak=%.3f GiB, %.3f s\n",
                    n, stats.states, stats.divisibility_tests,
                    stats.transitions,
                    (double)stats.peak_bytes /
                        (1024.0 * 1024.0 * 1024.0),
                    stats.seconds);
        }
        verify_value(&context, n, value, term_limb_count);
        write_value(&context, n, value, term_limb_count);
        memset(previous, 0, sizeof(previous));
        memcpy(previous, value,
               (size_t)term_limb_count * sizeof(uint64_t));
    }

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
