/*
 * A397205 -- maximum |S|*|T| for product-distinct S,T subsets of [n].
 *
 * For distinct x,y put r(x,y)=max(x,y)/min(x,y), reduced to lowest terms.
 * A product collision
 *
 *                         s1*t1 = s2*t2
 *
 * with (s1,t1)!=(s2,t2) necessarily has s1!=s2 and t1!=t2, and is
 * equivalent to r(s1,s2)=r(t1,t2).  Thus, after S is fixed, make a graph
 * on [n] in which x and y are adjacent exactly when r(x,y) occurs among
 * pairs from S.  The admissible sets T are precisely its independent sets.
 *
 * The outer recursion enumerates S.  At every node an exact maximum-clique
 * search in the complement graph finds a maximum T.  Equal sets of ratios
 * give equal graphs, so their answers are memoized.  The outer upper bound
 * uses both monotonicity (adding to S can only shrink the possible T) and
 * symmetry between S and T.  Every pruning test is an upper bound; the
 * result is therefore exact.  The displayed OEIS witnesses through n=35 are
 * used only as initial lower bounds; every one is checked before use, and the
 * search still proves that no larger pair exists.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic \
 *       397205_01.c -o 397205_01
 *
 * Usage examples:
 *   ./397205_01 20 --progress
 *   ./397205_01 40 --start 36 --progress --cache-power 26
 *   ./397205_01 --term 23 --witness --no-bfile
 *   ./397205_01 --check --no-bfile
 *
 * A positional N computes a(1),...,a(N).  Completed exact terms are
 * atomically recorded in b397205_01.txt by default.  --term N requires the
 * preceding b-file prefix, unless --no-bfile is used.  The deliberate limit
 * n<=63 lets both S,T and graph neighborhoods fit in uint64_t.  Memoization
 * grows only through 2^P slots (P=26 by default); at 80% occupancy it freezes
 * new insertions but keeps all old entries usable.  Freezing changes speed,
 * never the exact result.  For n=38..40, P=26 is about 4.75 GiB.
 */

#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAX_N 63
#define MAX_RATIOS ((MAX_N * (MAX_N - 1)) / 2)
#define MAX_RATIO_WORDS ((MAX_RATIOS + 63) / 64)
#define DEFAULT_N 20
#define INITIAL_CACHE_CAPACITY 1024U
#define DEFAULT_CACHE_POWER 26U
#define MIN_CACHE_POWER 10U
#define MAX_CACHE_POWER 28U
#define CACHE_GROW_NUMERATOR 7U
#define CACHE_GROW_DENOMINATOR 10U
#define CACHE_FREEZE_NUMERATOR 8U
#define CACHE_FREEZE_DENOMINATOR 10U

typedef struct {
    uint64_t word[MAX_RATIO_WORDS];
} RatioMask;

typedef struct {
    uint8_t size;
    uint64_t set;
} MISResult;

typedef struct {
    size_t capacity;
    size_t max_capacity;
    size_t used;
    int words;
    bool frozen;
    uint32_t *hashes;
    uint64_t *keys;
    uint64_t *sets;
} MISCache;

typedef struct {
    int n;
    int ratio_count;
    int words;
    int *ratio_id;
    uint64_t *ratio_adjacency;
    MISCache cache;
    int chosen[MAX_N];
    int best;
    uint64_t best_s;
    uint64_t best_t;
    uint64_t dfs_nodes;
    uint64_t mis_nodes;
    double started;
    double next_heartbeat;
} Solver;

typedef struct {
    Solver *solver;
    uint64_t adjacency[MAX_N];
    int best_size;
    uint64_t best_set;
} CliqueSearch;

static const char *output_path = "b397205_01.txt";
static bool write_bfile = true;
static bool show_stats = false;
static bool show_witness = false;
static unsigned requested_cache_power = DEFAULT_CACHE_POWER;

static const int known[36] = {
    0,
    1, 2, 4, 6, 9, 12, 16, 20, 25, 28,
    35, 40, 48, 50, 55, 60, 72, 78, 91, 98,
    105, 105, 120, 128, 144, 144, 153, 162, 180, 190,
    210, 220, 231, 231, 242
};

/* Public witnesses linked from https://oeis.org/A397205.  These accelerate a
 * standalone --term request; they are not accepted without direct checking. */
