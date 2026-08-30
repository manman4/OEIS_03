/*
 * A354830 -- exact sparse permanent implementation.
 *
 * Count permutations p of {1,...,n} satisfying
 *
 *     gcd(i, p(i)) > 1  for 2 <= i <= n.
 *
 * Column 1 can only be used by row 1, hence p(1)=1.  A prime q>n/2
 * can only be paired with itself and is another forced fixed point.  Remove
 * those vertices and form the symmetric bipartite graph on the remaining
 * integers, with an edge i--j exactly when gcd(i,j)>1.  The answer is the
 * number of perfect matchings of this graph.
 *
 * The solver repeatedly removes degree-one vertices, factors disconnected
 * balanced components, branches on a minimum-degree row or column, and
 * memoizes only reached pairs of row/column masks.  Values are stored as
 * exact fixed-length little-endian integers.  The length is chosen from the
 * rigorous upper bound m!, where m is the number of non-forced vertices, so
 * no CRT passes and no probabilistic reconstruction are needed.
 *
 * Safety policy:
 *   - n is explicitly limited to 60; the reduced graph then has at most 52
 *     vertices and fits in uint64_t masks;
 *   - every memo allocation and transient resize is checked against a 2 GiB
 *     default limit (override with A354830_01_MEMORY_MIB);
 *   - fixed-length addition and multiplication detect any unexpected carry;
 *   - the result is checked against m! and known terms through n=30;
 *   - --check also uses an independent row-layer DP through n=25;
 *   - b354830_1.txt is written through a .part file and atomically renamed
 *     only after every requested term succeeds.
 *
 * Build on Apple Silicon with Homebrew GMP:
 *
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic \
 *       -I/opt/homebrew/opt/gmp/include -L/opt/homebrew/opt/gmp/lib \
 *       354830_01.c -lgmp -o 354830_01
 *
 * Examples:
 *
 *   ./354830_01 --check
 *   ./354830_01 --term 30
 *   ./354830_01 25
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
#error "354830_01 requires a platform with 64-bit unsigned long"
#endif

#define MAX_SUPPORTED_N 60
#define MAX_ACTIVE_VERTICES 59
#define MAX_LIMBS 5U
#define KNOWN_MAX_N 30
#define DEFAULT_MAX_N 25
#define DEFAULT_CHECK_N 25
#define LAYER_CHECK_MAX_N 25
#define DEFAULT_MEMORY_MIB UINT64_C(2048)
#define MIN_MEMORY_MIB UINT64_C(16)
#define MAX_MEMORY_MIB UINT64_C(65536)
#define INITIAL_ENTRY_CAPACITY ((size_t)16)
#define INITIAL_TABLE_CAPACITY ((size_t)32)
#define MEMO_REPORT_STEP ((size_t)1000000)

__extension__ typedef unsigned __int128 uint128_t;

static const char *const known_terms[KNOWN_MAX_N + 1] = {
    "1",
    "1",
    "1",
    "1",
    "2",
    "2",
    "8",
    "8",
    "30",
    "72",
    "408",
    "408",
    "4104",
    "4104",
    "29640",
    "208704",
    "1437312",
    "1437312",
    "22653504",
    "22653504",
    "318695040",
    "2686493376",
    "27628410816",
    "27628410816",
    "575372874240",
    "1775480841216",
    "21115550048256",
    "132879856582656",
    "2321256928702464",
    "2321256928702464",
    "83095013944442880"
};

typedef enum {
    DP_OK = 0,
    DP_MEMORY_LIMIT,
    DP_ALLOCATION_FAILURE,
    DP_SIZE_OVERFLOW,
    DP_ARITHMETIC_OVERFLOW,
    DP_INTERNAL_ERROR
} DpStatus;

typedef struct {
    unsigned active_count;
    unsigned forced_count;
    unsigned values[MAX_ACTIVE_VERTICES];
    uint64_t adjacency[MAX_ACTIVE_VERTICES];
} ActiveGraph;

typedef struct {
    uint64_t calls;
    uint64_t transitions;
    size_t peak_states;
    uint64_t peak_bytes;
    uint64_t required_bytes;
    size_t bound_bits;
    size_t limbs;
    unsigned active_count;
    unsigned forced_count;
    double seconds;
} DpStats;

typedef struct {
    uint64_t *row_masks;
    uint64_t *column_masks;
    uint64_t *values;
    uint32_t *slots;
    size_t entry_capacity;
    size_t table_capacity;
    size_t count;
    size_t limbs;
} PairMemo;

typedef struct {
    uint64_t adjacency[MAX_ACTIVE_VERTICES];
    unsigned active_count;
    uint64_t memory_budget;
    PairMemo memo;
    DpStats *stats;
    bool report;
    size_t next_report;
} GraphSolver;

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

static bool checked_add_u64(uint64_t left, uint64_t right,
                            uint64_t *result)
{
    if (left > UINT64_MAX - right) {
        return false;
    }
    *result = left + right;
    return true;
}

static bool checked_mul_size(size_t left, size_t right, size_t *result)
{
    if (left != 0 && right > SIZE_MAX / left) {
        return false;
    }
    *result = left * right;
    return true;
}

static uint64_t saturating_add_u64(uint64_t left, uint64_t right)
{
    return UINT64_MAX - left < right ? UINT64_MAX : left + right;
}

static uint64_t mix64(uint64_t value)
{
    value ^= value >> 30;
    value *= UINT64_C(0xbf58476d1ce4e5b9);
    value ^= value >> 27;
    value *= UINT64_C(0x94d049bb133111eb);
    value ^= value >> 31;
    return value;
}

static unsigned gcd_positive(unsigned left, unsigned right)
{
    while (right != 0) {
        unsigned remainder = left % right;
        left = right;
        right = remainder;
    }
    return left;
}

static bool is_prime(unsigned value)
{
    if (value < 2U) {
        return false;
    }
    if ((value & 1U) == 0) {
        return value == 2U;
    }
    for (unsigned divisor = 3U;
         divisor <= value / divisor; divisor += 2U) {
        if (value % divisor == 0) {
            return false;
        }
    }
    return true;
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

static uint64_t memory_budget_bytes(void)
{
    const char *text = getenv("A354830_01_MEMORY_MIB");
    uint64_t mib = DEFAULT_MEMORY_MIB;
    if (text != NULL && *text != '\0') {
        char *end = NULL;
        errno = 0;
        unsigned long long parsed = strtoull(text, &end, 10);
        if (errno != 0 || end == text || *end != '\0' ||
            parsed < MIN_MEMORY_MIB || parsed > MAX_MEMORY_MIB) {
            fprintf(stderr,
                    "error: A354830_01_MEMORY_MIB must be in %" PRIu64
                    "..%" PRIu64 ": %s\n",
                    MIN_MEMORY_MIB, MAX_MEMORY_MIB, text);
            exit(EXIT_FAILURE);
        }
        mib = (uint64_t)parsed;
    }
    return mib << 20;
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
    size_t path_size;
    if (directory_length > SIZE_MAX - filename_length - 2U) {
        die("output path length overflow");
    }
    path_size = directory_length + filename_length + 2U;
    char *path = malloc(path_size);
    if (path == NULL) {
        die("could not allocate output path");
    }
    if (slash == NULL) {
        path[0] = '.';
    } else if (directory_length == 0) {
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

static DpStatus build_active_graph(int n, ActiveGraph *graph)
{
    memset(graph, 0, sizeof(*graph));
    if (n <= 1) {
        graph->forced_count = (unsigned)n;
        return DP_OK;
    }
    graph->forced_count = 1U; /* p(1)=1 */
    for (unsigned value = 2U; value <= (unsigned)n; ++value) {
        if (is_prime(value) && 2U * value > (unsigned)n) {
            ++graph->forced_count;
            continue;
        }
        if (graph->active_count >= MAX_ACTIVE_VERTICES) {
            return DP_INTERNAL_ERROR;
        }
        graph->values[graph->active_count++] = value;
    }
    if (graph->active_count >= 64U) {
        return DP_INTERNAL_ERROR;
    }
    for (unsigned row = 0; row < graph->active_count; ++row) {
        uint64_t mask = 0;
        for (unsigned column = 0; column < graph->active_count; ++column) {
            if (gcd_positive(graph->values[row],
                             graph->values[column]) > 1U) {
                mask |= UINT64_C(1) << column;
            }
        }
        graph->adjacency[row] = mask;
        if ((mask & (UINT64_C(1) << row)) == 0) {
            return DP_INTERNAL_ERROR;
        }
    }
    for (unsigned row = 0; row < graph->active_count; ++row) {
        for (unsigned column = 0; column < graph->active_count; ++column) {
            bool forward = (graph->adjacency[row] &
                            (UINT64_C(1) << column)) != 0;
            bool backward = (graph->adjacency[column] &
                             (UINT64_C(1) << row)) != 0;
            if (forward != backward) {
                return DP_INTERNAL_ERROR;
            }
        }
    }
    return DP_OK;
}

