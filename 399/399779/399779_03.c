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

/*
 * Optimized exact computation for Erdos problem #793 by minimum hitting-set
 * search.  The simpler 399779_02.c is retained as a reference implementation.
 *
 * F(n) is the maximum size of A subseteq {1,...,n} such that a does not
 * divide b*c whenever a,b,c are in A, a != b, and a != c.  The factors b
 * and c may be equal.  This is the strongly 2-primitive convention; it is
 * different from the newer 2-primitive convention in which b and c must be
 * distinct.
 *
 * A set is invalid precisely when it contains one of these hyperedges:
 *
 *   {a,b},       if a | b^2 or b | a^2;
 *   {a,b,c},     if a,b,c are distinct and one divides the product of the
 *                other two.
 *
 * Triples containing a forbidden pair are redundant and are omitted.  The
 * complement of a maximum valid set is a minimum hitting set of the resulting
 * rank-at-most-three hypergraph.  The search branches on an unhit hyperedge.
 * Incidence lists update hit edges and unit constraints incrementally, with a
 * reversible trail.  A greedy packing of disjoint unhit hyperedges gives a
 * rigorous lower bound on the number of further removals.
 *
 * For consecutive n, the exact preceding value supplies the safe bound
 *
 *   F(n) <= F(n-1) + 1,
 *
 * because deleting n from any valid subset of {1,...,n} leaves a valid subset
 * of {1,...,n-1}.  The first n requested never assumes an unavailable prior
 * value.  The optional --check mode scans all subsets for n <= 18 and tests
 * the original divisibility condition directly.
 *
 * Compile:
 *   cc -O3 -march=native -std=c17 -Wall -Wextra -Wpedantic \
 *      -o 399779_03 399779_03.c
 *   ./399779_03 --progress 60
 *   ./399779_03 --check --no-bfile 18
 *
 * A normal run writes b-file lines both to standard output and, atomically,
 * to b399779_03.txt.  Existing output is preserved unless --force is given.
 * Use --output FILE to choose another path or --no-bfile to suppress the
 * file while retaining standard output.
 *
 * References:
 *   https://www.erdosproblems.com/793
 *   https://www.ulam.ai/research/erdos793.pdf
 *   Related distinct-factor convention:
 *     https://math.dartmouth.edu/~carlp/4695pomerance.pdf
 *   Earlier work on the general distinct-factor problem:
 *     https://doi.org/10.1016/j.ejc.2010.11.010
 */

#define PROGRAM_NAME "399779_03"
#define DEFAULT_BFILE "b399779_03.txt"
#define DEFAULT_MAX_N 20
#define MAX_N 127
#define DIRECT_CHECK_MAX_N 18
#define HEARTBEAT_NODE_MASK ((UINT64_C(1) << 20) - UINT64_C(1))

__extension__ typedef unsigned __int128 Mask;

typedef struct {
    Mask mask;
} Constraint;

typedef enum {
    TRAIL_REMOVED_VERTEX,
    TRAIL_KEPT_VERTEX,
    TRAIL_HIT_EDGE,
    TRAIL_AVAILABLE_EDGE
} TrailKind;

typedef struct {
    size_t index;
    TrailKind kind;
} TrailEntry;

typedef struct {
    int n;
    Mask full_mask;
    Constraint *constraints;
    size_t constraint_count;
    unsigned degree[MAX_N];

    size_t incidence_offset[MAX_N + 1];
    size_t *incidence;
    uint8_t *edge_hit;
    uint8_t *edge_available;
    size_t *unit_queue;
    size_t unit_count;
    TrailEntry *trail;
    size_t trail_count;
    size_t trail_capacity;
    Mask removed_mask;
    Mask kept_mask;
    int removed_count;

    int best_removed;
    Mask best_removed_mask;
    int absolute_lower_removed;
    bool optimum_reached;
    uint64_t nodes;
    bool progress;
    double started;
    double next_report;
} Solver;

