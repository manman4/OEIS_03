/*
 * A399206: Number of permutations p of [n] such that the n-1 products
 *          p(i)*p(i+1) are all distinct.
 *
 * Version 05 uses a transfer DP over canonical linear-forest boundary
 * states.  Unlike version 02, it does not recursively enumerate every
 * choice to a leaf: after each product class, forests with identical future
 * behaviour are merged and their signed coefficients are added.
 *
 * Build:
 *   cc -O3 -std=c11 -Wall -Wextra -Wpedantic 399206_05.c -o 399206_05
 *
 * Examples:
 *   ./399206_05                 # n=0..23; updates b399206_05.txt
 *   ./399206_05 23 --stats
 *   ./399206_05 --target 23 --no-bfile --stats
 *   ./399206_05 --self-test 11
 *
 * Transfer DP
 * -----------
 * Regard {u,v} as an edge colored by u*v.  For a color occurring k times in
 * a Hamilton path, the weights
 *
 *   w(0)=1, w(1)=0, w(s)=(-1)^(s-1)*(s-1), s>=2,
 *
 * have sum C(k,s)w(s)=1 for k<=1 and 0 for k>=2.  Product classes occurring
 * only once need not be processed.
 *
 * A selected edge set contributes only if it is a linear forest.  Such a
 * forest is represented canonically by one 5-bit code per active vertex:
 *
 *   0: isolated/inactive, 1: internal (degree 2),
 *   mate+2: endpoint (degree 1), 31: endpoint whose mate is inactive.
 *
 * Endpoints of every nontrivial path component point to each other.  This is
 * sufficient for every future edge insertion: an edge is illegal exactly
 * when an endpoint is internal or its two endpoints already belong to the
 * same path.  It also determines m, the edge count, and c, the number of
 * nontrivial components.  Hence different forests represented by the same
 * state have identical future transitions and the same final multiplier
 * 2^c*(n-m)!; adding their coefficients is exact.  Once a vertex occurs in
 * no unprocessed product class, it is removed from the boundary.  The total
 * m and c are stored in ten additional bits, so this forgetting is exact and
 * makes the late DP layers collapse instead of retaining irrelevant labels.
 *
 * The boundary needs 5*23=115 bits; with the counters the complete 125-bit
 * state is stored in unsigned __int128.
 * All signed coefficient arithmetic is checked.  Completed b-file terms are
 * saved through an exclusive lock, fsync, and atomic rename.
 */

#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#if !defined(__SIZEOF_INT128__)
#error "399206_05.c requires __int128"
#endif

__extension__ typedef unsigned __int128 u128;
__extension__ typedef __int128 s128;

#define MAX_N 23U
#define DEFAULT_N 23U
#define DEFAULT_SELF_TEST_N 10U
#define MAX_SELF_TEST_N 11U
#define MAX_PRODUCT (MAX_N * MAX_N)
#define MAX_CLASS_EDGES (MAX_N / 2U)
#define MAX_CLASSES (MAX_PRODUCT + 1U)
#define CODE_BITS 5U
#define CODE_MASK 31U
#define CLOSED_CODE 31U
#define EDGE_COUNT_SHIFT (CODE_BITS * MAX_N)
#define COMPONENT_COUNT_SHIFT (EDGE_COUNT_SHIFT + 5U)
#define COUNTER_MASK 31U

_Static_assert(MAX_N < 64U, "vertex masks require fewer than 64 bits");
_Static_assert(MAX_N + 2U < CLOSED_CODE,
               "the closed-endpoint code must not name a vertex");
_Static_assert(COMPONENT_COUNT_SHIFT + 5U < 127U,
               "the state must not overlap the occupancy bit");
_Static_assert(MAX_CLASS_EDGES < 64U,
               "color-class subsets require fewer than 64 edges");

static const u128 OCCUPIED_BIT = (u128)1U << 127U;
static const u128 STATE_MASK = ~((u128)1U << 127U);
static const char *output_path = "b399206_05.txt";
static bool write_bfile = true;

typedef struct {
    uint8_t u;
    uint8_t v;
} Edge;

typedef struct {
    Edge edges[MAX_CLASS_EDGES];
    uint8_t count;
    uint64_t vertex_mask;
    unsigned conflict_score;
} ColorClass;

typedef struct {
    u128 marked_key;
    s128 coefficient;
} Entry;