static const uint64_t known_witness_s[36] = {
    UINT64_C(0x0), UINT64_C(0x1), UINT64_C(0x1), UINT64_C(0x3),
    UINT64_C(0x3), UINT64_C(0x7), UINT64_C(0xf), UINT64_C(0xf),
    UINT64_C(0x2f), UINT64_C(0x2f), UINT64_C(0xbf), UINT64_C(0xbf),
    UINT64_C(0x8bf), UINT64_C(0x8bf), UINT64_C(0xaff), UINT64_C(0xbff),
    UINT64_C(0x2bff), UINT64_C(0x2bff), UINT64_C(0x22bff),
    UINT64_C(0x22bff), UINT64_C(0xa2bff), UINT64_C(0x2ebff),
    UINT64_C(0x2ebff), UINT64_C(0x2ebff), UINT64_C(0x82ebff),
    UINT64_C(0x8aefff), UINT64_C(0x8aefff), UINT64_C(0x9a6bff),
    UINT64_C(0x9acfff), UINT64_C(0xbadfff), UINT64_C(0x209acfff),
    UINT64_C(0x20badfff), UINT64_C(0x28ba7fff), UINT64_C(0x1289a6fff),
    UINT64_C(0x1289a6fff), UINT64_C(0x52c8a6fff)
};

static const uint64_t known_witness_t[36] = {
    UINT64_C(0x0), UINT64_C(0x1), UINT64_C(0x3), UINT64_C(0x5),
    UINT64_C(0xd), UINT64_C(0x19), UINT64_C(0x31), UINT64_C(0x71),
    UINT64_C(0xd1), UINT64_C(0x1d1), UINT64_C(0x341), UINT64_C(0x741),
    UINT64_C(0x741), UINT64_C(0x1741), UINT64_C(0x3501),
    UINT64_C(0x7401), UINT64_C(0xd401), UINT64_C(0x1d401),
    UINT64_C(0x1d401), UINT64_C(0x5d401), UINT64_C(0x5d401),
    UINT64_C(0x1d1401), UINT64_C(0x1d1401), UINT64_C(0x5d1401),
    UINT64_C(0x5d1401), UINT64_C(0x1751001), UINT64_C(0x1751001),
    UINT64_C(0x5459401), UINT64_C(0xd651001), UINT64_C(0x1f450001),
    UINT64_C(0x1d651001), UINT64_C(0x5f450001), UINT64_C(0xd7450001),
    UINT64_C(0xd5651001), UINT64_C(0xd5651001), UINT64_C(0xd1751001)
};

static _Noreturn void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static void *checked_calloc(size_t count, size_t size)
{
    if (size != 0 && count > SIZE_MAX / size) die("allocation size overflow");
    if (count == 0) count = 1;
    if (size == 0) size = 1;
    void *result = calloc(count, size);
    if (result == NULL) die("out of memory");
    return result;
}

static int parse_n(const char *text)
{
    char *end = NULL;
    errno = 0;
    const long value = strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' ||
        value < 1 || value > MAX_N) {
        fprintf(stderr, "error: N must be in 1..%d: %s\n", MAX_N, text);
        exit(EXIT_FAILURE);
    }
    return (int)value;
}

static unsigned parse_cache_power(const char *text)
{
    char *end = NULL;
    errno = 0;
    const unsigned long value = strtoul(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' ||
        value < MIN_CACHE_POWER || value > MAX_CACHE_POWER) {
        fprintf(stderr, "error: cache power must be in %u..%u: %s\n",
                MIN_CACHE_POWER, MAX_CACHE_POWER, text);
        exit(EXIT_FAILURE);
    }
    return (unsigned)value;
}

static double now_seconds(void)
{
    struct timespec value;
    if (clock_gettime(CLOCK_MONOTONIC, &value) != 0)
        die("clock_gettime failed");
    return (double)value.tv_sec + (double)value.tv_nsec / 1e9;
}

static int gcd_int(int a, int b)
{
    while (b != 0) {
        const int remainder = a % b;
        a = b;
        b = remainder;
    }
    return a;
}

static unsigned popcount64(uint64_t value)
{
    return (unsigned)__builtin_popcountll(value);
}

static unsigned ctz64(uint64_t value)
{
    return (unsigned)__builtin_ctzll(value);
}

static uint64_t universe_mask(int n)
{
    return (UINT64_C(1) << (unsigned)n) - UINT64_C(1);
}

static uint64_t mix64(uint64_t x)
{
    x ^= x >> 30;
    x *= UINT64_C(0xbf58476d1ce4e5b9);
    x ^= x >> 27;
    x *= UINT64_C(0x94d049bb133111eb);
    return x ^ (x >> 31);
}