typedef struct {
    int min_n;
    int max_n;
    bool witness;
    bool progress;
    bool check;
    bool write_bfile;
    bool force;
    const char *output_path;
} Options;

static char *active_temporary_path = NULL;

static void remove_temporary_output(void)
{
    if (active_temporary_path != NULL) {
        (void)unlink(active_temporary_path);
    }
}

static void die(const char *message)
{
    fprintf(stderr, "%s: %s\n", PROGRAM_NAME, message);
    exit(EXIT_FAILURE);
}

static double now_seconds(void)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }
    return (double)ts.tv_sec + 1.0e-9 * (double)ts.tv_nsec;
}

static int popcount64(uint64_t value)
{
    return __builtin_popcountll((unsigned long long)value);
}

static int popcount_mask(Mask value)
{
    return popcount64((uint64_t)value) +
           popcount64((uint64_t)(value >> 64));
}

static int ctz_mask(Mask value)
{
    const uint64_t low = (uint64_t)value;
    if (low != 0) {
        return __builtin_ctzll((unsigned long long)low);
    }
    return 64 + __builtin_ctzll((unsigned long long)(value >> 64));
}

static Mask bit_for(int value)
{
    return (Mask)1 << (value - 1);
}

static bool is_prime(int value)
{
    if (value < 2) {
        return false;
    }
    for (int d = 2; d <= value / d; ++d) {
        if (value % d == 0) {
            return false;
        }
    }
    return true;
}

static int prime_count(int n)
{
    int count = 0;
    for (int value = 2; value <= n; ++value) {
        count += is_prime(value) ? 1 : 0;
    }
    return count;
}

static bool forbidden_pair(int a, int b)
{
    const int aa = a * a;
    const int bb = b * b;
    return bb % a == 0 || aa % b == 0;
}

static bool forbidden_triple(int a, int b, int c)
{
    return (b * c) % a == 0 ||
           (a * c) % b == 0 ||
           (a * b) % c == 0;
}

static size_t maximum_constraint_count(int n)
{
    const size_t nn = (size_t)n;
    return nn * (nn - 1U) / 2U +
           nn * (nn - 1U) * (nn - 2U) / 6U;
}

static void add_constraint(Solver *solver, size_t capacity, Mask mask)
{
    if (solver->constraint_count >= capacity) {
        die("internal constraint-capacity overflow");
    }
    solver->constraints[solver->constraint_count++].mask = mask;

    Mask work = mask;
    while (work != 0) {
        const int vertex = ctz_mask(work);
        ++solver->degree[vertex];
        work &= work - (Mask)1;
    }
}

static void build_constraints(Solver *solver, int n)
{
    memset(solver, 0, sizeof(*solver));
    solver->n = n;
    solver->full_mask = ((Mask)1 << n) - (Mask)1;

    const size_t capacity = maximum_constraint_count(n);
    solver->constraints = calloc(capacity == 0 ? 1U : capacity,
                                 sizeof(*solver->constraints));
    if (solver->constraints == NULL) {
        die("unable to allocate the constraint table");
    }

    bool pair[MAX_N + 1][MAX_N + 1] = {{false}};
    for (int a = 1; a <= n; ++a) {
        for (int b = a + 1; b <= n; ++b) {
            if (forbidden_pair(a, b)) {
                pair[a][b] = true;
                add_constraint(solver, capacity,
                               bit_for(a) | bit_for(b));
            }
        }
    }

    for (int a = 1; a <= n; ++a) {
        for (int b = a + 1; b <= n; ++b) {
            for (int c = b + 1; c <= n; ++c) {
                if (pair[a][b] || pair[a][c] || pair[b][c]) {
                    continue;
                }
                if (forbidden_triple(a, b, c)) {
                    add_constraint(solver, capacity,
                                   bit_for(a) | bit_for(b) | bit_for(c));
                }
            }
        }
    }
}