typedef struct {
    Entry *entries;
    size_t capacity;
    size_t occupied;
    size_t active;
} StateMap;

typedef struct {
    uint64_t transitions;
    size_t peak_states;
    size_t peak_slots;
    unsigned repeated_colors;
    bool show_layers;
} Statistics;

typedef struct {
    unsigned n;
    uint64_t used_vertices;
    bool used_product[MAX_PRODUCT + 1U];
    u128 count;
} DirectSearch;

static _Noreturn void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static s128 checked_add(s128 a, s128 b)
{
    s128 result;
    if (__builtin_add_overflow(a, b, &result))
        die("signed 128-bit addition overflow");
    return result;
}

static s128 checked_multiply(s128 a, s128 b)
{
    s128 result;
    if (__builtin_mul_overflow(a, b, &result))
        die("signed 128-bit multiplication overflow");
    return result;
}

static void u128_text(u128 value, char text[40])
{
    char reverse[40];
    size_t length = 0U;
    do {
        reverse[length++] = (char)('0' + (unsigned)(value % 10U));
        value /= 10U;
    } while (value != 0U);
    for (size_t i = 0U; i < length; ++i)
        text[i] = reverse[length - 1U - i];
    text[length] = '\0';
}

static void print_u128(FILE *stream, u128 value)
{
    char text[40];
    u128_text(value, text);
    if (fputs(text, stream) == EOF)
        die("cannot write result");
}

static void *xcalloc(size_t count, size_t size)
{
    if (size != 0U && count > SIZE_MAX / size)
        die("allocation size overflow");
    void *memory = calloc(count, size);
    if (memory == NULL)
        die("cannot allocate state table");
    return memory;
}

static uint64_t mix64(uint64_t x)
{
    x ^= x >> 30U;
    x *= UINT64_C(0xbf58476d1ce4e5b9);
    x ^= x >> 27U;
    x *= UINT64_C(0x94d049bb133111eb);
    return x ^ (x >> 31U);
}

static size_t state_hash(u128 key)
{
    const uint64_t low = (uint64_t)key;
    const uint64_t high = (uint64_t)(key >> 64U);
    return (size_t)mix64(low ^
                         high * UINT64_C(0x9e3779b97f4a7c15));
}

static void map_initialize(StateMap *map, size_t minimum)
{
    size_t capacity = 16U;
    while (capacity < minimum) {
        if (capacity > SIZE_MAX / 2U)
            die("state table capacity overflow");
        capacity *= 2U;
    }
    map->entries = xcalloc(capacity, sizeof(*map->entries));
    map->capacity = capacity;
    map->occupied = 0U;
    map->active = 0U;
}

static void map_release(StateMap *map)
{
    free(map->entries);
    memset(map, 0, sizeof(*map));
}

static void map_insert_without_growth(StateMap *map, u128 key, s128 value)
{
    size_t slot = state_hash(key) & (map->capacity - 1U);
    while ((map->entries[slot].marked_key & OCCUPIED_BIT) != 0U) {
        if ((map->entries[slot].marked_key & STATE_MASK) == key) {
            const s128 old_value = map->entries[slot].coefficient;
            const s128 new_value = checked_add(old_value, value);
            if (old_value == 0 && new_value != 0)
                ++map->active;
            else if (old_value != 0 && new_value == 0)
                --map->active;
            map->entries[slot].coefficient = new_value;
            return;
        }
        slot = (slot + 1U) & (map->capacity - 1U);
    }
    map->entries[slot].marked_key = key | OCCUPIED_BIT;
    map->entries[slot].coefficient = value;
    ++map->occupied;
    ++map->active;
}

static void map_rehash(StateMap *map, size_t new_capacity)
{
    StateMap replacement;
    map_initialize(&replacement, new_capacity);
    for (size_t i = 0U; i < map->capacity; ++i) {
        if ((map->entries[i].marked_key & OCCUPIED_BIT) == 0U ||
            map->entries[i].coefficient == 0)
            continue;
        map_insert_without_growth(
            &replacement, map->entries[i].marked_key & STATE_MASK,
            map->entries[i].coefficient);
    }
    free(map->entries);
    *map = replacement;
}

static void map_add(StateMap *map, u128 key, s128 value)
{
    if (value == 0)
        return;
    if (map->occupied >= map->capacity - map->capacity / 4U) {
        if (map->active + map->capacity / 8U < map->occupied)
            map_rehash(map, map->capacity);
    }
    if (map->occupied >= map->capacity - map->capacity / 4U) {
        if (map->capacity > SIZE_MAX / 2U)
            die("state table capacity overflow");
        map_rehash(map, map->capacity * 2U);
    }
    map_insert_without_growth(map, key, value);
}