static uint32_t ratio_mask_hash(const RatioMask *mask, int words)
{
    uint64_t hash = UINT64_C(0xcbf29ce484222325);
    for (int i = 0; i < words; ++i) {
        hash ^= mask->word[i];
        hash *= UINT64_C(0x100000001b3);
        hash ^= hash >> 32;
    }
    hash = mix64(hash ^ (uint64_t)words);
    uint32_t fingerprint = (uint32_t)(hash ^ (hash >> 32));
    if (fingerprint == 0) fingerprint = 1;
    return fingerprint;
}

static bool cache_key_equal(const MISCache *cache, size_t slot,
                            const RatioMask *key)
{
    return memcmp(cache->keys + slot * (size_t)cache->words, key->word,
                  (size_t)cache->words * sizeof(uint64_t)) == 0;
}

static void cache_allocate(MISCache *cache, size_t capacity,
                           size_t max_capacity, int words)
{
    cache->capacity = capacity;
    cache->max_capacity = max_capacity;
    cache->used = 0;
    cache->words = words;
    cache->frozen = false;
    cache->hashes = checked_calloc(capacity, sizeof(*cache->hashes));
    cache->keys = checked_calloc(capacity * (size_t)words,
                                 sizeof(*cache->keys));
    cache->sets = checked_calloc(capacity, sizeof(*cache->sets));
}

static void cache_free(MISCache *cache)
{
    free(cache->hashes);
    free(cache->keys);
    free(cache->sets);
    memset(cache, 0, sizeof(*cache));
}

static void cache_place(MISCache *cache, uint32_t hash,
                        const uint64_t *key, MISResult value)
{
    const size_t mask = cache->capacity - 1U;
    size_t slot = (size_t)hash & mask;
    while (cache->hashes[slot] != 0) slot = (slot + 1U) & mask;
    cache->hashes[slot] = hash;
    memcpy(cache->keys + slot * (size_t)cache->words, key,
           (size_t)cache->words * sizeof(uint64_t));
    cache->sets[slot] = value.set;
    ++cache->used;
}

static void cache_grow(MISCache *cache)
{
    if (cache->capacity >= cache->max_capacity ||
        cache->capacity > SIZE_MAX / 2U)
        die("invalid MIS cache growth");
    MISCache grown;
    cache_allocate(&grown, 2U * cache->capacity, cache->max_capacity,
                   cache->words);
    for (size_t slot = 0; slot < cache->capacity; ++slot) {
        if (cache->hashes[slot] == 0) continue;
        const MISResult value = {
            (uint8_t)popcount64(cache->sets[slot]), cache->sets[slot]
        };
        cache_place(&grown, cache->hashes[slot],
                    cache->keys + slot * (size_t)cache->words, value);
    }
    cache_free(cache);
    *cache = grown;
}

static bool cache_get(const MISCache *cache, const RatioMask *key,
                      uint32_t hash, MISResult *value)
{
    const size_t mask = cache->capacity - 1U;
    size_t slot = (size_t)hash & mask;
    while (cache->hashes[slot] != 0) {
        if (cache->hashes[slot] == hash && cache_key_equal(cache, slot, key)) {
            value->set = cache->sets[slot];
            value->size = (uint8_t)popcount64(value->set);
            return true;
        }
        slot = (slot + 1U) & mask;
    }
    return false;
}

static void cache_put(MISCache *cache, const RatioMask *key,
                      uint32_t hash, MISResult value)
{
    if (cache->frozen) return;
    if (cache->capacity < cache->max_capacity &&
        (cache->used + 1U) * CACHE_GROW_DENOMINATOR >=
        cache->capacity * CACHE_GROW_NUMERATOR) {
        cache_grow(cache);
    }
    if (cache->capacity == cache->max_capacity &&
        (cache->used + 1U) * CACHE_FREEZE_DENOMINATOR >=
        cache->capacity * CACHE_FREEZE_NUMERATOR) {
        cache->frozen = true;
        return;
    }
    cache_place(cache, hash, key->word, value);
}

