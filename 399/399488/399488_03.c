/*
 * A399488 -- tree-decomposition matching implementation (n <= 60).
 *
 * Count permutations p of {1,...,n} satisfying
 *
 *     lcm(i, p(i)) >= n
 *
 * for every i.  For n >= 2, p(1)=n and p(n)=1 are forced.  On the
 * remaining m=n-2 rows and columns, form the bipartite graph G whose edges
 * are the forbidden positions lcm(i,j)<n.
 *
 * This program differs computationally from both earlier implementations:
 *
 *   399488_01.c  sparse subset DP on allowed column masks;
 *   399488_02.c  dense component rook DP on every reachable column mask;
 *   399488_03.c  min-fill tree decomposition of the forbidden bipartite
 *                graph, followed by a small-boundary matching DP.
 *
 * The matching polynomial of G is its rook polynomial
 *
 *     R(x) = sum r_k x^k.
 *
 * Inclusion-exclusion is applied only after the tree-decomposition DP:
 *
 *     a(n) = sum_{k=0}^m (-1)^k r_k (m-k)!.
 *
 * Each DP state records which vertices in a separator are already matched.
 * The deterministic min-fill order has width at most 15 for n <= 60 in this
 * problem (the program still verifies a hard width limit at runtime), so the
 * boundary tables remain far smaller than 2^m arrays.
 * Up to nine fixed 31-bit prime residues are propagated simultaneously.  CRT
 * uses only as many primes as needed to exceed the rigorous bound (n-2)!,
 * and the reconstructed result is checked against the bound and every
 * residue.
 *
 * Safety policy:
 *   - the explicit supported range is n=0..60;
 *   - all allocation sizes and tracked allocations are checked;
 *   - the default memory limit is 1 GiB and can be changed with
 *     A399488_03_MEMORY_MIB;
 *   - unexpected decomposition width, invalid bags, allocation failure,
 *     and CRT inconsistency stop with an error;
 *   - b399488_03.txt is written through a .part file and atomically renamed
 *     only after all requested terms succeed.
 *
 * Build on Apple Silicon with Homebrew GMP:
 *
 *   clang -O3 -march=native -std=c11 -Wall -Wextra -Wpedantic \
 *       -I/opt/homebrew/opt/gmp/include -L/opt/homebrew/opt/gmp/lib \
 *       399488_03.c -lgmp -o 399488_03
 *
 * Examples:
 *
 *   ./399488_03 --check
 *   ./399488_03 --term 60
 *   ./399488_03 40 24
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
#define MAX_VERTICES (2 * MAX_REDUCED_N)
#define MAX_RESIDUES 9U
#define MAX_ALLOWED_WIDTH 20U
#define KNOWN_MAX_N 23
#define DEFAULT_MAX_N 35
#define DEFAULT_CHECK_N 23
#define DEFAULT_MEMORY_MIB UINT64_C(1024)
#define MIN_MEMORY_MIB UINT64_C(16)
#define MAX_MEMORY_MIB UINT64_C(65536)

_Static_assert(MAX_VERTICES <= 128,
               "VertexSet must be widened when MAX_VERTICES exceeds 128");

static const uint32_t moduli[MAX_RESIDUES] = {
    UINT32_C(2147483647),
    UINT32_C(2147483629),
    UINT32_C(2147483587),
    UINT32_C(2147483579),
    UINT32_C(2147483563),
    UINT32_C(2147483549),
    UINT32_C(2147483543),
    UINT32_C(2147483497),
    UINT32_C(2147483489)
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
    COMPUTE_WIDTH_LIMIT,
    COMPUTE_INTERNAL_ERROR
} ComputeStatus;

typedef struct {
    uint64_t low;
    uint64_t high;
} VertexSet;

typedef struct {
    uint64_t budget;
    uint64_t allocated;
    uint64_t peak;
    uint64_t required;
} MemoryTracker;

typedef struct {
    unsigned variable_count;
    unsigned degree;
    size_t state_count;
    size_t coefficient_count;
    size_t bytes;
    uint32_t *coefficients;
} PolyTable;

typedef struct {
    unsigned vertex;
    VertexSet separator;
    VertexSet local_neighbors;
    int parent;
    int first_child;
    int next_sibling;
    unsigned separator_size;
    PolyTable message;
} DecompositionNode;

typedef struct {
    uint64_t table_combinations;
    uint64_t coefficient_products;
    uint64_t peak_bytes;
    uint64_t required_bytes;
    unsigned elimination_width;
    size_t modulus_count;
    size_t bound_bits;
    double seconds;
} ComputeStats;

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
    const char *text = getenv("A399488_03_MEMORY_MIB");
    uint64_t mib = DEFAULT_MEMORY_MIB;

    if (text != NULL && *text != '\0') {
        char *end = NULL;
        unsigned long long parsed;

        errno = 0;
        parsed = strtoull(text, &end, 10);
        if (errno != 0 || end == text || *end != '\0' ||
            parsed < MIN_MEMORY_MIB || parsed > MAX_MEMORY_MIB) {
            fprintf(stderr,
                    "error: A399488_03_MEMORY_MIB must be in %" PRIu64
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

static VertexSet vertex_set_empty(void)
{
    VertexSet result = { 0, 0 };
    return result;
}

static VertexSet vertex_set_single(unsigned vertex)
{
    VertexSet result = vertex_set_empty();
    if (vertex < 64U) {
        result.low = UINT64_C(1) << vertex;
    } else {
        result.high = UINT64_C(1) << (vertex - 64U);
    }
    return result;
}

static bool vertex_set_is_empty(VertexSet set)
{
    return set.low == 0 && set.high == 0;
}

static bool vertex_set_contains(VertexSet set, unsigned vertex)
{
    if (vertex < 64U) {
        return (set.low & (UINT64_C(1) << vertex)) != 0;
    }
    return (set.high & (UINT64_C(1) << (vertex - 64U))) != 0;
}

static void vertex_set_add(VertexSet *set, unsigned vertex)
{
    if (vertex < 64U) {
        set->low |= UINT64_C(1) << vertex;
    } else {
        set->high |= UINT64_C(1) << (vertex - 64U);
    }
}

static void vertex_set_remove(VertexSet *set, unsigned vertex)
{
    if (vertex < 64U) {
        set->low &= ~(UINT64_C(1) << vertex);
    } else {
        set->high &= ~(UINT64_C(1) << (vertex - 64U));
    }
}

static VertexSet vertex_set_and(VertexSet left, VertexSet right)
{
    VertexSet result = {
        left.low & right.low,
        left.high & right.high
    };
    return result;
}

static VertexSet vertex_set_or(VertexSet left, VertexSet right)
{
    VertexSet result = {
        left.low | right.low,
        left.high | right.high
    };
    return result;
}

static bool vertex_set_subset(VertexSet subset, VertexSet superset)
{
    return (subset.low & ~superset.low) == 0 &&
           (subset.high & ~superset.high) == 0;
}

static unsigned vertex_set_count(VertexSet set)
{
    return (unsigned)__builtin_popcountll(set.low) +
           (unsigned)__builtin_popcountll(set.high);
}

static int vertex_set_take_first(VertexSet *set)
{
    if (set->low != 0) {
        unsigned vertex = (unsigned)__builtin_ctzll(set->low);
        set->low &= set->low - 1U;
        return (int)vertex;
    }
    if (set->high != 0) {
        unsigned bit = (unsigned)__builtin_ctzll(set->high);
        set->high &= set->high - 1U;
        return (int)(bit + 64U);
    }
    return -1;
}

static unsigned vertex_set_to_array(VertexSet set, unsigned *vertices)
{
    unsigned count = 0;
    int vertex;
    while ((vertex = vertex_set_take_first(&set)) >= 0) {
        vertices[count++] = (unsigned)vertex;
    }
    return count;
}

static VertexSet first_vertices(unsigned count)
{
    VertexSet set = vertex_set_empty();
    if (count >= 64U) {
        set.low = UINT64_MAX;
        unsigned high_count = count - 64U;
        set.high = high_count == 64U
                       ? UINT64_MAX
                       : (UINT64_C(1) << high_count) - 1U;
    } else if (count != 0) {
        set.low = (UINT64_C(1) << count) - 1U;
    }
    return set;
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

static bool residue_vector_is_zero(const uint32_t *vector,
                                   size_t modulus_count)
{
    for (size_t index = 0; index < modulus_count; ++index) {
        if (vector[index] != 0) {
            return false;
        }
    }
    return true;
}

static ComputeStatus table_allocate(PolyTable *table,
                                    unsigned variable_count,
                                    unsigned degree,
                                    size_t modulus_count,
                                    MemoryTracker *memory)
{
    memset(table, 0, sizeof(*table));
    if (variable_count > MAX_ALLOWED_WIDTH + 1U ||
        variable_count >= sizeof(size_t) * CHAR_BIT) {
        return COMPUTE_WIDTH_LIMIT;
    }
    table->variable_count = variable_count;
    table->degree = degree;
    table->state_count = (size_t)1U << variable_count;

    size_t coefficients_per_state;
    if (!checked_mul_size((size_t)degree + 1U, modulus_count,
                          &coefficients_per_state) ||
        !checked_mul_size(table->state_count, coefficients_per_state,
                          &table->coefficient_count) ||
        !checked_mul_size(table->coefficient_count, sizeof(uint32_t),
                          &table->bytes)) {
        memset(table, 0, sizeof(*table));
        return COMPUTE_SIZE_OVERFLOW;
    }
    table->coefficients =
        tracked_allocate(memory, table->bytes, true);
    if (table->coefficients == NULL) {
        memset(table, 0, sizeof(*table));
        return memory->required > memory->budget
                   ? COMPUTE_MEMORY_LIMIT
                   : COMPUTE_ALLOCATION_FAILURE;
    }
    return COMPUTE_OK;
}

static void table_destroy(PolyTable *table, MemoryTracker *memory)
{
    tracked_free(memory, table->coefficients, table->bytes);
    memset(table, 0, sizeof(*table));
}

static uint32_t *table_coefficient(PolyTable *table, size_t state,
                                   unsigned degree,
                                   size_t modulus_count)
{
    size_t offset =
        (state * ((size_t)table->degree + 1U) + degree) *
        modulus_count;
    return table->coefficients + offset;
}

static const uint32_t *table_coefficient_const(
    const PolyTable *table, size_t state, unsigned degree,
    size_t modulus_count)
{
    size_t offset =
        (state * ((size_t)table->degree + 1U) + degree) *
        modulus_count;
    return table->coefficients + offset;
}

static bool table_state_is_zero(const PolyTable *table, size_t state,
                                size_t modulus_count)
{
    for (unsigned degree = 0; degree <= table->degree; ++degree) {
        const uint32_t *coefficient = table_coefficient_const(
            table, state, degree, modulus_count);
        if (!residue_vector_is_zero(coefficient, modulus_count)) {
            return false;
        }
    }
    return true;
}

static void add_vector(uint32_t *destination, const uint32_t *source,
                       size_t modulus_count)
{
    for (size_t index = 0; index < modulus_count; ++index) {
        destination[index] = add_mod(destination[index], source[index],
                                     moduli[index]);
    }
}

static void add_product_vector(uint32_t *destination,
                               const uint32_t *left,
                               const uint32_t *right,
                               size_t modulus_count,
                               ComputeStats *stats)
{
    for (size_t index = 0; index < modulus_count; ++index) {
        uint32_t product = multiply_mod(left[index], right[index],
                                        moduli[index]);
        destination[index] = add_mod(destination[index], product,
                                     moduli[index]);
        stats->coefficient_products = saturating_add(
            stats->coefficient_products, 1);
    }
}

static ComputeStatus combine_child_table(
    PolyTable *current, const unsigned *bag_vertices,
    unsigned bag_size, PolyTable *child,
    VertexSet child_separator, unsigned maximum_degree,
    size_t modulus_count, MemoryTracker *memory,
    ComputeStats *stats)
{
    unsigned child_vertices[MAX_VERTICES];
    unsigned child_size = vertex_set_to_array(
        child_separator, child_vertices);
    if (child_size != child->variable_count) {
        return COMPUTE_INTERNAL_ERROR;
    }

    unsigned bit_mapping[MAX_ALLOWED_WIDTH + 1U];
    for (unsigned child_bit = 0; child_bit < child_size; ++child_bit) {
        unsigned bag_bit = 0;
        while (bag_bit < bag_size &&
               bag_vertices[bag_bit] != child_vertices[child_bit]) {
            ++bag_bit;
        }
        if (bag_bit == bag_size) {
            return COMPUTE_INTERNAL_ERROR;
        }
        bit_mapping[child_bit] = bag_bit;
    }

    size_t mapping_bytes;
    if (!checked_mul_size(child->state_count, sizeof(size_t),
                          &mapping_bytes)) {
        return COMPUTE_SIZE_OVERFLOW;
    }
    size_t *mapped_states =
        tracked_allocate(memory, mapping_bytes, false);
    if (mapped_states == NULL) {
        return memory->required > memory->budget
                   ? COMPUTE_MEMORY_LIMIT
                   : COMPUTE_ALLOCATION_FAILURE;
    }
    mapped_states[0] = 0;
    for (size_t state = 1; state < child->state_count; ++state) {
        size_t bit = state & (0U - state);
        unsigned child_bit = (unsigned)__builtin_ctzll(bit);
        mapped_states[state] = mapped_states[state ^ bit] |
                               ((size_t)1U << bit_mapping[child_bit]);
    }

    unsigned new_degree = current->degree + child->degree;
    if (new_degree > maximum_degree) {
        new_degree = maximum_degree;
    }
    PolyTable combined;
    ComputeStatus status = table_allocate(
        &combined, current->variable_count, new_degree,
        modulus_count, memory);
    if (status != COMPUTE_OK) {
        tracked_free(memory, mapped_states, mapping_bytes);
        return status;
    }

    size_t left_list_bytes;
    size_t right_list_bytes;
    if (!checked_mul_size(current->state_count, sizeof(size_t),
                          &left_list_bytes) ||
        !checked_mul_size(child->state_count, sizeof(size_t),
                          &right_list_bytes)) {
        table_destroy(&combined, memory);
        tracked_free(memory, mapped_states, mapping_bytes);
        return COMPUTE_SIZE_OVERFLOW;
    }
    size_t *active_left = tracked_allocate(
        memory, left_list_bytes, false);
    size_t *active_right = tracked_allocate(
        memory, right_list_bytes, false);
    if (active_left == NULL || active_right == NULL) {
        tracked_free(memory, active_right, right_list_bytes);
        tracked_free(memory, active_left, left_list_bytes);
        table_destroy(&combined, memory);
        tracked_free(memory, mapped_states, mapping_bytes);
        return memory->required > memory->budget
                   ? COMPUTE_MEMORY_LIMIT
                   : COMPUTE_ALLOCATION_FAILURE;
    }
    size_t active_left_count = 0;
    size_t active_right_count = 0;
    for (size_t state = 0; state < current->state_count; ++state) {
        if (!table_state_is_zero(current, state, modulus_count)) {
            active_left[active_left_count++] = state;
        }
    }
    for (size_t state = 0; state < child->state_count; ++state) {
        if (!table_state_is_zero(child, state, modulus_count)) {
            active_right[active_right_count++] = state;
        }
    }

    for (size_t left_entry = 0; left_entry < active_left_count;
         ++left_entry) {
        size_t left_state = active_left[left_entry];
        for (size_t right_entry = 0; right_entry < active_right_count;
             ++right_entry) {
            size_t right_state = active_right[right_entry];
            size_t mapped = mapped_states[right_state];
            if ((left_state & mapped) != 0) {
                continue;
            }
            size_t destination_state = left_state | mapped;
            stats->table_combinations = saturating_add(
                stats->table_combinations, 1);
            for (unsigned left_degree = 0;
                 left_degree <= current->degree; ++left_degree) {
                const uint32_t *left = table_coefficient_const(
                    current, left_state, left_degree, modulus_count);
                if (residue_vector_is_zero(left, modulus_count)) {
                    continue;
                }
                unsigned maximum_right = child->degree;
                if (maximum_right > new_degree - left_degree) {
                    maximum_right = new_degree - left_degree;
                }
                for (unsigned right_degree = 0;
                     right_degree <= maximum_right; ++right_degree) {
                    const uint32_t *right = table_coefficient_const(
                        child, right_state, right_degree,
                        modulus_count);
                    if (residue_vector_is_zero(right,
                                               modulus_count)) {
                        continue;
                    }
                    uint32_t *destination = table_coefficient(
                        &combined, destination_state,
                        left_degree + right_degree,
                        modulus_count);
                    add_product_vector(destination, left, right,
                                       modulus_count, stats);
                }
            }
        }
    }

    tracked_free(memory, active_right, right_list_bytes);
    tracked_free(memory, active_left, left_list_bytes);
    tracked_free(memory, mapped_states, mapping_bytes);
    table_destroy(current, memory);
    *current = combined;
    return COMPUTE_OK;
}

static ComputeStatus apply_local_edges(
    PolyTable *current, const unsigned *bag_vertices,
    unsigned bag_size, VertexSet local_neighbors,
    unsigned maximum_degree, size_t modulus_count,
    MemoryTracker *memory)
{
    unsigned neighbor_bits[MAX_ALLOWED_WIDTH + 1U];
    unsigned neighbor_count = 0;
    VertexSet scan = local_neighbors;
    int vertex;
    while ((vertex = vertex_set_take_first(&scan)) >= 0) {
        unsigned bit = 1;
        while (bit < bag_size &&
               bag_vertices[bit] != (unsigned)vertex) {
            ++bit;
        }
        if (bit == bag_size) {
            return COMPUTE_INTERNAL_ERROR;
        }
        neighbor_bits[neighbor_count++] = bit;
    }

    unsigned new_degree = current->degree;
    if (neighbor_count != 0 && new_degree < maximum_degree) {
        ++new_degree;
    }
    PolyTable updated;
    ComputeStatus status = table_allocate(
        &updated, current->variable_count, new_degree,
        modulus_count, memory);
    if (status != COMPUTE_OK) {
        return status;
    }

    for (size_t state = 0; state < current->state_count; ++state) {
        for (unsigned degree = 0; degree <= current->degree; ++degree) {
            const uint32_t *source = table_coefficient_const(
                current, state, degree, modulus_count);
            if (residue_vector_is_zero(source, modulus_count)) {
                continue;
            }
            uint32_t *unchanged = table_coefficient(
                &updated, state, degree, modulus_count);
            add_vector(unchanged, source, modulus_count);

            if ((state & 1U) != 0 || degree == maximum_degree) {
                continue;
            }
            for (unsigned index = 0; index < neighbor_count; ++index) {
                size_t neighbor_bit =
                    (size_t)1U << neighbor_bits[index];
                if ((state & neighbor_bit) != 0) {
                    continue;
                }
                size_t destination_state =
                    state | 1U | neighbor_bit;
                uint32_t *destination = table_coefficient(
                    &updated, destination_state, degree + 1U,
                    modulus_count);
                add_vector(destination, source, modulus_count);
            }
        }
    }

    table_destroy(current, memory);
    *current = updated;
    return COMPUTE_OK;
}

static ComputeStatus forget_vertex(PolyTable *message,
                                   PolyTable *current,
                                   size_t modulus_count,
                                   MemoryTracker *memory)
{
    if (current->variable_count == 0) {
        return COMPUTE_INTERNAL_ERROR;
    }
    ComputeStatus status = table_allocate(
        message, current->variable_count - 1U, current->degree,
        modulus_count, memory);
    if (status != COMPUTE_OK) {
        return status;
    }

    for (size_t state = 0; state < current->state_count; ++state) {
        size_t destination_state = state >> 1;
        for (unsigned degree = 0; degree <= current->degree; ++degree) {
            const uint32_t *source = table_coefficient_const(
                current, state, degree, modulus_count);
            if (residue_vector_is_zero(source, modulus_count)) {
                continue;
            }
            uint32_t *destination = table_coefficient(
                message, destination_state, degree, modulus_count);
            add_vector(destination, source, modulus_count);
        }
    }
    table_destroy(current, memory);
    return COMPUTE_OK;
}

static ComputeStatus multiply_root_polynomial(
    uint32_t *global, unsigned *global_degree,
    const PolyTable *root, unsigned maximum_degree,
    size_t modulus_count, ComputeStats *stats)
{
    if (root->variable_count != 0 || root->state_count != 1) {
        return COMPUTE_INTERNAL_ERROR;
    }
    uint32_t next[(MAX_REDUCED_N + 1U) * MAX_RESIDUES];
    memset(next, 0, sizeof(next));

    unsigned new_degree = *global_degree + root->degree;
    if (new_degree > maximum_degree) {
        new_degree = maximum_degree;
    }
    for (unsigned left_degree = 0;
         left_degree <= *global_degree; ++left_degree) {
        const uint32_t *left =
            global + (size_t)left_degree * modulus_count;
        for (unsigned right_degree = 0;
             right_degree <= root->degree &&
             left_degree + right_degree <= new_degree;
             ++right_degree) {
            const uint32_t *right = table_coefficient_const(
                root, 0, right_degree, modulus_count);
            uint32_t *destination =
                next + (size_t)(left_degree + right_degree) *
                           modulus_count;
            add_product_vector(destination, left, right,
                               modulus_count, stats);
        }
    }
    memcpy(global, next, sizeof(next));
    *global_degree = new_degree;
    return COMPUTE_OK;
}

static ComputeStatus build_forbidden_graph(int n, VertexSet *adjacency,
                                           unsigned *vertex_count,
                                           unsigned *reduced_size)
{
    for (unsigned vertex = 0; vertex < MAX_VERTICES; ++vertex) {
        adjacency[vertex] = vertex_set_empty();
    }
    if (n <= 1) {
        *vertex_count = 0;
        *reduced_size = 0;
        return COMPUTE_OK;
    }

    unsigned m = (unsigned)(n - 2);
    *reduced_size = m;
    *vertex_count = 2U * m;
    for (unsigned row = 0; row < m; ++row) {
        for (unsigned column = 0; column < m; ++column) {
            if (!allowed_pair(row + 2U, column + 2U,
                              (unsigned)n)) {
                unsigned left = row;
                unsigned right = m + column;
                vertex_set_add(&adjacency[left], right);
                vertex_set_add(&adjacency[right], left);
            }
        }
    }
    return COMPUTE_OK;
}

static uint64_t fill_score(unsigned vertex, VertexSet alive,
                           const VertexSet *adjacency,
                           unsigned *degree)
{
    VertexSet neighbors = vertex_set_and(adjacency[vertex], alive);
    vertex_set_remove(&neighbors, vertex);
    unsigned list[MAX_VERTICES];
    unsigned count = vertex_set_to_array(neighbors, list);
    *degree = count;
    uint64_t missing = 0;
    for (unsigned left = 0; left < count; ++left) {
        for (unsigned right = left + 1U; right < count; ++right) {
            if (!vertex_set_contains(adjacency[list[left]],
                                     list[right])) {
                ++missing;
            }
        }
    }
    return missing;
}

static ComputeStatus min_fill_decomposition(
    const VertexSet *original_adjacency, unsigned vertex_count,
    DecompositionNode *nodes, unsigned *order,
    unsigned *position, unsigned *width)
{
    VertexSet fill_adjacency[MAX_VERTICES];
    memcpy(fill_adjacency, original_adjacency,
           vertex_count * sizeof(*fill_adjacency));
    VertexSet alive = first_vertices(vertex_count);
    *width = 0;

    for (unsigned step = 0; step < vertex_count; ++step) {
        int best = -1;
        uint64_t best_fill = UINT64_MAX;
        unsigned best_degree = UINT_MAX;
        VertexSet scan = alive;
        int candidate;
        while ((candidate = vertex_set_take_first(&scan)) >= 0) {
            unsigned degree;
            uint64_t fill = fill_score(
                (unsigned)candidate, alive, fill_adjacency, &degree);
            if (best < 0 || fill < best_fill ||
                (fill == best_fill && degree < best_degree) ||
                (fill == best_fill && degree == best_degree &&
                 candidate < best)) {
                best = candidate;
                best_fill = fill;
                best_degree = degree;
            }
        }
        if (best < 0) {
            return COMPUTE_INTERNAL_ERROR;
        }

        unsigned vertex = (unsigned)best;
        VertexSet neighbors = vertex_set_and(
            fill_adjacency[vertex], alive);
        vertex_set_remove(&neighbors, vertex);
        unsigned degree = vertex_set_count(neighbors);
        if (degree > *width) {
            *width = degree;
        }
        if (degree > MAX_ALLOWED_WIDTH) {
            return COMPUTE_WIDTH_LIMIT;
        }
        nodes[vertex].vertex = vertex;
        nodes[vertex].separator = neighbors;
        nodes[vertex].separator_size = degree;
        nodes[vertex].parent = -1;
        nodes[vertex].first_child = -1;
        nodes[vertex].next_sibling = -1;
        order[step] = vertex;
        position[vertex] = step;

        unsigned list[MAX_VERTICES];
        unsigned count = vertex_set_to_array(neighbors, list);
        for (unsigned left = 0; left < count; ++left) {
            for (unsigned right = left + 1U; right < count; ++right) {
                vertex_set_add(&fill_adjacency[list[left]],
                               list[right]);
                vertex_set_add(&fill_adjacency[list[right]],
                               list[left]);
            }
        }
        vertex_set_remove(&alive, vertex);
    }

    for (unsigned vertex = 0; vertex < vertex_count; ++vertex) {
        VertexSet separator = nodes[vertex].separator;
        if (vertex_set_is_empty(separator)) {
            nodes[vertex].parent = -1;
        } else {
            int parent = -1;
            unsigned parent_position = UINT_MAX;
            VertexSet scan = separator;
            int candidate;
            while ((candidate = vertex_set_take_first(&scan)) >= 0) {
                if (position[candidate] < parent_position) {
                    parent = candidate;
                    parent_position = position[candidate];
                }
            }
            if (parent < 0 ||
                position[parent] <= position[vertex]) {
                return COMPUTE_INTERNAL_ERROR;
            }
            nodes[vertex].parent = parent;
            VertexSet parent_bag = vertex_set_or(
                nodes[parent].separator,
                vertex_set_single((unsigned)parent));
            if (!vertex_set_subset(separator, parent_bag)) {
                return COMPUTE_INTERNAL_ERROR;
            }
            nodes[vertex].next_sibling =
                nodes[parent].first_child;
            nodes[parent].first_child = (int)vertex;
        }
        nodes[vertex].local_neighbors = vertex_set_and(
            original_adjacency[vertex], nodes[vertex].separator);
    }
    return COMPUTE_OK;
}

static int compare_child_tables(const void *left, const void *right,
                                void *context)
{
    const DecompositionNode *nodes = context;
    int a = *(const int *)left;
    int b = *(const int *)right;
    if (nodes[a].message.state_count != nodes[b].message.state_count) {
        return nodes[a].message.state_count < nodes[b].message.state_count
                   ? -1 : 1;
    }
    if (nodes[a].message.degree != nodes[b].message.degree) {
        return nodes[a].message.degree < nodes[b].message.degree ? -1 : 1;
    }
    return a < b ? -1 : a > b;
}

static void sort_children(int *children, unsigned count,
                          const DecompositionNode *nodes)
{
    /* count <= 65, so a stable insertion sort is sufficient and portable. */
    for (unsigned index = 1; index < count; ++index) {
        int value = children[index];
        unsigned position = index;
        while (position > 0 &&
               compare_child_tables(&value, &children[position - 1U],
                                    (void *)nodes) < 0) {
            children[position] = children[position - 1U];
            --position;
        }
        children[position] = value;
    }
}