static void big_zero(uint64_t *value, size_t limbs)
{
    memset(value, 0, limbs * sizeof(*value));
}

static void big_one(uint64_t *value, size_t limbs)
{
    big_zero(value, limbs);
    value[0] = 1;
}

static bool big_add(uint64_t *destination, const uint64_t *source,
                    size_t limbs)
{
    uint64_t carry = 0;
    for (size_t index = 0; index < limbs; ++index) {
        uint128_t sum = (uint128_t)destination[index] +
                        source[index] + carry;
        destination[index] = (uint64_t)sum;
        carry = (uint64_t)(sum >> 64);
    }
    return carry == 0;
}

static bool big_multiply(uint64_t *destination, const uint64_t *left,
                         const uint64_t *right, size_t limbs)
{
    uint64_t product[MAX_LIMBS] = {0};
    for (size_t i = 0; i < limbs; ++i) {
        uint64_t carry = 0;
        size_t j = 0;
        for (; i + j < limbs; ++j) {
            uint128_t term = (uint128_t)left[i] * right[j] +
                             product[i + j] + carry;
            product[i + j] = (uint64_t)term;
            carry = (uint64_t)(term >> 64);
        }
        if (carry != 0) {
            return false;
        }
        for (; j < limbs; ++j) {
            if (left[i] != 0 && right[j] != 0) {
                return false;
            }
        }
    }
    memcpy(destination, product, limbs * sizeof(*destination));
    return true;
}