static void solver_build_ratios(Solver *solver)
{
    const int side = solver->n + 1;
    const size_t cells = (size_t)side * (size_t)side;
    int *canonical = checked_calloc(cells, sizeof(*canonical));
    solver->ratio_id = checked_calloc(cells, sizeof(*solver->ratio_id));
    for (size_t i = 0; i < cells; ++i) {
        canonical[i] = -1;
        solver->ratio_id[i] = -1;
    }

    for (int low = 1; low <= solver->n; ++low) {
        for (int high = low + 1; high <= solver->n; ++high) {
            const int divisor = gcd_int(low, high);
            const int denominator = low / divisor;
            const int numerator = high / divisor;
            const size_t ratio_cell =
                (size_t)numerator * (size_t)side + (size_t)denominator;
            if (canonical[ratio_cell] < 0)
                canonical[ratio_cell] = solver->ratio_count++;
            const int id = canonical[ratio_cell];
            solver->ratio_id[(size_t)low * (size_t)side + (size_t)high] = id;
            solver->ratio_id[(size_t)high * (size_t)side + (size_t)low] = id;
        }
    }
    free(canonical);
    solver->words = (solver->ratio_count + 63) / 64;
    if (solver->words > MAX_RATIO_WORDS) die("internal ratio-mask overflow");

    solver->ratio_adjacency = checked_calloc(
        (size_t)solver->ratio_count * (size_t)solver->n,
        sizeof(*solver->ratio_adjacency));
    for (int low = 1; low <= solver->n; ++low) {
        for (int high = low + 1; high <= solver->n; ++high) {
            const int id = solver->ratio_id[
                (size_t)low * (size_t)side + (size_t)high];
            uint64_t *adjacency = solver->ratio_adjacency +
                (size_t)id * (size_t)solver->n;
            adjacency[low - 1] |= UINT64_C(1) << (unsigned)(high - 1);
            adjacency[high - 1] |= UINT64_C(1) << (unsigned)(low - 1);
        }
    }
}

static void solver_free(Solver *solver)
{
    cache_free(&solver->cache);
    free(solver->ratio_id);
    free(solver->ratio_adjacency);
    memset(solver, 0, sizeof(*solver));
}

/* A proper coloring of the candidate subgraph bounds its clique number.
 * Vertices placed in one color class are pairwise nonadjacent. */
static int color_sort(const CliqueSearch *search, uint64_t candidates,
                      uint8_t order[MAX_N], uint8_t color_bound[MAX_N])
{
    int count = 0;
    uint8_t color = 0;
    uint64_t uncolored = candidates;
    while (uncolored != 0) {
        ++color;
        uint64_t available = uncolored;
        while (available != 0) {
            const unsigned vertex = ctz64(available);
            const uint64_t bit = UINT64_C(1) << vertex;
            order[count] = (uint8_t)vertex;
            color_bound[count++] = color;
            uncolored &= ~bit;
            available &= ~bit;
            available &= ~search->adjacency[vertex];
        }
    }
    return count;
}

static void maximum_clique_expand(CliqueSearch *search, uint64_t candidates,
                                  int current_size, uint64_t current_set)
{
    ++search->solver->mis_nodes;
    uint8_t order[MAX_N], color_bound[MAX_N];
    const int count = color_sort(search, candidates, order, color_bound);
    for (int i = count - 1; i >= 0; --i) {
        if (current_size + color_bound[i] <= search->best_size) return;
        const unsigned vertex = order[i];
        const uint64_t bit = UINT64_C(1) << vertex;
        const uint64_t next = candidates & search->adjacency[vertex];
        if (next != 0) {
            maximum_clique_expand(search, next, current_size + 1,
                                  current_set | bit);
        } else if (current_size + 1 > search->best_size) {
            search->best_size = current_size + 1;
            search->best_set = current_set | bit;
        }
        candidates &= ~bit;
    }
}

static uint64_t greedy_independent(const uint64_t conflict[MAX_N],
                                   uint64_t candidates)
{
    uint64_t result = 0;
    while (candidates != 0) {
        unsigned best_vertex = ctz64(candidates);
        unsigned best_degree =
            popcount64(conflict[best_vertex] & candidates);
        uint64_t scan = candidates & (candidates - 1U);
        while (scan != 0) {
            const unsigned vertex = ctz64(scan);
            scan &= scan - 1U;
            const unsigned degree = popcount64(conflict[vertex] & candidates);
            if (degree < best_degree) {
                best_degree = degree;
                best_vertex = vertex;
            }
        }
        const uint64_t bit = UINT64_C(1) << best_vertex;
        result |= bit;
        candidates &= ~bit;
        candidates &= ~conflict[best_vertex];
    }
    return result;
}

static MISResult maximum_independent_set(
    Solver *solver, const RatioMask *ratios,
    const uint64_t conflict[MAX_N])
{
    const uint32_t hash = ratio_mask_hash(ratios, solver->words);
    MISResult result;
    if (cache_get(&solver->cache, ratios, hash, &result)) return result;

    const uint64_t all = universe_mask(solver->n);
    const uint64_t greedy = greedy_independent(conflict, all);
    CliqueSearch search = {
        .solver = solver,
        .best_size = (int)popcount64(greedy),
        .best_set = greedy
    };
    for (int vertex = 0; vertex < solver->n; ++vertex) {
        search.adjacency[vertex] =
            all & ~conflict[vertex] &
            ~(UINT64_C(1) << (unsigned)vertex);
    }
    maximum_clique_expand(&search, all, 0, 0);
    result.size = (uint8_t)search.best_size;
    result.set = search.best_set;
    cache_put(&solver->cache, ratios, hash, result);
    return result;
}