static ComputeStatus matching_polynomial(
    uint32_t *global_polynomial, unsigned *global_degree,
    int n, unsigned reduced_size, const VertexSet *adjacency,
    unsigned vertex_count, size_t modulus_count,
    MemoryTracker *memory, bool report, ComputeStats *stats)
{
    DecompositionNode nodes[MAX_VERTICES];
    unsigned order[MAX_VERTICES];
    unsigned position[MAX_VERTICES];
    memset(nodes, 0, sizeof(nodes));
    for (unsigned vertex = 0; vertex < MAX_VERTICES; ++vertex) {
        nodes[vertex].parent = -1;
        nodes[vertex].first_child = -1;
        nodes[vertex].next_sibling = -1;
    }

    ComputeStatus status = min_fill_decomposition(
        adjacency, vertex_count, nodes, order, position,
        &stats->elimination_width);
    if (status != COMPUTE_OK) {
        return status;
    }

    if (report) {
        fprintf(stderr,
                "399488_03: n=%d forbidden graph vertices=%u, "
                "min-fill width=%u\n",
                n, vertex_count, stats->elimination_width);
    }

    memset(global_polynomial, 0,
           (MAX_REDUCED_N + 1U) * MAX_RESIDUES *
               sizeof(*global_polynomial));
    for (size_t residue = 0; residue < modulus_count; ++residue) {
        global_polynomial[residue] = 1;
    }
    *global_degree = 0;

    for (unsigned step = 0; step < vertex_count; ++step) {
        unsigned vertex = order[step];
        DecompositionNode *node = &nodes[vertex];
        unsigned bag_vertices[MAX_ALLOWED_WIDTH + 1U];
        bag_vertices[0] = vertex;
        unsigned separator_count = vertex_set_to_array(
            node->separator, bag_vertices + 1U);
        unsigned bag_size = separator_count + 1U;
        if (separator_count != node->separator_size ||
            bag_size > MAX_ALLOWED_WIDTH + 1U) {
            status = COMPUTE_INTERNAL_ERROR;
            goto cleanup;
        }

        PolyTable current;
        status = table_allocate(&current, bag_size, 0,
                                modulus_count, memory);
        if (status != COMPUTE_OK) {
            goto cleanup;
        }
        uint32_t *initial = table_coefficient(
            &current, 0, 0, modulus_count);
        for (size_t residue = 0; residue < modulus_count; ++residue) {
            initial[residue] = 1;
        }

        int children[MAX_VERTICES];
        unsigned child_count = 0;
        for (int child = node->first_child; child >= 0;
             child = nodes[child].next_sibling) {
            children[child_count++] = child;
        }
        sort_children(children, child_count, nodes);
        for (unsigned index = 0; index < child_count; ++index) {
            int child = children[index];
            status = combine_child_table(
                &current, bag_vertices, bag_size,
                &nodes[child].message, nodes[child].separator,
                reduced_size, modulus_count, memory, stats);
            if (status != COMPUTE_OK) {
                table_destroy(&current, memory);
                goto cleanup;
            }
            table_destroy(&nodes[child].message, memory);
        }

        status = apply_local_edges(
            &current, bag_vertices, bag_size,
            node->local_neighbors, reduced_size,
            modulus_count, memory);
        if (status != COMPUTE_OK) {
            table_destroy(&current, memory);
            goto cleanup;
        }
        status = forget_vertex(&node->message, &current,
                               modulus_count, memory);
        if (status != COMPUTE_OK) {
            table_destroy(&current, memory);
            goto cleanup;
        }

        if (node->parent < 0) {
            status = multiply_root_polynomial(
                global_polynomial, global_degree, &node->message,
                reduced_size, modulus_count, stats);
            table_destroy(&node->message, memory);
            if (status != COMPUTE_OK) {
                goto cleanup;
            }
        }
        if (report &&
            (step + 1U == vertex_count || (step + 1U) % 8U == 0)) {
            fprintf(stderr,
                    "399488_03: n=%d bags=%u/%u, active memory=%.2f "
                    "MiB\n",
                    n, step + 1U, vertex_count,
                    (double)memory->allocated /
                        (double)(UINT64_C(1) << 20));
        }
    }

    if (*global_degree != reduced_size) {
        status = COMPUTE_INTERNAL_ERROR;
    }

cleanup:
    for (unsigned vertex = 0; vertex < vertex_count; ++vertex) {
        table_destroy(&nodes[vertex].message, memory);
    }
    return status;
}

