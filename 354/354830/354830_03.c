/*
 * A354830 -- exact prime-support-type dynamic programming.
 *
 * Count permutations p of {1,...,n} satisfying
 *
 *     gcd(i, p(i)) > 1  for 2 <= i <= n.
 *
 * Row 1 must use column 1.  A prime q>n/2 must also use itself.  After
 * removing these forced fixed points, two remaining integers have identical
 * rows (and columns) precisely when they have the same set of prime divisors.
 * We combine such integers into types.  If type j has multiplicity m[j], the
 * desired permanent is
 *
 *   (product_j m[j]!) [x_1^m[1] ... x_t^m[t]]
 *       product_i (sum_{j: gcd(rad(i),rad(j))>1} x_j)^m[i].
 *
 * The coefficient is evaluated row by row.  A state contains only the number
 * of columns used from each type.  Counts use a mixed-radix array index and a
 * compact bit-field copy, avoiding hash tables and divisions in the inner
 * loop.  One dense residue array is reused for sequential 31-bit prime
 * passes; GMP performs rigorously bounded CRT reconstruction.  This algorithm
 * is deliberately different from the pair-of-subsets memoization in
 * 354830_01.c.
 *
 * Safety policy:
 *   - n is explicitly limited to 45; all mixed-radix indices and packed count
 *     vectors are proved to fit in uint32_t before allocation;
 *   - the complete workspace is sized with checked arithmetic and compared
 *     with a 4 GiB default limit (override with A354830_03_MEMORY_MIB);
 *   - fixed moduli are rechecked by deterministic 64-bit Miller--Rabin;
 *   - the CRT modulus product must exceed the rigorous active! bound, every
 *     reconstructed residue is rechecked, and the result must not exceed the
 *     bound;
 *   - --check compares with reference terms through n=33 and with an
 *     independent expanded-vertex subset DP through n=25;
 *   - b354830_3.txt is written through a .part file and atomically renamed.
 *
 * Build on Apple Silicon with Homebrew GMP:
 *
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic \
 *       -I/opt/homebrew/opt/gmp/include -L/opt/homebrew/opt/gmp/lib \
 *       354830_03.c -lgmp -o 354830_03
 *
 * Examples:
 *
 *   ./354830_03 --check
 *   ./354830_03 --term 45
 *   ./354830_03 45
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
#error "354830_03 requires a platform with 64-bit unsigned long"
#endif

#define MAX_SUPPORTED_N 45
#define MAX_ACTIVE_VERTICES 42U
#define MAX_TYPES 32U
#define REFERENCE_MAX_N 33
#define DEFAULT_MAX_N 30
#define DEFAULT_CHECK_N 30
#define INDEPENDENT_CHECK_MAX_N 25
#define DEFAULT_MEMORY_MIB UINT64_C(4096)
#define MIN_MEMORY_MIB UINT64_C(16)
#define MAX_MEMORY_MIB UINT64_C(65536)
#define PROGRESS_STATE_THRESHOLD UINT32_C(1000000)
#define MAX_CRT_PASSES 6U

__extension__ typedef unsigned __int128 uint128_t;

static const uint64_t crt_primes[MAX_CRT_PASSES] = {
    UINT64_C(2147483647),
    UINT64_C(2147483629),
    UINT64_C(2147483587),
    UINT64_C(2147483579),
    UINT64_C(2147483563),
    UINT64_C(2147483549)
};

/* n=31..33 were independently obtained with 354830_01. */
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
    DP_OK = 0,
    DP_MEMORY_LIMIT,
    DP_ALLOCATION_FAILURE,
    DP_SIZE_OVERFLOW,
    DP_ARITHMETIC_OVERFLOW,
    DP_INTERNAL_ERROR
} DpStatus;

typedef struct {
    uint32_t index;
    uint32_t counts;
} StateRecord;

_Static_assert(sizeof(StateRecord) == 8U,
               "StateRecord must remain an eight-byte packed pair");

typedef struct {
    unsigned active_count;
    unsigned forced_count;
    unsigned type_count;
    unsigned packed_bits;
    unsigned radical[MAX_TYPES];
    unsigned multiplicity[MAX_TYPES];
    uint32_t allowed[MAX_TYPES];
    uint32_t stride[MAX_TYPES];
    unsigned shift[MAX_TYPES];
    uint32_t count_mask[MAX_TYPES];
    uint32_t increment[MAX_TYPES];
    unsigned row_order[MAX_ACTIVE_VERTICES];
    uint32_t state_count;
    uint32_t full_counts;
    size_t max_layer_bound;
} TypePlan;

typedef struct {
    uint32_t *values;
    StateRecord *first;
    StateRecord *second;
    uint8_t *marks;
    size_t marks_bytes;
    size_t list_capacity;
    uint64_t allocated_bytes;
} Workspace;