static uint64_t pair_hash(uint64_t rows, uint64_t columns)
{
    uint64_t right = mix64(columns + UINT64_C(0xd6e8feb86659fd93));
    return mix64(rows) ^ ((right << 29) | (right >> 35));
}

static bool pair_storage_bytes(size_t entry_capacity,
                               size_t table_capacity, size_t limbs,
                               uint64_t *bytes)
{
    size_t row_bytes;
    size_t column_bytes;
    size_t value_count;
    size_t value_bytes;
    size_t slot_bytes;
    if (!checked_mul_size(entry_capacity, sizeof(uint64_t), &row_bytes) ||
        !checked_mul_size(entry_capacity, sizeof(uint64_t),
                          &column_bytes) ||
        !checked_mul_size(entry_capacity, limbs, &value_count) ||
        !checked_mul_size(value_count, sizeof(uint64_t), &value_bytes) ||
        !checked_mul_size(table_capacity, sizeof(uint32_t), &slot_bytes)) {
        return false;
    }
    uint64_t partial;
    return checked_add_u64((uint64_t)row_bytes,
                           (uint64_t)column_bytes, &partial) &&
           checked_add_u64(partial, (uint64_t)value_bytes, &partial) &&
           checked_add_u64(partial, (uint64_t)slot_bytes, bytes);
}

static uint64_t pair_memo_bytes(const PairMemo *memo)
{
    uint64_t bytes;
    return pair_storage_bytes(memo->entry_capacity,
                              memo->table_capacity, memo->limbs, &bytes)
               ? bytes : UINT64_MAX;
}

static DpStatus pair_memo_init(GraphSolver *solver, size_t limbs)
{
    PairMemo *memo = &solver->memo;
    memset(memo, 0, sizeof(*memo));
    memo->entry_capacity = INITIAL_ENTRY_CAPACITY;
    memo->table_capacity = INITIAL_TABLE_CAPACITY;
    memo->limbs = limbs;
    uint64_t bytes = pair_memo_bytes(memo);
    if (bytes == UINT64_MAX) {
        solver->stats->required_bytes = UINT64_MAX;
        return DP_SIZE_OVERFLOW;
    }
    if (bytes > solver->memory_budget) {
        solver->stats->required_bytes = bytes;
        return DP_MEMORY_LIMIT;
    }
    size_t value_count;
    if (!checked_mul_size(memo->entry_capacity, limbs, &value_count)) {
        solver->stats->required_bytes = UINT64_MAX;
        return DP_SIZE_OVERFLOW;
    }
    memo->row_masks = malloc(memo->entry_capacity * sizeof(uint64_t));
    memo->column_masks = malloc(memo->entry_capacity * sizeof(uint64_t));
    memo->values = malloc(value_count * sizeof(uint64_t));
    memo->slots = calloc(memo->table_capacity, sizeof(uint32_t));
    if (memo->row_masks == NULL || memo->column_masks == NULL ||
        memo->values == NULL || memo->slots == NULL) {
        free(memo->slots);
        free(memo->values);
        free(memo->column_masks);
        free(memo->row_masks);
        memset(memo, 0, sizeof(*memo));
        solver->stats->required_bytes = bytes;
        return DP_ALLOCATION_FAILURE;
    }
    solver->stats->peak_bytes = bytes;
    return DP_OK;
}

static void pair_memo_destroy(PairMemo *memo)
{
    free(memo->slots);
    free(memo->values);
    free(memo->column_masks);
    free(memo->row_masks);
    memset(memo, 0, sizeof(*memo));
}

static DpStatus pair_memo_rehash(GraphSolver *solver,
                                 size_t new_table_capacity)
{
    PairMemo *memo = &solver->memo;
    if (new_table_capacity == 0 ||
        new_table_capacity < memo->table_capacity ||
        (new_table_capacity & (new_table_capacity - 1U)) != 0 ||
        new_table_capacity > SIZE_MAX / sizeof(uint32_t)) {
        solver->stats->required_bytes = UINT64_MAX;
        return DP_SIZE_OVERFLOW;
    }
    uint64_t active = pair_memo_bytes(memo);
    uint64_t new_slot_bytes =
        (uint64_t)new_table_capacity * sizeof(uint32_t);
    uint64_t transient;
    if (active == UINT64_MAX ||
        !checked_add_u64(active, new_slot_bytes, &transient)) {
        solver->stats->required_bytes = UINT64_MAX;
        return DP_SIZE_OVERFLOW;
    }
    if (transient > solver->memory_budget) {
        solver->stats->required_bytes = transient;
        return DP_MEMORY_LIMIT;
    }
    if (transient > solver->stats->peak_bytes) {
        solver->stats->peak_bytes = transient;
    }
    uint32_t *new_slots = calloc(new_table_capacity, sizeof(uint32_t));
    if (new_slots == NULL) {
        solver->stats->required_bytes = transient;
        return DP_ALLOCATION_FAILURE;
    }
    size_t mask = new_table_capacity - 1U;
    for (size_t entry = 0; entry < memo->count; ++entry) {
        size_t slot = (size_t)pair_hash(memo->row_masks[entry],
                                        memo->column_masks[entry]) & mask;
        while (new_slots[slot] != 0) {
            slot = (slot + 1U) & mask;
        }
        new_slots[slot] = (uint32_t)(entry + 1U);
    }
    free(memo->slots);
    memo->slots = new_slots;
    memo->table_capacity = new_table_capacity;
    return DP_OK;
}

