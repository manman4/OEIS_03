#define _POSIX_C_SOURCE 200809L

/* 399654_02.c
 *
 * Independent exact search for A399654.
 *
 * The cycle spectrum of a simple graph G is
 *
 *     CS(G) = { L : G contains a cycle of length L }.
 *
 * This program does NOT enumerate all graphs and does NOT use the block/ear
 * search of 399654_01.c.  It decides every candidate
 * S subseteq {3,...,n} by searching unions of cycles.
 *
 * Completeness argument.
 * ----------------------
 * Suppose S is nonempty and is realized on at most n vertices.  Among all
 * witnesses choose an edge-minimal graph W.  For every edge e of W, deleting
 * e must destroy all cycles of at least one length L(e) in S.  Thus e belongs
 * to every L(e)-cycle of W.  If one cycle C_L is selected in W for every
 * L in S, every edge e belongs to C_{L(e)}, and therefore
 *
 *                     W = union_{L in S} C_L.                 (1)
 *
 * Let M=max(S).  Relabel a selected M-cycle as 0,1,...,M-1,0.
 * Starting with that fixed cycle, whenever the current graph is missing a
 * length L in S, add the selected C_L.  Every intermediate graph is a
 * subgraph of W, so its spectrum is a subset of S.  Consequently the search
 * below, which tries every labelled L-cycle, contains a branch reaching W
 * (or an earlier graph whose spectrum is already S).
 *
 * Conversely, a returned graph is accepted only after its spectrum has been
 * recomputed and found equal to S.  A witness on p<n vertices can be padded
 * with isolated vertices, so searching graphs on the fixed vertex set [n]
 * also covers witnesses on at most n vertices.
 *
 * The only pruning rules are:
 *   - reject a graph as soon as its spectrum is not a subset of S;
 *   - memoize an exact labelled graph after its complete subtree failed;
 *   - do not exceed sum(S) edges, as follows directly from (1).
 * Hash-table eviction can only lose memoization hits, not solutions.
 *
 * This is intended first as a timing experiment and an independent check of
 * the ear-search implementation.  It prints values on stdout and progress/
 * elapsed time on stderr; it deliberately does not write result files yet.
 *
 * Compile:
 *   cc -O3 -march=native -std=c17 -Wall -Wextra -Wpedantic \
 *      -o 399654_02 399654_02.c
 *
 * Run:
 *   /usr/bin/time -p ./399654_02 10
 *
 * The implementation supports n<=12, but a different complete algorithm is
 * not automatically a fast one.  Test small n before attempting n=12.
 */

#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAXN 12
#define TT_BITS 21
#define TT_SIZE ((size_t) 1u << TT_BITS)
#define CYCLE_PROGRESS_INTERVAL UINT64_C(10000000)
#define NODE_PROGRESS_INTERVAL  UINT64_C(1000000)
#define SPECTRUM_PROGRESS_INTERVAL 16

typedef struct {
    uint64_t lo;
    uint64_t hi;
} EdgeSet;

typedef struct {
    uint64_t lo;
    uint64_t hi;
    uint32_t generation;
} TTEntry;

static TTEntry *tt;
static uint32_t generation;

static int current_n;
static int target;
static int target_index;
static int target_total;
static int edge_bound;
static int maximum_length;
static EdgeSet base_graph;
static EdgeSet edge_bit[MAXN][MAXN];

static uint16_t reach[1 << MAXN];
static int witness[MAXN];
static int witness_found;

static uint64_t nodes;
static uint64_t cycles_tried;
static uint64_t next_node_report;
static uint64_t next_cycle_report;
static double candidate_start;

static double wall_seconds(void)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }
    return (double) ts.tv_sec + 1e-9 * (double) ts.tv_nsec;
}

static EdgeSet edge_or(EdgeSet a, EdgeSet b)
{
    EdgeSet result = {a.lo | b.lo, a.hi | b.hi};
    return result;
}

static int edge_equal(EdgeSet a, EdgeSet b)
{
    return a.lo == b.lo && a.hi == b.hi;
}

static int edge_popcount(EdgeSet a)
{
    return __builtin_popcountll(a.lo) + __builtin_popcountll(a.hi);
}

static int edge_less(EdgeSet a, EdgeSet b)
{
    return a.hi < b.hi || (a.hi == b.hi && a.lo < b.lo);
}

static void initialize_edge_bits(void)
{
    int u, v, index = 0;

    memset(edge_bit, 0, sizeof edge_bit);
    for (u = 0; u < MAXN; u++) {
        for (v = u + 1; v < MAXN; v++, index++) {
            if (index < 64)
                edge_bit[u][v].lo = UINT64_C(1) << index;
            else
                edge_bit[u][v].hi = UINT64_C(1) << (index - 64);
            edge_bit[v][u] = edge_bit[u][v];
        }
    }
}

