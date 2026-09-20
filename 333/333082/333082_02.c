/*
 * A333082 -- oriented ratio-class DP with a prime-border lift.
 *
 * This is an intentionally different algorithm from 333082_01.c.  Program
 * 01 uses 61-bit residues and folds transpose-related states after reciprocal
 * ratio classes.  This program keeps all oriented (row mask, column mask)
 * states, uses 31-bit modular passes, obtains a(19) from an 18 by 18 DP,
 * and computes a(20) and a(21) directly in their full universes.
 *
 * Ratio-class inclusion-exclusion
 * ---------------------------------
 * A permutation is a placement of nonattacking rooks.  For every reduced
 * positive fraction a/b, the cells with column/row ratio a/b are
 *
 *                         (b*t, a*t).
 *
 * If a placement uses u cells of a ratio class, then
 *
 *   [u <= 1] = 1 + sum_{r=2..u} C(u,r) (-1)^(r-1) (r-1).            (1)
 *
 * The DP expands (1) for every non-diagonal ratio class.  A state stores the
 * row and column masks of marked cells.  The diagonal ratio 1 is evaluated
 * analytically after the DP.
 *
 * Prime-border lift
 * -----------------
 * Let p=m+1 be prime.  Every non-corner border ratio j/p or p/i is reduced
 * with numerator or denominator p, so it cannot equal an interior ratio.
 * A lower-border ratio is below 1 and a right-border ratio is above 1, hence
 * the two border ratios cannot equal each other either.
 *
 * For a state, let s be the number of unused rows (and columns), and let d
 * be the number of indices unused on both sides.  Marking r diagonal cells
 * has coefficient c_0=1, c_1=0, and
 * c_r=(-1)^(r-1)(r-1) for r>=2.  Therefore the base completion factor is
 *
 *   F(s,d) = sum_r C(d,r)c_r(s-r)!.                                 (2)
 *
 * If the corner (p,p) is used, ratio 1 is already occupied, so the interior
 * must have no fixed point.  Its factor is
 *
 *   J(s,d) = sum_{r=0..d} C(d,r)(-1)^r(s-r)!.                        (3)
 *
 * Otherwise choose the row mapped to p and the image of p.  After r marked
 * diagonal cells there are (s-r)^2 choices for this ordered pair and
 * (s-r-1)! completions.  Thus
 *
 *   H(s,d) = sum_r C(d,r)c_r(s-r)^2(s-r-1)!,                         (4)
 *
 * where only terms with s-r>=1 occur.  The lifted completion factor is
 * J(s,d)+H(s,d).  Equations (1)--(4) are integer identities.
 *
 * The primality condition is essential.  For composite 21, for example,
 * 3/21=1/7 is already an interior ratio.  Consequently this program's
 * border lift deliberately stops at 18 -> 19.  The program computes n=20
 * and n=21 directly; it never applies the prime-border formula to 21.
 *
 * Exact arithmetic and checks
 * ---------------------------
 * DP passes use distinct primes below 2^31 and CRT continues until their
 * product exceeds the relevant factorial bound.  For n<=10, an independent
 * row-by-row search checks reduced ratio pairs directly.  The prime-border
 * identity is independently checked for every prime p<=11.  Published terms
 * through n=15 are post-computation checks only.
 *
 * Each finished row is flushed and fsynced to b333082_02_part.txt beside
 * the executable.  Complete success atomically replaces b333082_02.txt.
 * Progress is reported to stderr at most once per minute.
 *
 * Build:
 *
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic 333082_02.c \
 *     $(pkg-config --cflags --libs gmp) -o 333082_02
 *
 * Usage:
 *
 *   A333082_02_MEMORY_MIB=8192 ./333082_02 N [FROM]
 *
 * N is in 0..21.  FROM is at most 16; verified terms below FROM are copied
 * to keep both stdout and the b-file complete from offset 0.
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
#error "333082_02.c requires a platform with 64-bit unsigned long"
#endif

#define MAX_N 21
#define MAX_BASE_N 21
#define DIRECT_CHECK_MAX_N 10
#define VERIFIED_MAX_N 15
#define FIRST_UNVERIFIED_N (VERIFIED_MAX_N + 1)
#define MAX_CRT_PRIMES 3
#define INITIAL_CAPACITY ((size_t)16)
#define LOAD_NUMERATOR ((size_t)7)
#define LOAD_DENOMINATOR ((size_t)10)
#define DEFAULT_MEMORY_MIB UINT64_C(8192)
#define MIN_MEMORY_MIB UINT64_C(64)
#define MAX_MEMORY_MIB UINT64_C(65536)
#ifndef PROGRESS_INTERVAL_SECONDS
#define PROGRESS_INTERVAL_SECONDS 60.0
#endif

static const uint32_t available_moduli[MAX_CRT_PRIMES] = {
    UINT32_C(2147483647), UINT32_C(2147483629),
    UINT32_C(2147483587)
};

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
    uint64_t *keys;       /* key + 1; zero is an empty bucket */
    uint32_t *values;
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
            "A333082_02 n=%d progress CRT=%d/%d group=%zu/%zu phase=%s "
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
    const char *text = getenv("A333082_02_MEMORY_MIB");
    uint64_t mib = DEFAULT_MEMORY_MIB;
    if (text != NULL && *text != '\0') {
        char *end = NULL;
        errno = 0;
        unsigned long long value = strtoull(text, &end, 10);
        if (errno != 0 || end == text || *end != '\0' ||
            value < MIN_MEMORY_MIB || value > MAX_MEMORY_MIB) {
            fprintf(stderr,
                    "error: A333082_02_MEMORY_MIB must be in %" PRIu64
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
    const uint64_t bytes_per_slot =
        sizeof(uint64_t) + sizeof(uint32_t);
    if (capacity > UINT64_MAX / bytes_per_slot) {
        die("hash-table size overflow");
    }
    return (uint64_t)capacity * bytes_per_slot;
}

static void budget_acquire(MemoryBudget *budget, uint64_t bytes)
{
    if (bytes > budget->limit || budget->current > budget->limit - bytes) {
        fprintf(stderr,
                "error: sparse tables need more than %.3f GiB; "
                "raise A333082_02_MEMORY_MIB\n",
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

static void table_insert_raw(StateTable *table, uint64_t key, uint32_t value)
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

static uint32_t add_mod(uint32_t a, uint32_t b, uint32_t modulus)
{
    uint64_t sum = (uint64_t)a + b;
    if (sum >= modulus) {
        sum -= modulus;
    }
    return (uint32_t)sum;
}

static uint32_t multiply_small_mod(uint32_t value, uint32_t factor,
                                   uint32_t modulus)
{
    if (modulus == 0) {
        die("zero modulus");
    }
    return (uint32_t)((uint64_t)value * factor % modulus);
}

static void table_add(StateTable *table, uint64_t key, uint32_t addend,
                      uint32_t modulus, MemoryBudget *budget,
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

static int gcd_int(int left, int right)
{
    while (right != 0) {
        int remainder = left % right;
        left = right;
        right = remainder;
    }
    return left;
}

static int compare_groups(const void *left_pointer, const void *right_pointer)
{
    const RatioGroup *left = left_pointer;
    const RatioGroup *right = right_pointer;
    if (left->cell_count != right->cell_count) {
        return left->cell_count < right->cell_count ? -1 : 1;
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
    RatioGroup *groups =
        checked_calloc(maximum_groups, sizeof(*groups));
    size_t group_count = 0;
    size_t total_choices = 0;

    for (int numerator = 1; numerator <= n; ++numerator) {
        for (int denominator = 1; denominator <= n; ++denominator) {
            if (numerator == denominator ||
                gcd_int(numerator, denominator) != 1) {
                continue;
            }
            int maximum = numerator > denominator
                              ? numerator : denominator;
            int cell_count = n / maximum;
            if (cell_count < 2) {
                continue;
            }
            size_t subset_count = (size_t)1 << cell_count;
            size_t choice_count =
                subset_count - 1U - (size_t)cell_count;
            Choice *choices =
                checked_malloc(choice_count, sizeof(*choices));
            size_t at = 0;
            for (size_t subset = 0; subset < subset_count; ++subset) {
                int cardinality =
                    __builtin_popcountll((uint64_t)subset);
                if (cardinality < 2) {
                    continue;
                }
                uint32_t rows = 0;
                uint32_t columns = 0;
                for (int t = 1; t <= cell_count; ++t) {
                    if ((subset & ((size_t)1 << (t - 1))) != 0) {
                        rows |= UINT32_C(1)
                                << (denominator * t - 1);
                        columns |= UINT32_C(1)
                                   << (numerator * t - 1);
                    }
                }
                choices[at].rows = rows;
                choices[at].columns = columns;
                choices[at].coefficient =
                    (int8_t)((cardinality & 1)
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

static void dp_residues(int n, const RatioGroup *groups,
                        size_t group_count, uint32_t modulus,
                        uint64_t memory_limit, bool want_prime_lift,
                        uint32_t *base_answer, uint32_t *lifted_answer,
                        PassStats *stats, int crt_pass, int crt_count)
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

    for (size_t group_index = 0; group_index < group_count; ++group_index) {
        const RatioGroup *group = &groups[group_index];
        StateTable next = {0};
        table_init(&next, current.capacity, &budget);
        for (size_t bucket = 0; bucket < current.capacity; ++bucket) {
            if ((bucket & (size_t)16383) == 0) {
                report_progress(&progress, "ratio-transition",
                                group_index + 1, bucket,
                                current.capacity, next.size, &budget);
            }
            if (current.keys[bucket] == 0 || current.values[bucket] == 0) {
                continue;
            }
            uint64_t key = current.keys[bucket] - 1;
            uint32_t value = current.values[bucket];
            uint32_t rows = (uint32_t)(key & row_mask_limit);
            uint32_t columns = (uint32_t)(key >> n);
            ++stats->source_states;
            table_add(&next, key, value, modulus, &budget, stats);
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
                uint32_t weighted = multiply_small_mod(
                    value, (uint32_t)magnitude, modulus);
                if (choice->coefficient < 0 && weighted != 0) {
                    weighted = modulus - weighted;
                }
                uint32_t next_rows = rows | choice->rows;
                uint32_t next_columns = columns | choice->columns;
                uint64_t next_key = (uint64_t)next_rows |
                    ((uint64_t)next_columns << n);
                table_add(&next, next_key, weighted, modulus,
                          &budget, stats);
            }
        }
        table_clear(&current, &budget);
        current = next;
    }

    uint32_t factorial[MAX_BASE_N + 1] = {0};
    uint32_t choose[MAX_BASE_N + 1][MAX_BASE_N + 1] = {{0}};
    factorial[0] = 1;
    choose[0][0] = 1;
    for (int k = 1; k <= n; ++k) {
        factorial[k] = multiply_small_mod(factorial[k - 1],
                                          (uint32_t)k, modulus);
        choose[k][0] = 1;
        choose[k][k] = 1;
        for (int r = 1; r < k; ++r) {
            choose[k][r] =
                add_mod(choose[k - 1][r - 1], choose[k - 1][r],
                        modulus);
        }
    }
    uint32_t base = 0;
    uint32_t lifted = 0;
    uint32_t full_mask = (UINT32_C(1) << n) - 1;
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
        uint32_t value = current.values[bucket];

        uint32_t base_factor = factorial[unused];
        for (int r = 2; r <= diagonal; ++r) {
            uint32_t term = multiply_small_mod(
                factorial[unused - r], choose[diagonal][r], modulus);
            term = multiply_small_mod(term, (uint32_t)(r - 1),
                                      modulus);
            base_factor = (r & 1)
                              ? add_mod(base_factor, term, modulus)
                              : add_mod(base_factor,
                                        term == 0 ? 0 : modulus - term,
                                        modulus);
        }
        base = add_mod(base,
                       multiply_small_mod(value, base_factor, modulus),
                       modulus);

        if (want_prime_lift) {
            uint32_t fixed_factor = 0;
            for (int r = 0; r <= diagonal; ++r) {
                uint32_t term = multiply_small_mod(
                    factorial[unused - r], choose[diagonal][r],
                    modulus);
                fixed_factor =
                    (r & 1)
                        ? add_mod(fixed_factor,
                                  term == 0 ? 0 : modulus - term,
                                  modulus)
                        : add_mod(fixed_factor, term, modulus);
            }

            uint32_t border_factor = 0;
            if (unused > 0) {
                border_factor = multiply_small_mod(
                    factorial[unused], (uint32_t)unused, modulus);
            }
            for (int r = 2; r <= diagonal && r < unused; ++r) {
                int remaining = unused - r;
                uint32_t term = multiply_small_mod(
                    factorial[remaining], choose[diagonal][r],
                    modulus);
                term = multiply_small_mod(term, (uint32_t)(r - 1),
                                          modulus);
                term = multiply_small_mod(term, (uint32_t)remaining,
                                          modulus);
                border_factor =
                    (r & 1)
                        ? add_mod(border_factor, term, modulus)
                        : add_mod(border_factor,
                                  term == 0 ? 0 : modulus - term,
                                  modulus);
            }
            uint32_t lift_factor =
                add_mod(fixed_factor, border_factor, modulus);
            lifted = add_mod(
                lifted,
                multiply_small_mod(value, lift_factor, modulus),
                modulus);
        }
    }

    *base_answer = base;
    *lifted_answer = lifted;
    stats->final_states = current.size;
    stats->peak_bytes = budget.peak;
    stats->seconds = monotonic_seconds() - started;
    table_clear(&current, &budget);
    if (budget.current != 0) {
        die("internal memory-accounting leak");
    }
}

static void verify_moduli(void)
{
    mpz_t value;
    mpz_init(value);
    for (int i = 0; i < MAX_CRT_PRIMES; ++i) {
        mpz_set_ui(value, available_moduli[i]);
        if (mpz_probab_prime_p(value, 32) == 0) {
            die("a fixed CRT modulus is not prime");
        }
        for (int j = 0; j < i; ++j) {
            if (available_moduli[i] == available_moduli[j]) {
                die("duplicate CRT modulus");
            }
        }
    }
    mpz_clear(value);
}

static int required_moduli(const mpz_t bound)
{
    mpz_t product;
    mpz_init_set_ui(product, 1);
    int count = 0;
    while (mpz_cmp(product, bound) <= 0) {
        if (count >= MAX_CRT_PRIMES) {
            die("fixed CRT moduli do not exceed the factorial bound");
        }
        mpz_mul_ui(product, product, available_moduli[count]);
        ++count;
    }
    mpz_clear(product);
    return count;
}

static void reconstruct_crt(mpz_t result, const uint32_t *residues,
                            int count)
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
        unsigned long modulus = available_moduli[q];
        unsigned long current = mpz_fdiv_ui(result, modulus);
        uint32_t delta = residues[q] >= current
                             ? residues[q] - (uint32_t)current
                             : (uint32_t)(residues[q] + modulus - current);
        mpz_set_ui(modulus_integer, modulus);
        mpz_set_ui(product_modulus, mpz_fdiv_ui(product, modulus));
        if (mpz_invert(inverse, product_modulus, modulus_integer) == 0) {
            die("CRT moduli are not coprime");
        }
        uint64_t multiplier =
            (uint64_t)delta * mpz_get_ui(inverse) % modulus;
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
        size_t ratio = (size_t)numerator * ((size_t)n + 1) +
                       (size_t)denominator;
        if (used_ratios[ratio]) {
            continue;
        }
        used_ratios[ratio] = true;
        uint64_t add = direct_search(n, row + 1,
                                     used_columns | bit, used_ratios);
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
    bool *used_ratios =
        checked_calloc(side * side, sizeof(*used_ratios));
    uint64_t result = direct_search(n, 1, 0, used_ratios);
    free(used_ratios);
    return result;
}

static bool is_prime_int(int value)
{
    if (value < 2) {
        return false;
    }
    for (int divisor = 2; divisor <= value / divisor; ++divisor) {
        if (value % divisor == 0) {
            return value == divisor;
        }
    }
    return true;
}

static void verify_prime_lift_small(uint64_t memory_limit)
{
    const uint32_t modulus = available_moduli[0];
    for (int base_n = 1; base_n <= DIRECT_CHECK_MAX_N; ++base_n) {
        if (!is_prime_int(base_n + 1)) {
            continue;
        }
        size_t group_count = 0;
        size_t choice_count = 0;
        RatioGroup *groups =
            make_groups(base_n, &group_count, &choice_count);
        (void)choice_count;
        uint32_t base_residue = 0;
        uint32_t lifted_residue = 0;
        PassStats stats = {0};
        dp_residues(base_n, groups, group_count, modulus, memory_limit,
                    true, &base_residue, &lifted_residue, &stats, 1, 1);
        if (direct_count(base_n) != base_residue ||
            direct_count(base_n + 1) != lifted_residue) {
            die("small direct check of the prime-border lift failed");
        }
        free_groups(groups, group_count);
    }
    fprintf(stderr,
            "A333082_02 prime-border lift direct-check=ok through p=11\n");
}

static void verify_base_answer(const mpz_t answer, int n)
{
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
}

static void calculate_base_and_optional_lift(
    mpz_t base_answer, mpz_t lifted_answer, int n, bool want_prime_lift,
    uint64_t memory_limit)
{
    if (n < 0 || n > MAX_BASE_N) {
        die("invalid base universe");
    }
    if (want_prime_lift && n + 1 != 19) {
        die("prime lift is implemented only for 18 to 19");
    }
    size_t group_count = 0;
    size_t choice_count = 0;
    RatioGroup *groups = make_groups(n, &group_count, &choice_count);

    mpz_t bound;
    mpz_init(bound);
    mpz_fac_ui(bound, (unsigned long)(want_prime_lift ? n + 1 : n));
    int modulus_count = required_moduli(bound);
    uint32_t base_residues[MAX_CRT_PRIMES] = {0};
    uint32_t lifted_residues[MAX_CRT_PRIMES] = {0};
    double total_seconds = 0.0;
    uint64_t maximum_peak = 0;
    size_t final_states = 0;
    uint64_t compatible = 0;

    for (int q = 0; q < modulus_count; ++q) {
        PassStats stats = {0};
        dp_residues(n, groups, group_count, available_moduli[q],
                    memory_limit, want_prime_lift, &base_residues[q],
                    &lifted_residues[q], &stats, q + 1, modulus_count);
        total_seconds += stats.seconds;
        if (stats.peak_bytes > maximum_peak) {
            maximum_peak = stats.peak_bytes;
        }
        if (q == 0) {
            final_states = stats.final_states;
            compatible = stats.compatible_choices;
        }
        fprintf(stderr,
                "A333082_02 base=%d CRT-pass=%d/%d done %.3fs "
                "peak=%.3f MiB\n",
                n, q + 1, modulus_count, stats.seconds,
                (double)stats.peak_bytes / (1024.0 * 1024.0));
    }

    reconstruct_crt(base_answer, base_residues, modulus_count);
    mpz_t base_bound;
    mpz_init(base_bound);
    mpz_fac_ui(base_bound, (unsigned long)n);
    if (mpz_sgn(base_answer) < 0 || mpz_cmp(base_answer, base_bound) > 0) {
        die("base CRT result is outside 0..n!");
    }
    for (int q = 0; q < modulus_count; ++q) {
        if (mpz_fdiv_ui(base_answer, available_moduli[q]) !=
            base_residues[q]) {
            die("base CRT residue replay failed");
        }
    }
    verify_base_answer(base_answer, n);

    if (want_prime_lift) {
        reconstruct_crt(lifted_answer, lifted_residues, modulus_count);
        if (mpz_sgn(lifted_answer) < 0 || mpz_cmp(lifted_answer, bound) > 0) {
            die("lifted CRT result is outside 0..(n+1)!");
        }
        for (int q = 0; q < modulus_count; ++q) {
            if (mpz_fdiv_ui(lifted_answer, available_moduli[q]) !=
                lifted_residues[q]) {
                die("lifted CRT residue replay failed");
            }
        }
    } else {
        mpz_set_ui(lifted_answer, 0);
    }

    fprintf(stderr,
            "A333082_02 base=%d groups=%zu choices=%zu states=%zu "
            "compatible=%" PRIu64 " CRT=%d total=%.3fs peak=%.3f MiB%s%s\n",
            n, group_count, choice_count, final_states, compatible,
            modulus_count, total_seconds,
            (double)maximum_peak / (1024.0 * 1024.0),
            n <= DIRECT_CHECK_MAX_N ? " direct-check=ok" : "",
            n <= VERIFIED_MAX_N ? " known-term=ok" : "");

    mpz_clear(base_bound);
    mpz_clear(bound);
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
    verify_moduli();
    verify_prime_lift_small(memory_limit);

    char *part_path =
        path_beside_executable(argv[0], "b333082_02_part.txt");
    char *final_path = path_beside_executable(argv[0], "b333082_02.txt");
    FILE *bfile = fopen(part_path, "w");
    if (bfile == NULL) {
        fprintf(stderr, "error: could not open %s: %s\n",
                part_path, strerror(errno));
        free(final_path);
        free(part_path);
        return EXIT_FAILURE;
    }

    mpz_t answer;
    mpz_t lifted;
    mpz_inits(answer, lifted, NULL);
    for (int n = 0; n < first_n; ++n) {
        if (mpz_set_str(answer, verified_terms[n], 10) != 0) {
            die("invalid built-in verification term");
        }
        write_term(bfile, part_path, n, answer);
        fprintf(stderr, "A333082_02 n=%d loaded from verified prefix\n", n);
    }

    for (int n = first_n; n <= maximum_n; ++n) {
        bool lift_to_19 = n == 18 && maximum_n >= 19;
        calculate_base_and_optional_lift(answer, lifted, n, lift_to_19,
                                         memory_limit);
        write_term(bfile, part_path, n, answer);
        if (lift_to_19) {
            write_term(bfile, part_path, 19, lifted);
            ++n;
        }
    }
    mpz_clears(lifted, answer, NULL);

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