static void initialize_incremental_state(Solver *solver)
{
    solver->incidence_offset[0] = 0;
    for (int vertex = 0; vertex < solver->n; ++vertex) {
        solver->incidence_offset[vertex + 1] =
            solver->incidence_offset[vertex] + solver->degree[vertex];
    }
    const size_t incidence_count = solver->incidence_offset[solver->n];
    solver->incidence = malloc((incidence_count == 0 ? 1U : incidence_count) *
                               sizeof(*solver->incidence));
    solver->edge_hit = calloc(solver->constraint_count == 0 ? 1U :
                              solver->constraint_count,
                              sizeof(*solver->edge_hit));
    solver->edge_available = malloc((solver->constraint_count == 0 ? 1U :
                                     solver->constraint_count) *
                                    sizeof(*solver->edge_available));
    solver->unit_queue = malloc((solver->constraint_count == 0 ? 1U :
                                 solver->constraint_count) *
                                sizeof(*solver->unit_queue));
    if (solver->constraint_count >
        (SIZE_MAX - (size_t)solver->n) / 4U) {
        die("incremental trail capacity overflow");
    }
    solver->trail_capacity = (size_t)solver->n +
                             4U * solver->constraint_count;
    solver->trail = malloc((solver->trail_capacity == 0 ? 1U :
                            solver->trail_capacity) * sizeof(*solver->trail));
    if (solver->incidence == NULL || solver->edge_hit == NULL ||
        solver->edge_available == NULL || solver->unit_queue == NULL ||
        solver->trail == NULL) {
        die("unable to allocate incremental search tables");
    }

    size_t cursor[MAX_N];
    for (int vertex = 0; vertex < solver->n; ++vertex) {
        cursor[vertex] = solver->incidence_offset[vertex];
    }
    for (size_t edge_index = 0;
         edge_index < solver->constraint_count; ++edge_index) {
        Mask work = solver->constraints[edge_index].mask;
        solver->edge_available[edge_index] = (uint8_t)popcount_mask(work);
        while (work != 0) {
            const int vertex = ctz_mask(work);
            solver->incidence[cursor[vertex]++] = edge_index;
            work &= work - (Mask)1;
        }
    }

    solver->unit_count = 0;
    solver->trail_count = 0;
    solver->removed_mask = 0;
    solver->kept_mask = 0;
    solver->removed_count = 0;
}

static void free_solver(Solver *solver)
{
    free(solver->constraints);
    free(solver->incidence);
    free(solver->edge_hit);
    free(solver->edge_available);
    free(solver->unit_queue);
    free(solver->trail);
}

static bool valid_by_constraints(const Solver *solver, Mask selected)
{
    for (size_t i = 0; i < solver->constraint_count; ++i) {
        const Mask edge = solver->constraints[i].mask;
        if ((selected & edge) == edge) {
            return false;
        }
    }
    return true;
}

/* Test the original quantified condition, independently of the hyperedges. */
static bool valid_direct(Mask selected, int n)
{
    for (int a = 1; a <= n; ++a) {
        if ((selected & bit_for(a)) == 0) {
            continue;
        }
        for (int b = 1; b <= n; ++b) {
            if (b == a || (selected & bit_for(b)) == 0) {
                continue;
            }
            for (int c = b; c <= n; ++c) {
                if (c == a || (selected & bit_for(c)) == 0) {
                    continue;
                }
                if ((b * c) % a == 0) {
                    return false;
                }
            }
        }
    }
    return true;
}

static void seed_from_order(Solver *solver, const int order[MAX_N])
{
    Mask selected = 0;
    for (int i = 0; i < solver->n; ++i) {
        const Mask candidate = selected | bit_for(order[i]);
        if (valid_by_constraints(solver, candidate)) {
            selected = candidate;
        }
    }
    const int removed = solver->n - popcount_mask(selected);
    if (removed < solver->best_removed) {
        solver->best_removed = removed;
        solver->best_removed_mask = solver->full_mask ^ selected;
    }
}