static size_t map_nonzero_count(const StateMap *map)
{
    return map->active;
}

static unsigned popcount_u64(uint64_t value)
{
#if defined(__clang__) || defined(__GNUC__)
    return (unsigned)__builtin_popcountll(value);
#else
    unsigned count = 0U;
    while (value != 0U) {
        value &= value - 1U;
        ++count;
    }
    return count;
#endif
}

static unsigned trailing_index_u64(uint64_t value)
{
#if defined(__clang__) || defined(__GNUC__)
    return (unsigned)__builtin_ctzll(value);
#else
    unsigned index = 0U;
    while ((value & 1U) == 0U) {
        value >>= 1U;
        ++index;
    }
    return index;
#endif
}

static unsigned state_code(u128 state, unsigned vertex)
{
    return (unsigned)((state >> (CODE_BITS * vertex)) & CODE_MASK);
}

static u128 state_set_code(u128 state, unsigned vertex, unsigned code)
{
    const unsigned shift = CODE_BITS * vertex;
    const u128 mask = (u128)CODE_MASK << shift;
    return (state & ~mask) | ((u128)code << shift);
}

static unsigned state_counter(u128 state, unsigned shift)
{
    return (unsigned)((state >> shift) & COUNTER_MASK);
}

static u128 state_set_counter(u128 state, unsigned shift, unsigned value)
{
    const u128 mask = (u128)COUNTER_MASK << shift;
    return (state & ~mask) | ((u128)value << shift);
}

/* Insert an edge into a canonical path-forest state. */
static bool state_add_edge(u128 *state_pointer, Edge edge)
{
    u128 state = *state_pointer;
    const unsigned u = edge.u;
    const unsigned v = edge.v;
    const unsigned code_u = state_code(state, u);
    const unsigned code_v = state_code(state, v);
    if (code_u == 1U || code_v == 1U)
        return false;

    int component_delta = 0;
    if (code_u == 0U && code_v == 0U) {
        state = state_set_code(state, u, v + 2U);
        state = state_set_code(state, v, u + 2U);
        component_delta = 1;
    } else if (code_u == 0U) {
        const unsigned mate_v = code_v == CLOSED_CODE
            ? MAX_N : code_v - 2U;
        state = state_set_code(state, v, 1U);
        if (mate_v == MAX_N) {
            state = state_set_code(state, u, CLOSED_CODE);
        } else {
            state = state_set_code(state, u, mate_v + 2U);
            state = state_set_code(state, mate_v, u + 2U);
        }
    } else if (code_v == 0U) {
        const unsigned mate_u = code_u == CLOSED_CODE
            ? MAX_N : code_u - 2U;
        state = state_set_code(state, u, 1U);
        if (mate_u == MAX_N) {
            state = state_set_code(state, v, CLOSED_CODE);
        } else {
            state = state_set_code(state, v, mate_u + 2U);
            state = state_set_code(state, mate_u, v + 2U);
        }
    } else {
        const unsigned mate_u = code_u == CLOSED_CODE
            ? MAX_N : code_u - 2U;
        const unsigned mate_v = code_v == CLOSED_CODE
            ? MAX_N : code_v - 2U;
        if (mate_u == v || mate_v == u)
            return false;
        state = state_set_code(state, u, 1U);
        state = state_set_code(state, v, 1U);
        if (mate_u != MAX_N && mate_v != MAX_N) {
            state = state_set_code(state, mate_u, mate_v + 2U);
            state = state_set_code(state, mate_v, mate_u + 2U);
        } else if (mate_u != MAX_N) {
            state = state_set_code(state, mate_u, CLOSED_CODE);
        } else if (mate_v != MAX_N) {
            state = state_set_code(state, mate_v, CLOSED_CODE);
        }
        component_delta = -1;
    }

    const unsigned old_edges = state_counter(state, EDGE_COUNT_SHIFT);
    const unsigned old_components =
        state_counter(state, COMPONENT_COUNT_SHIFT);
    if (old_edges >= MAX_N ||
        (component_delta < 0 && old_components == 0U))
        die("invalid forest counters before edge insertion");
    state = state_set_counter(state, EDGE_COUNT_SHIFT, old_edges + 1U);
    state = state_set_counter(
        state, COMPONENT_COUNT_SHIFT,
        (unsigned)((int)old_components + component_delta));
    *state_pointer = state;
    return true;
}

