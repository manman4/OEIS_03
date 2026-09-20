/*
 * A333082 -- sparse ratio-class inclusion-exclusion DP.
 *
 * Count permutations sigma of [n] for which the rational numbers
 * sigma(k)/k are pairwise distinct.
 *
 * Ratio classes
 * -------------
 * Regard (i,j) as a cell in an n by n board.  A permutation selects one
 * cell in every row and column.  If a/b is a reduced positive fraction,
 * the cells having j/i=a/b are exactly
 *
 *                         (b*t, a*t),
 *
 * for 1 <= t <= floor(n/max(a,b)).  Distinct cells in one ratio class do
 * not share a row or column.
 *
 * If a full rook placement contains u cells from one ratio class, then
 *
 *   [u <= 1] = 1 + sum_{r=2..u} C(u,r) (-1)^(r-1) (r-1).            (1)
 *
 * For u>=2 this follows from the binomial identities for the zeroth and
 * first alternating moments; the sum after the initial 1 is -1.  Thus a
 * nontrivial ratio class contributes either no marked cell with weight 1,
 * or a subset S of at least two cells with weight
 * (-1)^(|S|-1)(|S|-1).
 *
 * The sparse DP processes every non-diagonal ratio class once.  A state is
 * the pair of masks of rows and columns occupied by marked cells.  States
 * with the same masks can be combined because compatibility with future
 * classes depends only on these masks.
 *
 * Transposition sends the class a/b to its reciprocal b/a.  Reciprocal
 * classes are therefore processed in adjacent pairs.  The product of their
 * two transition operators commutes with transposition (the two class
 * polynomials commute), so after each complete pair the states (R,C) and
 * (C,R) are folded together.  The stored coefficient is their orbit sum.
 * Equation (2) is symmetric in R and C, so this exact quotient is also valid
 * at the final evaluation.  No quotient is taken between the two members of
 * a reciprocal pair.
 *
 * The diagonal class j/i=1 contains n cells and would have exponentially
 * many explicit choices.  It is left until the final evaluation.  For a DP
 * state with k marked rows, put m=n-k and let d be the number of indices
 * unused as both a row and a column.  Exactly C(d,r) diagonal subsets of
 * size r are compatible, and each leaves (m-r)! completions.  The state's
 * exact completion multiplier is therefore
 *
 *   F(m,d) = m! + sum_{r=2..d} C(d,r)(-1)^(r-1)(r-1)(m-r)!.          (2)
 *
 * Summing weight(state)*F(m,d) gives a(n).  Equations (1) and (2) are
 * identities over the integers; no heuristic pruning is used.
 *
 * Exact arithmetic and validation
 * -------------------------------
 * Sparse passes are performed modulo distinct 61-bit primes.  CRT continues
 * until the product of the moduli is strictly greater than n!.  Since the
 * answer is a number of permutations, 0 <= a(n) <= n!, so the reconstructed
 * nonnegative residue is the unique exact answer.
 *
 * For n<=10 an independent DFS reduces every candidate ratio sigma(k)/k to
 * a coprime numerator-denominator pair and checks uniqueness directly.  The
 * published terms through n=15 are post-computation checks only.
 *
 * Every completed term is flushed and fsynced to b333082_01_part.txt beside
 * the executable.  Complete success atomically replaces b333082_01.txt.
 * Progress is reported to stderr at most once per minute.
 *
 * Build:
 *
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic 333082_01.c \
 *     $(pkg-config --cflags --libs gmp) -o 333082_01
 *
 * Usage:
 *
 *   A333082_MEMORY_MIB=8192 ./333082_01 N [FROM]
 *
 * N is in 0..31.  FROM is at most 16; verified terms below FROM are copied
 * so stdout and the b-file remain complete from offset 0.  The exponential
 * state count normally makes the practical limit smaller than 31.
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
#error "333082_01.c requires a platform with 64-bit unsigned long"
#endif

#if !defined(__SIZEOF_INT128__)
#error "333082_01.c requires unsigned __int128"
#endif

__extension__ typedef unsigned __int128 U128;

#define MAX_N 31
#define DIRECT_CHECK_MAX_N 10
#define VERIFIED_MAX_N 15
#define FIRST_UNVERIFIED_N (VERIFIED_MAX_N + 1)
#define MAX_CRT_PRIMES 4
#define INITIAL_CAPACITY ((size_t)16)
#define LOAD_NUMERATOR ((size_t)7)
#define LOAD_DENOMINATOR ((size_t)10)
#define DEFAULT_MEMORY_MIB UINT64_C(8192)
#define MIN_MEMORY_MIB UINT64_C(64)
#define MAX_MEMORY_MIB UINT64_C(65536)
#ifndef PROGRESS_INTERVAL_SECONDS
#define PROGRESS_INTERVAL_SECONDS 60.0
#endif

static const char *const verified_terms[VERIFIED_MAX_N + 1] = {
    "1", "1", "1", "5", "13", "79", "345", "2785",
    "19595", "171141", "1555181", "18998391", "184988197",
    "2675065491", "35582840563", "490186693669"
};

typedef struct {
    uint32_t rows;
    uint32_t columns;
    int8_t coefficient;
} Choice;

typedef struct {
    Choice *choices;
    size_t choice_count;
    int numerator;
    int denominator;
    int cell_count;
} RatioGroup;

typedef struct {
    uint64_t *keys;       /* key + 1; zero denotes an empty bucket */
    uint64_t *values;
    size_t capacity;
    size_t size;
    uint64_t bytes;
} StateTable;

