/*
 * A399488 -- independent rook-polynomial implementation.
 *
 * Count permutations p of {1,...,n} such that
 *
 *     lcm(i, p(i)) >= n
 *
 * for every i.  For n >= 2 the placements p(1)=n and p(n)=1 are
 * forced.  On the remaining m=n-2 rows and columns, let F be the board of
 * forbidden positions
 *
 *     F[i,j] = 1  iff  lcm(i,j) < n.
 *
 * If r_k is the number of ways to put k nonattacking rooks on F, then
 * inclusion-exclusion gives
 *
 *     a(n) = sum_{k=0}^m (-1)^k r_k (m-k)!.
 *
 * This is algorithmically independent of 399488_01.c: 01 computes the
 * permanent directly on allowed edges, whereas 02 factors the forbidden
 * board into connected components, computes each component's rook
 * polynomial, multiplies those polynomials, and applies the formula above.
 *
 * Each component DP uses a dense residue array indexed by used-column mask.
 * Reachable masks are discovered once and stored in popcount buckets; the
 * same transition plan is then reused for several fixed 31-bit primes.
 * Their product is required to exceed the rigorous bound (n-2)!, after which
 * CRT reconstructs the unique exact value.  A final bound and residue check
 * is mandatory.
 *
 * Safety policy:
 *   - n is limited to 60 because the reduced board uses uint64_t masks;
 *   - every allocation and size calculation is checked;
 *   - tracked active/transient storage is limited to 2 GiB by default
 *     (override with A399488_02_MEMORY_MIB);
 *   - allocation, memory-limit, and internal-consistency failures stop;
 *   - b399488_02.txt is first written as .part and atomically renamed only
 *     after every requested term succeeds.
 *
 * Build on Apple Silicon with Homebrew GMP:
 *
 *   clang -O3 -march=native -std=c11 -Wall -Wextra -Wpedantic \
 *       -I/opt/homebrew/opt/gmp/include -L/opt/homebrew/opt/gmp/lib \
 *       399488_02.c -lgmp -o 399488_02
 *
 * Examples:
 *
 *   ./399488_02 --check
 *   ./399488_02 --term 32
 *   ./399488_02 34 24
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

#define MAX_SUPPORTED_N 60
#define MAX_REDUCED_N (MAX_SUPPORTED_N - 2)
#define KNOWN_MAX_N 23
#define DEFAULT_MAX_N 30
#define DEFAULT_CHECK_N 23
#define DEFAULT_MEMORY_MIB UINT64_C(2048)
#define MIN_MEMORY_MIB UINT64_C(16)
#define MAX_MEMORY_MIB UINT64_C(65536)
#define MAX_MODULUS_COUNT ((size_t)12)
#define PROGRESS_STATE_STEP UINT64_C(1000000)

static const uint32_t moduli[MAX_MODULUS_COUNT] = {
    UINT32_C(2147483647),
    UINT32_C(2147483629),
    UINT32_C(2147483587),
    UINT32_C(2147483579),
    UINT32_C(2147483563),
    UINT32_C(2147483549),
    UINT32_C(2147483543),
    UINT32_C(2147483497),
    UINT32_C(2147483489),
    UINT32_C(2147483477),
    UINT32_C(2147483423),
    UINT32_C(2147483399)
};

static const char *const known_terms[KNOWN_MAX_N + 1] = {
    "1",
    "1",
    "1",
    "0",
    "1",
    "0",
    "4",
    "0",
    "16",
    "72",
    "852",
    "2416",
    "48816",
    "48096",
    "1446288",
    "11702752",
    "163327732",
    "1923710688",
    "53547611664",
    "272669284768",
    "8810469698192",
    "74007254144256",
    "1578780234656896",
    "25654561507372672"
};

typedef enum {
    COMPUTE_OK = 0,
    COMPUTE_MEMORY_LIMIT,
    COMPUTE_ALLOCATION_FAILURE,
    COMPUTE_SIZE_OVERFLOW,
    COMPUTE_INTERNAL_ERROR
} ComputeStatus;

typedef struct {
    uint64_t budget;
    uint64_t allocated;
    uint64_t peak;
    uint64_t required;
} MemoryTracker;

typedef struct {
    unsigned size;
    bool masks_are_32_bit;
    uint64_t state_capacity;
    void *bucket_masks;
    size_t mask_bytes;
    size_t *bucket_offsets;
    size_t *bucket_counts;
    size_t *row_limits;
    uint64_t *row_adjacency;
    size_t small_array_bytes;
    uint64_t reachable_states;
    uint64_t plan_transitions;
} ComponentPlan;

typedef struct {
    int n;
    unsigned reduced_size;
    unsigned singleton_count;
    ComponentPlan *plans;
    size_t plan_count;
} RookProblem;

typedef struct {
    uint64_t plan_states;
    uint64_t plan_transitions;
    uint64_t modular_transitions;
    uint64_t peak_bytes;
    uint64_t required_bytes;
    size_t modulus_count;
    size_t bound_bits;
    double seconds;
} ComputeStats;

typedef struct {
    uint64_t mask;
    unsigned degree;
    unsigned original_index;
} RowSpec;

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

static int parse_n(const char *text, const char *label)
{
    char *end = NULL;
    long value;

    errno = 0;
    value = strtol(text, &end, 10);
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
    const char *text = getenv("A399488_02_MEMORY_MIB");
    uint64_t mib = DEFAULT_MEMORY_MIB;

    if (text != NULL && *text != '\0') {
        char *end = NULL;
        unsigned long long parsed;

        errno = 0;
        parsed = strtoull(text, &end, 10);
        if (errno != 0 || end == text || *end != '\0' ||
            parsed < MIN_MEMORY_MIB || parsed > MAX_MEMORY_MIB) {
            fprintf(stderr,
                    "error: A399488_02_MEMORY_MIB must be in %" PRIu64
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
                              sizeof(executable) - 1);
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
        memcpy(executable, argv0, length + 1);
    }

    const char *base = realpath(executable, resolved);
    if (base == NULL) {
        base = executable;
    }
    const char *slash = strrchr(base, '/');
    size_t directory_length =
        slash == NULL ? 1 : (size_t)(slash - base);
    size_t filename_length = strlen(filename);
    char *path = malloc(directory_length + 1 + filename_length + 1);
    if (path == NULL) {
        die("could not allocate the output path");
    }

    if (slash == NULL) {
        path[0] = '.';
    } else if (directory_length == 0) {
        path[0] = '/';
        directory_length = 1;
    } else {
        memcpy(path, base, directory_length);
    }
    path[directory_length] = '/';
    memcpy(path + directory_length + 1, filename,
           filename_length + 1);
    return path;
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

static void *tracked_allocate(MemoryTracker *memory, size_t bytes,
                              bool clear)
{
    uint64_t needed;
    if (!checked_add_u64(memory->allocated, (uint64_t)bytes, &needed)) {
        memory->required = UINT64_MAX;
        return NULL;
    }
    if (needed > memory->budget) {
        if (needed > memory->required) {
            memory->required = needed;
        }
        return NULL;
    }

    void *pointer = clear ? calloc(1, bytes) : malloc(bytes);
    if (pointer == NULL) {
        if (needed > memory->required) {
            memory->required = needed;
        }
        return NULL;
    }
    memory->allocated = needed;
    if (needed > memory->peak) {
        memory->peak = needed;
    }
    return pointer;
}

static void tracked_free(MemoryTracker *memory, void *pointer,
                         size_t bytes)
{
    if (pointer == NULL) {
        return;
    }
    free(pointer);
    if ((uint64_t)bytes > memory->allocated) {
        die("internal memory-accounting error");
    }
    memory->allocated -= (uint64_t)bytes;
}

static uint64_t saturating_add(uint64_t left, uint64_t right)
{
    return UINT64_MAX - left < right ? UINT64_MAX : left + right;
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

static bool allowed_pair(unsigned left, unsigned right, unsigned n)
{
    unsigned divisor = gcd_positive(left, right);
    uint64_t lcm = ((uint64_t)left / divisor) * (uint64_t)right;
    return lcm >= (uint64_t)n;
}

static bool forbidden_pair(unsigned left, unsigned right, unsigned n)
{
    return !allowed_pair(left, right, n);
}

static uint64_t low_bit(uint64_t value)
{
    return value & (UINT64_C(0) - value);
}

static unsigned bit_count(uint64_t value)
{
    return (unsigned)__builtin_popcountll(value);
}

static unsigned bit_index(uint64_t bit)
{
    return (unsigned)__builtin_ctzll(bit);
}

static int compare_rows(const void *left, const void *right)
{
    const RowSpec *a = left;
    const RowSpec *b = right;
    if (a->degree != b->degree) {
        return a->degree < b->degree ? -1 : 1;
    }
    if (a->mask != b->mask) {
        return a->mask > b->mask ? -1 : 1;
    }
    if (a->original_index != b->original_index) {
        return a->original_index < b->original_index ? -1 : 1;
    }
    return 0;
}

static uint64_t plan_mask_at(const ComponentPlan *plan, size_t index)
{
    if (plan->masks_are_32_bit) {
        return ((const uint32_t *)plan->bucket_masks)[index];
    }
    return ((const uint64_t *)plan->bucket_masks)[index];
}

static void plan_set_mask(ComponentPlan *plan, size_t index,
                          uint64_t mask)
{
    if (plan->masks_are_32_bit) {
        ((uint32_t *)plan->bucket_masks)[index] = (uint32_t)mask;
    } else {
        ((uint64_t *)plan->bucket_masks)[index] = mask;
    }
}

static bool seen_test_and_set(uint8_t *seen, uint64_t state)
{
    size_t byte = (size_t)(state >> 3);
    uint8_t bit = (uint8_t)(UINT8_C(1) << (state & 7U));
    bool was_set = (seen[byte] & bit) != 0;
    seen[byte] |= bit;
    return was_set;
}

static void component_plan_destroy(ComponentPlan *plan,
                                   MemoryTracker *memory)
{
    tracked_free(memory, plan->row_adjacency,
                 plan->small_array_bytes == 0 ? 0 :
                     plan->size * sizeof(*plan->row_adjacency));
    tracked_free(memory, plan->row_limits,
                 plan->small_array_bytes == 0 ? 0 :
                     (size_t)plan->size * (plan->size + 1U) *
                         sizeof(*plan->row_limits));
    tracked_free(memory, plan->bucket_counts,
                 plan->small_array_bytes == 0 ? 0 :
                     (plan->size + 1U) * sizeof(*plan->bucket_counts));
    tracked_free(memory, plan->bucket_offsets,
                 plan->small_array_bytes == 0 ? 0 :
                     (plan->size + 1U) * sizeof(*plan->bucket_offsets));
    tracked_free(memory, plan->bucket_masks,
                 plan->mask_bytes);
    memset(plan, 0, sizeof(*plan));
}

static ComputeStatus build_component_plan(
    ComponentPlan *plan, const uint64_t *local_adjacency,
    unsigned size, MemoryTracker *memory, bool report,
    ComputeStats *stats)
{
    memset(plan, 0, sizeof(*plan));
    if (size < 2 || size > MAX_REDUCED_N) {
        return COMPUTE_INTERNAL_ERROR;
    }

    plan->size = size;
    plan->masks_are_32_bit = size <= 32U;
    plan->state_capacity = UINT64_C(1) << size;
    if (plan->state_capacity > (uint64_t)SIZE_MAX) {
        return COMPUTE_SIZE_OVERFLOW;
    }

    size_t state_capacity = (size_t)plan->state_capacity;
    size_t mask_width = plan->masks_are_32_bit
                            ? sizeof(uint32_t) : sizeof(uint64_t);
    if (!checked_mul_size(state_capacity, mask_width,
                          &plan->mask_bytes)) {
        return COMPUTE_SIZE_OVERFLOW;
    }

    size_t vector_bytes;
    size_t limit_count;
    size_t limit_bytes;
    size_t adjacency_bytes;
    if (!checked_mul_size((size_t)size + 1U, sizeof(size_t),
                          &vector_bytes) ||
        !checked_mul_size((size_t)size, (size_t)size + 1U,
                          &limit_count) ||
        !checked_mul_size(limit_count, sizeof(size_t), &limit_bytes) ||
        !checked_mul_size((size_t)size, sizeof(uint64_t),
                          &adjacency_bytes)) {
        return COMPUTE_SIZE_OVERFLOW;
    }
    uint64_t small_total =
        (uint64_t)vector_bytes * 2U + limit_bytes + adjacency_bytes;
    if (small_total > SIZE_MAX) {
        return COMPUTE_SIZE_OVERFLOW;
    }
    plan->small_array_bytes = (size_t)small_total;

    plan->bucket_masks = tracked_allocate(memory, plan->mask_bytes, false);
    plan->bucket_offsets = tracked_allocate(memory, vector_bytes, true);
    plan->bucket_counts = tracked_allocate(memory, vector_bytes, true);
    plan->row_limits = tracked_allocate(memory, limit_bytes, true);
    plan->row_adjacency =
        tracked_allocate(memory, adjacency_bytes, false);
    if (plan->bucket_masks == NULL || plan->bucket_offsets == NULL ||
        plan->bucket_counts == NULL || plan->row_limits == NULL ||
        plan->row_adjacency == NULL) {
        component_plan_destroy(plan, memory);
        return memory->required > memory->budget
                   ? COMPUTE_MEMORY_LIMIT
                   : COMPUTE_ALLOCATION_FAILURE;
    }

    size_t choose = 1;
    size_t offset = 0;
    for (unsigned k = 0; k <= size; ++k) {
        plan->bucket_offsets[k] = offset;
        if (choose > state_capacity - offset) {
            component_plan_destroy(plan, memory);
            return COMPUTE_INTERNAL_ERROR;
        }
        offset += choose;
        if (k < size) {
            uint64_t numerator = (uint64_t)choose * (size - k);
            choose = (size_t)(numerator / (k + 1U));
        }
    }
    if (offset != state_capacity) {
        component_plan_destroy(plan, memory);
        return COMPUTE_INTERNAL_ERROR;
    }

    RowSpec rows[MAX_REDUCED_N];
    for (unsigned row = 0; row < size; ++row) {
        rows[row].mask = local_adjacency[row];
        rows[row].degree = bit_count(local_adjacency[row]);
        rows[row].original_index = row;
    }
    qsort(rows, size, sizeof(*rows), compare_rows);
    for (unsigned row = 0; row < size; ++row) {
        plan->row_adjacency[row] = rows[row].mask;
    }

    size_t seen_bytes = (state_capacity + 7U) / 8U;
    uint8_t *seen = tracked_allocate(memory, seen_bytes, true);
    if (seen == NULL) {
        component_plan_destroy(plan, memory);
        return memory->required > memory->budget
                   ? COMPUTE_MEMORY_LIMIT
                   : COMPUTE_ALLOCATION_FAILURE;
    }

    (void)seen_test_and_set(seen, 0);
    plan_set_mask(plan, plan->bucket_offsets[0], 0);
    plan->bucket_counts[0] = 1;

    uint64_t next_report = PROGRESS_STATE_STEP;
    uint64_t full_mask = plan->state_capacity - 1U;
    for (unsigned row = 0; row < size; ++row) {
        for (unsigned k = 0; k <= size; ++k) {
            plan->row_limits[(size_t)row * (size + 1U) + k] =
                plan->bucket_counts[k];
        }

        int maximum_k = row < size ? (int)row : (int)size - 1;
        for (int k = maximum_k; k >= 0; --k) {
            size_t source_count = plan->bucket_counts[k];
            size_t source_offset = plan->bucket_offsets[k];
            for (size_t entry = 0; entry < source_count; ++entry) {
                uint64_t mask = plan_mask_at(plan,
                                             source_offset + entry);
                uint64_t choices =
                    plan->row_adjacency[row] & ~mask & full_mask;
                while (choices != 0) {
                    uint64_t bit = low_bit(choices);
                    choices ^= bit;
                    plan->plan_transitions = saturating_add(
                        plan->plan_transitions, 1);
                    uint64_t destination = mask | bit;
                    if (!seen_test_and_set(seen, destination)) {
                        size_t bucket = (size_t)k + 1U;
                        size_t index = plan->bucket_counts[bucket]++;
                        size_t capacity =
                            (bucket == size)
                                ? state_capacity -
                                      plan->bucket_offsets[bucket]
                                : plan->bucket_offsets[bucket + 1U] -
                                      plan->bucket_offsets[bucket];
                        if (index >= capacity) {
                            tracked_free(memory, seen, seen_bytes);
                            component_plan_destroy(plan, memory);
                            return COMPUTE_INTERNAL_ERROR;
                        }
                        plan_set_mask(
                            plan,
                            plan->bucket_offsets[bucket] + index,
                            destination);
                        ++plan->reachable_states;
                    }
                }
            }
        }

        uint64_t reached = 0;
        for (unsigned k = 0; k <= size; ++k) {
            reached = saturating_add(reached,
                                     plan->bucket_counts[k]);
        }
        if (report &&
            (row + 1U == size || reached >= next_report)) {
            fprintf(stderr,
                    "399488_02: plan c=%u row=%u/%u, states=%" PRIu64
                    ", memory=%.2f GiB\n",
                    size, row + 1U, size, reached,
                    (double)memory->allocated /
                        (double)(UINT64_C(1) << 30));
            while (next_report <= reached &&
                   next_report <= UINT64_MAX - PROGRESS_STATE_STEP) {
                next_report += PROGRESS_STATE_STEP;
            }
        }
    }

    plan->reachable_states = 0;
    for (unsigned k = 0; k <= size; ++k) {
        plan->reachable_states = saturating_add(
            plan->reachable_states,
            (uint64_t)plan->bucket_counts[k]);
    }
    stats->plan_states = saturating_add(stats->plan_states,
                                        plan->reachable_states);
    stats->plan_transitions = saturating_add(
        stats->plan_transitions, plan->plan_transitions);
    tracked_free(memory, seen, seen_bytes);
    return COMPUTE_OK;
}

static void rook_problem_destroy(RookProblem *problem,
                                 MemoryTracker *memory)
{
    if (problem->plans != NULL) {
        for (size_t index = 0; index < problem->plan_count; ++index) {
            component_plan_destroy(&problem->plans[index], memory);
        }
        size_t plan_bytes =
            (size_t)MAX_REDUCED_N * sizeof(*problem->plans);
        tracked_free(memory, problem->plans, plan_bytes);
    }
    memset(problem, 0, sizeof(*problem));
}

static ComputeStatus build_rook_problem(RookProblem *problem, int n,
                                        MemoryTracker *memory,
                                        bool report,
                                        ComputeStats *stats)
{
    memset(problem, 0, sizeof(*problem));
    problem->n = n;
    if (n <= 1) {
        return COMPUTE_OK;
    }
    problem->reduced_size = (unsigned)(n - 2);
    unsigned m = problem->reduced_size;
    if (m == 0) {
        return COMPUTE_OK;
    }

    uint64_t adjacency[MAX_REDUCED_N] = { 0 };
    for (unsigned row = 0; row < m; ++row) {
        for (unsigned column = 0; column < m; ++column) {
            if (forbidden_pair(row + 2U, column + 2U,
                               (unsigned)n)) {
                adjacency[row] |= UINT64_C(1) << column;
            }
        }
    }

    size_t plan_bytes =
        (size_t)MAX_REDUCED_N * sizeof(*problem->plans);
    problem->plans = tracked_allocate(memory, plan_bytes, true);
    if (problem->plans == NULL) {
        return memory->required > memory->budget
                   ? COMPUTE_MEMORY_LIMIT
                   : COMPUTE_ALLOCATION_FAILURE;
    }

    uint64_t unseen = (UINT64_C(1) << m) - 1U;
    while (unseen != 0) {
        uint64_t component = low_bit(unseen);
        uint64_t frontier = component;
        unseen ^= component;
        while (frontier != 0) {
            uint64_t neighbors = 0;
            uint64_t scan = frontier;
            while (scan != 0) {
                uint64_t bit = low_bit(scan);
                scan ^= bit;
                neighbors |= adjacency[bit_index(bit)];
            }
            uint64_t added = neighbors & unseen;
            if (added == 0) {
                break;
            }
            component |= added;
            unseen ^= added;
            frontier = added;
        }

        unsigned component_size = bit_count(component);
        if (component_size == 1U) {
            ++problem->singleton_count;
            continue;
        }

        unsigned global_index[MAX_REDUCED_N];
        unsigned count = 0;
        uint64_t scan = component;
        while (scan != 0) {
            uint64_t bit = low_bit(scan);
            scan ^= bit;
            global_index[count++] = bit_index(bit);
        }
        if (count != component_size ||
            problem->plan_count >= MAX_REDUCED_N) {
            rook_problem_destroy(problem, memory);
            return COMPUTE_INTERNAL_ERROR;
        }

        uint64_t local_adjacency[MAX_REDUCED_N] = { 0 };
        for (unsigned row = 0; row < component_size; ++row) {
            for (unsigned column = 0; column < component_size;
                 ++column) {
                if ((adjacency[global_index[row]] &
                     (UINT64_C(1) << global_index[column])) != 0) {
                    local_adjacency[row] |=
                        UINT64_C(1) << column;
                }
            }
        }

        if (report) {
            fprintf(stderr,
                    "399488_02: n=%d building forbidden component "
                    "%zu, size=%u\n",
                    n, problem->plan_count + 1U, component_size);
        }
        ComputeStatus status = build_component_plan(
            &problem->plans[problem->plan_count], local_adjacency,
            component_size, memory, report, stats);
        if (status != COMPUTE_OK) {
            rook_problem_destroy(problem, memory);
            return status;
        }
        ++problem->plan_count;
    }

    if (report) {
        fprintf(stderr,
                "399488_02: n=%d forbidden graph: %zu nontrivial "
                "component%s, %u singleton%s, plan memory=%.2f GiB\n",
                n, problem->plan_count,
                problem->plan_count == 1 ? "" : "s",
                problem->singleton_count,
                problem->singleton_count == 1 ? "" : "s",
                (double)memory->allocated /
                    (double)(UINT64_C(1) << 30));
    }
    return COMPUTE_OK;
}

static uint32_t add_mod(uint32_t left, uint32_t right,
                        uint32_t modulus)
{
    uint64_t sum = (uint64_t)left + right;
    if (sum >= modulus) {
        sum -= modulus;
    }
    return (uint32_t)sum;
}

static uint32_t subtract_mod(uint32_t left, uint32_t right,
                             uint32_t modulus)
{
    return left >= right ? left - right
                         : (uint32_t)((uint64_t)left + modulus - right);
}

static uint32_t multiply_mod(uint32_t left, uint32_t right,
                             uint32_t modulus)
{
    return (uint32_t)(((uint64_t)left * right) % modulus);
}

static ComputeStatus evaluate_component(
    const ComponentPlan *plan, uint32_t modulus,
    uint32_t *polynomial, MemoryTracker *memory, bool report,
    int n, size_t pass, size_t pass_count, ComputeStats *stats)
{
    size_t state_capacity = (size_t)plan->state_capacity;
    size_t value_bytes;
    if (!checked_mul_size(state_capacity, sizeof(uint32_t),
                          &value_bytes)) {
        return COMPUTE_SIZE_OVERFLOW;
    }
    uint32_t *values = tracked_allocate(memory, value_bytes, true);
    if (values == NULL) {
        return memory->required > memory->budget
                   ? COMPUTE_MEMORY_LIMIT
                   : COMPUTE_ALLOCATION_FAILURE;
    }
    values[0] = 1;
    uint64_t full_mask = plan->state_capacity - 1U;

    for (unsigned row = 0; row < plan->size; ++row) {
        int maximum_k = row < plan->size
                            ? (int)row : (int)plan->size - 1;
        for (int k = maximum_k; k >= 0; --k) {
            size_t source_count =
                plan->row_limits[(size_t)row *
                                     (plan->size + 1U) +
                                 (size_t)k];
            size_t source_offset = plan->bucket_offsets[k];
            for (size_t entry = 0; entry < source_count; ++entry) {
                uint64_t mask = plan_mask_at(
                    plan, source_offset + entry);
                uint32_t source = values[(size_t)mask];
                if (source == 0) {
                    continue;
                }
                uint64_t choices =
                    plan->row_adjacency[row] & ~mask & full_mask;
                while (choices != 0) {
                    uint64_t bit = low_bit(choices);
                    choices ^= bit;
                    size_t destination = (size_t)(mask | bit);
                    values[destination] = add_mod(
                        values[destination], source, modulus);
                    stats->modular_transitions = saturating_add(
                        stats->modular_transitions, 1);
                }
            }
        }
        if (report && (row + 1U == plan->size ||
                       row == 0 || (row + 1U) % 4U == 0)) {
            fprintf(stderr,
                    "399488_02: n=%d pass=%zu/%zu c=%u row=%u/%u, "
                    "memory=%.2f GiB\n",
                    n, pass, pass_count, plan->size, row + 1U,
                    plan->size,
                    (double)memory->allocated /
                        (double)(UINT64_C(1) << 30));
        }
    }

    for (unsigned k = 0; k <= plan->size; ++k) {
        uint32_t sum = 0;
        size_t offset = plan->bucket_offsets[k];
        size_t count = plan->bucket_counts[k];
        for (size_t entry = 0; entry < count; ++entry) {
            uint64_t mask = plan_mask_at(plan, offset + entry);
            sum = add_mod(sum, values[(size_t)mask], modulus);
        }
        polynomial[k] = sum;
    }

    tracked_free(memory, values, value_bytes);
    return COMPUTE_OK;
}

static ComputeStatus answer_modulo(uint32_t *answer,
                                   const RookProblem *problem,
                                   uint32_t modulus,
                                   MemoryTracker *memory,
                                   bool report, size_t pass,
                                   size_t pass_count,
                                   ComputeStats *stats)
{
    if (problem->n <= 1) {
        *answer = 1;
        return COMPUTE_OK;
    }

    unsigned m = problem->reduced_size;
    uint32_t rook[MAX_REDUCED_N + 1] = { 0 };
    uint32_t next[MAX_REDUCED_N + 1] = { 0 };
    uint32_t component[MAX_REDUCED_N + 1] = { 0 };
    rook[0] = 1;
    unsigned degree = 0;

    for (unsigned singleton = 0;
         singleton < problem->singleton_count; ++singleton) {
        for (int k = (int)degree; k >= 0; --k) {
            rook[k + 1] = add_mod(rook[k + 1], rook[k], modulus);
        }
        ++degree;
    }

    for (size_t index = 0; index < problem->plan_count; ++index) {
        const ComponentPlan *plan = &problem->plans[index];
        memset(component, 0, sizeof(component));
        ComputeStatus status = evaluate_component(
            plan, modulus, component, memory, report, problem->n,
            pass, pass_count, stats);
        if (status != COMPUTE_OK) {
            return status;
        }

        memset(next, 0, sizeof(next));
        for (unsigned left = 0; left <= degree; ++left) {
            for (unsigned right = 0; right <= plan->size; ++right) {
                uint32_t product = multiply_mod(
                    rook[left], component[right], modulus);
                next[left + right] = add_mod(
                    next[left + right], product, modulus);
            }
        }
        degree += plan->size;
        memcpy(rook, next, sizeof(rook));
    }
    if (degree != m) {
        return COMPUTE_INTERNAL_ERROR;
    }

    uint32_t factorial[MAX_REDUCED_N + 1] = { 0 };
    factorial[0] = 1;
    for (unsigned k = 1; k <= m; ++k) {
        factorial[k] = multiply_mod(factorial[k - 1U], k, modulus);
    }

    uint32_t result = 0;
    for (unsigned k = 0; k <= m; ++k) {
        uint32_t term = multiply_mod(rook[k], factorial[m - k],
                                     modulus);
        result = (k & 1U) == 0
                     ? add_mod(result, term, modulus)
                     : subtract_mod(result, term, modulus);
    }
    *answer = result;
    return COMPUTE_OK;
}

static size_t choose_moduli(const mpz_t bound, mpz_t product)
{
    mpz_set_ui(product, 1);
    for (size_t count = 1; count <= MAX_MODULUS_COUNT; ++count) {
        mpz_mul_ui(product, product,
                   (unsigned long)moduli[count - 1U]);
        if (mpz_cmp(product, bound) > 0) {
            return count;
        }
    }
    return 0;
}

static uint32_t power_mod(uint32_t base, uint32_t exponent,
                          uint32_t modulus)
{
    uint64_t result = 1;
    uint64_t factor = base;
    while (exponent != 0) {
        if ((exponent & 1U) != 0) {
            result = (result * factor) % modulus;
        }
        factor = (factor * factor) % modulus;
        exponent >>= 1;
    }
    return (uint32_t)result;
}

static bool reconstruct_crt(mpz_t result, mpz_t product,
                            const uint32_t *residues,
                            size_t modulus_count)
{
    mpz_set_ui(result, 0);
    mpz_set_ui(product, 1);
    for (size_t index = 0; index < modulus_count; ++index) {
        uint32_t modulus = moduli[index];
        uint32_t product_mod =
            (uint32_t)mpz_fdiv_ui(product, (unsigned long)modulus);
        if (product_mod == 0) {
            return false;
        }
        uint32_t inverse =
            power_mod(product_mod, modulus - 2U, modulus);
        uint32_t result_mod =
            (uint32_t)mpz_fdiv_ui(result, (unsigned long)modulus);
        uint32_t difference = subtract_mod(
            residues[index], result_mod, modulus);
        unsigned long multiplier = (unsigned long)multiply_mod(
            difference, inverse, modulus);
        mpz_addmul_ui(result, product, multiplier);
        mpz_mul_ui(product, product, (unsigned long)modulus);
    }

    for (size_t index = 0; index < modulus_count; ++index) {
        if (mpz_fdiv_ui(result, (unsigned long)moduli[index]) !=
            residues[index]) {
            return false;
        }
    }
    return true;
}

static const char *status_text(ComputeStatus status)
{
    switch (status) {
    case COMPUTE_OK:
        return "ok";
    case COMPUTE_MEMORY_LIMIT:
        return "memory limit exceeded";
    case COMPUTE_ALLOCATION_FAILURE:
        return "allocation failed";
    case COMPUTE_SIZE_OVERFLOW:
        return "allocation-size calculation overflowed";
    case COMPUTE_INTERNAL_ERROR:
        return "internal consistency check failed";
    }
    return "unknown error";
}

static ComputeStatus compute_exact(mpz_t result, int n,
                                   uint64_t memory_budget,
                                   bool report, ComputeStats *stats)
{
    memset(stats, 0, sizeof(*stats));
    double start = monotonic_seconds();

    mpz_t bound;
    mpz_t selected_product;
    mpz_t reconstructed_product;
    mpz_inits(bound, selected_product, reconstructed_product, NULL);
    mpz_fac_ui(bound, (unsigned long)(n >= 2 ? n - 2 : 0));
    stats->bound_bits = mpz_sizeinbase(bound, 2);
    stats->modulus_count = choose_moduli(bound, selected_product);
    if (stats->modulus_count == 0) {
        mpz_clears(reconstructed_product, selected_product, bound,
                   NULL);
        return COMPUTE_INTERNAL_ERROR;
    }

    MemoryTracker memory = {
        .budget = memory_budget,
        .allocated = 0,
        .peak = 0,
        .required = 0
    };
    RookProblem problem;
    ComputeStatus status = build_rook_problem(
        &problem, n, &memory, report, stats);
    if (status != COMPUTE_OK) {
        stats->peak_bytes = memory.peak;
        stats->required_bytes = memory.required;
        stats->seconds = monotonic_seconds() - start;
        mpz_clears(reconstructed_product, selected_product, bound,
                   NULL);
        return status;
    }

    if (report) {
        fprintf(stderr,
                "399488_02: n=%d rook inclusion-exclusion, "
                "bound=%zu bits, %zu prime passes, memory limit=%.2f "
                "GiB\n",
                n, stats->bound_bits, stats->modulus_count,
                (double)memory_budget /
                    (double)(UINT64_C(1) << 30));
    }

    uint32_t residues[MAX_MODULUS_COUNT] = { 0 };
    for (size_t index = 0; index < stats->modulus_count; ++index) {
        if (report) {
            fprintf(stderr,
                    "399488_02: n=%d pass %zu/%zu, p=%" PRIu32
                    "\n",
                    n, index + 1U, stats->modulus_count,
                    moduli[index]);
        }
        status = answer_modulo(
            &residues[index], &problem, moduli[index], &memory,
            report && index == 0, index + 1U,
            stats->modulus_count, stats);
        if (status != COMPUTE_OK) {
            break;
        }
        if (report) {
            fprintf(stderr,
                    "399488_02: n=%d pass %zu/%zu ok, residue=%" PRIu32
                    ", peak=%.2f GiB\n",
                    n, index + 1U, stats->modulus_count,
                    residues[index],
                    (double)memory.peak /
                        (double)(UINT64_C(1) << 30));
        }
    }

    if (status == COMPUTE_OK) {
        bool valid = reconstruct_crt(
                         result, reconstructed_product, residues,
                         stats->modulus_count) &&
                     mpz_cmp(reconstructed_product, bound) > 0 &&
                     mpz_sgn(result) >= 0 &&
                     mpz_cmp(result, bound) <= 0;
        status = valid ? COMPUTE_OK : COMPUTE_INTERNAL_ERROR;
    }

    rook_problem_destroy(&problem, &memory);
    if (memory.allocated != 0 && status == COMPUTE_OK) {
        status = COMPUTE_INTERNAL_ERROR;
    }
    stats->peak_bytes = memory.peak;
    stats->required_bytes = memory.required;
    stats->seconds = monotonic_seconds() - start;
    mpz_clears(reconstructed_product, selected_product, bound, NULL);
    return status;
}

static void report_failure(int n, ComputeStatus status,
                           uint64_t memory_budget,
                           const ComputeStats *stats)
{
    fprintf(stderr, "error: A399488 n=%d: %s\n",
            n, status_text(status));
    if (status == COMPUTE_MEMORY_LIMIT ||
        status == COMPUTE_ALLOCATION_FAILURE) {
        if (stats->required_bytes != 0 &&
            stats->required_bytes != UINT64_MAX) {
            fprintf(stderr,
                    "required at failed allocation: %.2f GiB; "
                    "configured limit: %.2f GiB\n",
                    (double)stats->required_bytes /
                        (double)(UINT64_C(1) << 30),
                    (double)memory_budget /
                        (double)(UINT64_C(1) << 30));
        }
        fprintf(stderr,
                "Increase A399488_02_MEMORY_MIB only when enough "
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
        die("invalid built-in A399488 term");
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

static uint64_t brute_force_rec(unsigned row, unsigned n,
                                uint64_t used_columns)
{
    if (row > n) {
        return 1;
    }
    uint64_t result = 0;
    for (unsigned column = 1; column <= n; ++column) {
        uint64_t bit = UINT64_C(1) << (column - 1U);
        if ((used_columns & bit) == 0 &&
            allowed_pair(row, column, n)) {
            result += brute_force_rec(row + 1U, n,
                                      used_columns | bit);
        }
    }
    return result;
}

static void compute_checked(mpz_t value, int n,
                            uint64_t memory_budget, bool report)
{
    ComputeStats stats;
    ComputeStatus status = compute_exact(
        value, n, memory_budget, report, &stats);
    if (status != COMPUTE_OK) {
        report_failure(n, status, memory_budget, &stats);
        exit(EXIT_FAILURE);
    }
    verify_known_value(value, n);
    if (report) {
        fprintf(stderr,
                "399488_02: n=%d done, plan states=%" PRIu64
                ", plan transitions=%" PRIu64
                ", modular transitions=%" PRIu64
                ", peak allocation=%.2f GiB, %.3f s\n",
                n, stats.plan_states, stats.plan_transitions,
                stats.modular_transitions,
                (double)stats.peak_bytes /
                    (double)(UINT64_C(1) << 30),
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
        if (n <= 10) {
            uint64_t brute = brute_force_rec(1, (unsigned)n, 0);
            if (mpz_cmp_ui(value, (unsigned long)brute) != 0) {
                gmp_fprintf(stderr,
                            "error: rook and brute-force methods "
                            "disagree at n=%d: %Zd versus %" PRIu64
                            "\n",
                            n, value, brute);
                mpz_clear(value);
                return EXIT_FAILURE;
            }
        }
    }
    mpz_clear(value);
    printf("ok: rook inclusion-exclusion agrees with the known "
           "A399488 prefix for n=0..%d; brute force independently "
           "agrees for n=0..%d\n",
           max_n, max_n < 10 ? max_n : 10);
    return EXIT_SUCCESS;
}

static void write_known(FILE *stream, int n)
{
    if (n < 0 || n > KNOWN_MAX_N) {
        die("requested built-in term is unavailable");
    }
    if (fprintf(stream, "%d %s\n", n, known_terms[n]) < 0) {
        die("could not write a built-in b-file term");
    }
}

static void produce_b_file(const char *argv0, int max_n, int start_n,
                           uint64_t memory_budget)
{
    if (start_n > max_n + 1 || start_n > KNOWN_MAX_N + 1) {
        fprintf(stderr,
                "error: START_N must be at most min(MAX_N+1,%d)\n",
                KNOWN_MAX_N + 1);
        exit(EXIT_FAILURE);
    }

    char *path = path_beside_executable(argv0, "b399488_02.txt");
    size_t part_size = strlen(path) + strlen(".part") + 1U;
    char *part = malloc(part_size);
    if (part == NULL) {
        free(path);
        die("could not allocate the temporary output path");
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

    for (int n = 0; n < start_n && n <= max_n; ++n) {
        write_known(stream, n);
    }
    if (start_n > 0) {
        fprintf(stderr,
                "399488_02: using built-in verified prefix n=0..%d\n",
                start_n - 1);
    }

    mpz_t value;
    mpz_init(value);
    for (int n = start_n; n <= max_n; ++n) {
        compute_checked(value, n, memory_budget, true);
        if (gmp_fprintf(stream, "%d %Zd\n", n, value) < 0 ||
            fflush(stream) != 0) {
            mpz_clear(value);
            fclose(stream);
            free(part);
            free(path);
            die("could not write the A399488 b-file");
        }
    }
    mpz_clear(value);

    if (fclose(stream) != 0) {
        free(part);
        free(path);
        die("could not close the A399488 b-file");
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
            "usage: %s [MAX_N [START_N]]\n"
            "       %s --term N\n"
            "       %s --check [MAX_N]\n"
            "\n"
            "MAX_N defaults to %d and may be at most %d.\n"
            "START_N defaults to 0. Terms below START_N are read from\n"
            "the built-in verified prefix, which ends at n=%d.\n"
            "The default memory limit is %" PRIu64
            " MiB; override it with\n"
            "A399488_02_MEMORY_MIB.\n",
            program, program, program, DEFAULT_MAX_N,
            MAX_SUPPORTED_N, KNOWN_MAX_N, DEFAULT_MEMORY_MIB);
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

    if (argc > 3) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    int max_n = argc >= 2 ? parse_n(argv[1], "MAX_N")
                          : DEFAULT_MAX_N;
    int start_n = argc == 3 ? parse_n(argv[2], "START_N") : 0;
    produce_b_file(argv[0], max_n, start_n, memory_budget);
    return EXIT_SUCCESS;
}