/* Remove vertices which occur in no later product class. */
static u128 state_forget(u128 state, uint64_t forget_mask, unsigned n)
{
    while (forget_mask != 0U) {
        const unsigned vertex = trailing_index_u64(forget_mask);
        forget_mask &= forget_mask - 1U;
        const unsigned code = state_code(state, vertex);
        if (code >= 2U && code != CLOSED_CODE) {
            const unsigned mate = code - 2U;
            if (mate >= n || state_code(state, mate) != vertex + 2U)
                die("invalid endpoint pairing while forgetting a vertex");
            state = state_set_code(state, mate, CLOSED_CODE);
        }
        state = state_set_code(state, vertex, 0U);
    }
    return state;
}

static int compare_classes(const void *left, const void *right)
{
    const ColorClass *a = left;
    const ColorClass *b = right;
    if (a->count != b->count)
        return a->count < b->count ? 1 : -1;
    if (a->conflict_score != b->conflict_score)
        return a->conflict_score < b->conflict_score ? 1 : -1;
    if (a->vertex_mask != b->vertex_mask)
        return a->vertex_mask < b->vertex_mask ? 1 : -1;
    return 0;
}

/* Greedily keep the vertex frontier small; ties prefer stronger classes. */
static void order_classes(ColorClass classes[MAX_CLASSES],
                          unsigned class_count, unsigned n)
{
    unsigned remaining[MAX_N] = {0};
    uint64_t seen = 0U;
    for (unsigned i = 0U; i < class_count; ++i)
        for (unsigned v = 0U; v < n; ++v)
            if ((classes[i].vertex_mask & (UINT64_C(1) << v)) != 0U)
                ++remaining[v];

    for (unsigned position = 0U; position < class_count; ++position) {
        unsigned best = position;
        unsigned best_frontier = UINT_MAX;
        for (unsigned candidate = position; candidate < class_count;
             ++candidate) {
            const uint64_t after_seen = seen | classes[candidate].vertex_mask;
            unsigned frontier = 0U;
            for (unsigned v = 0U; v < n; ++v) {
                const bool in_class =
                    (classes[candidate].vertex_mask &
                     (UINT64_C(1) << v)) != 0U;
                if ((after_seen & (UINT64_C(1) << v)) != 0U &&
                    remaining[v] - (unsigned)in_class != 0U)
                    ++frontier;
            }
            if (frontier < best_frontier ||
                (frontier == best_frontier &&
                 classes[candidate].count > classes[best].count) ||
                (frontier == best_frontier &&
                 classes[candidate].count == classes[best].count &&
                 classes[candidate].conflict_score >
                     classes[best].conflict_score)) {
                best = candidate;
                best_frontier = frontier;
            }
        }
        const ColorClass temporary = classes[position];
        classes[position] = classes[best];
        classes[best] = temporary;
        seen |= classes[position].vertex_mask;
        for (unsigned v = 0U; v < n; ++v)
            if ((classes[position].vertex_mask & (UINT64_C(1) << v)) != 0U)
                --remaining[v];
    }
}

static unsigned make_color_classes(unsigned n, ColorClass classes[MAX_CLASSES])
{
    ColorClass buckets[MAX_PRODUCT + 1U];
    memset(buckets, 0, sizeof(buckets));
    for (unsigned u = 0U; u < n; ++u) {
        for (unsigned v = u + 1U; v < n; ++v) {
            const unsigned product = (u + 1U) * (v + 1U);
            ColorClass *class_ = &buckets[product];
            if (class_->count == MAX_CLASS_EDGES)
                die("a product class exceeds the matching bound");
            class_->edges[class_->count++] = (Edge){(uint8_t)u, (uint8_t)v};
            class_->vertex_mask |= (UINT64_C(1) << u) |
                                   (UINT64_C(1) << v);
        }
    }

    unsigned class_count = 0U;
    for (unsigned product = 1U; product <= n * n; ++product) {
        if (buckets[product].count <= 1U)
            continue;
        if (popcount_u64(buckets[product].vertex_mask) !=
            2U * buckets[product].count)
            die("equal-product edges unexpectedly share an endpoint");
        classes[class_count++] = buckets[product];
    }
    for (unsigned i = 0U; i < class_count; ++i) {
        unsigned score = 0U;
        for (unsigned j = 0U; j < class_count; ++j)
            score += popcount_u64(classes[i].vertex_mask &
                                  classes[j].vertex_mask);
        classes[i].conflict_score = score;
    }
    qsort(classes, class_count, sizeof(classes[0]), compare_classes);
    order_classes(classes, class_count, n);
    return class_count;
}