static void seed_upper_bound(Solver *solver)
{
    Mask primes = 0;
    for (int value = 1; value <= solver->n; ++value) {
        if (is_prime(value)) {
            primes |= bit_for(value);
        }
    }
    solver->best_removed = solver->n - popcount_mask(primes);
    solver->best_removed_mask = solver->full_mask ^ primes;

    int order[MAX_N];
    for (int i = 0; i < solver->n; ++i) {
        order[i] = i + 1;
    }
    seed_from_order(solver, order);

    for (int i = 0; i < solver->n; ++i) {
        order[i] = solver->n - i;
    }
    seed_from_order(solver, order);

    for (int i = 0; i < solver->n; ++i) {
        order[i] = i + 1;
    }
    for (int i = 1; i < solver->n; ++i) {
        const int value = order[i];
        int j = i;
        while (j > 0) {
            const int left = order[j - 1];
            const unsigned dl = solver->degree[left - 1];
            const unsigned dv = solver->degree[value - 1];
            if (dl < dv || (dl == dv && left < value)) {
                break;
            }
            order[j] = left;
            --j;
        }
        order[j] = value;
    }
    seed_from_order(solver, order);
}

static void seed_from_mask(Solver *solver, Mask selected)
{
    if ((selected & ~solver->full_mask) != 0 ||
        !valid_by_constraints(solver, selected)) {
        die("invalid carried seed set");
    }
    const int removed = solver->n - popcount_mask(selected);
    if (removed < solver->best_removed) {
        solver->best_removed = removed;
        solver->best_removed_mask = solver->full_mask ^ selected;
    }
}

static void report_progress(Solver *solver)
{
    if (!solver->progress ||
        (solver->nodes & HEARTBEAT_NODE_MASK) != UINT64_C(0)) {
        return;
    }
    const double elapsed = now_seconds() - solver->started;
    if (elapsed < solver->next_report) {
        return;
    }
    fprintf(stderr,
            "%s: n=%d current F(n)=%d nodes=%" PRIu64
            " elapsed=%.1f s\n",
            PROGRAM_NAME, solver->n, solver->n - solver->best_removed,
            solver->nodes, elapsed);
    solver->next_report = elapsed + 10.0;
}

static int edge_score(const Solver *solver, Mask candidates)
{
    int score = 0;
    while (candidates != 0) {
        const int vertex = ctz_mask(candidates);
        score += (int)solver->degree[vertex];
        candidates &= candidates - (Mask)1;
    }
    return score;
}

static void push_trail(Solver *solver, TrailKind kind, size_t index)
{
    if (solver->trail_count >= solver->trail_capacity) {
        die("internal incremental-trail overflow");
    }
    solver->trail[solver->trail_count++] = (TrailEntry){index, kind};
}

static bool assign_removed(Solver *solver, int vertex)
{
    const Mask bit = (Mask)1 << vertex;
    if ((solver->removed_mask & bit) != 0) {
        return true;
    }
    if ((solver->kept_mask & bit) != 0) {
        return false;
    }

    push_trail(solver, TRAIL_REMOVED_VERTEX, (size_t)vertex);
    solver->removed_mask |= bit;
    ++solver->removed_count;

    for (size_t position = solver->incidence_offset[vertex];
         position < solver->incidence_offset[vertex + 1]; ++position) {
        const size_t edge_index = solver->incidence[position];
        if (solver->edge_hit[edge_index] == 0) {
            push_trail(solver, TRAIL_HIT_EDGE, edge_index);
            solver->edge_hit[edge_index] = 1;
        }
    }
    return true;
}