typedef struct {
    uint64_t limit;
    uint64_t current;
    uint64_t peak;
} MemoryBudget;

typedef struct {
    uint64_t source_states;
    uint64_t compatible_choices;
    uint64_t insertions;
    size_t final_states;
    uint64_t peak_bytes;
    double seconds;
} PassStats;

typedef struct {
    int n;
    int crt_pass;
    int crt_count;
    size_t group_count;
    double started;
    double last_report;
} Progress;

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

static void report_progress(Progress *progress, const char *phase,
                            size_t group_number, size_t scanned,
                            size_t scan_total, size_t states,
                            const MemoryBudget *budget)
{
    double now = monotonic_seconds();
    if (now - progress->last_report < PROGRESS_INTERVAL_SECONDS) {
        return;
    }
    double percentage = scan_total == 0
                            ? 100.0
                            : 100.0 * (double)scanned / (double)scan_total;
    fprintf(stderr,
            "A333082 n=%d progress CRT=%d/%d group=%zu/%zu phase=%s "
            "scan=%.1f%% states=%zu elapsed=%.0fs peak=%.1f MiB\n",
            progress->n, progress->crt_pass, progress->crt_count,
            group_number, progress->group_count, phase, percentage, states,
            now - progress->started,
            (double)budget->peak / (1024.0 * 1024.0));
    fflush(stderr);
    progress->last_report = now;
}

static void *checked_malloc(size_t count, size_t size)
{
    if (size != 0 && count > SIZE_MAX / size) {
        die("allocation size overflow");
    }
    void *memory = malloc(count * size);
    if (memory == NULL && count != 0) {
        die("memory allocation failed");
    }
    return memory;
}

static void *checked_calloc(size_t count, size_t size)
{
    if (size != 0 && count > SIZE_MAX / size) {
        die("allocation size overflow");
    }
    void *memory = calloc(count, size);
    if (memory == NULL && count != 0) {
        die("memory allocation failed");
    }
    return memory;
}

static int parse_int_range(const char *text, int minimum, int maximum,
                           const char *name)
{
    char *end = NULL;
    errno = 0;
    long value = strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' ||
        value < minimum || value > maximum) {
        fprintf(stderr, "error: %s must be in %d..%d: %s\n",
                name, minimum, maximum, text);
        exit(EXIT_FAILURE);
    }
    return (int)value;
}