static u128 calculate(unsigned n, Statistics *statistics)
{
    ColorClass classes[MAX_CLASSES];
    const unsigned class_count = make_color_classes(n, classes);
    statistics->repeated_colors = class_count;

    StateMap current;
    map_initialize(&current, 16U);
    map_add(&current, 0U, 1);
    statistics->peak_states = 1U;
    statistics->peak_slots = current.capacity;

    unsigned remaining_occurrences[MAX_N] = {0};
    for (unsigned i = 0U; i < class_count; ++i)
        for (unsigned vertex = 0U; vertex < n; ++vertex)
            if ((classes[i].vertex_mask &
                 (UINT64_C(1) << vertex)) != 0U)
                ++remaining_occurrences[vertex];

    for (unsigned class_index = 0U; class_index < class_count; ++class_index) {
        const ColorClass *class_ = &classes[class_index];
        uint64_t forget_mask = 0U;
        for (unsigned vertex = 0U; vertex < n; ++vertex) {
            if ((class_->vertex_mask & (UINT64_C(1) << vertex)) == 0U)
                continue;
            if (remaining_occurrences[vertex] == 0U)
                die("invalid remaining-occurrence count");
            if (--remaining_occurrences[vertex] == 0U)
                forget_mask |= UINT64_C(1) << vertex;
        }
        StateMap next;
        map_initialize(&next, current.active + 16U);
        const uint64_t subset_count = UINT64_C(1) << class_->count;

        for (size_t slot = 0U; slot < current.capacity; ++slot) {
            const Entry *entry = &current.entries[slot];
            if ((entry->marked_key & OCCUPIED_BIT) == 0U ||
                entry->coefficient == 0)
                continue;
            const u128 initial = entry->marked_key & STATE_MASK;
            map_add(&next, state_forget(initial, forget_mask, n),
                    entry->coefficient);
            if (statistics->transitions == UINT64_MAX)
                die("transition statistic overflow");
            ++statistics->transitions;

            for (uint64_t subset = 0U; subset < subset_count; ++subset) {
                const unsigned selected = popcount_u64(subset);
                if (selected < 2U)
                    continue;
                u128 state = initial;
                bool possible = true;
                for (unsigned edge_index = 0U;
                     edge_index < class_->count; ++edge_index) {
                    if ((subset & (UINT64_C(1) << edge_index)) != 0U &&
                        !state_add_edge(&state, class_->edges[edge_index])) {
                        possible = false;
                        break;
                    }
                }
                if (!possible)
                    continue;
                const s128 weight = (selected & 1U)
                    ? (s128)(selected - 1U)
                    : -(s128)(selected - 1U);
                state = state_forget(state, forget_mask, n);
                map_add(&next, state,
                        checked_multiply(entry->coefficient, weight));
                if (statistics->transitions == UINT64_MAX)
                    die("transition statistic overflow");
                ++statistics->transitions;
            }
        }

        const size_t states = map_nonzero_count(&next);
        if (statistics->show_layers) {
            fprintf(stderr, "  color %u/%u: states=%zu, slots=%zu\n",
                    class_index + 1U, class_count, states, next.capacity);
        }
        if (states > statistics->peak_states)
            statistics->peak_states = states;
        if (next.capacity > statistics->peak_slots)
            statistics->peak_slots = next.capacity;
        map_release(&current);
        current = next;
    }

    s128 factorial[MAX_N + 1U];
    factorial[0] = 1;
    for (unsigned i = 1U; i <= n; ++i)
        factorial[i] = checked_multiply(factorial[i - 1U], (s128)i);

    s128 total = 0;
    for (size_t slot = 0U; slot < current.capacity; ++slot) {
        const Entry *entry = &current.entries[slot];
        if ((entry->marked_key & OCCUPIED_BIT) == 0U ||
            entry->coefficient == 0)
            continue;
        const u128 state = entry->marked_key & STATE_MASK;
        for (unsigned vertex = 0U; vertex < n; ++vertex) {
            const unsigned code = state_code(state, vertex);
            if (code != 0U)
                die("nonempty frontier after the last product class");
        }
        const unsigned edges = state_counter(state, EDGE_COUNT_SHIFT);
        const unsigned components =
            state_counter(state, COMPONENT_COUNT_SHIFT);
        if (edges >= n && n != 0U)
            die("linear forest has too many edges");
        s128 containing_paths = factorial[n - edges];
        for (unsigned i = 0U; i < components; ++i)
            containing_paths = checked_multiply(containing_paths, 2);
        total = checked_add(
            total, checked_multiply(entry->coefficient, containing_paths));
    }
    map_release(&current);
    if (total < 0 || total > factorial[n])
        die("transfer-DP result is outside 0..n!");
    return (u128)total;
}