/* Compute all cycle lengths from scratch.  A path is recorded only when its
 * first vertex is its smallest vertex; this avoids irrelevant starts without
 * affecting existence. */
static int cycle_spectrum(const int *adj)
{
    int spec = 0;
    int full = 1 << current_n;
    int start;

    for (start = 0; start < current_n; start++) {
        int allowed = (full - 1) & ~((1 << start) - 1);
        int mask;

        memset(reach, 0, sizeof(uint16_t) * (size_t) full);
        reach[1 << start] = (uint16_t) (1u << start);
        for (mask = 1 << start; mask < full; mask++) {
            int endpoints = reach[mask];
            int length;

            if (endpoints == 0) continue;
            length = __builtin_popcount((unsigned int) mask);
            while (endpoints != 0) {
                int v = __builtin_ctz((unsigned int) endpoints);
                int neighbours;
                endpoints &= endpoints - 1;

                if (length >= 3 && ((adj[v] >> start) & 1))
                    spec |= 1 << length;
                neighbours = adj[v] & allowed & ~mask;
                while (neighbours != 0) {
                    int w = __builtin_ctz((unsigned int) neighbours);
                    neighbours &= neighbours - 1;
                    reach[mask | (1 << w)] |= (uint16_t) (1u << w);
                }
            }
        }
    }
    return spec;
}

static void format_set(char *buffer, size_t size, int set)
{
    size_t used = 0;
    int L;
    int first = 1;

    if (size == 0) return;
    buffer[0] = '\0';
    if (used + 1 < size) buffer[used++] = '{';
    for (L = 3; L <= current_n; L++) {
        int written;
        if (!((set >> L) & 1)) continue;
        written = snprintf(buffer + used, size - used, "%s%d",
                           first ? "" : ",", L);
        if (written < 0 || (size_t) written >= size - used) {
            buffer[size - 1] = '\0';
            return;
        }
        used += (size_t) written;
        first = 0;
    }
    if (used + 1 < size) buffer[used++] = '}';
    buffer[used < size ? used : size - 1] = '\0';
}

static void report_inner_progress(void)
{
    char set_text[4 * MAXN];
    int report = 0;

    if (nodes >= next_node_report) {
        while (next_node_report <= nodes)
            next_node_report += NODE_PROGRESS_INTERVAL;
        report = 1;
    }
    if (cycles_tried >= next_cycle_report) {
        while (next_cycle_report <= cycles_tried)
            next_cycle_report += CYCLE_PROGRESS_INTERVAL;
        report = 1;
    }
    if (!report) return;

    format_set(set_text, sizeof set_text, target);
    fprintf(stderr,
            "  [n=%d: candidate %d/%d (%.1f%%) %s: "
            "cycles=%" PRIu64 ", nodes=%" PRIu64 ", %.1f s]\n",
            current_n, target_index, target_total,
            100.0 * (double) target_index / (double) target_total,
            set_text, cycles_tried, nodes,
            wall_seconds() - candidate_start);
}

static size_t tt_slot(EdgeSet graph)
{
    uint64_t hash = graph.lo * UINT64_C(0x9E3779B97F4A7C15) ^
                    graph.hi * UINT64_C(0xC2B2AE3D27D4EB4F);
    hash ^= hash >> 29;
    hash *= UINT64_C(0x165667B19E3779F9);
    hash ^= hash >> 32;
    return (size_t) hash & (TT_SIZE - 1);
}

static int tt_contains(EdgeSet graph)
{
    TTEntry *entry = &tt[tt_slot(graph)];
    return entry->generation == generation &&
           entry->lo == graph.lo && entry->hi == graph.hi;
}

static void tt_store(EdgeSet graph)
{
    TTEntry *entry = &tt[tt_slot(graph)];
    entry->lo = graph.lo;
    entry->hi = graph.hi;
    entry->generation = generation;
}

static uint64_t cycle_count(int n, int length)
{
    uint64_t product = 1;
    int i;

    /* n! / (2*length*(n-length)!) =
     * (n choose length)*(length-1)!/2. */
    for (i = n - length + 1; i <= n; i++) product *= (uint64_t) i;
    return product / (2u * (uint64_t) length);
}