typedef struct {
    uint64_t transitions;
    uint64_t state_visits;
    size_t peak_layer;
    uint64_t peak_bytes;
    uint64_t required_bytes;
    size_t bound_bits;
    unsigned passes;
    unsigned active_count;
    unsigned forced_count;
    unsigned type_count;
    uint32_t index_states;
    size_t max_layer_bound;
    double seconds;
} DpStats;

typedef struct {
    unsigned count;
    unsigned forced_count;
    unsigned values[MAX_ACTIVE_VERTICES];
    uint64_t adjacency[MAX_ACTIVE_VERTICES];
} ExpandedGraph;

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

static uint64_t memory_budget_bytes(void)
{
    const char *text = getenv("A354830_03_MEMORY_MIB");
    uint64_t mib = DEFAULT_MEMORY_MIB;
    if (text != NULL && *text != '\0') {
        char *end = NULL;
        errno = 0;
        unsigned long long parsed = strtoull(text, &end, 10);
        if (errno != 0 || end == text || *end != '\0' ||
            parsed < MIN_MEMORY_MIB || parsed > MAX_MEMORY_MIB) {
            fprintf(stderr,
                    "error: A354830_03_MEMORY_MIB must be in %" PRIu64
                    "..%" PRIu64 ": %s\n",
                    MIN_MEMORY_MIB, MAX_MEMORY_MIB, text);
            exit(EXIT_FAILURE);
        }
        mib = (uint64_t)parsed;
    }
    return mib << 20;
}

static uint64_t multiply_mod(uint64_t left, uint64_t right,
                             uint64_t modulus)
{
    return (uint64_t)((uint128_t)left * right % modulus);
}