static DpStatus pair_memo_grow_entries(GraphSolver *solver)
{
    PairMemo *memo = &solver->memo;
    size_t increase = memo->entry_capacity / 4U;
    if (increase == 0) {
        increase = 1;
    }
    if (memo->entry_capacity > SIZE_MAX - increase) {
        solver->stats->required_bytes = UINT64_MAX;
        return DP_SIZE_OVERFLOW;
    }
    size_t new_capacity = memo->entry_capacity + increase;
    if (new_capacity >= (size_t)UINT32_MAX) {
        solver->stats->required_bytes = UINT64_MAX;
        return DP_MEMORY_LIMIT;
    }
    size_t new_value_count;
    size_t new_row_bytes;
    size_t new_column_bytes;
    size_t new_value_bytes;
    if (!checked_mul_size(new_capacity, memo->limbs, &new_value_count) ||
        !checked_mul_size(new_capacity, sizeof(uint64_t), &new_row_bytes) ||
        !checked_mul_size(new_capacity, sizeof(uint64_t),
                          &new_column_bytes) ||
        !checked_mul_size(new_value_count, sizeof(uint64_t),
                          &new_value_bytes)) {
        solver->stats->required_bytes = UINT64_MAX;
        return DP_SIZE_OVERFLOW;
    }
    uint64_t transient = pair_memo_bytes(memo);
    if (transient == UINT64_MAX ||
        !checked_add_u64(transient, (uint64_t)new_row_bytes,
                         &transient) ||
        !checked_add_u64(transient, (uint64_t)new_column_bytes,
                         &transient) ||
        !checked_add_u64(transient, (uint64_t)new_value_bytes,
                         &transient)) {
        solver->stats->required_bytes = UINT64_MAX;
        return DP_SIZE_OVERFLOW;
    }
    if (transient > solver->memory_budget) {
        solver->stats->required_bytes = transient;
        return DP_MEMORY_LIMIT;
    }
    if (transient > solver->stats->peak_bytes) {
        solver->stats->peak_bytes = transient;
    }
    uint64_t *new_rows = malloc(new_row_bytes);
    uint64_t *new_columns = malloc(new_column_bytes);
    uint64_t *new_values = malloc(new_value_bytes);
    if (new_rows == NULL || new_columns == NULL || new_values == NULL) {
        free(new_values);
        free(new_columns);
        free(new_rows);
        solver->stats->required_bytes = transient;
        return DP_ALLOCATION_FAILURE;
    }
    memcpy(new_rows, memo->row_masks,
           memo->count * sizeof(*new_rows));
    memcpy(new_columns, memo->column_masks,
           memo->count * sizeof(*new_columns));
    memcpy(new_values, memo->values,
           memo->count * memo->limbs * sizeof(*new_values));
    free(memo->values);
    free(memo->column_masks);
    free(memo->row_masks);
    memo->row_masks = new_rows;
    memo->column_masks = new_columns;
    memo->values = new_values;
    memo->entry_capacity = new_capacity;
    return DP_OK;
}

static bool pair_memo_lookup(const PairMemo *memo, uint64_t rows,
                             uint64_t columns, uint64_t *result)
{
    size_t mask = memo->table_capacity - 1U;
    size_t slot = (size_t)pair_hash(rows, columns) & mask;
    while (memo->slots[slot] != 0) {
        size_t entry = (size_t)memo->slots[slot] - 1U;
        if (memo->row_masks[entry] == rows &&
            memo->column_masks[entry] == columns) {
            memcpy(result, memo->values + entry * memo->limbs,
                   memo->limbs * sizeof(*result));
            return true;
        }
        slot = (slot + 1U) & mask;
    }
    return false;
}