/* Independent definition-level oracle for --self-test. */
static void direct_recursion(DirectSearch *search, unsigned depth,
                             unsigned last)
{
    if (depth == search->n) {
        if (search->count == ~(u128)0)
            die("direct-search count overflow");
        ++search->count;
        return;
    }
    for (unsigned vertex = 1U; vertex <= search->n; ++vertex) {
        const uint64_t bit = UINT64_C(1) << (vertex - 1U);
        if ((search->used_vertices & bit) != 0U)
            continue;
        const unsigned product = depth == 0U ? 0U : last * vertex;
        if (depth != 0U && search->used_product[product])
            continue;
        search->used_vertices |= bit;
        if (depth != 0U)
            search->used_product[product] = true;
        direct_recursion(search, depth + 1U, vertex);
        if (depth != 0U)
            search->used_product[product] = false;
        search->used_vertices ^= bit;
    }
}

static u128 calculate_direct(unsigned n)
{
    DirectSearch search;
    memset(&search, 0, sizeof(search));
    search.n = n;
    direct_recursion(&search, 0U, 0U);
    return search.count;
}

/* ---------------- Atomic, resumable b-file output ---------------- */

static bool parse_u128_token(const char *text, const char **end, u128 *value)
{
    if (*text < '0' || *text > '9')
        return false;
    const u128 maximum = ~(u128)0;
    u128 result = 0U;
    do {
        const unsigned digit = (unsigned)(*text - '0');
        if (result > (maximum - digit) / 10U)
            return false;
        result = 10U * result + digit;
        ++text;
    } while (*text >= '0' && *text <= '9');
    *end = text;
    *value = result;
    return true;
}

static char *path_with_suffix(const char *suffix)
{
    const size_t path_length = strlen(output_path);
    const size_t suffix_length = strlen(suffix);
    if (path_length > SIZE_MAX - suffix_length - 1U)
        die("output path is too long");
    char *path = malloc(path_length + suffix_length + 1U);
    if (path == NULL)
        die("cannot allocate a derived output path");
    memcpy(path, output_path, path_length);
    memcpy(path + path_length, suffix, suffix_length + 1U);
    return path;
}

static int lock_bfile(void)
{
    char *lock_path = path_with_suffix(".lock");
    const int descriptor = open(lock_path, O_RDWR | O_CREAT, 0666);
    free(lock_path);
    if (descriptor < 0)
        die("cannot open the b-file lock");
    struct flock lock = {.l_type = F_WRLCK, .l_whence = SEEK_SET};
    while (fcntl(descriptor, F_SETLKW, &lock) != 0) {
        if (errno != EINTR) {
            close(descriptor);
            die("cannot lock the b-file");
        }
    }
    return descriptor;
}

static void unlock_bfile(int descriptor)
{
    struct flock lock = {.l_type = F_UNLCK, .l_whence = SEEK_SET};
    if (fcntl(descriptor, F_SETLK, &lock) != 0 || close(descriptor) != 0)
        die("cannot unlock the b-file");
}