static bool assign_kept(Solver *solver, int vertex)
{
    const Mask bit = (Mask)1 << vertex;
    if ((solver->kept_mask & bit) != 0) {
        return true;
    }
    if ((solver->removed_mask & bit) != 0) {
        return false;
    }

    push_trail(solver, TRAIL_KEPT_VERTEX, (size_t)vertex);
    solver->kept_mask |= bit;

    for (size_t position = solver->incidence_offset[vertex];
         position < solver->incidence_offset[vertex + 1]; ++position) {
        const size_t edge_index = solver->incidence[position];
        if (solver->edge_hit[edge_index] != 0) {
            continue;
        }
        if (solver->edge_available[edge_index] == 0) {
            die("internal available-count underflow");
        }
        push_trail(solver, TRAIL_AVAILABLE_EDGE, edge_index);
        --solver->edge_available[edge_index];
        if (solver->edge_available[edge_index] == 0) {
            return false;
        }
        if (solver->edge_available[edge_index] == 1) {
            if (solver->unit_count >= solver->constraint_count) {
                die("internal unit-queue overflow");
            }
            solver->unit_queue[solver->unit_count++] = edge_index;
        }
    }
    return true;
}

static void rollback(Solver *solver, size_t trail_checkpoint,
                     size_t queue_checkpoint)
{
    while (solver->trail_count > trail_checkpoint) {
        const TrailEntry entry = solver->trail[--solver->trail_count];
        switch (entry.kind) {
        case TRAIL_REMOVED_VERTEX:
            solver->removed_mask &= ~((Mask)1 << entry.index);
            --solver->removed_count;
            break;
        case TRAIL_KEPT_VERTEX:
            solver->kept_mask &= ~((Mask)1 << entry.index);
            break;
        case TRAIL_HIT_EDGE:
            solver->edge_hit[entry.index] = 0;
            break;
        case TRAIL_AVAILABLE_EDGE:
            ++solver->edge_available[entry.index];
            break;
        }
    }
    solver->unit_count = queue_checkpoint;
}

static bool propagate_units(Solver *solver)
{
    while (solver->unit_count != 0) {
        const size_t edge_index =
            solver->unit_queue[--solver->unit_count];
        if (solver->edge_hit[edge_index] != 0 ||
            solver->edge_available[edge_index] != 1) {
            continue;
        }
        const Mask undecided = solver->constraints[edge_index].mask &
            ~(solver->removed_mask | solver->kept_mask);
        if (popcount_mask(undecided) != 1) {
            die("internal unit-constraint inconsistency");
        }
        const int vertex = ctz_mask(undecided);
        if (!assign_removed(solver, vertex) ||
            solver->removed_count >= solver->best_removed) {
            return false;
        }
    }
    return true;
}