static size_t choose_modulus_count(const mpz_t bound, mpz_t product)
{
    mpz_set_ui(product, 1);
    for (size_t count = 1; count <= MAX_RESIDUES; ++count) {
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
    stats->modulus_count = choose_modulus_count(bound,
                                                selected_product);
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
    VertexSet adjacency[MAX_VERTICES];
    unsigned vertex_count;
    unsigned reduced_size;
    ComputeStatus status = build_forbidden_graph(
        n, adjacency, &vertex_count, &reduced_size);

    uint32_t rook[(MAX_REDUCED_N + 1U) * MAX_RESIDUES];
    unsigned rook_degree = 0;
    if (status == COMPUTE_OK) {
        status = matching_polynomial(
            rook, &rook_degree, n, reduced_size, adjacency,
            vertex_count, stats->modulus_count, &memory,
            report, stats);
    }

    uint32_t residues[MAX_RESIDUES] = { 0 };
    if (status == COMPUTE_OK) {
        if (rook_degree != reduced_size) {
            status = COMPUTE_INTERNAL_ERROR;
        } else {
            uint32_t factorial[MAX_RESIDUES] = { 0 };
            for (size_t residue = 0;
                 residue < stats->modulus_count; ++residue) {
                factorial[residue] = 1;
            }
            uint32_t factorials[(MAX_REDUCED_N + 1U) * MAX_RESIDUES];
            memset(factorials, 0, sizeof(factorials));
            for (size_t residue = 0;
                 residue < stats->modulus_count; ++residue) {
                factorials[residue] = 1;
            }
            for (unsigned k = 1; k <= reduced_size; ++k) {
                for (size_t residue = 0;
                     residue < stats->modulus_count; ++residue) {
                    factorial[residue] = multiply_mod(
                        factorial[residue], k, moduli[residue]);
                    factorials[(size_t)k * stats->modulus_count +
                               residue] = factorial[residue];
                }
            }

            for (unsigned k = 0; k <= reduced_size; ++k) {
                for (size_t residue = 0;
                     residue < stats->modulus_count; ++residue) {
                    uint32_t term = multiply_mod(
                        rook[(size_t)k * stats->modulus_count +
                             residue],
                        factorials[(size_t)(reduced_size - k) *
                                       stats->modulus_count +
                                   residue],
                        moduli[residue]);
                    residues[residue] =
                        (k & 1U) == 0
                            ? add_mod(residues[residue], term,
                                      moduli[residue])
                            : subtract_mod(residues[residue], term,
                                           moduli[residue]);
                }
            }
        }
    }

    if (status == COMPUTE_OK) {
        bool valid = reconstruct_crt(
                         result, reconstructed_product, residues,
                         stats->modulus_count) &&
                     mpz_cmp(reconstructed_product, bound) > 0 &&
                     mpz_sgn(result) >= 0 &&
                     mpz_cmp(result, bound) <= 0;
        for (size_t residue = 0;
             valid && residue < stats->modulus_count; ++residue) {
            valid = mpz_fdiv_ui(result,
                                (unsigned long)moduli[residue]) ==
                    residues[residue];
        }
        status = valid ? COMPUTE_OK : COMPUTE_INTERNAL_ERROR;
    }

    if (memory.allocated != 0 && status == COMPUTE_OK) {
        status = COMPUTE_INTERNAL_ERROR;
    }
    stats->peak_bytes = memory.peak;
    stats->required_bytes = memory.required;
    stats->seconds = monotonic_seconds() - start;
    if (report && status == COMPUTE_OK) {
        fprintf(stderr,
                "399488_03: n=%d exact CRT, bound=%zu bits, "
                "%zu primes, width=%u\n",
                n, stats->bound_bits, stats->modulus_count,
                stats->elimination_width);
    }
    mpz_clears(reconstructed_product, selected_product, bound, NULL);
    return status;
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
    case COMPUTE_WIDTH_LIMIT:
        return "tree-decomposition width limit exceeded";
    case COMPUTE_INTERNAL_ERROR:
        return "internal consistency check failed";
    }
    return "unknown error";
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
                    "required at failed allocation: %.2f MiB; "
                    "configured limit: %.2f MiB\n",
                    (double)stats->required_bytes /
                        (double)(UINT64_C(1) << 20),
                    (double)memory_budget /
                        (double)(UINT64_C(1) << 20));
        }
        fprintf(stderr,
                "Increase A399488_03_MEMORY_MIB only when enough "
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
                "399488_03: n=%d done, width=%u, table pairs=%" PRIu64
                ", coefficient products=%" PRIu64
                ", peak allocation=%.2f MiB, %.3f s\n",
                n, stats.elimination_width,
                stats.table_combinations,
                stats.coefficient_products,
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
        if (n <= 10) {
            uint64_t brute = brute_force_rec(1, (unsigned)n, 0);
            if (mpz_cmp_ui(value, (unsigned long)brute) != 0) {
                gmp_fprintf(stderr,
                            "error: tree DP and brute force disagree "
                            "at n=%d: %Zd versus %" PRIu64 "\n",
                            n, value, brute);
                mpz_clear(value);
                return EXIT_FAILURE;
            }
        }
    }
    mpz_clear(value);
    printf("ok: tree-decomposition DP agrees with the known A399488 "
           "prefix for n=0..%d; brute force independently agrees "
           "for n=0..%d\n",
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

    char *path = path_beside_executable(argv0, "b399488_03.txt");
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
                "399488_03: using built-in verified prefix n=0..%d\n",
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
            "MAX_N defaults to %d and is limited to %d.\n"
            "START_N defaults to 0. Terms below START_N are read from\n"
            "the built-in verified prefix, which ends at n=%d.\n"
            "The default memory limit is %" PRIu64
            " MiB; override it with\n"
            "A399488_03_MEMORY_MIB.\n",
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