static double cache_mebibytes(const MISCache *cache)
{
    const size_t bytes_per_slot =
        sizeof(uint32_t) + sizeof(uint64_t) +
        (size_t)cache->words * sizeof(uint64_t);
    return (double)cache->capacity * (double)bytes_per_slot /
           (1024.0 * 1024.0);
}

static void maybe_heartbeat(Solver *solver, int next, int chosen_count)
{
    if (!show_stats || (solver->dfs_nodes & UINT64_C(0x3ffff)) != 0) return;
    const double elapsed = now_seconds() - solver->started;
    if (elapsed < solver->next_heartbeat) return;
    fprintf(stderr,
            "397205_01: progress n=%d best=%d |S|=%d next=%d "
            "dfs=%" PRIu64 " mis=%" PRIu64 " cache=%zu/%zu "
            "(%.0f MiB)%s elapsed=%.1f s\n",
            solver->n, solver->best, chosen_count, next, solver->dfs_nodes,
            solver->mis_nodes, solver->cache.used, solver->cache.capacity,
            cache_mebibytes(&solver->cache),
            solver->cache.frozen ? " frozen" : "", elapsed);
    solver->next_heartbeat = elapsed + 10.0;
}

static bool graph_independent(const uint64_t adjacency[MAX_N], uint64_t set)
{
    uint64_t scan = set;
    while (scan != 0) {
        const unsigned vertex = ctz64(scan);
        scan &= scan - 1U;
        if ((adjacency[vertex] & set) != 0) return false;
    }
    return true;
}

/* One greedy coloring of the complement is a cheap rigorous upper bound on
 * the independence number of the conflict graph. */
static int independent_coloring_upper(Solver *solver,
                                      const uint64_t conflict[MAX_N])
{
    const uint64_t all = universe_mask(solver->n);
    CliqueSearch coloring = {.solver = solver};
    for (int vertex = 0; vertex < solver->n; ++vertex) {
        coloring.adjacency[vertex] =
            all & ~conflict[vertex] &
            ~(UINT64_C(1) << (unsigned)vertex);
    }
    uint8_t order[MAX_N], bounds[MAX_N];
    const int count = color_sort(&coloring, all, order, bounds);
    return count == 0 ? 0 : (int)bounds[count - 1];
}

static void search_subsets(Solver *solver, int next, int chosen_count,
                           const RatioMask *ratios,
                           const uint64_t conflict[MAX_N],
                           MISResult inherited_t, bool t_is_exact,
                           uint64_t s_mask)
{
    ++solver->dfs_nodes;
    maybe_heartbeat(solver, next, chosen_count);
    if (chosen_count > (int)inherited_t.size) return;
    const int remaining = solver->n - next + 1;
    const int maximum_s_size = chosen_count + remaining;
    const int bounded_t_size =
        (int)inherited_t.size < maximum_s_size ?
        (int)inherited_t.size : maximum_s_size;
    if (maximum_s_size * bounded_t_size <= solver->best) return;

    const MISResult t = t_is_exact ? inherited_t :
        maximum_independent_set(solver, ratios, conflict);
    /* By symmetry, enumerate only the orientation |S|<=|T|.  If this is
     * already false, descendants only enlarge S and add conflict edges, so
     * they cannot restore it.  The swapped orientation is searched elsewhere. */
    if (chosen_count > (int)t.size) return;
    const int product = chosen_count * (int)t.size;
    if (product > solver->best) {
        solver->best = product;
        solver->best_s = s_mask;
        solver->best_t = t.set;
    }

    const int maximum_t_size =
        (int)t.size < maximum_s_size ? (int)t.size : maximum_s_size;
    if (maximum_s_size * maximum_t_size <= solver->best || next > solver->n)
        return;

    RatioMask included;
    memcpy(included.word, ratios->word,
           (size_t)solver->words * sizeof(*included.word));
    uint64_t included_conflict[MAX_N];
    memcpy(included_conflict, conflict,
           (size_t)solver->n * sizeof(*included_conflict));
    const int side = solver->n + 1;
    for (int i = 0; i < chosen_count; ++i) {
        const int id = solver->ratio_id[
            (size_t)next * (size_t)side + (size_t)solver->chosen[i]];
        const uint64_t bit = UINT64_C(1) << (unsigned)(id & 63);
        if ((included.word[id >> 6] & bit) != 0) continue;
        included.word[id >> 6] |= bit;
        const uint64_t *source = solver->ratio_adjacency +
            (size_t)id * (size_t)solver->n;
        for (int vertex = 0; vertex < solver->n; ++vertex)
            included_conflict[vertex] |= source[vertex];
    }
    solver->chosen[chosen_count] = next;
    const bool included_t_is_exact =
        graph_independent(included_conflict, t.set);
    MISResult included_t = t;
    if (!included_t_is_exact) {
        const int upper = independent_coloring_upper(solver,
                                                     included_conflict);
        if (upper < (int)included_t.size) included_t.size = (uint8_t)upper;
    }
    search_subsets(solver, next + 1, chosen_count + 1, &included,
                   included_conflict, included_t, included_t_is_exact,
                   s_mask | (UINT64_C(1) << (unsigned)(next - 1)));
    search_subsets(solver, next + 1, chosen_count, ratios, conflict,
                   t, true, s_mask);
}