static int choose_missing_length(int spec)
{
    uint64_t best_count = UINT64_MAX;
    int best = -1;
    int L;

    for (L = 3; L <= current_n; L++) {
        uint64_t count;
        if (!((target >> L) & 1) || ((spec >> L) & 1)) continue;
        count = cycle_count(current_n, L);
        if (count < best_count) {
            best_count = count;
            best = L;
        }
    }
    return best;
}

static int solve_graph(const int *adj, EdgeSet graph, int edges, int level);

typedef struct {
    const int *parent_adj;
    EdgeSet parent_graph;
    int parent_edges;
    int length;
    int path[MAXN];
    int level;
    int success;
} CycleSearch;

typedef struct {
    const CycleSearch *search;
    EdgeSet original_child;
    int map[MAXN];
    int outside[MAXN];
    int outside_count;
    unsigned used_images;
} RootCanonical;

static int transformed_child_is_smaller(const RootCanonical *canonical)
{
    EdgeSet transformed = base_graph;
    int i;

    for (i = 0; i < canonical->search->length; i++) {
        int u = canonical->map[canonical->search->path[i]];
        int v = canonical->map[
            canonical->search->path[(i + 1) % canonical->search->length]];
        transformed = edge_or(transformed, edge_bit[u][v]);
    }
    return edge_less(transformed, canonical->original_child);
}

static int root_outside_maps_have_smaller(RootCanonical *canonical, int at)
{
    int image;

    if (at == canonical->outside_count)
        return transformed_child_is_smaller(canonical);
    for (image = maximum_length; image < current_n; image++) {
        unsigned bit = 1u << image;
        if (canonical->used_images & bit) continue;
        canonical->map[canonical->outside[at]] = image;
        canonical->used_images |= bit;
        if (root_outside_maps_have_smaller(canonical, at + 1)) return 1;
        canonical->used_images &= ~bit;
    }
    return 0;
}

/* At the first added cycle, quotient by every automorphism of the fixed base
 * C_M together with every permutation of the isolated vertices.  This only
 * chooses an orbit representative and cannot remove all solution branches. */
static int root_cycle_is_canonical(const CycleSearch *search,
                                   EdgeSet original_child)
{
    RootCanonical canonical;
    int seen_outside[MAXN] = {0};
    int orientation, shift, i;

    memset(&canonical, 0, sizeof canonical);
    canonical.search = search;
    canonical.original_child = original_child;
    for (i = 0; i < search->length; i++) {
        int v = search->path[i];
        if (v >= maximum_length && !seen_outside[v]) {
            seen_outside[v] = 1;
            canonical.outside[canonical.outside_count++] = v;
        }
    }

    for (orientation = 0; orientation < 2; orientation++) {
        for (shift = 0; shift < maximum_length; shift++) {
            for (i = 0; i < maximum_length; i++) {
                int image = orientation ? shift - i : shift + i;
                image %= maximum_length;
                if (image < 0) image += maximum_length;
                canonical.map[i] = image;
            }
            canonical.used_images = 0;
            if (root_outside_maps_have_smaller(&canonical, 0)) return 0;
        }
    }
    return 1;
}

static int try_completed_cycle(CycleSearch *search)
{
    int child_adj[MAXN];
    EdgeSet child_graph = search->parent_graph;
    int i;
    int child_edges;

    cycles_tried++;
    report_inner_progress();

    memcpy(child_adj, search->parent_adj, sizeof child_adj);
    for (i = 0; i < search->length; i++) {
        int u = search->path[i];
        int v = search->path[(i + 1) % search->length];
        child_graph = edge_or(child_graph, edge_bit[u][v]);
        child_adj[u] |= 1 << v;
        child_adj[v] |= 1 << u;
    }
    if (edge_equal(child_graph, search->parent_graph)) return 0;
    if (search->level == 0 &&
        !root_cycle_is_canonical(search, child_graph)) return 0;
    child_edges = edge_popcount(child_graph);
    if (child_edges > edge_bound) return 0;
    return solve_graph(child_adj, child_graph, child_edges, search->level + 1);
}

/* Enumerate every undirected cycle of the requested length exactly once:
 * path[0] is its smallest vertex and path[1] < path[length-1] chooses one
 * of the two orientations. */
static int enumerate_cycles(CycleSearch *search, int depth, unsigned used)
{
    int v;

    if (depth == search->length) {
        if (search->path[1] >= search->path[search->length - 1]) return 0;
        if (try_completed_cycle(search)) {
            search->success = 1;
            return 1;
        }
        return 0;
    }

    for (v = search->path[0] + 1; v < current_n; v++) {
        if ((used >> v) & 1u) continue;
        search->path[depth] = v;
        if (enumerate_cycles(search, depth + 1, used | (1u << v))) return 1;
    }
    return 0;
}