static uint64_t parse_memory_limit(void)
{
    const char *text = getenv("A333082_MEMORY_MIB");
    uint64_t mib = DEFAULT_MEMORY_MIB;
    if (text != NULL && *text != '\0') {
        char *end = NULL;
        errno = 0;
        unsigned long long value = strtoull(text, &end, 10);
        if (errno != 0 || end == text || *end != '\0' ||
            value < MIN_MEMORY_MIB || value > MAX_MEMORY_MIB) {
            fprintf(stderr,
                    "error: A333082_MEMORY_MIB must be in %" PRIu64
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
    char *path = checked_malloc(directory_length + filename_length + 2, 1);
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

static uint64_t mix64(uint64_t value)
{
    value ^= value >> 30;
    value *= UINT64_C(0xbf58476d1ce4e5b9);
    value ^= value >> 27;
    value *= UINT64_C(0x94d049bb133111eb);
    return value ^ (value >> 31);
}

static uint64_t table_bytes(size_t capacity)
{
    if (capacity > UINT64_MAX / (2U * sizeof(uint64_t))) {
        die("hash-table size overflow");
    }
    return (uint64_t)capacity * 2U * sizeof(uint64_t);
}

static void budget_acquire(MemoryBudget *budget, uint64_t bytes)
{
    if (bytes > budget->limit || budget->current > budget->limit - bytes) {
        fprintf(stderr,
                "error: sparse tables need more than %.3f GiB; "
                "raise A333082_MEMORY_MIB\n",
                (double)budget->limit / (double)(UINT64_C(1) << 30));
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
        die("internal memory-accounting underflow");
    }
    budget->current -= bytes;
}

static void table_init(StateTable *table, size_t capacity,
                       MemoryBudget *budget)
{
    if (capacity < INITIAL_CAPACITY) {
        capacity = INITIAL_CAPACITY;
    }
    if ((capacity & (capacity - 1)) != 0) {
        die("hash capacity is not a power of two");
    }
    table->bytes = table_bytes(capacity);
    budget_acquire(budget, table->bytes);
    table->keys = checked_calloc(capacity, sizeof(*table->keys));
    table->values = checked_calloc(capacity, sizeof(*table->values));
    table->capacity = capacity;
    table->size = 0;
}

static void table_clear(StateTable *table, MemoryBudget *budget)
{
    free(table->values);
    free(table->keys);
    budget_release(budget, table->bytes);
    memset(table, 0, sizeof(*table));
}

static size_t table_slot(const StateTable *table, uint64_t stored_key)
{
    size_t slot = (size_t)mix64(stored_key) & (table->capacity - 1);
    while (table->keys[slot] != 0 && table->keys[slot] != stored_key) {
        slot = (slot + 1) & (table->capacity - 1);
    }
    return slot;
}

static void table_insert_raw(StateTable *table, uint64_t key, uint64_t value)
{
    uint64_t stored_key = key + 1;
    if (stored_key == 0) {
        die("packed state key overflow");
    }
    size_t slot = table_slot(table, stored_key);
    if (table->keys[slot] != 0) {
        die("duplicate state during rehash");
    }
    table->keys[slot] = stored_key;
    table->values[slot] = value;
    ++table->size;
}

static void table_grow(StateTable *table, MemoryBudget *budget)
{
    if (table->capacity > SIZE_MAX / 2) {
        die("hash capacity overflow");
    }
    StateTable larger = {0};
    table_init(&larger, table->capacity * 2, budget);
    for (size_t i = 0; i < table->capacity; ++i) {
        if (table->keys[i] != 0) {
            table_insert_raw(&larger, table->keys[i] - 1,
                             table->values[i]);
        }
    }
    table_clear(table, budget);
    *table = larger;
}

static uint64_t add_mod(uint64_t left, uint64_t right, uint64_t modulus)
{
    uint64_t sum = left + right;
    if (sum >= modulus) {
        sum -= modulus;
    }
    return sum;
}

static uint64_t subtract_mod(uint64_t left, uint64_t right,
                             uint64_t modulus)
{
    return left >= right ? left - right : left + modulus - right;
}

static uint64_t multiply_mod(uint64_t left, uint64_t right,
                             uint64_t modulus)
{
    if (modulus == 0) {
        die("zero modulus");
    }
    return (uint64_t)((U128)left * right % modulus);
}

static void table_add(StateTable *table, uint64_t key, uint64_t addend,
                      uint64_t modulus, MemoryBudget *budget,
                      PassStats *stats)
{
    if (addend == 0) {
        return;
    }
    uint64_t stored_key = key + 1;
    if (stored_key == 0) {
        die("packed state key overflow");
    }
    size_t slot = table_slot(table, stored_key);
    if (table->keys[slot] != 0) {
        table->values[slot] =
            add_mod(table->values[slot], addend, modulus);
        return;
    }
    if (table->size + 1 >
        table->capacity * LOAD_NUMERATOR / LOAD_DENOMINATOR) {
        table_grow(table, budget);
        slot = table_slot(table, stored_key);
        if (table->keys[slot] != 0) {
            die("state appeared during hash growth");
        }
    }
    table->keys[slot] = stored_key;
    table->values[slot] = addend;
    ++table->size;
    ++stats->insertions;
}

static int compare_groups(const void *left_pointer, const void *right_pointer)
{
    const RatioGroup *left = left_pointer;
    const RatioGroup *right = right_pointer;
    if (left->cell_count != right->cell_count) {
        return left->cell_count < right->cell_count ? -1 : 1;
    }
    int left_low = left->numerator < left->denominator
                       ? left->numerator : left->denominator;
    int right_low = right->numerator < right->denominator
                        ? right->numerator : right->denominator;
    if (left_low != right_low) {
        return left_low < right_low ? -1 : 1;
    }
    int left_high = left->numerator > left->denominator
                        ? left->numerator : left->denominator;
    int right_high = right->numerator > right->denominator
                         ? right->numerator : right->denominator;
    if (left_high != right_high) {
        return left_high < right_high ? -1 : 1;
    }
    if (left->numerator != right->numerator) {
        return left->numerator < right->numerator ? -1 : 1;
    }
    return (left->denominator > right->denominator) -
           (left->denominator < right->denominator);
}

static RatioGroup *make_groups(int n, size_t *group_count_out,
                               size_t *choice_count_out)
{
    size_t maximum_groups = (size_t)n * (size_t)n;
    RatioGroup *groups = checked_calloc(maximum_groups, sizeof(*groups));
    size_t group_count = 0;
    size_t total_choices = 0;

    for (int numerator = 1; numerator <= n; ++numerator) {
        for (int denominator = 1; denominator <= n; ++denominator) {
            if (numerator == denominator ||
                gcd_int(numerator, denominator) != 1) {
                continue;
            }
            int cell_count = n /
                (numerator > denominator ? numerator : denominator);
            if (cell_count < 2) {
                continue;
            }
            size_t subset_count = (size_t)1 << cell_count;
            size_t choice_count =
                subset_count - 1U - (size_t)cell_count;
            Choice *choices = checked_malloc(choice_count, sizeof(*choices));
            size_t at = 0;
            for (size_t subset = 0; subset < subset_count; ++subset) {
                int cardinality = __builtin_popcountll((uint64_t)subset);
                if (cardinality < 2) {
                    continue;
                }
                uint32_t rows = 0;
                uint32_t columns = 0;
                for (int t = 1; t <= cell_count; ++t) {
                    if ((subset & ((size_t)1 << (t - 1))) != 0) {
                        rows |= UINT32_C(1) << (denominator * t - 1);
                        columns |= UINT32_C(1) << (numerator * t - 1);
                    }
                }
                choices[at].rows = rows;
                choices[at].columns = columns;
                choices[at].coefficient = (int8_t)((cardinality & 1)
                                             ? cardinality - 1
                                             : -(cardinality - 1));
                ++at;
            }
            if (at != choice_count) {
                die("ratio-class enumeration mismatch");
            }
            groups[group_count].choices = choices;
            groups[group_count].choice_count = choice_count;
            groups[group_count].numerator = numerator;
            groups[group_count].denominator = denominator;
            groups[group_count].cell_count = cell_count;
            ++group_count;
            if (total_choices > SIZE_MAX - choice_count) {
                die("choice-count overflow");
            }
            total_choices += choice_count;
        }
    }

    qsort(groups, group_count, sizeof(*groups), compare_groups);
    *group_count_out = group_count;
    *choice_count_out = total_choices;
    return groups;
}

static void free_groups(RatioGroup *groups, size_t group_count)
{
    for (size_t i = 0; i < group_count; ++i) {
        free(groups[i].choices);
    }
    free(groups);
}

static void make_completion_weights(uint64_t weights[MAX_N + 1][MAX_N + 1],
                                    int n, uint64_t modulus)
{
    uint64_t factorial[MAX_N + 1] = {0};
    uint64_t choose[MAX_N + 1][MAX_N + 1] = {{0}};
    factorial[0] = 1;
    for (int i = 1; i <= n; ++i) {
        factorial[i] = multiply_mod(factorial[i - 1], (uint64_t)i,
                                    modulus);
    }
    choose[0][0] = 1;
    for (int i = 1; i <= n; ++i) {
        choose[i][0] = 1;
        choose[i][i] = 1;
        for (int j = 1; j < i; ++j) {
            choose[i][j] = add_mod(choose[i - 1][j - 1],
                                   choose[i - 1][j], modulus);
        }
    }

    memset(weights, 0,
           (size_t)(MAX_N + 1) * (MAX_N + 1) * sizeof(uint64_t));
    for (int unused = 0; unused <= n; ++unused) {
        for (int diagonal = 0; diagonal <= unused; ++diagonal) {
            uint64_t value = factorial[unused];
            for (int r = 2; r <= diagonal; ++r) {
                uint64_t term = multiply_mod(choose[diagonal][r],
                                             (uint64_t)(r - 1), modulus);
                term = multiply_mod(term, factorial[unused - r], modulus);
                value = (r & 1) ? add_mod(value, term, modulus)
                                : subtract_mod(value, term, modulus);
            }
            weights[unused][diagonal] = value;
        }
    }
}

static uint64_t pack_state(uint32_t rows, uint32_t columns, int n)
{
    return (uint64_t)rows | ((uint64_t)columns << n);
}

static uint64_t pack_canonical_state(uint32_t rows, uint32_t columns, int n)
{
    if (rows > columns) {
        uint32_t temporary = rows;
        rows = columns;
        columns = temporary;
    }
    return pack_state(rows, columns, n);
}

static void apply_group(StateTable *destination, const StateTable *source,
                        const RatioGroup *group, int n,
                        uint64_t row_mask_limit, uint64_t modulus,
                        MemoryBudget *budget, PassStats *stats,
                        Progress *progress, size_t group_number)
{
    table_init(destination, source->capacity, budget);
    for (size_t bucket = 0; bucket < source->capacity; ++bucket) {
        if ((bucket & (size_t)16383) == 0) {
            report_progress(progress, "ratio-transition", group_number,
                            bucket, source->capacity,
                            destination->size, budget);
        }
        if (source->keys[bucket] == 0 || source->values[bucket] == 0) {
            continue;
        }
        uint64_t key = source->keys[bucket] - 1;
        uint64_t value = source->values[bucket];
        uint32_t rows = (uint32_t)(key & row_mask_limit);
        uint32_t columns = (uint32_t)(key >> n);
        ++stats->source_states;
        table_add(destination, key, value, modulus, budget, stats);
        for (size_t c = 0; c < group->choice_count; ++c) {
            const Choice *choice = &group->choices[c];
            if ((rows & choice->rows) != 0 ||
                (columns & choice->columns) != 0) {
                continue;
            }
            ++stats->compatible_choices;
            unsigned magnitude = choice->coefficient < 0
                                     ? (unsigned)(-choice->coefficient)
                                     : (unsigned)choice->coefficient;
            uint64_t weighted = multiply_mod(value, magnitude, modulus);
            if (choice->coefficient < 0 && weighted != 0) {
                weighted = modulus - weighted;
            }
            uint32_t next_rows = rows | choice->rows;
            uint32_t next_columns = columns | choice->columns;
            uint64_t next_key = pack_state(next_rows, next_columns, n);
            table_add(destination, next_key, weighted, modulus,
                      budget, stats);
        }
    }
}

static void fold_transpose(StateTable *table, int n,
                           uint64_t row_mask_limit, uint64_t modulus,
                           MemoryBudget *budget, PassStats *stats,
                           Progress *progress, size_t group_number)
{
    StateTable folded = {0};
    size_t initial_capacity = table->capacity / 2;
    if (initial_capacity < INITIAL_CAPACITY) {
        initial_capacity = INITIAL_CAPACITY;
    }
    table_init(&folded, initial_capacity, budget);
    for (size_t bucket = 0; bucket < table->capacity; ++bucket) {
        if ((bucket & (size_t)16383) == 0) {
            report_progress(progress, "transpose-fold", group_number,
                            bucket, table->capacity, folded.size,
                            budget);
        }
        if (table->keys[bucket] == 0 || table->values[bucket] == 0) {
            continue;
        }
        uint64_t key = table->keys[bucket] - 1;
        uint32_t rows = (uint32_t)(key & row_mask_limit);
        uint32_t columns = (uint32_t)(key >> n);
        uint64_t canonical = pack_canonical_state(rows, columns, n);
        table_add(&folded, canonical, table->values[bucket], modulus,
                  budget, stats);
    }
    table_clear(table, budget);
    *table = folded;
}

static uint64_t dp_residue(int n, const RatioGroup *groups,
                           size_t group_count, uint64_t modulus,
                           uint64_t memory_limit, PassStats *stats,
                           int crt_pass, int crt_count)
{
    double started = monotonic_seconds();
    Progress progress = {
        n, crt_pass, crt_count, group_count, started, started
    };
    MemoryBudget budget = {memory_limit, 0, 0};
    StateTable current = {0};
    table_init(&current, INITIAL_CAPACITY, &budget);
    table_add(&current, 0, 1, modulus, &budget, stats);
    uint64_t row_mask_limit = (UINT64_C(1) << n) - 1;

    if ((group_count & 1U) != 0) {
        die("non-diagonal ratio groups do not form reciprocal pairs");
    }
    for (size_t group_index = 0; group_index < group_count;
         group_index += 2) {
        const RatioGroup *first = &groups[group_index];
        const RatioGroup *second = &groups[group_index + 1];
        if (first->numerator != second->denominator ||
            first->denominator != second->numerator) {
            die("ratio groups are not adjacent reciprocal pairs");
        }
        StateTable next = {0};
        apply_group(&next, &current, first, n, row_mask_limit,
                    modulus, &budget, stats, &progress, group_index + 1);
        table_clear(&current, &budget);
        current = next;

        memset(&next, 0, sizeof(next));
        apply_group(&next, &current, second, n, row_mask_limit,
                    modulus, &budget, stats, &progress, group_index + 2);
        table_clear(&current, &budget);
        current = next;
        fold_transpose(&current, n, row_mask_limit, modulus,
                       &budget, stats, &progress, group_index + 2);
    }

    uint64_t completion[MAX_N + 1][MAX_N + 1];
    make_completion_weights(completion, n, modulus);
    uint32_t full_mask = (UINT32_C(1) << n) - 1;
    uint64_t answer = 0;
    for (size_t bucket = 0; bucket < current.capacity; ++bucket) {
        if ((bucket & (size_t)16383) == 0) {
            report_progress(&progress, "final-sum", group_count,
                            bucket, current.capacity, current.size,
                            &budget);
        }
        if (current.keys[bucket] == 0 || current.values[bucket] == 0) {
            continue;
        }
        uint64_t key = current.keys[bucket] - 1;
        uint32_t rows = (uint32_t)(key & row_mask_limit);
        uint32_t columns = (uint32_t)(key >> n);
        int row_count = __builtin_popcount(rows);
        int column_count = __builtin_popcount(columns);
        if (row_count != column_count) {
            die("DP state has unequal row and column cardinalities");
        }
        int unused = n - row_count;
        int diagonal = __builtin_popcount(full_mask & ~(rows | columns));
        uint64_t term = multiply_mod(current.values[bucket],
                                     completion[unused][diagonal], modulus);
        answer = add_mod(answer, term, modulus);
    }

    stats->final_states = current.size;
    stats->peak_bytes = budget.peak;
    stats->seconds = monotonic_seconds() - started;
    table_clear(&current, &budget);
    if (budget.current != 0) {
        die("internal memory-accounting leak");
    }
    return answer;
}

static int generate_moduli(uint64_t *moduli, const mpz_t bound)
{
    mpz_t candidate;
    mpz_t product;
    mpz_init(candidate);
    mpz_init_set_ui(product, 1);
    mpz_set_ui(candidate, 1);
    mpz_mul_2exp(candidate, candidate, 61);
    mpz_sub_ui(candidate, candidate, UINT64_C(50000000));
    int count = 0;
    while (mpz_cmp(product, bound) <= 0) {
        if (count >= MAX_CRT_PRIMES) {
            die("not enough CRT moduli");
        }
        mpz_nextprime(candidate, candidate);
        if (mpz_sizeinbase(candidate, 2) > 61) {
            die("failed to generate a 61-bit CRT modulus");
        }
        moduli[count] = (uint64_t)mpz_get_ui(candidate);
        mpz_mul_ui(product, product, (unsigned long)moduli[count]);
        ++count;
    }
    mpz_clear(product);
    mpz_clear(candidate);
    return count;
}

static void reconstruct_crt(mpz_t result, const uint64_t *residues,
                            const uint64_t *moduli, int count)
{
    mpz_t product;
    mpz_t modulus_integer;
    mpz_t product_modulus;
    mpz_t inverse;
    mpz_init_set_ui(product, 1);
    mpz_init(modulus_integer);
    mpz_init(product_modulus);
    mpz_init(inverse);
    mpz_set_ui(result, 0);
    for (int q = 0; q < count; ++q) {
        if (moduli[q] < 2) {
            die("invalid CRT modulus");
        }
        unsigned long modulus = (unsigned long)moduli[q];
        unsigned long current = mpz_fdiv_ui(result, modulus);
        uint64_t delta = residues[q] >= current
                             ? residues[q] - current
                             : residues[q] + moduli[q] - current;
        mpz_set_ui(modulus_integer, modulus);
        mpz_set_ui(product_modulus, mpz_fdiv_ui(product, modulus));
        if (mpz_invert(inverse, product_modulus, modulus_integer) == 0) {
            die("CRT moduli are not pairwise coprime");
        }
        uint64_t multiplier =
            (uint64_t)((U128)delta * (uint64_t)mpz_get_ui(inverse) %
                       moduli[q]);
        mpz_addmul_ui(result, product, (unsigned long)multiplier);
        mpz_mul_ui(product, product, modulus);
    }
    mpz_clear(inverse);
    mpz_clear(product_modulus);
    mpz_clear(modulus_integer);
    mpz_clear(product);
}

static uint64_t direct_search(int n, int row, uint32_t used_columns,
                              bool *used_ratios)
{
    if (row > n) {
        return 1;
    }
    uint64_t count = 0;
    for (int column = 1; column <= n; ++column) {
        uint32_t bit = UINT32_C(1) << (column - 1);
        if ((used_columns & bit) != 0) {
            continue;
        }
        int divisor = gcd_int(column, row);
        int numerator = column / divisor;
        int denominator = row / divisor;
        size_t ratio = (size_t)numerator * (size_t)(n + 1) +
                       (size_t)denominator;
        if (used_ratios[ratio]) {
            continue;
        }
        used_ratios[ratio] = true;
        uint64_t add = direct_search(n, row + 1, used_columns | bit,
                                     used_ratios);
        if (count > UINT64_MAX - add) {
            die("direct verifier overflow");
        }
        count += add;
        used_ratios[ratio] = false;
    }
    return count;
}

static uint64_t direct_count(int n)
{
    size_t side = (size_t)n + 1;
    bool *used_ratios = checked_calloc(side * side, sizeof(*used_ratios));
    uint64_t result = direct_search(n, 1, 0, used_ratios);
    free(used_ratios);
    return result;
}

static void calculate_term(mpz_t answer, int n, uint64_t memory_limit)
{
    size_t group_count = 0;
    size_t choice_count = 0;
    RatioGroup *groups = make_groups(n, &group_count, &choice_count);
    mpz_t factorial_bound;
    mpz_init(factorial_bound);
    mpz_fac_ui(factorial_bound, (unsigned long)n);
    uint64_t moduli[MAX_CRT_PRIMES];
    uint64_t residues[MAX_CRT_PRIMES];
    int modulus_count = generate_moduli(moduli, factorial_bound);
    double total_seconds = 0.0;
    uint64_t maximum_peak = 0;
    size_t final_states = 0;
    uint64_t compatible = 0;

    for (int q = 0; q < modulus_count; ++q) {
        PassStats stats = {0};
        residues[q] = dp_residue(n, groups, group_count, moduli[q],
                                 memory_limit, &stats, q + 1,
                                 modulus_count);
        total_seconds += stats.seconds;
        if (stats.peak_bytes > maximum_peak) {
            maximum_peak = stats.peak_bytes;
        }
        if (q == 0) {
            final_states = stats.final_states;
            compatible = stats.compatible_choices;
        }
    }

    reconstruct_crt(answer, residues, moduli, modulus_count);
    if (mpz_sgn(answer) < 0 || mpz_cmp(answer, factorial_bound) > 0) {
        die("CRT reconstruction is outside 0..n!");
    }
    for (int q = 0; q < modulus_count; ++q) {
        if (mpz_fdiv_ui(answer, (unsigned long)moduli[q]) != residues[q]) {
            die("CRT residue replay failed");
        }
    }
    if (n <= DIRECT_CHECK_MAX_N) {
        uint64_t direct = direct_count(n);
        if (!mpz_fits_ulong_p(answer) || mpz_get_ui(answer) != direct) {
            die("independent direct enumeration disagrees with the DP");
        }
    }
    if (n <= VERIFIED_MAX_N) {
        mpz_t expected;
        mpz_init(expected);
        if (mpz_set_str(expected, verified_terms[n], 10) != 0) {
            die("invalid built-in verification term");
        }
        if (mpz_cmp(answer, expected) != 0) {
            die("computed value disagrees with the built-in OEIS term");
        }
        mpz_clear(expected);
    }

    fprintf(stderr,
            "A333082 n=%d groups=%zu choices=%zu states=%zu "
            "compatible=%" PRIu64 " CRT=%d time=%.3fs peak=%.3f MiB%s%s\n",
            n, group_count, choice_count, final_states, compatible,
            modulus_count, total_seconds,
            (double)maximum_peak / (1024.0 * 1024.0),
            n <= DIRECT_CHECK_MAX_N ? " direct-check=ok" : "",
            n <= VERIFIED_MAX_N ? " known-term=ok" : "");

    mpz_clear(factorial_bound);
    free_groups(groups, group_count);
}

static void write_term(FILE *bfile, const char *part_path, int n,
                       const mpz_t answer)
{
    if (gmp_fprintf(bfile, "%d %Zd\n", n, answer) < 0) {
        die("could not write a b-file term");
    }
    if (fflush(bfile) != 0 || fsync(fileno(bfile)) != 0) {
        fprintf(stderr, "error: could not flush %s: %s\n",
                part_path, strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (gmp_printf("%d %Zd\n", n, answer) < 0 || fflush(stdout) != 0) {
        die("could not write stdout");
    }
}

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s N [FROM]\n"
            "  write exact A333082 terms 0..N, with N <= %d\n"
            "  0 <= FROM <= min(N,%d)\n",
            program, MAX_N, FIRST_UNVERIFIED_N);
}

int main(int argc, char **argv)
{
    if (argc != 2 && argc != 3) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    int maximum_n = parse_int_range(argv[1], 0, MAX_N, "N");
    int maximum_from = maximum_n < FIRST_UNVERIFIED_N
                           ? maximum_n
                           : FIRST_UNVERIFIED_N;
    int first_n = argc == 3
                      ? parse_int_range(argv[2], 0, maximum_from, "FROM")
                      : 0;
    uint64_t memory_limit = parse_memory_limit();
    char *part_path =
        path_beside_executable(argv[0], "b333082_01_part.txt");
    char *final_path = path_beside_executable(argv[0], "b333082_01.txt");
    FILE *bfile = fopen(part_path, "w");
    if (bfile == NULL) {
        fprintf(stderr, "error: could not open %s: %s\n",
                part_path, strerror(errno));
        free(final_path);
        free(part_path);
        return EXIT_FAILURE;
    }

    mpz_t answer;
    mpz_init(answer);
    for (int n = 0; n < first_n; ++n) {
        if (mpz_set_str(answer, verified_terms[n], 10) != 0) {
            die("invalid built-in verification term");
        }
        write_term(bfile, part_path, n, answer);
        fprintf(stderr, "A333082 n=%d loaded from verified prefix\n", n);
    }
    for (int n = first_n; n <= maximum_n; ++n) {
        calculate_term(answer, n, memory_limit);
        write_term(bfile, part_path, n, answer);
    }
    mpz_clear(answer);

    if (fclose(bfile) != 0) {
        fprintf(stderr, "error: could not close %s: %s\n",
                part_path, strerror(errno));
        free(final_path);
        free(part_path);
        return EXIT_FAILURE;
    }
    if (rename(part_path, final_path) != 0) {
        fprintf(stderr, "error: could not rename %s to %s: %s\n",
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