static void search(Solver *solver)
{
    if (solver->optimum_reached) {
        return;
    }
    if (solver->nodes != UINT64_MAX) {
        ++solver->nodes;
    }
    report_progress(solver);

    if (solver->removed_count >= solver->best_removed) {
        return;
    }

    const size_t entry_trail = solver->trail_count;
    const size_t entry_queue = solver->unit_count;
    if (!propagate_units(solver)) {
        rollback(solver, entry_trail, entry_queue);
        return;
    }

    Mask chosen = 0;
    int chosen_count = INT_MAX;
    int chosen_score = -1;
    Mask packed = 0;
    int lower_bound = 0;

    for (size_t i = 0; i < solver->constraint_count; ++i) {
        if (solver->edge_hit[i] != 0) {
            continue;
        }
        const Mask edge = solver->constraints[i].mask;
        const Mask candidates = edge &
            ~(solver->removed_mask | solver->kept_mask);
        const int count = popcount_mask(candidates);
        if (count == 0 || count != solver->edge_available[i]) {
            die("internal incremental edge-state inconsistency");
        }
        const int score = edge_score(solver, candidates);
        if (count < chosen_count ||
            (count == chosen_count && score > chosen_score)) {
            chosen = candidates;
            chosen_count = count;
            chosen_score = score;
        }
        if ((candidates & packed) == 0) {
            packed |= candidates;
            ++lower_bound;
        }
    }

    if (chosen == 0) {
        solver->best_removed = solver->removed_count;
        solver->best_removed_mask = solver->removed_mask;
        solver->optimum_reached =
            solver->best_removed == solver->absolute_lower_removed;
        if (solver->progress) {
            fprintf(stderr,
                    "%s: improved n=%d F(n)=%d removed=%d nodes=%" PRIu64
                    "\n",
                    PROGRAM_NAME, solver->n,
                    solver->n - solver->removed_count,
                    solver->removed_count, solver->nodes);
        }
        rollback(solver, entry_trail, entry_queue);
        return;
    }

    if (solver->removed_count + lower_bound >= solver->best_removed) {
        rollback(solver, entry_trail, entry_queue);
        return;
    }

    int vertices[3];
    int count = 0;
    while (chosen != 0) {
        const int vertex = ctz_mask(chosen);
        vertices[count++] = vertex;
        chosen &= chosen - (Mask)1;
    }
    for (int i = 1; i < count; ++i) {
        const int vertex = vertices[i];
        int j = i;
        while (j > 0 &&
               solver->degree[vertices[j - 1]] < solver->degree[vertex]) {
            vertices[j] = vertices[j - 1];
            --j;
        }
        vertices[j] = vertex;
    }

    /* Classify every solution by the first removed vertex of the chosen edge.
     * Vertices earlier in that order can therefore be declared kept. */
    for (int i = 0; i < count; ++i) {
        const size_t branch_trail = solver->trail_count;
        const size_t branch_queue = solver->unit_count;
        bool consistent = true;
        for (int j = 0; j < i && consistent; ++j) {
            consistent = assign_kept(solver, vertices[j]);
        }
        if (consistent) {
            consistent = assign_removed(solver, vertices[i]);
        }
        if (consistent) {
            search(solver);
        }
        rollback(solver, branch_trail, branch_queue);
        if (solver->optimum_reached) {
            break;
        }
    }
    rollback(solver, entry_trail, entry_queue);
}

static int direct_maximum(int n, uint64_t *witness)
{
    const uint64_t end = UINT64_C(1) << n;
    int best = 0;
    uint64_t best_mask = 0;
    for (uint64_t selected = 0; selected < end; ++selected) {
        const int size = popcount64(selected);
        if (size > best && valid_direct((Mask)selected, n)) {
            best = size;
            best_mask = selected;
        }
    }
    *witness = best_mask;
    return best;
}

static int solve_one(int n, bool progress, bool have_previous,
                     int previous_answer, Mask previous_witness, Mask *witness,
                     uint64_t *nodes, size_t *constraint_count,
                     double *elapsed)
{
    Solver solver;
    build_constraints(&solver, n);
    solver.progress = progress;
    seed_upper_bound(&solver);
    if (have_previous) {
        if (popcount_mask(previous_witness) != previous_answer) {
            free_solver(&solver);
            die("invalid preceding witness size");
        }
        seed_from_mask(&solver, previous_witness);
        const Mask with_new_value = previous_witness | bit_for(n);
        if (valid_by_constraints(&solver, with_new_value)) {
            seed_from_mask(&solver, with_new_value);
        }
        solver.absolute_lower_removed = n - previous_answer - 1;
    } else {
        solver.absolute_lower_removed = 0;
    }
    if (solver.best_removed < solver.absolute_lower_removed) {
        free_solver(&solver);
        die("preceding-term bound contradicts a valid seed");
    }
    solver.nodes = 0;
    solver.started = now_seconds();
    solver.next_report = 10.0;

    if (solver.best_removed > solver.absolute_lower_removed) {
        initialize_incremental_state(&solver);
        search(&solver);
    }

    const Mask selected = solver.full_mask ^ solver.best_removed_mask;
    const int answer = n - solver.best_removed;
    if (popcount_mask(selected) != answer ||
        !valid_by_constraints(&solver, selected) ||
        !valid_direct(selected, n)) {
        free_solver(&solver);
        die("internal witness verification failed");
    }

    *witness = selected;
    *nodes = solver.nodes;
    *constraint_count = solver.constraint_count;
    *elapsed = now_seconds() - solver.started;
    free_solver(&solver);
    return answer;
}