static bool product_distinct(int n, uint64_t s_mask, uint64_t t_mask)
{
    bool *seen = checked_calloc((size_t)n * (size_t)n + 1U, sizeof(*seen));
    bool distinct = true;
    for (int s = 1; s <= n && distinct; ++s) {
        if ((s_mask & (UINT64_C(1) << (unsigned)(s - 1))) == 0) continue;
        for (int t = 1; t <= n; ++t) {
            if ((t_mask & (UINT64_C(1) << (unsigned)(t - 1))) == 0) continue;
            if (seen[s * t]) {
                distinct = false;
                break;
            }
            seen[s * t] = true;
        }
    }
    free(seen);
    return distinct;
}

static void solver_seed(Solver *solver, uint64_t s_mask, uint64_t t_mask)
{
    const uint64_t all = universe_mask(solver->n);
    s_mask &= all;
    t_mask &= all;
    if (s_mask == 0 || t_mask == 0 ||
        !product_distinct(solver->n, s_mask, t_mask))
        return;
    const int product = (int)(popcount64(s_mask) * popcount64(t_mask));
    if (product > solver->best) {
        solver->best = product;
        solver->best_s = s_mask;
        solver->best_t = t_mask;
    }
}

static int solve_term(int n, uint64_t seed_s, uint64_t seed_t,
                      uint64_t *answer_s, uint64_t *answer_t)
{
    Solver solver = {.n = n};
    solver_build_ratios(&solver);
    const size_t maximum_cache_capacity =
        (size_t)1U << requested_cache_power;
    cache_allocate(&solver.cache, INITIAL_CACHE_CAPACITY,
                   maximum_cache_capacity, solver.words);
    solver_seed(&solver, seed_s, seed_t);
    const int witness_n = n < (int)(sizeof(known_witness_s) /
                                    sizeof(known_witness_s[0])) ? n : 35;
    solver_seed(&solver, known_witness_s[witness_n],
                known_witness_t[witness_n]);
    solver.started = now_seconds();
    solver.next_heartbeat = 10.0;
    if (show_stats) {
        fprintf(stderr,
                "397205_01: start n=%d lower_bound=%d ratios=%d "
                "cache_limit=2^%u (%.0f MiB)\n",
                n, solver.best, solver.ratio_count, requested_cache_power,
                (double)maximum_cache_capacity *
                    (sizeof(uint32_t) + sizeof(uint64_t) +
                     (size_t)solver.words * sizeof(uint64_t)) /
                    (1024.0 * 1024.0));
    }
    const RatioMask empty = {{0}};
    const uint64_t empty_conflict[MAX_N] = {0};
    const MISResult initial_t = {(uint8_t)n, universe_mask(n)};
    search_subsets(&solver, 1, 0, &empty, empty_conflict, initial_t, true, 0);

    if (!product_distinct(n, solver.best_s, solver.best_t))
        die("internal witness verification failed");
    if (n < (int)(sizeof(known) / sizeof(known[0])) &&
        solver.best != known[n]) {
        fprintf(stderr, "error: regression mismatch at n=%d: got %d, "
                        "expected %d\n", n, solver.best, known[n]);
        exit(EXIT_FAILURE);
    }
    if (show_stats) {
        fprintf(stderr,
                "397205_01: n=%d exact=%d ratios=%d dfs=%" PRIu64
                " mis=%" PRIu64 " cache=%zu/%zu (%.0f MiB)%s %.3f s\n",
                n, solver.best, solver.ratio_count, solver.dfs_nodes,
                solver.mis_nodes, solver.cache.used, solver.cache.capacity,
                cache_mebibytes(&solver.cache),
                solver.cache.frozen ? " frozen" : "",
                now_seconds() - solver.started);
    }
    *answer_s = solver.best_s;
    *answer_t = solver.best_t;
    const int answer = solver.best;
    solver_free(&solver);
    return answer;
}