static int search_cycles_of_length(CycleSearch *search)
{
    int start;

    for (start = 0; start + search->length <= current_n; start++) {
        search->path[0] = start;
        if (enumerate_cycles(search, 1, 1u << start)) return 1;
    }
    return 0;
}

static int solve_graph(const int *adj, EdgeSet graph, int edges, int level)
{
    int spec;
    int missing;
    CycleSearch search;

    (void) edges;
    nodes++;
    report_inner_progress();

    if (tt_contains(graph)) return 0;
    spec = cycle_spectrum(adj);
    if (spec & ~target) {
        tt_store(graph);
        return 0;
    }
    if (spec == target) {
        memcpy(witness, adj, sizeof witness);
        witness_found = 1;
        return 1;
    }

    missing = choose_missing_length(spec);
    if (missing < 0) {
        tt_store(graph);
        return 0;
    }

    memset(&search, 0, sizeof search);
    search.parent_adj = adj;
    search.parent_graph = graph;
    search.parent_edges = edges;
    search.length = missing;
    search.level = level;
    if (search_cycles_of_length(&search)) return 1;

    tt_store(graph);
    return 0;
}

static int realizable(int set, int n)
{
    int adj[MAXN] = {0};
    EdgeSet graph = {0, 0};
    int maximum;
    int i;

    if (set == 0) return 1;
    maximum = 31 - __builtin_clz((unsigned int) set);
    if (maximum > n) return 0;

    current_n = n;
    target = set;
    maximum_length = maximum;
    edge_bound = 0;
    for (i = 3; i <= n; i++)
        if ((set >> i) & 1) edge_bound += i;
    if (edge_bound > n * (n - 1) / 2) edge_bound = n * (n - 1) / 2;

    if (++generation == 0) {
        memset(tt, 0, TT_SIZE * sizeof *tt);
        generation = 1;
    }
    nodes = 0;
    cycles_tried = 0;
    next_node_report = NODE_PROGRESS_INTERVAL;
    next_cycle_report = CYCLE_PROGRESS_INTERVAL;
    candidate_start = wall_seconds();
    witness_found = 0;

    for (i = 0; i < maximum; i++) {
        int u = i;
        int v = (i + 1) % maximum;
        adj[u] |= 1 << v;
        adj[v] |= 1 << u;
        graph = edge_or(graph, edge_bit[u][v]);
    }

    base_graph = graph;

    if (!solve_graph(adj, graph, maximum, 0)) return 0;
    if (!witness_found || cycle_spectrum(witness) != set) {
        fprintf(stderr, "internal error: invalid witness for spectrum %d\n", set);
        exit(EXIT_FAILURE);
    }
    return 1;
}

static void usage(const char *program)
{
    fprintf(stderr, "usage: %s [nmax]\n", program);
    fprintf(stderr, "  0 <= nmax <= %d (default 9)\n", MAXN);
}

int main(int argc, char **argv)
{
    int nmax = 9;
    int known[1 << (MAXN - 2)] = {0};
    int n;

    if (argc > 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    if (argc == 2) {
        char *end = NULL;
        long value;
        errno = 0;
        value = strtol(argv[1], &end, 10);
        if (errno != 0 || end == argv[1] || *end != '\0' ||
            value < 0 || value > MAXN) {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
        nmax = (int) value;
    }

    tt = calloc(TT_SIZE, sizeof *tt);
    if (tt == NULL) {
        perror("calloc transposition table");
        return EXIT_FAILURE;
    }
    initialize_edge_bits();
    known[0] = 1;

    for (n = 0; n <= nmax; n++) {
        double n_start = wall_seconds();
        int count = 1;
        int index;

        current_n = n;
        target_total = n >= 3 ? (1 << (n - 2)) - 1 : 0;
        for (index = 1; index <= target_total; index++) {
            int set = index << 3;
            target_index = index;
            if (!known[index] && realizable(set, n)) known[index] = 1;
            if (known[index]) count++;

            if (index % SPECTRUM_PROGRESS_INTERVAL == 0 ||
                index == target_total) {
                fprintf(stderr,
                        "  [n=%d: completed spectra %d/%d (%.1f%%), "
                        "realized=%d, %.1f s]\n",
                        n, index, target_total,
                        100.0 * (double) index / (double) target_total,
                        count, wall_seconds() - n_start);
            }
        }
        printf("%d %d\n", n, count);
        fflush(stdout);
        fprintf(stderr, "  [n=%d: %.2f s]\n", n, wall_seconds() - n_start);
    }

    free(tt);
    return EXIT_SUCCESS;
}