static DpStatus pair_memo_store(GraphSolver *solver, uint64_t rows,
                                uint64_t columns,
                                const uint64_t *value)
{
    PairMemo *memo = &solver->memo;
    for (;;) {
        size_t mask = memo->table_capacity - 1U;
        size_t slot = (size_t)pair_hash(rows, columns) & mask;
        while (memo->slots[slot] != 0) {
            size_t entry = (size_t)memo->slots[slot] - 1U;
            if (memo->row_masks[entry] == rows &&
                memo->column_masks[entry] == columns) {
                memcpy(memo->values + entry * memo->limbs, value,
                       memo->limbs * sizeof(*value));
                return DP_OK;
            }
            slot = (slot + 1U) & mask;
        }
        size_t maximum_load =
            memo->table_capacity - memo->table_capacity / 5U;
        if (memo->count + 1U > maximum_load) {
            if (memo->table_capacity > SIZE_MAX / 2U) {
                solver->stats->required_bytes = UINT64_MAX;
                return DP_SIZE_OVERFLOW;
            }
            DpStatus status = pair_memo_rehash(
                solver, memo->table_capacity * 2U);
            if (status != DP_OK) {
                return status;
            }
            continue;
        }
        if (memo->count == memo->entry_capacity) {
            DpStatus status = pair_memo_grow_entries(solver);
            if (status != DP_OK) {
                return status;
            }
        }
        if (memo->count >= (size_t)UINT32_MAX - 1U) {
            solver->stats->required_bytes = UINT64_MAX;
            return DP_MEMORY_LIMIT;
        }
        size_t entry = memo->count++;
        memo->row_masks[entry] = rows;
        memo->column_masks[entry] = columns;
        memcpy(memo->values + entry * memo->limbs, value,
               memo->limbs * sizeof(*value));
        memo->slots[slot] = (uint32_t)(entry + 1U);
        if (memo->count > solver->stats->peak_states) {
            solver->stats->peak_states = memo->count;
        }
        if (solver->report && memo->count >= solver->next_report) {
            fprintf(stderr,
                    "354830_01: active=%u memo=%zu calls=%" PRIu64
                    " allocated=%.2f MiB\n",
                    solver->active_count, memo->count,
                    solver->stats->calls,
                    (double)pair_memo_bytes(memo) /
                        (double)(UINT64_C(1) << 20));
            solver->next_report += MEMO_REPORT_STEP;
        }
        return DP_OK;
    }
}

static uint64_t neighbor_union(const GraphSolver *solver,
                               uint64_t vertices,
                               uint64_t allowed_targets)
{
    uint64_t neighbors = 0;
    while (vertices != 0) {
        uint64_t bit = vertices & (UINT64_C(0) - vertices);
        vertices ^= bit;
        unsigned index = (unsigned)__builtin_ctzll(bit);
        neighbors |= solver->adjacency[index];
    }
    return neighbors & allowed_targets;
}

static DpStatus solve_graph_state(GraphSolver *solver, uint64_t rows,
                                  uint64_t columns, uint64_t *result)
{
    solver->stats->calls = saturating_add_u64(solver->stats->calls, 1U);
    size_t limbs = solver->memo.limbs;
    if (rows == 0 || columns == 0) {
        if (rows == 0 && columns == 0) {
            big_one(result, limbs);
        } else {
            big_zero(result, limbs);
        }
        return DP_OK;
    }

    for (;;) {
        bool changed = false;
        uint64_t scan = rows;
        while (scan != 0) {
            uint64_t row_bit = scan & (UINT64_C(0) - scan);
            scan ^= row_bit;
            unsigned row = (unsigned)__builtin_ctzll(row_bit);
            uint64_t choices = solver->adjacency[row] & columns;
            unsigned degree = (unsigned)__builtin_popcountll(choices);
            if (degree == 0) {
                big_zero(result, limbs);
                return DP_OK;
            }
            if (degree == 1U) {
                rows ^= row_bit;
                columns ^= choices;
                changed = true;
                break;
            }
        }
        if (changed) {
            if (rows == 0) {
                if (columns == 0) {
                    big_one(result, limbs);
                } else {
                    big_zero(result, limbs);
                }
                return DP_OK;
            }
            continue;
        }
        scan = columns;
        while (scan != 0) {
            uint64_t column_bit = scan & (UINT64_C(0) - scan);
            scan ^= column_bit;
            unsigned column = (unsigned)__builtin_ctzll(column_bit);
            uint64_t choices = solver->adjacency[column] & rows;
            unsigned degree = (unsigned)__builtin_popcountll(choices);
            if (degree == 0) {
                big_zero(result, limbs);
                return DP_OK;
            }
            if (degree == 1U) {
                columns ^= column_bit;
                rows ^= choices;
                changed = true;
                break;
            }
        }
        if (!changed) {
            break;
        }
        if (rows == 0) {
            if (columns == 0) {
                big_one(result, limbs);
            } else {
                big_zero(result, limbs);
            }
            return DP_OK;
        }
    }

    if (__builtin_popcountll(rows) != __builtin_popcountll(columns)) {
        big_zero(result, limbs);
        return DP_OK;
    }
    if (rows > columns) {
        uint64_t temporary = rows;
        rows = columns;
        columns = temporary;
    }
    if (pair_memo_lookup(&solver->memo, rows, columns, result)) {
        return DP_OK;
    }

    uint64_t component_rows = rows & (UINT64_C(0) - rows);
    uint64_t component_columns = 0;
    uint64_t frontier_rows = component_rows;
    while (frontier_rows != 0) {
        uint64_t new_columns =
            neighbor_union(solver, frontier_rows, columns) &
            ~component_columns;
        if (new_columns == 0) {
            break;
        }
        component_columns |= new_columns;
        uint64_t new_rows =
            neighbor_union(solver, new_columns, rows) & ~component_rows;
        component_rows |= new_rows;
        frontier_rows = new_rows;
    }

    DpStatus status;
    if (component_rows != rows || component_columns != columns) {
        if (__builtin_popcountll(component_rows) !=
            __builtin_popcountll(component_columns)) {
            big_zero(result, limbs);
        } else {
            uint64_t left[MAX_LIMBS];
            uint64_t right[MAX_LIMBS];
            status = solve_graph_state(solver, component_rows,
                                       component_columns, left);
            if (status != DP_OK) {
                return status;
            }
            status = solve_graph_state(solver, rows ^ component_rows,
                                       columns ^ component_columns, right);
            if (status != DP_OK) {
                return status;
            }
            if (!big_multiply(result, left, right, limbs)) {
                return DP_ARITHMETIC_OVERFLOW;
            }
        }
        return pair_memo_store(solver, rows, columns, result);
    }

    unsigned best_degree = UINT_MAX;
    bool use_row = true;
    uint64_t selected_bit = 0;
    uint64_t choices = 0;
    uint64_t scan = rows;
    while (scan != 0) {
        uint64_t bit = scan & (UINT64_C(0) - scan);
        scan ^= bit;
        unsigned index = (unsigned)__builtin_ctzll(bit);
        uint64_t available = solver->adjacency[index] & columns;
        unsigned degree = (unsigned)__builtin_popcountll(available);
        if (degree < best_degree) {
            best_degree = degree;
            use_row = true;
            selected_bit = bit;
            choices = available;
        }
    }
    scan = columns;
    while (scan != 0) {
        uint64_t bit = scan & (UINT64_C(0) - scan);
        scan ^= bit;
        unsigned index = (unsigned)__builtin_ctzll(bit);
        uint64_t available = solver->adjacency[index] & rows;
        unsigned degree = (unsigned)__builtin_popcountll(available);
        if (degree < best_degree) {
            best_degree = degree;
            use_row = false;
            selected_bit = bit;
            choices = available;
        }
    }
    if (selected_bit == 0 || choices == 0 || best_degree == UINT_MAX) {
        return DP_INTERNAL_ERROR;
    }

    big_zero(result, limbs);
    while (choices != 0) {
        uint64_t choice = choices & (UINT64_C(0) - choices);
        choices ^= choice;
        solver->stats->transitions = saturating_add_u64(
            solver->stats->transitions, 1U);
        uint64_t term[MAX_LIMBS];
        status = use_row
                     ? solve_graph_state(solver, rows ^ selected_bit,
                                         columns ^ choice, term)
                     : solve_graph_state(solver, rows ^ choice,
                                         columns ^ selected_bit, term);
        if (status != DP_OK) {
            return status;
        }
        if (!big_add(result, term, limbs)) {
            return DP_ARITHMETIC_OVERFLOW;
        }
    }
    return pair_memo_store(solver, rows, columns, result);
}