static void print_witness(FILE *stream, Mask witness, int n)
{
    fprintf(stream, "{");
    bool first = true;
    for (int value = 1; value <= n; ++value) {
        if ((witness & bit_for(value)) != 0) {
            fprintf(stream, "%s%d", first ? "" : ",", value);
            first = false;
        }
    }
    fprintf(stream, "}");
}

static int parse_int(const char *text, const char *what)
{
    errno = 0;
    char *end = NULL;
    const long value = strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' ||
        value < 1 || value > MAX_N) {
        fprintf(stderr, "%s: invalid %s: %s (expected 1..%d)\n",
                PROGRAM_NAME, what, text, MAX_N);
        exit(EXIT_FAILURE);
    }
    return (int)value;
}

static void usage(FILE *stream)
{
    fprintf(stream,
            "Usage: %s [--min-n N] [--witness] [--progress] [--check] "
            "[--output FILE | --no-bfile] [--force] [nmax]\n"
            "  --min-n N   start at N (default 1)\n"
            "  --witness   print one maximum set for every n\n"
            "  --progress  report search progress every about 10 seconds\n"
            "  --check     direct exhaustive cross-check for n <= %d\n"
            "  --output F  save the b-file as F (default %s)\n"
            "  --no-bfile  do not save a b-file\n"
            "  --force     replace an existing output file atomically\n"
            "  nmax        final n (default %d, hard limit %d)\n"
            "B-file lines are also printed to standard output; diagnostics "
            "use standard error.\n",
            PROGRAM_NAME, DIRECT_CHECK_MAX_N, DEFAULT_BFILE,
            DEFAULT_MAX_N, MAX_N);
}

static Options parse_options(int argc, char **argv)
{
    Options options = {
        .min_n = 1,
        .max_n = DEFAULT_MAX_N,
        .witness = false,
        .progress = false,
        .check = false,
        .write_bfile = true,
        .force = false,
        .output_path = DEFAULT_BFILE
    };
    bool positional_seen = false;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--min-n") == 0) {
            if (++i >= argc) {
                die("--min-n requires an argument");
            }
            options.min_n = parse_int(argv[i], "minimum n");
        } else if (strcmp(argv[i], "--witness") == 0) {
            options.witness = true;
        } else if (strcmp(argv[i], "--progress") == 0) {
            options.progress = true;
        } else if (strcmp(argv[i], "--check") == 0) {
            options.check = true;
        } else if (strcmp(argv[i], "--output") == 0) {
            if (++i >= argc || argv[i][0] == '\0') {
                die("--output requires a nonempty path");
            }
            options.output_path = argv[i];
            options.write_bfile = true;
        } else if (strcmp(argv[i], "--no-bfile") == 0) {
            options.write_bfile = false;
        } else if (strcmp(argv[i], "--force") == 0) {
            options.force = true;
        } else if (strcmp(argv[i], "-h") == 0 ||
                   strcmp(argv[i], "--help") == 0) {
            usage(stdout);
            exit(EXIT_SUCCESS);
        } else if (argv[i][0] == '-') {
            fprintf(stderr, "%s: unknown option: %s\n",
                    PROGRAM_NAME, argv[i]);
            usage(stderr);
            exit(EXIT_FAILURE);
        } else {
            if (positional_seen) {
                die("at most one positional nmax is allowed");
            }
            options.max_n = parse_int(argv[i], "maximum n");
            positional_seen = true;
        }
    }
    if (options.min_n > options.max_n) {
        die("--min-n must not exceed nmax");
    }
    if (options.force && !options.write_bfile) {
        die("--force cannot be combined with --no-bfile");
    }
    return options;
}

