/*
 * A333710 -- independent exact top-down memoization + CRT verifier.
 *
 * This implementation is deliberately different from 333710_01.c.
 * 01 propagates path counts through two adjacent subset-size layers using
 * fixed-width exact integers.  This program evaluates the recurrence
 *
 *   F(empty)=1,
 *   F(S)=sum F(S\{j})  for j in S such that j divides q(S)*|S|,
 *   q(S\{j})=q(S)*|S|/j,
 *
 * recursively and memoizes F(S) for every reached subset.  It stores two
 * 61-bit modular residues in one Robin-Hood hash-table traversal and performs
 * a rigorous CRT reconstruction afterward.
 *
 * The quotient q(S)=product(S)/|S|! is uniquely determined by S, so it is
 * carried down the recursion but is not stored in the memo table.  For
 * n<=31 it fits uint64_t because q(S)<=C(n,|S|).  q*|S| is formed in
 * unsigned 128-bit arithmetic when needed.
 *
 * Safety:
 *
 *   0 <= a(n) <= n!.
 *
 * Distinct prime moduli are selected until their product exceeds n!.  CRT is
 * therefore unique.  The reconstructed value is checked against n! and
 * replayed modulo every prime.  The complete subset key is compared in the
 * hash table; hash collisions cannot change the result.  Table growth is
 * guarded by A333710_02_MEMORY_MIB before allocation.
 *
 * Every nonprime n=0..N is independently computed by this top-down
 * algorithm.  For a prime p, the proved identity a(p)=2*a(p-1) is used;
 * built-in values are used only to check n<=29, never as inputs.  Thus N=31
 * independently recomputes every composite through 30.  Complete success
 * writes b333710_1.txt beside the executable; progress is flushed to
 * b333710_1_part.txt and the formal b-file is overwritten on success.
 *
 * Build:
 *
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic \
 *     -I/opt/homebrew/include -L/opt/homebrew/lib \
 *     333710_02.c -lgmp -o 333710_02
 *
 * Usage:
 *
 *   A333710_02_MEMORY_MIB=6144 ./333710_02 31
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
#error "333710_02 requires a platform with 64-bit unsigned long"
#endif

#define MAX_N 31
#define VERIFIED_MAX_N 29
#define MAX_MODULUS_COUNT 2
#define INITIAL_CAPACITY ((size_t)1 << 16)
#define LOAD_PERCENT 85
#define DEFAULT_MEMORY_MIB UINT64_C(6144)
#define MIN_MEMORY_MIB UINT64_C(64)
#define MAX_MEMORY_MIB UINT64_C(65536)
#define REPORT_INTERVAL UINT64_C(10000000)

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
    uint64_t *keys;
    uint64_t *values;
    size_t capacity;
    size_t size;
    size_t grow_at;
    size_t modulus_count;
    uint64_t current_bytes;
    uint64_t peak_bytes;
    uint64_t memory_limit;
    int term_n;
    double start_seconds;
} MemoTable;

typedef struct {
    MemoTable memo;
    uint64_t moduli[MAX_MODULUS_COUNT];
    size_t modulus_count;
    uint64_t one[MAX_MODULUS_COUNT];
    uint64_t computed_states;
    uint64_t memo_hits;
    uint64_t divisibility_tests;
    uint64_t transitions;
    uint64_t next_report;
    double start_seconds;
} RecurrenceContext;

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

static bool is_prime(int n)
{
    if (n < 2) {
        return false;
    }
    for (int divisor = 2; divisor <= n / divisor; ++divisor) {
        if (n % divisor == 0) {
            return false;
        }
    }
    return true;
}

static uint64_t parse_memory_limit(void)
{
    const char *text = getenv("A333710_02_MEMORY_MIB");
    uint64_t mib = DEFAULT_MEMORY_MIB;
    if (text != NULL && *text != '\0') {
        char *end = NULL;
        errno = 0;
        unsigned long long value = strtoull(text, &end, 10);
        if (errno != 0 || end == text || *end != '\0' ||
            value < MIN_MEMORY_MIB || value > MAX_MEMORY_MIB) {
            fprintf(stderr,
                    "error: A333710_02_MEMORY_MIB must be in %" PRIu64
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

static uint64_t mix_mask(uint64_t value)
{
    value ^= value >> 30;
    value *= UINT64_C(0xbf58476d1ce4e5b9);
    value ^= value >> 27;
    value *= UINT64_C(0x94d049bb133111eb);
    value ^= value >> 31;
    return value;
}

static uint64_t table_bytes(size_t capacity, size_t modulus_count)
{
    uint64_t words = UINT64_C(1) + (uint64_t)modulus_count;
    if ((uint64_t)capacity > UINT64_MAX /
        (words * (uint64_t)sizeof(uint64_t))) {
        die("memo table byte count overflow");
    }
    return (uint64_t)capacity * words *
           (uint64_t)sizeof(uint64_t);
}

static size_t probe_distance(size_t slot, uint64_t key,
                             size_t capacity)
{
    size_t mask = capacity - 1;
    size_t home = (size_t)mix_mask(key) & mask;
    return (slot - home) & mask;
}

static void allocate_table_arrays(size_t capacity, size_t modulus_count,
                                  uint64_t **keys, uint64_t **values)
{
    if (modulus_count == 0 || modulus_count > MAX_MODULUS_COUNT) {
        die("invalid memo-table modulus count");
    }
    if (capacity > SIZE_MAX / modulus_count ||
        capacity * modulus_count > SIZE_MAX / sizeof(uint64_t)) {
        die("memo table allocation size overflow");
    }
    *keys = calloc(capacity, sizeof(**keys));
    *values = calloc(capacity * modulus_count, sizeof(**values));
    if (*keys == NULL || *values == NULL) {
        free(*values);
        free(*keys);
        die("cannot allocate memo table");
    }
}

static size_t robin_insert_raw(uint64_t *keys, uint64_t *values,
                               size_t capacity, size_t modulus_count,
                               uint64_t key, const uint64_t *value)
{
    uint64_t carried_key = key;
    uint64_t carried_value[MAX_MODULUS_COUNT] = { 0, 0 };
    memcpy(carried_value, value,
           modulus_count * sizeof(uint64_t));
    bool carrying_original = true;
    size_t original_slot = SIZE_MAX;
    size_t mask = capacity - 1;
    size_t slot = (size_t)mix_mask(carried_key) & mask;
    size_t distance = 0;

    for (;;) {
        if (keys[slot] == 0) {
            keys[slot] = carried_key;
            memcpy(values + slot * modulus_count, carried_value,
                   modulus_count * sizeof(uint64_t));
            if (carrying_original) {
                original_slot = slot;
            }
            return original_slot;
        }
        if (keys[slot] == carried_key) {
            die("duplicate Robin-Hood insertion");
        }
        size_t resident_distance =
            probe_distance(slot, keys[slot], capacity);
        if (resident_distance < distance) {
            uint64_t displaced_key = keys[slot];
            uint64_t displaced_value[MAX_MODULUS_COUNT] = { 0, 0 };
            memcpy(displaced_value, values + slot * modulus_count,
                   modulus_count * sizeof(uint64_t));
            keys[slot] = carried_key;
            memcpy(values + slot * modulus_count, carried_value,
                   modulus_count * sizeof(uint64_t));
            if (carrying_original) {
                original_slot = slot;
                carrying_original = false;
            }
            carried_key = displaced_key;
            memcpy(carried_value, displaced_value,
                   modulus_count * sizeof(uint64_t));
            distance = resident_distance;
        }
        slot = (slot + 1) & mask;
        ++distance;
        if (distance >= capacity) {
            die("Robin-Hood table is full");
        }
    }
}

static void memo_initialize(MemoTable *table, size_t modulus_count,
                            uint64_t memory_limit, int term_n,
                            double start_seconds)
{
    memset(table, 0, sizeof(*table));
    table->capacity = INITIAL_CAPACITY;
    table->modulus_count = modulus_count;
    table->memory_limit = memory_limit;
    table->term_n = term_n;
    table->start_seconds = start_seconds;
    table->grow_at = (table->capacity / 100) * LOAD_PERCENT;
    table->current_bytes = table_bytes(table->capacity, modulus_count);
    table->peak_bytes = table->current_bytes;
    if (table->current_bytes > memory_limit) {
        die("memory limit is too small for the initial memo table");
    }
    allocate_table_arrays(table->capacity, modulus_count,
                          &table->keys, &table->values);
}

static void memo_destroy(MemoTable *table)
{
    free(table->values);
    free(table->keys);
    memset(table, 0, sizeof(*table));
}

static void memo_grow(MemoTable *table)
{
    if (table->capacity > SIZE_MAX / 2) {
        die("memo table capacity overflow");
    }
    size_t new_capacity = table->capacity * 2;
    uint64_t new_bytes = table_bytes(new_capacity,
                                     table->modulus_count);
    if (new_bytes > table->memory_limit ||
        table->current_bytes > table->memory_limit - new_bytes) {
        fprintf(stderr,
                "error: A333710_02 n=%d memory limit exceeded\n"
                "required during failed growth: %.3f GiB; "
                "configured limit: %.3f GiB\n",
                table->term_n,
                (double)(table->current_bytes + new_bytes) /
                    (1024.0 * 1024.0 * 1024.0),
                (double)table->memory_limit /
                    (1024.0 * 1024.0 * 1024.0));
        exit(EXIT_FAILURE);
    }

    fprintf(stderr,
            "333710_02: n=%d growing memo at %zu states: "
            "%.3f -> %.3f GiB, %.3f s\n",
            table->term_n, table->size,
            (double)table->current_bytes /
                (1024.0 * 1024.0 * 1024.0),
            (double)new_bytes / (1024.0 * 1024.0 * 1024.0),
            monotonic_seconds() - table->start_seconds);

    uint64_t *new_keys = NULL;
    uint64_t *new_values = NULL;
    allocate_table_arrays(new_capacity, table->modulus_count,
                          &new_keys, &new_values);
    for (size_t slot = 0; slot < table->capacity; ++slot) {
        uint64_t key = table->keys[slot];
        if (key == 0) {
            continue;
        }
        (void)robin_insert_raw(
            new_keys, new_values, new_capacity,
            table->modulus_count, key,
            table->values + slot * table->modulus_count);
    }
    uint64_t growth_peak = table->current_bytes + new_bytes;
    if (growth_peak > table->peak_bytes) {
        table->peak_bytes = growth_peak;
    }
    free(table->values);
    free(table->keys);
    table->keys = new_keys;
    table->values = new_values;
    table->capacity = new_capacity;
    table->current_bytes = new_bytes;
    table->grow_at = (new_capacity / 100) * LOAD_PERCENT;
    fprintf(stderr,
            "333710_02: n=%d memo growth complete, %.3f s\n",
            table->term_n,
            monotonic_seconds() - table->start_seconds);
}

static const uint64_t *memo_lookup(const MemoTable *table,
                                   uint64_t key)
{
    size_t mask = table->capacity - 1;
    size_t slot = (size_t)mix_mask(key) & mask;
    size_t distance = 0;
    while (table->keys[slot] != 0) {
        if (table->keys[slot] == key) {
            return table->values + slot * table->modulus_count;
        }
        size_t resident_distance =
            probe_distance(slot, table->keys[slot], table->capacity);
        if (resident_distance < distance) {
            return NULL;
        }
        slot = (slot + 1) & mask;
        ++distance;
        if (distance >= table->capacity) {
            return NULL;
        }
    }
    return NULL;
}

static const uint64_t *memo_insert(MemoTable *table, uint64_t key,
                                   const uint64_t *value)
{
    if (table->size + 1 > table->grow_at) {
        memo_grow(table);
    }
    size_t slot = robin_insert_raw(
        table->keys, table->values, table->capacity,
        table->modulus_count, key, value);
    ++table->size;
    return table->values + slot * table->modulus_count;
}

static uint64_t add_mod(uint64_t left, uint64_t right,
                        uint64_t modulus)
{
    uint64_t sum = left + right;
    return sum >= modulus ? sum - modulus : sum;
}

static uint64_t subtract_mod(uint64_t left, uint64_t right,
                             uint64_t modulus)
{
    return left >= right ? left - right : modulus - (right - left);
}

static uint64_t multiply_mod(uint64_t left, uint64_t right,
                             uint64_t modulus)
{
    if (modulus == 0) {
        die("zero CRT modulus");
    }
    return (uint64_t)(((__uint128_t)left * right) % modulus);
}

static uint64_t power_mod(uint64_t base, uint64_t exponent,
                          uint64_t modulus)
{
    uint64_t result = 1;
    while (exponent != 0) {
        if ((exponent & 1U) != 0) {
            result = multiply_mod(result, base, modulus);
        }
        base = multiply_mod(base, base, modulus);
        exponent >>= 1;
    }
    return result;
}

static const uint64_t *count_state(RecurrenceContext *context,
                                   uint64_t subset,
                                   uint64_t quotient)
{
    if (context->modulus_count == 0 ||
        context->modulus_count > MAX_MODULUS_COUNT) {
        die("invalid recurrence modulus count");
    }
    if (subset == 0) {
        if (quotient != 1) {
            die("empty subset has a quotient other than one");
        }
        return context->one;
    }

    const uint64_t *cached = memo_lookup(&context->memo, subset);
    if (cached != NULL) {
        ++context->memo_hits;
        return cached;
    }
    ++context->computed_states;
    if (context->computed_states >= context->next_report) {
        fprintf(stderr,
                "333710_02: n=%d, states=%" PRIu64
                ", hits=%" PRIu64 ", transitions=%" PRIu64
                ", table=%.3f GiB, %.3f s\n",
                context->memo.term_n, context->computed_states,
                context->memo_hits, context->transitions,
                (double)context->memo.current_bytes /
                    (1024.0 * 1024.0 * 1024.0),
                monotonic_seconds() - context->start_seconds);
        context->next_report += REPORT_INTERVAL;
    }

    int size = __builtin_popcountll(subset);
    uint64_t sum[MAX_MODULUS_COUNT] = { 0, 0 };
    bool numerator_fits = quotient <= UINT64_MAX / (uint64_t)size;
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
        ++context->divisibility_tests;

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
                __uint128_t next = numerator128 / element;
                if (next > UINT64_MAX) {
                    die("subset quotient exceeded uint64_t");
                }
                child_quotient = (uint64_t)next;
            }
        }
        if (!divisible) {
            continue;
        }

        ++context->transitions;
        const uint64_t *child = count_state(
            context, subset ^ bit, child_quotient);
        for (size_t pass = 0; pass < context->modulus_count; ++pass) {
            sum[pass] = add_mod(sum[pass], child[pass],
                                context->moduli[pass]);
        }
    }
    return memo_insert(&context->memo, subset, sum);
}

static size_t choose_moduli(uint64_t *moduli, mpz_t product,
                            const mpz_t bound)
{
    mpz_t candidate;
    mpz_t prime;
    mpz_inits(candidate, prime, NULL);
    mpz_set_ui(candidate, 1);
    mpz_mul_2exp(candidate, candidate, 60);
    mpz_set_ui(product, 1);

    size_t count = 0;
    while (mpz_cmp(product, bound) <= 0) {
        if (count >= MAX_MODULUS_COUNT) {
            mpz_clears(candidate, prime, NULL);
            die("factorial bound needs more configured CRT primes");
        }
        mpz_nextprime(prime, candidate);
        uint64_t modulus = (uint64_t)mpz_get_ui(prime);
        if (modulus >= (UINT64_C(1) << 63)) {
            mpz_clears(candidate, prime, NULL);
            die("selected CRT modulus is too large");
        }
        moduli[count++] = modulus;
        mpz_mul_ui(product, product, (unsigned long)modulus);
        mpz_set(candidate, prime);
    }
    mpz_clears(candidate, prime, NULL);
    return count;
}

static void reconstruct_crt(mpz_t result, const uint64_t *residues,
                            const uint64_t *moduli, size_t count)
{
    mpz_t product;
    mpz_init_set_ui(product, 1);
    mpz_set_ui(result, 0);
    for (size_t index = 0; index < count; ++index) {
        uint64_t modulus = moduli[index];
        uint64_t product_mod =
            (uint64_t)mpz_fdiv_ui(product, (unsigned long)modulus);
        uint64_t inverse = power_mod(product_mod, modulus - 2,
                                     modulus);
        uint64_t result_mod =
            (uint64_t)mpz_fdiv_ui(result, (unsigned long)modulus);
        uint64_t difference = subtract_mod(
            residues[index], result_mod, modulus);
        uint64_t multiplier = multiply_mod(
            difference, inverse, modulus);
        mpz_addmul_ui(result, product, (unsigned long)multiplier);
        mpz_mul_ui(product, product, (unsigned long)modulus);
    }
    mpz_clear(product);
}

static void compute_exact_term(mpz_t result, int n,
                               uint64_t memory_limit)
{
    mpz_t bound;
    mpz_t modulus_product;
    mpz_inits(bound, modulus_product, NULL);
    mpz_fac_ui(bound, (unsigned long)n);

    RecurrenceContext context;
    memset(&context, 0, sizeof(context));
    context.modulus_count = choose_moduli(
        context.moduli, modulus_product, bound);
    for (size_t pass = 0; pass < context.modulus_count; ++pass) {
        context.one[pass] = 1;
    }
    context.next_report = REPORT_INTERVAL;
    context.start_seconds = monotonic_seconds();
    memo_initialize(&context.memo, context.modulus_count,
                    memory_limit, n, context.start_seconds);

    fprintf(stderr,
            "333710_02: n=%d top-down memo + packed CRT, "
            "%zu prime%s, bound=%zu bits, load limit=%d%%, "
            "memory limit=%.3f GiB\n",
            n, context.modulus_count,
            context.modulus_count == 1 ? "" : "s",
            mpz_sizeinbase(bound, 2), LOAD_PERCENT,
            (double)memory_limit / (1024.0 * 1024.0 * 1024.0));

    uint64_t full = n == 0 ? 0 : (UINT64_C(1) << n) - 1;
    const uint64_t *residues = count_state(&context, full, 1);
    uint64_t residue_copy[MAX_MODULUS_COUNT] = { 0, 0 };
    memcpy(residue_copy, residues,
           context.modulus_count * sizeof(uint64_t));
    reconstruct_crt(result, residue_copy, context.moduli,
                    context.modulus_count);
    if (mpz_sgn(result) < 0 || mpz_cmp(result, bound) > 0) {
        die("CRT result lies outside 0..n!");
    }
    for (size_t pass = 0; pass < context.modulus_count; ++pass) {
        if ((uint64_t)mpz_fdiv_ui(
                result, (unsigned long)context.moduli[pass]) !=
            residue_copy[pass]) {
            die("CRT residue replay failed");
        }
    }
    fprintf(stderr,
            "333710_02: n=%d exact done, states=%" PRIu64
            ", hits=%" PRIu64 ", tests=%" PRIu64
            ", transitions=%" PRIu64 ", table=%.3f GiB, "
            "growth peak=%.3f GiB, %.3f s\n",
            n, context.computed_states, context.memo_hits,
            context.divisibility_tests, context.transitions,
            (double)context.memo.current_bytes /
                (1024.0 * 1024.0 * 1024.0),
            (double)context.memo.peak_bytes /
                (1024.0 * 1024.0 * 1024.0),
            monotonic_seconds() - context.start_seconds);
    memo_destroy(&context.memo);
    mpz_clears(bound, modulus_product, NULL);
}

static void flush_output(FILE *output, const char *path)
{
    if (fflush(output) != 0 || fsync(fileno(output)) != 0) {
        fprintf(stderr, "error: cannot flush %s: %s\n",
                path, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

static void write_mpz(FILE *output, const char *path,
                      int n, const mpz_t value)
{
    if (gmp_fprintf(output, "%d %Zd\n", n, value) < 0) {
        die("cannot write b-file term");
    }
    flush_output(output, path);
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
    uint64_t memory_limit = parse_memory_limit();
    char *part_path = path_beside_executable(
        argv[0], "b333710_1_part.txt");
    char *final_path = path_beside_executable(
        argv[0], "b333710_1.txt");
    FILE *output = fopen(part_path, "w");
    if (output == NULL) {
        fprintf(stderr, "error: cannot open %s: %s\n",
                part_path, strerror(errno));
        free(final_path);
        free(part_path);
        return EXIT_FAILURE;
    }

    mpz_t value;
    mpz_t expected;
    mpz_inits(value, expected, NULL);
    for (int n = 0; n <= maximum_n; ++n) {
        if (is_prime(n)) {
            /*
             * At i=p-1, the final entry must divide p, hence is 1 or p.
             * If it is p, delete it.  If it is 1, delete it and replace the
             * unique p in the prefix by 1.  Since gcd(p,i!)=1 for i<=p-2,
             * this replacement preserves every earlier condition.  Both
             * operations are reversible and give a(p)=2*a(p-1).
             */
            mpz_mul_ui(value, value, 2);
            fprintf(stderr,
                    "333710_02: n=%d is prime; used a(n)=2*a(n-1)\n",
                    n);
        } else {
            compute_exact_term(value, n, memory_limit);
        }
        if (n <= VERIFIED_MAX_N) {
            if (mpz_set_str(expected, verified_terms[n], 10) != 0) {
                die("invalid built-in verified term");
            }
            if (mpz_cmp(value, expected) != 0) {
                die("top-down CRT value differs from verified prefix");
            }
        }
        write_mpz(output, part_path, n, value);
    }
    mpz_clears(value, expected, NULL);

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