static const char *status_text(DpStatus status)
{
    switch (status) {
    case DP_OK:
        return "ok";
    case DP_MEMORY_LIMIT:
        return "memory limit exceeded";
    case DP_ALLOCATION_FAILURE:
        return "allocation failed";
    case DP_SIZE_OVERFLOW:
        return "allocation-size calculation overflowed";
    case DP_ARITHMETIC_OVERFLOW:
        return "fixed-length exact arithmetic overflowed its bound";
    case DP_INTERNAL_ERROR:
        return "internal consistency check failed";
    }
    return "unknown error";
}

static DpStatus permanent_exact(mpz_t result, int n,
                                uint64_t memory_budget, bool report,
                                DpStats *stats)
{
    memset(stats, 0, sizeof(*stats));
    double start = monotonic_seconds();
    ActiveGraph graph;
    DpStatus status = build_active_graph(n, &graph);
    stats->active_count = graph.active_count;
    stats->forced_count = graph.forced_count;
    if (status != DP_OK) {
        stats->seconds = monotonic_seconds() - start;
        return status;
    }

    mpz_t bound;
    mpz_init(bound);
    mpz_fac_ui(bound, (unsigned long)graph.active_count);
    stats->bound_bits = mpz_sizeinbase(bound, 2);
    stats->limbs = (stats->bound_bits + 63U) / 64U;
    if (stats->limbs == 0) {
        stats->limbs = 1;
    }
    if (stats->limbs > MAX_LIMBS) {
        mpz_clear(bound);
        stats->seconds = monotonic_seconds() - start;
        return DP_INTERNAL_ERROR;
    }
    if (graph.active_count == 0) {
        mpz_set_ui(result, 1);
        stats->calls = 1;
        stats->peak_states = 1;
        stats->seconds = monotonic_seconds() - start;
        mpz_clear(bound);
        return DP_OK;
    }

    GraphSolver solver;
    memset(&solver, 0, sizeof(solver));
    solver.active_count = graph.active_count;
    solver.memory_budget = memory_budget;
    solver.stats = stats;
    solver.report = report;
    solver.next_report = MEMO_REPORT_STEP;
    memcpy(solver.adjacency, graph.adjacency,
           graph.active_count * sizeof(*solver.adjacency));
    status = pair_memo_init(&solver, stats->limbs);
    if (status != DP_OK) {
        mpz_clear(bound);
        stats->seconds = monotonic_seconds() - start;
        return status;
    }

    if (report) {
        fprintf(stderr,
                "354830_01: n=%d active=%u forced=%u, exact bound=%zu "
                "bits, %zu limb%s, memory limit=%.2f GiB\n",
                n, graph.active_count, graph.forced_count,
                stats->bound_bits, stats->limbs,
                stats->limbs == 1U ? "" : "s",
                (double)memory_budget /
                    (double)(UINT64_C(1) << 30));
    }
    uint64_t limbs[MAX_LIMBS];
    uint64_t full_mask =
        (UINT64_C(1) << graph.active_count) - 1U;
    status = solve_graph_state(&solver, full_mask, full_mask, limbs);
    if (status == DP_OK) {
        mpz_import(result, stats->limbs, -1, sizeof(uint64_t), 0, 0,
                   limbs);
        if (mpz_sgn(result) < 0 || mpz_cmp(result, bound) > 0) {
            status = DP_INTERNAL_ERROR;
        }
    }
    uint64_t active_bytes = pair_memo_bytes(&solver.memo);
    if (active_bytes > stats->peak_bytes) {
        stats->peak_bytes = active_bytes;
    }
    pair_memo_destroy(&solver.memo);
    mpz_clear(bound);
    stats->seconds = monotonic_seconds() - start;
    return status;
}