static uint64_t power_mod(uint64_t base, uint64_t exponent,
                          uint64_t modulus)
{
    uint64_t result = 1U;
    while (exponent != 0U) {
        if ((exponent & 1U) != 0U) {
            result = multiply_mod(result, base, modulus);
        }
        exponent >>= 1;
        if (exponent != 0U) {
            base = multiply_mod(base, base, modulus);
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
    for (size_t index = 0;
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
    for (size_t index = 0; index < sizeof(bases) / sizeof(bases[0]);
         ++index) {
        uint64_t base = bases[index] % value;
        if (base == 0U) {
            continue;
        }
        uint64_t witness = power_mod(base, odd_part, value);
        if (witness == 1U || witness == value - 1U) {
            continue;
        }
        bool passed = false;
        for (unsigned round = 1U; round < twos; ++round) {
            witness = multiply_mod(witness, witness, value);
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

static DpStatus build_type_plan(int n, TypePlan *plan)
{
    memset(plan, 0, sizeof(*plan));
    if (n <= 1) {
        plan->forced_count = (unsigned)n;
        plan->state_count = 1U;
        plan->max_layer_bound = 1U;
        return DP_OK;
    }
    plan->forced_count = 1U; /* p(1)=1 */
    for (unsigned value = 2U; value <= (unsigned)n; ++value) {
        if (is_prime_small(value) && 2U * value > (unsigned)n) {
            ++plan->forced_count;
            continue;
        }
        unsigned radical = squarefree_radical(value);
        unsigned type = 0U;
        while (type < plan->type_count &&
               plan->radical[type] != radical) {
            ++type;
        }
        if (type == plan->type_count) {
            if (plan->type_count >= MAX_TYPES) {
                return DP_SIZE_OVERFLOW;
            }
            plan->radical[type] = radical;
            ++plan->type_count;
        }
        ++plan->multiplicity[type];
        ++plan->active_count;
    }
    if (plan->active_count > MAX_ACTIVE_VERTICES ||
        plan->type_count > 32U) {
        return DP_SIZE_OVERFLOW;
    }

    for (unsigned row = 0U; row < plan->type_count; ++row) {
        uint32_t mask = 0U;
        for (unsigned column = 0U; column < plan->type_count; ++column) {
            if (gcd_positive(plan->radical[row],
                             plan->radical[column]) > 1U) {
                mask |= UINT32_C(1) << column;
            }
        }
        if ((mask & (UINT32_C(1) << row)) == 0U) {
            return DP_INTERNAL_ERROR;
        }
        plan->allowed[row] = mask;
    }

    uint64_t states = 1U;
    unsigned packed_bits = 0U;
    uint32_t full_counts = 0U;
    for (unsigned type = 0U; type < plan->type_count; ++type) {
        unsigned capacity = plan->multiplicity[type];
        if (states > UINT32_MAX) {
            return DP_SIZE_OVERFLOW;
        }
        plan->stride[type] = (uint32_t)states;
        if (!checked_mul_u64(states, (uint64_t)capacity + 1U,
                             &states) || states > UINT32_MAX) {
            return DP_SIZE_OVERFLOW;
        }
        unsigned bits = 0U;
        uint32_t field_values = 1U;
        while (field_values <= capacity) {
            if (bits == 31U) {
                return DP_SIZE_OVERFLOW;
            }
            field_values <<= 1;
            ++bits;
        }
        if (packed_bits > 32U - bits) {
            return DP_SIZE_OVERFLOW;
        }
        plan->shift[type] = packed_bits;
        plan->count_mask[type] = field_values - 1U;
        plan->increment[type] = UINT32_C(1) << packed_bits;
        full_counts |= (uint32_t)capacity << packed_bits;
        packed_bits += bits;
    }
    plan->state_count = (uint32_t)states;
    plan->packed_bits = packed_bits;
    plan->full_counts = full_counts;

    uint64_t layer[MAX_ACTIVE_VERTICES + 1U] = {0};
    uint64_t next[MAX_ACTIVE_VERTICES + 1U] = {0};
    layer[0] = 1U;
    unsigned degree_limit = 0U;
    for (unsigned type = 0U; type < plan->type_count; ++type) {
        memset(next, 0, sizeof(next));
        for (unsigned used = 0U; used <= degree_limit; ++used) {
            for (unsigned add = 0U;
                 add <= plan->multiplicity[type]; ++add) {
                if (!checked_add_u64(next[used + add], layer[used],
                                     &next[used + add])) {
                    return DP_SIZE_OVERFLOW;
                }
            }
        }
        degree_limit += plan->multiplicity[type];
        memcpy(layer, next, sizeof(layer));
    }
    uint64_t layer_sum = 0U;
    for (unsigned used = 0U; used <= plan->active_count; ++used) {
        if (!checked_add_u64(layer_sum, layer[used], &layer_sum)) {
            return DP_SIZE_OVERFLOW;
        }
        if (layer[used] > plan->max_layer_bound) {
            if (layer[used] > SIZE_MAX) {
                return DP_SIZE_OVERFLOW;
            }
            plan->max_layer_bound = (size_t)layer[used];
        }
    }
    if (layer_sum != states || plan->max_layer_bound == 0U) {
        return DP_INTERNAL_ERROR;
    }

    unsigned type_order[MAX_TYPES];
    bool chosen[MAX_TYPES] = {false};
    for (unsigned position = 0U; position < plan->type_count; ++position) {
        int best = -1;
        unsigned best_weighted = UINT_MAX;
        unsigned best_categories = UINT_MAX;
        for (unsigned type = 0U; type < plan->type_count; ++type) {
            if (chosen[type]) {
                continue;
            }
            unsigned weighted = 0U;
            uint32_t neighbors = plan->allowed[type];
            for (unsigned column = 0U; column < plan->type_count;
                 ++column) {
                if ((neighbors & (UINT32_C(1) << column)) != 0U) {
                    weighted += plan->multiplicity[column];
                }
            }
            unsigned categories = (unsigned)__builtin_popcount(neighbors);
            if (best < 0 || weighted < best_weighted ||
                (weighted == best_weighted &&
                 categories < best_categories) ||
                (weighted == best_weighted &&
                 categories == best_categories &&
                 plan->radical[type] < plan->radical[(unsigned)best])) {
                best = (int)type;
                best_weighted = weighted;
                best_categories = categories;
            }
        }
        if (best < 0) {
            return DP_INTERNAL_ERROR;
        }
        type_order[position] = (unsigned)best;
        chosen[(unsigned)best] = true;
    }
    unsigned row_position = 0U;
    for (unsigned order = 0U; order < plan->type_count; ++order) {
        unsigned type = type_order[order];
        for (unsigned copy = 0U; copy < plan->multiplicity[type]; ++copy) {
            if (row_position >= plan->active_count) {
                return DP_INTERNAL_ERROR;
            }
            plan->row_order[row_position++] = type;
        }
    }
    return row_position == plan->active_count ? DP_OK : DP_INTERNAL_ERROR;
}

static DpStatus workspace_init(const TypePlan *plan,
                               size_t list_capacity,
                               uint64_t memory_budget,
                               Workspace *workspace, DpStats *stats)
{
    memset(workspace, 0, sizeof(*workspace));
    if (list_capacity == 0U) {
        return DP_INTERNAL_ERROR;
    }
    uint64_t value_bytes;
    uint64_t list_bytes;
    uint64_t both_lists;
    uint64_t total;
    uint64_t marks_bytes = ((uint64_t)plan->state_count + 7U) / 8U;
    if (!checked_mul_u64(plan->state_count, sizeof(uint32_t),
                         &value_bytes) ||
        !checked_mul_u64((uint64_t)list_capacity,
                         sizeof(StateRecord), &list_bytes) ||
        !checked_mul_u64(list_bytes, 2U, &both_lists) ||
        !checked_add_u64(value_bytes, both_lists, &total) ||
        !checked_add_u64(total, marks_bytes, &total) ||
        value_bytes > SIZE_MAX || list_bytes > SIZE_MAX ||
        marks_bytes > SIZE_MAX) {
        stats->required_bytes = UINT64_MAX;
        return DP_SIZE_OVERFLOW;
    }
    if (list_bytes == 0U || marks_bytes == 0U) {
        return DP_INTERNAL_ERROR;
    }
    stats->required_bytes = total;
    if (total > memory_budget) {
        return DP_MEMORY_LIMIT;
    }
    workspace->values = calloc((size_t)plan->state_count,
                               sizeof(*workspace->values));
    workspace->first = malloc((size_t)list_bytes);
    workspace->second = malloc((size_t)list_bytes);
    workspace->marks = calloc((size_t)marks_bytes, 1U);
    if (workspace->values == NULL || workspace->first == NULL ||
        workspace->second == NULL || workspace->marks == NULL) {
        free(workspace->marks);
        free(workspace->second);
        free(workspace->first);
        free(workspace->values);
        memset(workspace, 0, sizeof(*workspace));
        return DP_ALLOCATION_FAILURE;
    }
    workspace->marks_bytes = (size_t)marks_bytes;
    workspace->list_capacity = list_capacity;
    workspace->allocated_bytes = total;
    if (total > stats->peak_bytes) {
        stats->peak_bytes = total;
    }
    return DP_OK;
}

static void workspace_destroy(Workspace *workspace)
{
    free(workspace->marks);
    free(workspace->second);
    free(workspace->first);
    free(workspace->values);
    memset(workspace, 0, sizeof(*workspace));
}

static bool state_is_marked(const Workspace *workspace, uint32_t index)
{
    return (workspace->marks[index >> 3] &
            (uint8_t)(UINT8_C(1) << (index & 7U))) != 0U;
}

static void mark_state(Workspace *workspace, uint32_t index)
{
    workspace->marks[index >> 3] |=
        (uint8_t)(UINT8_C(1) << (index & 7U));
}

static void unmark_state(Workspace *workspace, uint32_t index)
{
    workspace->marks[index >> 3] &=
        (uint8_t)~(UINT8_C(1) << (index & 7U));
}

static DpStatus measure_reachable_peak(const TypePlan *plan,
                                       uint64_t memory_budget,
                                       DpStats *stats,
                                       size_t *reachable_peak)
{
    Workspace workspace;
    memset(&workspace, 0, sizeof(workspace));
    if (plan->max_layer_bound == 0U) {
        return DP_INTERNAL_ERROR;
    }
    uint64_t list_bytes;
    uint64_t both_lists;
    uint64_t total;
    uint64_t marks_bytes = ((uint64_t)plan->state_count + 7U) / 8U;
    if (!checked_mul_u64((uint64_t)plan->max_layer_bound,
                         sizeof(StateRecord), &list_bytes) ||
        !checked_mul_u64(list_bytes, 2U, &both_lists) ||
        !checked_add_u64(both_lists, marks_bytes, &total) ||
        list_bytes > SIZE_MAX || marks_bytes > SIZE_MAX) {
        stats->required_bytes = UINT64_MAX;
        return DP_SIZE_OVERFLOW;
    }
    if (list_bytes == 0U || marks_bytes == 0U) {
        return DP_INTERNAL_ERROR;
    }
    stats->required_bytes = total;
    if (total > memory_budget) {
        return DP_MEMORY_LIMIT;
    }
    workspace.first = malloc((size_t)list_bytes);
    workspace.second = malloc((size_t)list_bytes);
    workspace.marks = calloc((size_t)marks_bytes, 1U);
    if (workspace.first == NULL || workspace.second == NULL ||
        workspace.marks == NULL) {
        workspace_destroy(&workspace);
        return DP_ALLOCATION_FAILURE;
    }
    workspace.marks_bytes = (size_t)marks_bytes;
    workspace.list_capacity = plan->max_layer_bound;
    workspace.allocated_bytes = total;
    if (total > stats->peak_bytes) {
        stats->peak_bytes = total;
    }

    StateRecord *current = workspace.first;
    StateRecord *next = workspace.second;
    size_t current_count = 1U;
    size_t peak = 1U;
    current[0].index = 0U;
    current[0].counts = 0U;
    mark_state(&workspace, 0U);
    for (unsigned position = 0U; position < plan->active_count;
         ++position) {
        uint32_t choices_for_row =
            plan->allowed[plan->row_order[position]];
        size_t next_count = 0U;
        for (size_t state_number = 0U; state_number < current_count;
             ++state_number) {
            StateRecord source = current[state_number];
            unmark_state(&workspace, source.index);
            uint32_t choices = choices_for_row;
            while (choices != 0U) {
                unsigned column_type = (unsigned)__builtin_ctz(choices);
                choices &= choices - 1U;
                uint32_t count =
                    (source.counts >> plan->shift[column_type]) &
                    plan->count_mask[column_type];
                if (count >= plan->multiplicity[column_type]) {
                    continue;
                }
                uint32_t destination =
                    source.index + plan->stride[column_type];
                if (!state_is_marked(&workspace, destination)) {
                    if (next_count >= workspace.list_capacity) {
                        workspace_destroy(&workspace);
                        return DP_INTERNAL_ERROR;
                    }
                    mark_state(&workspace, destination);
                    next[next_count].index = destination;
                    next[next_count].counts =
                        source.counts + plan->increment[column_type];
                    ++next_count;
                }
            }
        }
        if (next_count == 0U) {
            workspace_destroy(&workspace);
            return DP_INTERNAL_ERROR;
        }
        StateRecord *swap = current;
        current = next;
        next = swap;
        current_count = next_count;
        if (current_count > peak) {
            peak = current_count;
        }
    }
    uint32_t full_index = plan->state_count - 1U;
    if (current_count != 1U || current[0].index != full_index ||
        current[0].counts != plan->full_counts ||
        !state_is_marked(&workspace, full_index)) {
        workspace_destroy(&workspace);
        return DP_INTERNAL_ERROR;
    }
    unmark_state(&workspace, full_index);
    workspace_destroy(&workspace);
    *reachable_peak = peak;
    return DP_OK;
}

static uint32_t add_mod(uint32_t left, uint32_t right,
                        uint32_t modulus)
{
    uint64_t sum = (uint64_t)left + right;
    return (uint32_t)(sum >= modulus ? sum - modulus : sum);
}

static DpStatus modular_coefficient(const TypePlan *plan,
                                    Workspace *workspace,
                                    uint64_t modulus, unsigned pass,
                                    unsigned passes, int n, bool report,
                                    DpStats *stats, uint64_t *residue)
{
    if (workspace->values[0] != 0U || state_is_marked(workspace, 0U)) {
        return DP_INTERNAL_ERROR;
    }
    StateRecord *current = workspace->first;
    StateRecord *next = workspace->second;
    size_t current_count = 1U;
    current[0].index = 0U;
    current[0].counts = 0U;
    workspace->values[0] = 1U;
    mark_state(workspace, 0U);
    uint64_t pass_transitions = 0U;
    uint64_t pass_visits = 0U;
    double pass_start = monotonic_seconds();

    for (unsigned position = 0U; position < plan->active_count;
         ++position) {
        unsigned row_type = plan->row_order[position];
        uint32_t allowed = plan->allowed[row_type];
        size_t next_count = 0U;
        pass_visits = saturating_add_u64(pass_visits, current_count);
        for (size_t state_number = 0U; state_number < current_count;
             ++state_number) {
            StateRecord source = current[state_number];
            uint32_t value = workspace->values[source.index];
            workspace->values[source.index] = 0U;
            unmark_state(workspace, source.index);
            uint32_t choices = allowed;
            while (choices != 0U) {
                unsigned column_type = (unsigned)__builtin_ctz(choices);
                choices &= choices - 1U;
                uint32_t count =
                    (source.counts >> plan->shift[column_type]) &
                    plan->count_mask[column_type];
                if (count >= plan->multiplicity[column_type]) {
                    continue;
                }
                uint32_t destination =
                    source.index + plan->stride[column_type];
                uint32_t destination_counts =
                    source.counts + plan->increment[column_type];
                if (!state_is_marked(workspace, destination)) {
                    if (next_count >= workspace->list_capacity) {
                        return DP_INTERNAL_ERROR;
                    }
                    mark_state(workspace, destination);
                    next[next_count].index = destination;
                    next[next_count].counts = destination_counts;
                    ++next_count;
                }
                workspace->values[destination] = add_mod(
                    workspace->values[destination], value,
                    (uint32_t)modulus);
                pass_transitions = saturating_add_u64(
                    pass_transitions, 1U);
            }
        }
        if (next_count == 0U) {
            return DP_INTERNAL_ERROR;
        }
        StateRecord *swap = current;
        current = next;
        next = swap;
        current_count = next_count;
        if (current_count > stats->peak_layer) {
            stats->peak_layer = current_count;
        }
        if (report && plan->state_count >= PROGRESS_STATE_THRESHOLD) {
            fprintf(stderr,
                    "354830_03: n=%d pass=%u/%u layer=%u/%u "
                    "row-radical=%u states=%zu transitions=%" PRIu64
                    "\n",
                    n, pass, passes, position + 1U,
                    plan->active_count, plan->radical[row_type],
                    current_count, pass_transitions);
        }
    }
    pass_visits = saturating_add_u64(pass_visits, current_count);
    uint32_t full_index = plan->state_count - 1U;
    if (current_count != 1U || current[0].index != full_index ||
        current[0].counts != plan->full_counts ||
        !state_is_marked(workspace, full_index)) {
        return DP_INTERNAL_ERROR;
    }
    uint64_t coefficient = workspace->values[full_index];
    workspace->values[full_index] = 0U;
    unmark_state(workspace, full_index);

    uint64_t label_factor = 1U;
    for (unsigned type = 0U; type < plan->type_count; ++type) {
        for (unsigned factor = 2U;
             factor <= plan->multiplicity[type]; ++factor) {
            label_factor = multiply_mod(label_factor, factor, modulus);
        }
    }
    *residue = multiply_mod(coefficient, label_factor, modulus);
    stats->transitions = saturating_add_u64(stats->transitions,
                                             pass_transitions);
    stats->state_visits = saturating_add_u64(stats->state_visits,
                                              pass_visits);
    if (report) {
        fprintf(stderr,
                "354830_03: n=%d pass %u/%u ok, residue=%" PRIu64
                ", states=%" PRIu64 ", transitions=%" PRIu64
                ", %.3f s\n",
                n, pass, passes, *residue, pass_visits,
                pass_transitions, monotonic_seconds() - pass_start);
    }
    return DP_OK;
}

static DpStatus reconstruct_crt(mpz_t result, const uint64_t *residues,
                                unsigned passes, const mpz_t bound)
{
    mpz_t product;
    mpz_init_set_ui(product, 1U);
    mpz_set_ui(result, 0U);
    for (unsigned pass = 0U; pass < passes; ++pass) {
        uint64_t prime = crt_primes[pass];
        uint64_t result_mod = mpz_fdiv_ui(result, (unsigned long)prime);
        uint64_t product_mod = mpz_fdiv_ui(product,
                                            (unsigned long)prime);
        if (product_mod == 0U) {
            mpz_clear(product);
            return DP_INTERNAL_ERROR;
        }
        uint64_t inverse = power_mod(product_mod, prime - 2U, prime);
        if (multiply_mod(product_mod, inverse, prime) != 1U) {
            mpz_clear(product);
            return DP_INTERNAL_ERROR;
        }
        uint64_t difference = residues[pass] >= result_mod
                                  ? residues[pass] - result_mod
                                  : prime - (result_mod - residues[pass]);
        uint64_t multiplier = multiply_mod(difference, inverse, prime);
        mpz_addmul_ui(result, product, (unsigned long)multiplier);
        mpz_mul_ui(product, product, (unsigned long)prime);
    }
    if (mpz_cmp(product, bound) <= 0 || mpz_sgn(result) < 0 ||
        mpz_cmp(result, bound) > 0) {
        mpz_clear(product);
        return DP_INTERNAL_ERROR;
    }
    for (unsigned pass = 0U; pass < passes; ++pass) {
        if (mpz_fdiv_ui(result, (unsigned long)crt_primes[pass]) !=
            residues[pass]) {
            mpz_clear(product);
            return DP_INTERNAL_ERROR;
        }
    }
    mpz_clear(product);
    return DP_OK;
}

static DpStatus permanent_exact(mpz_t result, int n,
                                uint64_t memory_budget, bool report,
                                DpStats *stats)
{
    memset(stats, 0, sizeof(*stats));
    double start = monotonic_seconds();
    TypePlan plan;
    DpStatus status = build_type_plan(n, &plan);
    stats->active_count = plan.active_count;
    stats->forced_count = plan.forced_count;
    stats->type_count = plan.type_count;
    stats->index_states = plan.state_count;
    stats->max_layer_bound = plan.max_layer_bound;
    if (status != DP_OK) {
        stats->seconds = monotonic_seconds() - start;
        return status;
    }
    mpz_t bound;
    mpz_init(bound);
    mpz_fac_ui(bound, (unsigned long)plan.active_count);
    stats->bound_bits = mpz_sizeinbase(bound, 2);
    if (plan.active_count == 0U) {
        mpz_set_ui(result, 1U);
        stats->passes = 0U;
        stats->peak_layer = 1U;
        stats->seconds = monotonic_seconds() - start;
        mpz_clear(bound);
        return DP_OK;
    }

    mpz_t prime_product;
    mpz_init_set_ui(prime_product, 1U);
    unsigned passes = 0U;
    while (mpz_cmp(prime_product, bound) <= 0) {
        if (passes >= MAX_CRT_PASSES ||
            !is_prime_u64(crt_primes[passes])) {
            mpz_clear(prime_product);
            mpz_clear(bound);
            stats->seconds = monotonic_seconds() - start;
            return DP_INTERNAL_ERROR;
        }
        mpz_mul_ui(prime_product, prime_product,
                   (unsigned long)crt_primes[passes]);
        ++passes;
    }
    mpz_clear(prime_product);
    stats->passes = passes;

    if (report) {
        fprintf(stderr,
                "354830_03: n=%d measuring reachable layers, "
                "index states=%" PRIu32 ", theoretical max layer=%zu\n",
                n, plan.state_count, plan.max_layer_bound);
    }
    size_t reachable_peak = 0U;
    status = measure_reachable_peak(&plan, memory_budget, stats,
                                    &reachable_peak);
    if (status != DP_OK) {
        mpz_clear(bound);
        stats->seconds = monotonic_seconds() - start;
        return status;
    }
    stats->peak_layer = reachable_peak;

    Workspace workspace;
    status = workspace_init(&plan, reachable_peak, memory_budget,
                            &workspace, stats);
    if (status != DP_OK) {
        mpz_clear(bound);
        stats->seconds = monotonic_seconds() - start;
        return status;
    }
    if (report) {
        fprintf(stderr,
                "354830_03: n=%d active=%u forced=%u types=%u, "
                "index states=%" PRIu32 ", reachable peak=%zu "
                "(bound=%zu), "
                "packed=%u bits, exact bound=%zu bits, %u prime "
                "pass%s, memory=%.2f GiB\n",
                n, plan.active_count, plan.forced_count, plan.type_count,
                plan.state_count, reachable_peak, plan.max_layer_bound,
                plan.packed_bits,
                stats->bound_bits, passes, passes == 1U ? "" : "es",
                (double)workspace.allocated_bytes /
                    (double)(UINT64_C(1) << 30));
    }
    uint64_t residues[MAX_CRT_PASSES] = {0};
    for (unsigned pass = 0U; pass < passes; ++pass) {
        if (report) {
            fprintf(stderr,
                    "354830_03: n=%d pass %u/%u, p=%" PRIu64 "\n",
                    n, pass + 1U, passes, crt_primes[pass]);
        }
        status = modular_coefficient(
            &plan, &workspace, crt_primes[pass], pass + 1U, passes,
            n, report, stats, &residues[pass]);
        if (status != DP_OK) {
            break;
        }
    }
    if (status == DP_OK) {
        status = reconstruct_crt(result, residues, passes, bound);
    }
    workspace_destroy(&workspace);
    mpz_clear(bound);
    stats->seconds = monotonic_seconds() - start;
    return status;
}

static DpStatus build_expanded_graph(int n, ExpandedGraph *graph)
{
    memset(graph, 0, sizeof(*graph));
    if (n <= 1) {
        graph->forced_count = (unsigned)n;
        return DP_OK;
    }
    graph->forced_count = 1U;
    for (unsigned value = 2U; value <= (unsigned)n; ++value) {
        if (is_prime_small(value) && 2U * value > (unsigned)n) {
            ++graph->forced_count;
        } else {
            if (graph->count >= MAX_ACTIVE_VERTICES) {
                return DP_SIZE_OVERFLOW;
            }
            graph->values[graph->count++] = value;
        }
    }
    for (unsigned row = 0U; row < graph->count; ++row) {
        uint64_t mask = 0U;
        for (unsigned column = 0U; column < graph->count; ++column) {
            if (gcd_positive(graph->values[row],
                             graph->values[column]) > 1U) {
                mask |= UINT64_C(1) << column;
            }
        }
        graph->adjacency[row] = mask;
    }
    return DP_OK;
}

static DpStatus independent_subset_dp(uint64_t *result, int n,
                                      uint64_t memory_budget,
                                      DpStats *stats)
{
    ExpandedGraph graph;
    DpStatus status = build_expanded_graph(n, &graph);
    if (status != DP_OK) {
        return status;
    }
    if (graph.count == 0U) {
        *result = 1U;
        return DP_OK;
    }
    if (graph.count >= sizeof(size_t) * CHAR_BIT || graph.count >= 64U) {
        return DP_SIZE_OVERFLOW;
    }
    size_t state_count = (size_t)1U << graph.count;
    uint64_t bytes;
    if (!checked_mul_u64((uint64_t)state_count, sizeof(uint64_t),
                         &bytes) || bytes > SIZE_MAX) {
        return DP_SIZE_OVERFLOW;
    }
    if (bytes > memory_budget) {
        stats->required_bytes = bytes;
        return DP_MEMORY_LIMIT;
    }
    uint64_t *dp = calloc(state_count, sizeof(*dp));
    if (dp == NULL) {
        stats->required_bytes = bytes;
        return DP_ALLOCATION_FAILURE;
    }
    unsigned order[MAX_ACTIVE_VERTICES];
    bool chosen[MAX_ACTIVE_VERTICES] = {false};
    for (unsigned position = 0U; position < graph.count; ++position) {
        int best = -1;
        unsigned best_degree = UINT_MAX;
        for (unsigned row = 0U; row < graph.count; ++row) {
            if (!chosen[row]) {
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
        chosen[(unsigned)best] = true;
    }
    dp[0] = 1U;
    for (unsigned position = 0U; position < graph.count; ++position) {
        for (size_t state = 0U; state < state_count; ++state) {
            if ((unsigned)__builtin_popcountll((uint64_t)state) !=
                position || dp[state] == 0U) {
                continue;
            }
            uint64_t available = graph.adjacency[order[position]] &
                                 ~(uint64_t)state;
            while (available != 0U) {
                uint64_t bit = available & (UINT64_C(0) - available);
                available ^= bit;
                size_t destination = state | (size_t)bit;
                if (dp[destination] > UINT64_MAX - dp[state]) {
                    free(dp);
                    return DP_ARITHMETIC_OVERFLOW;
                }
                dp[destination] += dp[state];
            }
        }
    }
    *result = dp[state_count - 1U];
    free(dp);
    return DP_OK;
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
        return "state or allocation size exceeded its proved limit";
    case DP_ARITHMETIC_OVERFLOW:
        return "independent exact arithmetic overflowed";
    case DP_INTERNAL_ERROR:
        return "internal consistency check failed";
    }
    return "unknown error";
}

static void report_failure(int n, DpStatus status,
                           uint64_t memory_budget,
                           const DpStats *stats)
{
    fprintf(stderr, "error: A354830 n=%d: %s\n",
            n, status_text(status));
    if (status == DP_MEMORY_LIMIT || status == DP_ALLOCATION_FAILURE) {
        if (stats->required_bytes != 0U &&
            stats->required_bytes != UINT64_MAX) {
            fprintf(stderr,
                    "required workspace: %.2f MiB; configured limit: "
                    "%.2f MiB\n",
                    (double)stats->required_bytes /
                        (double)(UINT64_C(1) << 20),
                    (double)memory_budget /
                        (double)(UINT64_C(1) << 20));
        }
        fprintf(stderr,
                "Increase A354830_03_MEMORY_MIB only when enough "
                "physical RAM is available.\n");
    }
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
                    "error: reference check failed at n=%d: "
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
    DpStatus status = permanent_exact(value, n, memory_budget,
                                      report, &stats);
    if (status != DP_OK) {
        report_failure(n, status, memory_budget, &stats);
        exit(EXIT_FAILURE);
    }
    verify_reference_value(value, n);
    if (report) {
        fprintf(stderr,
                "354830_03: n=%d done, active=%u, types=%u, "
                "index states=%" PRIu32 ", peak layer=%zu, "
                "state visits=%" PRIu64 ", modular transitions=%" PRIu64
                ", peak allocation=%.2f MiB, %.3f s\n",
                n, stats.active_count, stats.type_count,
                stats.index_states, stats.peak_layer, stats.state_visits,
                stats.transitions,
                (double)stats.peak_bytes /
                    (double)(UINT64_C(1) << 20), stats.seconds);
    }
}

static int check_terms(int max_n, uint64_t memory_budget)
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
        compute_checked(value, n, memory_budget, false);
        if (n <= INDEPENDENT_CHECK_MAX_N) {
            DpStats independent_stats;
            memset(&independent_stats, 0, sizeof(independent_stats));
            uint64_t independent = 0U;
            DpStatus status = independent_subset_dp(
                &independent, n, memory_budget, &independent_stats);
            if (status != DP_OK) {
                report_failure(n, status, memory_budget,
                               &independent_stats);
                mpz_clear(value);
                return EXIT_FAILURE;
            }
            if (mpz_cmp_ui(value, (unsigned long)independent) != 0) {
                gmp_fprintf(stderr,
                            "error: type DP and independent subset DP "
                            "disagree at n=%d: %Zd versus %" PRIu64 "\n",
                            n, value, independent);
                mpz_clear(value);
                return EXIT_FAILURE;
            }
        }
    }
    mpz_clear(value);
    printf("ok: type-compressed CRT DP agrees with A354830 reference "
           "terms for n=0..%d; independent expanded subset DP agrees "
           "for n=0..%d\n",
           max_n, max_n < INDEPENDENT_CHECK_MAX_N
                      ? max_n : INDEPENDENT_CHECK_MAX_N);
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

static void produce_b_file(const char *argv0, int max_n,
                           uint64_t memory_budget)
{
    char *path = path_beside_executable(argv0, "b354830_3.txt");
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
            "Every b-file term is computed by 354830_03 itself.\n"
            "The default memory limit is %" PRIu64
            " MiB; override it with\n"
            "A354830_03_MEMORY_MIB.\n",
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
        return check_terms(max_n, memory_budget);
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