static FILE *open_temporary_output(const Options *options)
{
    if (!options->write_bfile) {
        return NULL;
    }
    if (!options->force) {
        errno = 0;
        if (access(options->output_path, F_OK) == 0) {
            fprintf(stderr,
                    "%s: output file already exists: %s (use --force to "
                    "replace it)\n",
                    PROGRAM_NAME, options->output_path);
            exit(EXIT_FAILURE);
        }
        if (errno != ENOENT) {
            fprintf(stderr, "%s: cannot inspect output file %s: %s\n",
                    PROGRAM_NAME, options->output_path, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    const size_t length = strlen(options->output_path) + 48U;
    active_temporary_path = malloc(length);
    if (active_temporary_path == NULL) {
        die("unable to allocate the temporary output path");
    }
    const int written = snprintf(active_temporary_path, length, "%s.part.%ld",
                                 options->output_path, (long)getpid());
    if (written < 0 || (size_t)written >= length) {
        die("temporary output path is too long");
    }
    FILE *stream = fopen(active_temporary_path, "wx");
    if (stream == NULL) {
        fprintf(stderr, "%s: cannot create temporary output %s: %s\n",
                PROGRAM_NAME, active_temporary_path, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return stream;
}

static void finish_output(FILE *stream, const Options *options)
{
    if (stream == NULL) {
        return;
    }
    if (fclose(stream) != 0) {
        fprintf(stderr, "%s: cannot close temporary output %s: %s\n",
                PROGRAM_NAME, active_temporary_path, strerror(errno));
        exit(EXIT_FAILURE);
    }

    int status;
    if (options->force) {
        status = rename(active_temporary_path, options->output_path);
    } else {
        status = link(active_temporary_path, options->output_path);
        if (status == 0) {
            status = unlink(active_temporary_path);
        }
    }
    if (status != 0) {
        fprintf(stderr, "%s: cannot install output file %s: %s\n",
                PROGRAM_NAME, options->output_path, strerror(errno));
        exit(EXIT_FAILURE);
    }
    free(active_temporary_path);
    active_temporary_path = NULL;
    fprintf(stderr, "saved %s\n", options->output_path);
}

int main(int argc, char **argv)
{
    const Options options = parse_options(argc, argv);
    if (atexit(remove_temporary_output) != 0) {
        die("cannot register temporary-file cleanup");
    }
    FILE *output = open_temporary_output(&options);
    bool have_previous = false;
    int previous_answer = 0;
    Mask previous_witness = 0;

    for (int n = options.min_n; n <= options.max_n; ++n) {
        Mask witness = 0;
        uint64_t nodes = 0;
        size_t constraints = 0;
        double elapsed = 0.0;
        const int answer = solve_one(n, options.progress, have_previous,
                                     previous_answer, previous_witness,
                                     &witness, &nodes, &constraints, &elapsed);
        if (options.check && n <= DIRECT_CHECK_MAX_N) {
            uint64_t direct_witness = 0;
            const int direct = direct_maximum(n, &direct_witness);
            if (direct != answer ||
                !valid_direct((Mask)direct_witness, n)) {
                die("direct exhaustive cross-check failed");
            }
        }

        const int pi = prime_count(n);
        if (printf("%d %d\n", n, answer) < 0 || fflush(stdout) != 0 ||
            (output != NULL &&
             (fprintf(output, "%d %d\n", n, answer) < 0 ||
              fflush(output) != 0))) {
            die("cannot write b-file output");
        }
        fprintf(stderr,
                "[n=%d: F(n)=%d, pi(n)=%d, excess=%d, constraints=%zu, "
                "nodes=%" PRIu64 ", %.3f s",
                n, answer, pi, answer - pi, constraints, nodes, elapsed);
        if (options.check && n <= DIRECT_CHECK_MAX_N) {
            fprintf(stderr, ", checked");
        }
        if (options.witness) {
            fprintf(stderr, ", witness=");
            print_witness(stderr, witness, n);
        }
        fprintf(stderr, "]\n");
        have_previous = true;
        previous_answer = answer;
        previous_witness = witness;
    }
    finish_output(output, &options);
    return EXIT_SUCCESS;
}