static void print_set(FILE *stream, int n, uint64_t mask)
{
    fputc('{', stream);
    bool first = true;
    for (int value = 1; value <= n; ++value) {
        if ((mask & (UINT64_C(1) << (unsigned)(value - 1))) == 0) continue;
        if (!first) fputc(',', stream);
        fprintf(stream, "%d", value);
        first = false;
    }
    fputc('}', stream);
}

static int lock_bfile(void)
{
    char *path = malloc(strlen(output_path) + 6U);
    if (path == NULL) die("cannot allocate lock path");
    sprintf(path, "%s.lock", output_path);
    const int fd = open(path, O_RDWR | O_CREAT, 0666);
    free(path);
    if (fd < 0) die("cannot open b-file lock");
    struct flock lock = {.l_type = F_WRLCK, .l_whence = SEEK_SET};
    while (fcntl(fd, F_SETLKW, &lock) != 0) {
        if (errno != EINTR) die("cannot lock b-file");
    }
    return fd;
}

static void unlock_bfile(int fd)
{
    struct flock lock = {.l_type = F_UNLCK, .l_whence = SEEK_SET};
    if (fcntl(fd, F_SETLK, &lock) != 0 || close(fd) != 0)
        die("cannot unlock b-file");
}

/* Return the first missing index; A397205 starts at 1. */
static int read_bfile(int values[MAX_N + 1])
{
    FILE *input = fopen(output_path, "r");
    if (input == NULL) {
        if (errno == ENOENT) return 1;
        die("cannot read b-file");
    }
    char line[256];
    int next = 1;
    while (fgets(line, sizeof(line), input) != NULL) {
        char *p = line;
        while (*p == ' ' || *p == '\t') ++p;
        if (*p == '\0' || *p == '\n' || *p == '#') continue;
        int index, value;
        char extra;
        if (sscanf(p, "%d %d %c", &index, &value, &extra) != 2 ||
            index != next || index < 1 || index > MAX_N || value < 1) {
            fclose(input);
            die("b-file is malformed or has a gap");
        }
        values[next++] = value;
    }
    if (ferror(input) || fclose(input) != 0) die("cannot finish b-file read");
    return next;
}

static void require_recordable(int n)
{
    if (!write_bfile) return;
    int values[MAX_N + 1];
    const int fd = lock_bfile();
    const int next = read_bfile(values);
    unlock_bfile(fd);
    if (n > next) {
        fprintf(stderr,
                "error: %s ends before n=%d; compute the missing prefix "
                "first, or use --no-bfile\n", output_path, n);
        exit(EXIT_FAILURE);
    }
}

static void record_term(int n, int value)
{
    if (!write_bfile) return;
    int values[MAX_N + 1];
    const int lock_fd = lock_bfile();
    int next = read_bfile(values);
    if (n < next) {
        if (values[n] != value) die("computed value disagrees with b-file");
        unlock_bfile(lock_fd);
        return;
    }
    if (n != next) die("b-file gap while recording");
    values[next++] = value;

    char *temporary = malloc(strlen(output_path) + 32U);
    if (temporary == NULL) die("cannot allocate temporary path");
    sprintf(temporary, "%s.tmp.%ld", output_path, (long)getpid());
    FILE *output = fopen(temporary, "w");
    if (output == NULL) die("cannot create temporary b-file");
    bool failed = false;
    for (int i = 1; i < next; ++i)
        if (fprintf(output, "%d %d\n", i, values[i]) < 0) failed = true;
    if (!failed && fflush(output) != 0) failed = true;
    if (!failed && fsync(fileno(output)) != 0) failed = true;
    if (fclose(output) != 0) failed = true;
    if (failed || rename(temporary, output_path) != 0) {
        unlink(temporary);
        free(temporary);
        die("cannot atomically update b-file");
    }
    free(temporary);
    unlock_bfile(lock_fd);
}