static DpStatus permanent_layer_u64(uint64_t *result, int n,
                                    uint64_t memory_budget,
                                    DpStats *stats)
{
    memset(stats, 0, sizeof(*stats));
    double start = monotonic_seconds();
    ActiveGraph graph;
    DpStatus status = build_active_graph(n, &graph);
    stats->active_count = graph.active_count;
    stats->forced_count = graph.forced_count;
    if (status != DP_OK) {
        return status;
    }
    uint64_t factorial = 1;
    for (unsigned value = 2; value <= graph.active_count; ++value) {
        if (factorial > UINT64_MAX / value) {
            return DP_ARITHMETIC_OVERFLOW;
        }
        factorial *= value;
    }
    if (graph.active_count == 0) {
        *result = 1;
        stats->peak_states = 1;
        stats->seconds = monotonic_seconds() - start;
        return DP_OK;
    }
    if (graph.active_count >= sizeof(size_t) * CHAR_BIT) {
        return DP_SIZE_OVERFLOW;
    }
    size_t state_count = (size_t)1U << graph.active_count;
    size_t bytes;
    if (!checked_mul_size(state_count, sizeof(uint64_t), &bytes)) {
        return DP_SIZE_OVERFLOW;
    }
    if ((uint64_t)bytes > memory_budget) {
        stats->required_bytes = (uint64_t)bytes;
        return DP_MEMORY_LIMIT;
    }
    uint64_t *dp = calloc(state_count, sizeof(*dp));
    if (dp == NULL) {
        stats->required_bytes = (uint64_t)bytes;
        return DP_ALLOCATION_FAILURE;
    }
    stats->peak_bytes = (uint64_t)bytes;
    dp[0] = 1;

    unsigned order[MAX_ACTIVE_VERTICES];
    bool selected[MAX_ACTIVE_VERTICES] = {false};
    for (unsigned position = 0; position < graph.active_count; ++position) {
        int best = -1;
        unsigned best_degree = UINT_MAX;
        for (unsigned row = 0; row < graph.active_count; ++row) {
            if (!selected[row]) {
                unsigned degree =
                    (unsigned)__builtin_popcountll(graph.adjacency[row]);
                if (best < 0 || degree < best_degree) {
                    best = (int)row;
                    best_degree = degree;
                }
            }
        }
        if (best < 0) {
            free(dp);
            return DP_INTERNAL_ERROR;
        }
        order[position] = (unsigned)best;
        selected[best] = true;
    }

    size_t active_states = 1;
    for (unsigned position = 0; position < graph.active_count; ++position) {
        size_t next_active = 0;
        uint64_t row_mask = graph.adjacency[order[position]];
        for (size_t state = 0; state < state_count; ++state) {
            uint64_t value = dp[state];
            if (value == 0 ||
                (unsigned)__builtin_popcountll((uint64_t)state) !=
                    position) {
                continue;
            }
            uint64_t available = row_mask & ~(uint64_t)state;
            while (available != 0) {
                uint64_t bit = available & (UINT64_C(0) - available);
                available ^= bit;
                size_t destination = state | (size_t)bit;
                if (UINT64_MAX - dp[destination] < value) {
                    free(dp);
                    return DP_ARITHMETIC_OVERFLOW;
                }
                if (dp[destination] == 0) {
                    ++next_active;
                }
                dp[destination] += value;
                stats->transitions = saturating_add_u64(
                    stats->transitions, 1U);
            }
        }
        active_states = next_active;
        if (active_states > stats->peak_states) {
            stats->peak_states = active_states;
        }
    }
    *result = dp[state_count - 1U];
    if (*result > factorial) {
        free(dp);
        return DP_INTERNAL_ERROR;
    }
    free(dp);
    stats->seconds = monotonic_seconds() - start;
    return DP_OK;
}