static unsigned read_bfile(u128 values[MAX_N + 1U])
{
    FILE *input = fopen(output_path, "r");
    if (input == NULL) {
        if (errno == ENOENT)
            return 0U;
        die("cannot read the b-file");
    }
    char line[256];
    unsigned next = 0U;
    while (fgets(line, sizeof(line), input) != NULL) {
        const size_t length = strlen(line);
        if (length == sizeof(line) - 1U && line[length - 1U] != '\n') {
            fclose(input);
            die("the b-file contains an overlong line");
        }
        char *cursor = line;
        while (*cursor == ' ' || *cursor == '\t')
            ++cursor;
        if (*cursor == '\0' || *cursor == '\n' || *cursor == '#')
            continue;
        errno = 0;
        char *index_end = NULL;
        const uintmax_t index = strtoumax(cursor, &index_end, 10);
        if (errno == ERANGE || index_end == cursor || index > MAX_N ||
            index != next || (*index_end != ' ' && *index_end != '\t')) {
            fclose(input);
            die("the b-file is malformed or has a gap");
        }
        cursor = index_end;
        while (*cursor == ' ' || *cursor == '\t')
            ++cursor;
        const char *value_end = NULL;
        u128 value;
        if (!parse_u128_token(cursor, &value_end, &value)) {
            fclose(input);
            die("the b-file contains an invalid value");
        }
        cursor = (char *)value_end;
        while (*cursor == ' ' || *cursor == '\t')
            ++cursor;
        if (*cursor == '\r')
            ++cursor;
        if (*cursor == '\n')
            ++cursor;
        if (*cursor != '\0') {
            fclose(input);
            die("the b-file contains trailing data");
        }
        values[next++] = value;
    }
    if (ferror(input) || fclose(input) != 0)
        die("cannot finish reading the b-file");
    return next;
}

static void validate_bfile(void)
{
    if (!write_bfile)
        return;
    u128 values[MAX_N + 1U];
    const int descriptor = lock_bfile();
    (void)read_bfile(values);
    unlock_bfile(descriptor);
}

static void record_term(unsigned n, u128 value)
{
    if (!write_bfile)
        return;
    u128 values[MAX_N + 1U];
    const int lock_descriptor = lock_bfile();
    unsigned prefix = read_bfile(values);
    if (n < prefix) {
        if (values[n] != value) {
            unlock_bfile(lock_descriptor);
            die("a computed term disagrees with the b-file");
        }
        unlock_bfile(lock_descriptor);
        return;
    }
    if (n != prefix) {
        unlock_bfile(lock_descriptor);
        die("cannot record a term after a b-file gap");
    }
    values[prefix++] = value;

    char *temporary = path_with_suffix(".tmp.XXXXXX");
    const int temporary_descriptor = mkstemp(temporary);
    if (temporary_descriptor < 0) {
        unlock_bfile(lock_descriptor);
        free(temporary);
        die("cannot create a temporary b-file");
    }
    FILE *output = fdopen(temporary_descriptor, "w");
    if (output == NULL) {
        close(temporary_descriptor);
        unlink(temporary);
        unlock_bfile(lock_descriptor);
        free(temporary);
        die("cannot open the temporary b-file stream");
    }
    bool failed = false;
    for (unsigned index = 0U; index < prefix; ++index) {
        char number[40];
        u128_text(values[index], number);
        if (fprintf(output, "%u %s\n", index, number) < 0)
            failed = true;
    }
    if (!failed && fflush(output) != 0)
        failed = true;
    if (!failed && fsync(fileno(output)) != 0)
        failed = true;
    if (fclose(output) != 0)
        failed = true;
    if (failed || rename(temporary, output_path) != 0) {
        unlink(temporary);
        unlock_bfile(lock_descriptor);
        free(temporary);
        die("cannot atomically update the b-file");
    }
    free(temporary);
    unlock_bfile(lock_descriptor);
}

static unsigned parse_n(const char *text, unsigned maximum,
                        const char *description)
{
    if (text == NULL || *text == '\0')
        die("empty integer argument");
    errno = 0;
    char *end = NULL;
    const uintmax_t value = strtoumax(text, &end, 10);
    if (errno == ERANGE || end == text || *end != '\0' || value > maximum) {
        fprintf(stderr, "error: %s must be in 0..%u: %s\n",
                description, maximum, text);
        exit(EXIT_FAILURE);
    }
    return (unsigned)value;
}

static double monotonic_seconds(void)
{
    struct timespec value;
    if (clock_gettime(CLOCK_MONOTONIC, &value) != 0)
        die("clock_gettime failed");
    return (double)value.tv_sec + 1.0e-9 * (double)value.tv_nsec;
}

static void print_term(unsigned n, u128 value)
{
    if (fprintf(stdout, "%u ", n) < 0)
        die("cannot write result index");
    print_u128(stdout, value);
    if (fputc('\n', stdout) == EOF)
        die("cannot finish result line");
}

