/*
 * A333083 -- sparse inclusion-exclusion DP with exact CRT recovery.
 *
 * Count permutations sigma of [n] for which the n products i*sigma(i)
 * are pairwise distinct.
 *
 * View (i,j) as a cell of the n by n multiplication table.  A permutation
 * selects one cell in every row and column.  For a product p, let C_p be
 * the cells (i,j) with i*j=p, and let m be the number of selected cells in
 * C_p.  The following finite identity is the key inclusion-exclusion step:
 *
 *   [m <= 1] = 1 + sum_{r=2..m} C(m,r) (-1)^(r-1) (r-1).             (1)
 *
 * Consequently, for every C_p we may select either no marked cell, with
 * weight 1, or any subset S of at least two cells, with weight
 * (-1)^(|S|-1)(|S|-1).  Multiplying (1) over all p leaves exactly the
 * permutations having no repeated product.
 *
 * Marked cells from different product classes must form a partial
 * permutation.  A fixed product class itself has no repeated row or column,
 * so every one of its subsets is internally compatible.  The DP processes
 * each nontrivial C_p once and records only the masks of occupied rows and
 * columns.  States with the same two masks can be combined: compatibility
 * with every future class, and the number (n-k)! of completions after k
 * rows and columns have been marked, depend only on those masks.  Thus
 *
 *   a(n) = sum_state weight(state) * (n-popcount(rows(state)))!.      (2)
 *
 * Coefficients in this inclusion-exclusion sum can be much larger than the
 * answer.  Each sparse DP pass is therefore performed modulo a distinct
 * 61-bit prime.  CRT passes continue until their product is strictly larger
 * than n!.  Since 0 <= a(n) <= n!, the reconstructed nonnegative residue is
 * then the unique exact answer.  No probabilistic step is used.
 *
 * For n <= 10 an independent depth-first enumeration checks the defining
 * condition directly.  Results through n=15 are also compared with the
 * terms published in OEIS; those terms are checks only and are not used by
 * the computation.
 *
 * Build:
 *
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic 333083_01.c \
 *     $(pkg-config --cflags --libs gmp) -o 333083_01
 *
 * Usage:
 *
 *   A333083_MEMORY_MIB=4096 ./333083_01 N [FROM]
 *
 * Every completed term is written to b333083_01_part.txt beside the executable
 * and made durable with fflush+fsync.  After all terms through N succeed, the
 * partial file atomically replaces b333083_01.txt.  The same b-file rows go to
 * stdout and progress goes to stderr.  If FROM is given, the verified prefix
 * below FROM is copied without running the DP; hence both outputs still cover
 * 0..N.  N is limited to 31 by the packed pair of 31-bit masks.  The
 * exponential state count normally makes the practical limit much smaller.
 * The environment variable bounds the two sparse hash tables (64..65536 MiB);
 * every size and allocation is checked before use.
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
#error "333083_01.c requires a platform with 64-bit unsigned long"
#endif

#if !defined(__SIZEOF_INT128__)
#error "333083_01.c requires unsigned __int128"
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
#define DEFAULT_MEMORY_MIB UINT64_C(4096)
#define MIN_MEMORY_MIB UINT64_C(64)
#define MAX_MEMORY_MIB UINT64_C(65536)

static const char *const verified_terms[VERIFIED_MAX_N + 1] = {
    "1", "1", "1", "3", "13", "67", "305", "2359",
    "16495", "141643", "1273691", "15580299", "152788607",
    "2206382433", "28916044241", "399450183613"
};

typedef struct {
    uint32_t rows;
    uint32_t columns;
    int8_t coefficient;
} Choice;

typedef struct {
    Choice *choices;
    size_t choice_count;
    int product;
    int cell_count;
} ProductGroup;

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
    const char *text = getenv("A333083_MEMORY_MIB");
    uint64_t mib = DEFAULT_MEMORY_MIB;
    if (text != NULL && *text != '\0') {
        char *end = NULL;
        errno = 0;
        unsigned long long value = strtoull(text, &end, 10);
        if (errno != 0 || end == text || *end != '\0' ||
            value < MIN_MEMORY_MIB || value > MAX_MEMORY_MIB) {
            fprintf(stderr,
                    "error: A333083_MEMORY_MIB must be in %" PRIu64
                    "..%" PRIu64 ": %s\n",
                    MIN_MEMORY_MIB, MAX_MEMORY_MIB, text);
            exit(EXIT_FAILURE);
        }
        mib = (uint64_t)value;
    }
    return mib * UINT64_C(1024) * UINT64_C(1024);
}

static void *checked_malloc(size_t count, size_t size);

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
                "raise A333083_MEMORY_MIB\n",
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

static uint64_t mix64(uint64_t x)
{
    x ^= x >> 30;
    x *= UINT64_C(0xbf58476d1ce4e5b9);
    x ^= x >> 27;
    x *= UINT64_C(0x94d049bb133111eb);
    return x ^ (x >> 31);
}

static void table_init(StateTable *table, size_t capacity,
                       MemoryBudget *budget)
{
    if (capacity < INITIAL_CAPACITY) {
        capacity = INITIAL_CAPACITY;
    }
    if ((capacity & (capacity - 1)) != 0) {
        die("internal hash capacity is not a power of two");
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
        die("duplicate state during hash-table rebuild");
    }
    table->keys[slot] = stored_key;
    table->values[slot] = value;
    ++table->size;
}

static void table_grow(StateTable *table, MemoryBudget *budget)
{
    if (table->capacity > SIZE_MAX / 2) {
        die("hash-table capacity overflow");
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

static uint64_t add_mod(uint64_t a, uint64_t b, uint64_t modulus)
{
    uint64_t sum = a + b;
    if (sum >= modulus) {
        sum -= modulus;
    }
    return sum;
}

static uint64_t multiply_small_mod(uint64_t value, unsigned factor,
                                   uint64_t modulus)
{
    if (modulus == 0) {
        die("zero modulus");
    }
    return (uint64_t)((U128)value * factor % modulus);
}

static void table_add(StateTable *table, uint64_t key, uint64_t addend,
                      uint64_t modulus, MemoryBudget *budget,
                      PassStats *stats)
{
    if (addend == 0) {
        return;
    }
    if (table->size + 1 >
        table->capacity * LOAD_NUMERATOR / LOAD_DENOMINATOR) {
        table_grow(table, budget);
    }
    uint64_t stored_key = key + 1;
    if (stored_key == 0) {
        die("packed state key overflow");
    }
    size_t slot = table_slot(table, stored_key);
    if (table->keys[slot] == 0) {
        table->keys[slot] = stored_key;
        table->values[slot] = addend;
        ++table->size;
        ++stats->insertions;
    } else {
        table->values[slot] =
            add_mod(table->values[slot], addend, modulus);
    }
}

static int compare_groups(const void *left_pointer, const void *right_pointer)
{
    const ProductGroup *left = left_pointer;
    const ProductGroup *right = right_pointer;
    if (left->cell_count != right->cell_count) {
        return left->cell_count < right->cell_count ? -1 : 1;
    }
    return (left->product > right->product) -
           (left->product < right->product);
}

static ProductGroup *make_groups(int n, size_t *group_count_out,
                                 size_t *choice_count_out)
{
    size_t maximum_groups = (size_t)n * (size_t)n;
    ProductGroup *groups =
        checked_calloc(maximum_groups, sizeof(*groups));
    size_t group_count = 0;
    size_t total_choices = 0;

    for (int product = 1; product <= n * n; ++product) {
        uint32_t cell_rows[MAX_N];
        uint32_t cell_columns[MAX_N];
        int cell_count = 0;
        for (int row = 1; row <= n; ++row) {
            if (product % row != 0) {
                continue;
            }
            int column = product / row;
            if (column >= 1 && column <= n) {
                cell_rows[cell_count] = UINT32_C(1) << (row - 1);
                cell_columns[cell_count] =
                    UINT32_C(1) << (column - 1);
                ++cell_count;
            }
        }
        if (cell_count < 2) {
            continue;
        }
        if (cell_count >= (int)(sizeof(size_t) * CHAR_BIT)) {
            die("too many cells in a product class");
        }
        size_t subset_count = (size_t)1 << cell_count;
        size_t choice_count = subset_count - 1U - (size_t)cell_count;
        Choice *choices = checked_malloc(choice_count, sizeof(*choices));
        size_t at = 0;
        for (size_t subset = 0; subset < subset_count; ++subset) {
            int cardinality = __builtin_popcountll((uint64_t)subset);
            if (cardinality < 2) {
                continue;
            }
            uint32_t rows = 0;
            uint32_t columns = 0;
            for (int cell = 0; cell < cell_count; ++cell) {
                if ((subset & ((size_t)1 << cell)) != 0) {
                    rows |= cell_rows[cell];
                    columns |= cell_columns[cell];
                }
            }
            int coefficient = (cardinality & 1)
                                  ? cardinality - 1
                                  : -(cardinality - 1);
            choices[at].rows = rows;
            choices[at].columns = columns;
            choices[at].coefficient = (int8_t)coefficient;
            ++at;
        }
        if (at != choice_count) {
            die("internal product-class enumeration error");
        }
        groups[group_count].choices = choices;
        groups[group_count].choice_count = choice_count;
        groups[group_count].product = product;
        groups[group_count].cell_count = cell_count;
        ++group_count;
        if (total_choices > SIZE_MAX - choice_count) {
            die("choice-count overflow");
        }
        total_choices += choice_count;
    }

    qsort(groups, group_count, sizeof(*groups), compare_groups);
    *group_count_out = group_count;
    *choice_count_out = total_choices;
    return groups;
}

static void free_groups(ProductGroup *groups, size_t group_count)
{
    for (size_t i = 0; i < group_count; ++i) {
        free(groups[i].choices);
    }
    free(groups);
}

static uint64_t dp_residue(int n, const ProductGroup *groups,
                           size_t group_count, uint64_t modulus,
                           uint64_t memory_limit, PassStats *stats)
{
    double started = monotonic_seconds();
    MemoryBudget budget = {memory_limit, 0, 0};
    StateTable current = {0};
    table_init(&current, INITIAL_CAPACITY, &budget);
    table_add(&current, 0, 1, modulus, &budget, stats);

    uint64_t row_mask_limit = (UINT64_C(1) << n) - 1;
    for (size_t group_index = 0; group_index < group_count; ++group_index) {
        const ProductGroup *group = &groups[group_index];
        StateTable next = {0};
        table_init(&next, current.capacity, &budget);

        for (size_t bucket = 0; bucket < current.capacity; ++bucket) {
            if (current.keys[bucket] == 0 || current.values[bucket] == 0) {
                continue;
            }
            uint64_t key = current.keys[bucket] - 1;
            uint64_t value = current.values[bucket];
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
                uint64_t weighted =
                    multiply_small_mod(value, magnitude, modulus);
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

    uint64_t factorial[MAX_N + 1];
    factorial[0] = 1;
    for (int k = 1; k <= n; ++k) {
        factorial[k] = multiply_small_mod(factorial[k - 1], (unsigned)k,
                                          modulus);
    }
    uint64_t answer = 0;
    for (size_t bucket = 0; bucket < current.capacity; ++bucket) {
        if (current.keys[bucket] == 0 || current.values[bucket] == 0) {
            continue;
        }
        uint64_t key = current.keys[bucket] - 1;
        uint32_t rows = (uint32_t)(key & row_mask_limit);
        int occupied = __builtin_popcount(rows);
        uint64_t term = (uint64_t)((U128)current.values[bucket] *
                                   factorial[n - occupied] % modulus);
        answer = add_mod(answer, term, modulus);
    }

    stats->final_states = current.size;
    table_clear(&current, &budget);
    if (budget.current != 0) {
        die("internal memory-accounting leak");
    }
    stats->peak_bytes = budget.peak;
    stats->seconds = monotonic_seconds() - started;
    return answer;
}

static int generate_moduli(uint64_t *moduli, const mpz_t upper_bound)
{
    mpz_t candidate;
    mpz_t product;
    mpz_init(candidate);
    mpz_init_set_ui(product, 1);
    mpz_set_ui(candidate, 1);
    mpz_mul_2exp(candidate, candidate, 61);
    mpz_sub_ui(candidate, candidate, UINT64_C(50000000));

    int count = 0;
    while (mpz_cmp(product, upper_bound) <= 0) {
        if (count >= MAX_CRT_PRIMES) {
            die("not enough CRT moduli");
        }
        mpz_nextprime(candidate, candidate);
        if (mpz_sizeinbase(candidate, 2) > 61) {
            die("failed to generate a 61-bit CRT prime");
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
                              bool *used_products)
{
    if (row > n) {
        return 1;
    }
    uint64_t count = 0;
    for (int column = 1; column <= n; ++column) {
        uint32_t bit = UINT32_C(1) << (column - 1);
        int product = row * column;
        if ((used_columns & bit) != 0 || used_products[product]) {
            continue;
        }
        used_products[product] = true;
        uint64_t add = direct_search(n, row + 1,
                                     used_columns | bit, used_products);
        if (count > UINT64_MAX - add) {
            die("direct verifier count overflow");
        }
        count += add;
        used_products[product] = false;
    }
    return count;
}

static uint64_t direct_count(int n)
{
    size_t product_count = (size_t)n * (size_t)n + 1;
    bool *used_products =
        checked_calloc(product_count, sizeof(*used_products));
    uint64_t result = direct_search(n, 1, 0, used_products);
    free(used_products);
    return result;
}

static void calculate_term(mpz_t answer, int n, uint64_t memory_limit,
                           bool report)
{
    size_t group_count = 0;
    size_t choice_count = 0;
    ProductGroup *groups = make_groups(n, &group_count, &choice_count);

    mpz_t factorial_bound;
    mpz_init(factorial_bound);
    mpz_fac_ui(factorial_bound, (unsigned long)n);
    uint64_t moduli[MAX_CRT_PRIMES];
    uint64_t residues[MAX_CRT_PRIMES];
    int modulus_count = generate_moduli(moduli, factorial_bound);

    double total_seconds = 0.0;
    uint64_t maximum_peak = 0;
    size_t final_states = 0;
    uint64_t compatible_choices = 0;
    for (int q = 0; q < modulus_count; ++q) {
        PassStats stats = {0};
        residues[q] = dp_residue(n, groups, group_count, moduli[q],
                                 memory_limit, &stats);
        total_seconds += stats.seconds;
        if (stats.peak_bytes > maximum_peak) {
            maximum_peak = stats.peak_bytes;
        }
        if (q == 0) {
            final_states = stats.final_states;
            compatible_choices = stats.compatible_choices;
        }
    }

    reconstruct_crt(answer, residues, moduli, modulus_count);
    if (mpz_sgn(answer) < 0 || mpz_cmp(answer, factorial_bound) > 0) {
        die("CRT reconstruction is outside the rigorous interval 0..n!");
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

    if (report) {
        fprintf(stderr,
                "A333083 n=%d groups=%zu choices=%zu states=%zu "
                "compatible=%" PRIu64 " CRT=%d time=%.3fs peak=%.3f MiB"
                "%s%s\n",
                n, group_count, choice_count, final_states,
                compatible_choices, modulus_count, total_seconds,
                (double)maximum_peak / (1024.0 * 1024.0),
                n <= DIRECT_CHECK_MAX_N ? " direct-check=ok" : "",
                n <= VERIFIED_MAX_N ? " known-term=ok" : "");
    }

    mpz_clear(factorial_bound);
    free_groups(groups, group_count);
}

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s N [FROM]\n"
            "  write exact A333083 terms 0..N (default FROM=0)\n"
            "  terms below FROM come from the verified built-in prefix\n"
            "  0 <= FROM <= min(N,%d), N <= %d\n",
            program, FIRST_UNVERIFIED_N, MAX_N);
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
        die("could not write a result to stdout");
    }
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
        path_beside_executable(argv[0], "b333083_01_part.txt");
    char *final_path = path_beside_executable(argv[0], "b333083_01.txt");
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
        fprintf(stderr, "A333083 n=%d loaded from verified prefix\n", n);
    }
    for (int n = first_n; n <= maximum_n; ++n) {
        calculate_term(answer, n, memory_limit, true);
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