static void report_failure(int n, DpStatus status,
                           uint64_t memory_budget,
                           const DpStats *stats)
{
    fprintf(stderr, "error: A354830 n=%d: %s\n",
            n, status_text(status));
    if (status == DP_MEMORY_LIMIT ||
        status == DP_ALLOCATION_FAILURE) {
        if (stats->required_bytes != 0 &&
            stats->required_bytes != UINT64_MAX) {
            fprintf(stderr,
                    "required at failed allocation: %.2f MiB; "
                    "configured limit: %.2f MiB\n",
                    (double)stats->required_bytes /
                        (double)(UINT64_C(1) << 20),
                    (double)memory_budget /
                        (double)(UINT64_C(1) << 20));
        }
        fprintf(stderr,
                "Increase A354830_01_MEMORY_MIB only when enough "
                "physical RAM is available.\n");
    }
}

static void verify_known_value(const mpz_t value, int n)
{
    if (n < 0 || n > KNOWN_MAX_N) {
        return;
    }
    mpz_t expected;
    mpz_init(expected);
    if (mpz_set_str(expected, known_terms[n], 10) != 0) {
        mpz_clear(expected);
        die("invalid built-in A354830 term");
    }
    if (mpz_cmp(value, expected) != 0) {
        gmp_fprintf(stderr,
                    "error: known-term check failed at n=%d: "
                    "got %Zd, expected %Zd\n",
                    n, value, expected);
        mpz_clear(expected);
        exit(EXIT_FAILURE);
    }
    mpz_clear(expected);
}

static void compute_checked(mpz_t value, int n,
                            uint64_t memory_budget, bool report)
{
    DpStats stats;
    DpStatus status = permanent_exact(
        value, n, memory_budget, report, &stats);
    if (status != DP_OK) {
        report_failure(n, status, memory_budget, &stats);
        exit(EXIT_FAILURE);
    }
    verify_known_value(value, n);
    if (report) {
        fprintf(stderr,
                "354830_01: n=%d done, active=%u, memo=%zu, "
                "calls=%" PRIu64 ", transitions=%" PRIu64
                ", peak allocation=%.2f MiB, %.3f s\n",
                n, stats.active_count, stats.peak_states,
                stats.calls, stats.transitions,
                (double)stats.peak_bytes /
                    (double)(UINT64_C(1) << 20),
                stats.seconds);
    }
}

static int check_known_terms(int max_n, uint64_t memory_budget)
{
    if (max_n > KNOWN_MAX_N) {
        fprintf(stderr,
                "error: --check maximum is %d because later terms "
                "are not built in\n",
                KNOWN_MAX_N);
        return EXIT_FAILURE;
    }
    mpz_t value;
    mpz_init(value);
    for (int n = 0; n <= max_n; ++n) {
        compute_checked(value, n, memory_budget, false);
        if (n <= LAYER_CHECK_MAX_N) {
            uint64_t layer_value = 0;
            DpStats layer_stats;
            DpStatus status = permanent_layer_u64(
                &layer_value, n, memory_budget, &layer_stats);
            if (status != DP_OK) {
                report_failure(n, status, memory_budget, &layer_stats);
                mpz_clear(value);
                return EXIT_FAILURE;
            }
            if (mpz_cmp_ui(value, (unsigned long)layer_value) != 0) {
                gmp_fprintf(stderr,
                            "error: graph and layer DP disagree at n=%d: "
                            "%Zd versus %" PRIu64 "\n",
                            n, value, layer_value);
                mpz_clear(value);
                return EXIT_FAILURE;
            }
        }
    }
    mpz_clear(value);
    printf("ok: exact graph DP agrees with the known A354830 prefix "
           "for n=0..%d; independent layer DP agrees for n=0..%d\n",
           max_n, max_n < LAYER_CHECK_MAX_N
                      ? max_n : LAYER_CHECK_MAX_N);
    return EXIT_SUCCESS;
}

static void produce_b_file(const char *argv0, int max_n,
                           uint64_t memory_budget)
{
    char *path = path_beside_executable(argv0, "b354830_1.txt");
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
        compute_checked(value, n, memory_budget, true);
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
            "Every b-file term is computed by 354830_01 itself.\n"
            "The default memory limit is %" PRIu64
            " MiB; override it with\n"
            "A354830_01_MEMORY_MIB.\n",
            program, program, program, DEFAULT_MAX_N,
            MAX_SUPPORTED_N, DEFAULT_MEMORY_MIB);
}

int main(int argc, char **argv)
{
    uint64_t memory_budget = memory_budget_bytes();
    if (argc >= 2 && strcmp(argv[1], "--check") == 0) {
        if (argc > 3) {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
        int max_n = argc == 3
                        ? parse_n(argv[2], "check MAX_N")
                        : DEFAULT_CHECK_N;
        return check_known_terms(max_n, memory_budget);
    }
    if (argc >= 2 && strcmp(argv[1], "--term") == 0) {
        if (argc != 3) {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
        int n = parse_n(argv[2], "N");
        mpz_t value;
        mpz_init(value);
        compute_checked(value, n, memory_budget, true);
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
    produce_b_file(argv[0], max_n, memory_budget);
    return EXIT_SUCCESS;
}