static void run_self_test(unsigned maximum)
{
    for (unsigned n = 0U; n <= maximum; ++n) {
        Statistics statistics = {0};
        const u128 fast = calculate(n, &statistics);
        const u128 direct = calculate_direct(n);
        if (fast != direct) {
            fprintf(stderr, "self-test mismatch at n=%u: transfer-DP=", n);
            print_u128(stderr, fast);
            fprintf(stderr, ", direct=");
            print_u128(stderr, direct);
            fputc('\n', stderr);
            exit(EXIT_FAILURE);
        }
    }
    printf("self-test passed through n=%u\n", maximum);
}

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s [N] [--stats] [--output FILE|--no-bfile]\n"
            "       %s --target N [--stats] [--output FILE|--no-bfile]\n"
            "       %s --self-test [N]\n"
            "N defaults to %u and must be in 0..%u.\n"
            "Each completed term is recorded in %s by default.\n",
            program, program, program, DEFAULT_N, MAX_N, output_path);
}

int main(int argc, char **argv)
{
    unsigned maximum = DEFAULT_N;
    bool target_only = false;
    bool stats = false;
    bool self_test = false;
    bool have_n = false;
    bool output_option = false;
    bool no_bfile_option = false;

    for (int argument = 1; argument < argc; ++argument) {
        if (strcmp(argv[argument], "--help") == 0 ||
            strcmp(argv[argument], "-h") == 0) {
            usage(argv[0]);
            return EXIT_SUCCESS;
        }
        if (strcmp(argv[argument], "--stats") == 0) {
            if (stats)
                die("duplicate --stats");
            stats = true;
            continue;
        }
        if (strcmp(argv[argument], "--output") == 0) {
            if (output_option || argument + 1 >= argc ||
                argv[argument + 1][0] == '\0')
                die("invalid --output");
            output_option = true;
            output_path = argv[++argument];
            continue;
        }
        if (strcmp(argv[argument], "--no-bfile") == 0) {
            if (no_bfile_option)
                die("duplicate --no-bfile");
            no_bfile_option = true;
            continue;
        }
        if (strcmp(argv[argument], "--target") == 0) {
            if (target_only || self_test || have_n || argument + 1 >= argc)
                die("invalid --target");
            target_only = true;
            maximum = parse_n(argv[++argument], MAX_N, "target");
            have_n = true;
            continue;
        }
        if (strcmp(argv[argument], "--self-test") == 0) {
            if (self_test || target_only || have_n)
                die("invalid --self-test");
            self_test = true;
            maximum = DEFAULT_SELF_TEST_N;
            if (argument + 1 < argc && argv[argument + 1][0] != '-') {
                maximum = parse_n(argv[++argument], MAX_SELF_TEST_N,
                                  "self-test limit");
                have_n = true;
            }
            continue;
        }
        if (argv[argument][0] == '-' || have_n || target_only || self_test) {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
        maximum = parse_n(argv[argument], MAX_N, "N");
        have_n = true;
    }

    if (output_option && no_bfile_option)
        die("--output and --no-bfile are mutually exclusive");
    if (self_test) {
        if (output_option || stats)
            die("--output and --stats are not used with --self-test");
        run_self_test(maximum);
        return EXIT_SUCCESS;
    }
    write_bfile = !no_bfile_option;
    validate_bfile();

    const unsigned first = target_only ? maximum : 0U;
    for (unsigned n = first; n <= maximum; ++n) {
        const double started = monotonic_seconds();
        Statistics statistics = {0};
        statistics.show_layers = stats;
        const u128 value = calculate(n, &statistics);
        record_term(n, value);
        print_term(n, value);
        if (fflush(stdout) != 0)
            die("cannot flush result output");
        if (stats) {
            const double mib = (double)statistics.peak_slots *
                               (double)sizeof(Entry) / (1024.0 * 1024.0);
            if (fprintf(stderr,
                        "n=%u: %.3f s, colors=%u, transitions=%" PRIu64
                        ", peak states=%zu, largest table %.1f MiB\n",
                        n, monotonic_seconds() - started,
                        statistics.repeated_colors, statistics.transitions,
                        statistics.peak_states, mib) < 0)
                die("cannot write statistics");
        }
        if (n == maximum)
            break;
    }
    if (write_bfile &&
        fprintf(stderr, "b-file verified/updated: %s\n", output_path) < 0)
        die("cannot write the b-file completion message");
    return EXIT_SUCCESS;
}