static int brute_force_term(int n)
{
    const uint64_t limit = UINT64_C(1) << (unsigned)n;
    int best = 0;
    for (uint64_t s = 1; s < limit; ++s) {
        const int s_size = (int)popcount64(s);
        for (uint64_t t = 1; t < limit; ++t) {
            const int product = s_size * (int)popcount64(t);
            if (product > best && product_distinct(n, s, t)) best = product;
        }
    }
    return best;
}

static void self_check(void)
{
    for (int n = 1; n < (int)(sizeof(known) / sizeof(known[0])); ++n) {
        if (!product_distinct(n, known_witness_s[n], known_witness_t[n]) ||
            (int)(popcount64(known_witness_s[n]) *
                  popcount64(known_witness_t[n])) != known[n])
            die("invalid built-in witness");
    }
    uint64_t seed_s = 0, seed_t = 0;
    for (int n = 1; n <= 8; ++n) {
        uint64_t answer_s, answer_t;
        const int answer = solve_term(n, seed_s, seed_t,
                                      &answer_s, &answer_t);
        const int brute = brute_force_term(n);
        if (answer != brute) die("self-check disagrees with brute force");
        seed_s = answer_s;
        seed_t = answer_t;
    }
    fprintf(stderr, "397205_01: self-check passed for n=1..8\n");
}

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s [N] [--start N] [--progress] [--witness] "
            "[--cache-power P] [--output FILE|--no-bfile]\n"
            "       %s --term N [--progress] [--witness] "
            "[--cache-power P] [--output FILE|--no-bfile]\n"
            "       %s --check [--progress] [--cache-power P] [--no-bfile]\n"
            "--progress and --stats are synonyms; progress is printed "
            "about every 10 seconds.\n"
            "P must be in %u..%u; the default is %u.  A full cache stops "
            "growing without affecting exactness.\n",
            program, program, program, MIN_CACHE_POWER, MAX_CACHE_POWER,
            DEFAULT_CACHE_POWER);
}

int main(int argc, char **argv)
{
    int n = DEFAULT_N;
    int start_n = 1;
    bool positional = false, term_only = false, check = false;
    bool have_start = false;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--term") == 0) {
            if (term_only || positional || check || have_start || ++i == argc)
                die("invalid --term usage");
            term_only = true;
            n = parse_n(argv[i]);
        } else if (strcmp(argv[i], "--check") == 0) {
            if (term_only || positional || check || have_start)
                die("invalid --check usage");
            check = true;
        } else if (strcmp(argv[i], "--start") == 0) {
            if (term_only || check || have_start || ++i == argc)
                die("invalid --start usage");
            have_start = true;
            start_n = parse_n(argv[i]);
        } else if (strcmp(argv[i], "--stats") == 0 ||
                   strcmp(argv[i], "--progress") == 0) {
            show_stats = true;
        } else if (strcmp(argv[i], "--cache-power") == 0) {
            if (++i == argc) die("--cache-power needs a value");
            requested_cache_power = parse_cache_power(argv[i]);
        } else if (strcmp(argv[i], "--witness") == 0) {
            show_witness = true;
        } else if (strcmp(argv[i], "--output") == 0) {
            if (++i == argc || argv[i][0] == '\0') die("--output needs a file");
            output_path = argv[i];
            write_bfile = true;
        } else if (strcmp(argv[i], "--no-bfile") == 0) {
            write_bfile = false;
        } else if (strcmp(argv[i], "--help") == 0 ||
                   strcmp(argv[i], "-h") == 0) {
            usage(argv[0]);
            return EXIT_SUCCESS;
        } else if (argv[i][0] == '-') {
            usage(argv[0]);
            return EXIT_FAILURE;
        } else {
            if (positional || term_only || check) die("multiple N arguments");
            positional = true;
            n = parse_n(argv[i]);
        }
    }

    if (check) {
        self_check();
        return EXIT_SUCCESS;
    }
    if (have_start && start_n > n) die("--start must not exceed N");

    const int first = term_only ? n : start_n;
    uint64_t seed_s = 0, seed_t = 0;
    for (int k = first; k <= n; ++k) {
        require_recordable(k);
        uint64_t answer_s, answer_t;
        const int answer = solve_term(k, seed_s, seed_t,
                                      &answer_s, &answer_t);
        printf("%d %d\n", k, answer);
        if (fflush(stdout) != 0) die("cannot flush result");
        if (show_witness) {
            fprintf(stderr, "397205_01: n=%d S=", k);
            print_set(stderr, k, answer_s);
            fprintf(stderr, " T=");
            print_set(stderr, k, answer_t);
            fputc('\n', stderr);
        }
        record_term(k, answer);
        seed_s = answer_s;
        seed_t = answer_t;
    }
    return EXIT_SUCCESS;
}
