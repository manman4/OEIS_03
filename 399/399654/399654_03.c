#define _POSIX_C_SOURCE 200809L

/* 399654_03.c
 *
 * Independent SAT-based exact search for A399654.
 *
 * For a simple graph G, let CS(G) be the set of its cycle lengths.  For
 * every S subseteq {3,...,n}, this program asks whether CS(G)=S for some
 * graph on n labelled vertices.  Isolated vertices make this equivalent
 * to allowing at most n vertices.
 *
 * This method does not use the block/ear search of 399654_01.c or the
 * cycle-union search of 399654_02.c.
 *
 * SAT encoding for a nonempty S
 * --------------------------------
 * - One Boolean variable x_uv says whether edge uv is present.
 * - Let M=max(S).  Any witness can be relabelled so that
 *   0,1,...,M-1,0 is an M-cycle, so those edges are fixed present.
 * - For every other L in S, position variables describe one ordered
 *   simple L-cycle and imply the necessary edge variables.
 * - It is sufficient to search graphs with at most sum(S) edges.  Indeed,
 *   in an edge-minimal witness every edge is essential for some L in S,
 *   so one selected L-cycle for each L covers every edge.
 * - Edges are initially phased false so that sparse models are preferred.
 *
 * Cycles of lengths outside S are excluded lazily.  After every SAT model,
 * an independent cycle enumerator computes CS(G).  For every forbidden
 * cycle C present in the model, the clause
 *
 *                         OR_{e in C} not x_e
 *
 * is added and the same incremental SAT instance is solved again.
 *
 * Correctness
 * -----------
 * A returned SAT model is accepted only if the independent enumerator finds
 * exactly S.  If the solver returns UNSAT, an exact witness cannot exist:
 * after relabelling one of its M-cycles it would satisfy the fixed base
 * cycle, all required-cycle constraints, and every sound forbidden-cycle
 * clause added so far.  Termination is guaranteed because every nonfinal
 * SAT model adds at least one previously absent clause and K_n has finitely
 * many cycles.
 *
 * Dependency (Homebrew on macOS):
 *   brew install cadical
 *
 * Compile:
 *   cc -O3 -march=native -mmacosx-version-min=26.0 \
 *      -std=c17 -Wall -Wextra -Wpedantic \
 *      -I/opt/homebrew/opt/cadical/include \
 *      -o 399654_03 399654_03.c \
 *      /opt/homebrew/opt/cadical/lib/libcadical.a -lc++
 *
 * Run:
 *   /usr/bin/time -p ./399654_03 10
 *
 * Values are printed on stdout.  Progress, SAT rounds, lazy clauses, cycle
 * scans, and elapsed times are printed on stderr.  No result file is written.
 */

#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <ccadical.h>

#define MAXN 12
#define SPECTRUM_PROGRESS_INTERVAL 16
#define REPORT_INTERVAL_SECONDS 5.0
#define ENUM_REPORT_INTERVAL UINT64_C(1000000)

typedef struct {
    int n;
    int index;
    int total;
    int target;
    uint64_t rounds;
    uint64_t cuts;
    uint64_t cycles_scanned;
    double start;
    double next_report;
} Progress;

static int current_n;
static int target;
static int edge_var[MAXN][MAXN];
static int next_var;
static int position_var[MAXN + 1][MAXN][MAXN];
static int model_adj[MAXN];
static int cycle_path[MAXN];
static int enumerated_spectrum;
static uint64_t model_cuts;
static int scan_length;
static int scan_forbidden;
static int scan_found;
static Progress *active_progress;

static double wall_seconds(void)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }
    return (double) ts.tv_sec + 1e-9 * (double) ts.tv_nsec;
}

static void format_set(char *buffer, size_t size, int set)
{
    size_t used = 0;
    int first = 1;
    int L;

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

static void print_candidate_progress(Progress *progress, const char *stage)
{
    char set_text[4 * MAXN];
    double elapsed = wall_seconds() - progress->start;

    format_set(set_text, sizeof set_text, progress->target);
    fprintf(stderr,
            "  [n=%d: candidate %d/%d (%.1f%%) %s: %s, "
            "SAT rounds=%" PRIu64 ", cuts=%" PRIu64
            ", scanned cycles=%" PRIu64 ", %.1f s]\n",
            progress->n, progress->index, progress->total,
            100.0 * (double) progress->index / (double) progress->total,
            set_text, stage, progress->rounds, progress->cuts,
            progress->cycles_scanned, elapsed);
}

static void maybe_report(Progress *progress, const char *stage)
{
    double now = wall_seconds();
    if (now < progress->next_report) return;
    print_candidate_progress(progress, stage);
    progress->next_report = now + REPORT_INTERVAL_SECONDS;
}

static int terminate_callback(void *state)
{
    Progress *progress = state;
    maybe_report(progress, "solving");
    return 0;
}

static void add_unit(CCaDiCaL *solver, int a)
{
    ccadical_add(solver, a);
    ccadical_add(solver, 0);
}

static void add_binary(CCaDiCaL *solver, int a, int b)
{
    ccadical_add(solver, a);
    ccadical_add(solver, b);
    ccadical_add(solver, 0);
}

static void add_ternary(CCaDiCaL *solver, int a, int b, int c)
{
    ccadical_add(solver, a);
    ccadical_add(solver, b);
    ccadical_add(solver, c);
    ccadical_add(solver, 0);
}

static void initialize_edge_variables(CCaDiCaL *solver)
{
    int u, v;

    memset(edge_var, 0, sizeof edge_var);
    next_var = 1;
    for (u = 0; u < current_n; u++) {
        for (v = u + 1; v < current_n; v++) {
            edge_var[u][v] = edge_var[v][u] = next_var++;
            /* Lazy clauses added after solve still refer to every edge. */
            ccadical_freeze(solver, edge_var[u][v]);
            ccadical_phase(solver, -edge_var[u][v]);
        }
    }
}

static void add_fixed_maximum_cycle(CCaDiCaL *solver, int maximum)
{
    int i;
    for (i = 0; i < maximum; i++)
        add_unit(solver, edge_var[i][(i + 1) % maximum]);
}

/* A monotone sequential counter.  counter[i][j] means that at least j of
 * the first i edge variables are true.  Only the forward implications are
 * needed to enforce not counter[m][bound+1]. */
static void add_edge_bound(CCaDiCaL *solver, int bound)
{
    int variables[MAXN * (MAXN - 1) / 2];
    int counter[MAXN * (MAXN - 1) / 2 + 1][MAXN * (MAXN - 1) / 2 + 1];
    int m = 0;
    int i, j, u, v;

    for (u = 0; u < current_n; u++)
        for (v = u + 1; v < current_n; v++)
            variables[m++] = edge_var[u][v];
    if (bound >= m) return;
    memset(counter, 0, sizeof counter);

    for (i = 1; i <= m; i++) {
        int largest = i < bound + 1 ? i : bound + 1;
        for (j = 1; j <= largest; j++) counter[i][j] = next_var++;

        /* x_i implies that at least one of the first i variables is true. */
        add_binary(solver, -variables[i - 1], counter[i][1]);

        for (j = 1; j <= largest; j++) {
            if (j <= i - 1)
                add_binary(solver, -counter[i - 1][j], counter[i][j]);
            if (j >= 2 && j - 1 <= i - 1)
                add_ternary(solver, -variables[i - 1],
                            -counter[i - 1][j - 1], counter[i][j]);
        }
    }
    add_unit(solver, -counter[m][bound + 1]);
}

/* Describe one simple L-cycle by its vertex at every cyclic position. */
static void add_required_cycle(CCaDiCaL *solver, int length)
{
    int position, other_position, u, v;

    for (position = 0; position < length; position++)
        for (v = 0; v < current_n; v++)
            position_var[length][position][v] = next_var++;

    /* Exactly one vertex at every position. */
    for (position = 0; position < length; position++) {
        for (v = 0; v < current_n; v++)
            ccadical_add(solver, position_var[length][position][v]);
        ccadical_add(solver, 0);
        for (u = 0; u < current_n; u++)
            for (v = u + 1; v < current_n; v++)
                add_binary(solver,
                           -position_var[length][position][u],
                           -position_var[length][position][v]);
    }

    /* A vertex occurs in at most one position. */
    for (v = 0; v < current_n; v++)
        for (position = 0; position < length; position++)
            for (other_position = position + 1;
                 other_position < length; other_position++)
                add_binary(solver,
                           -position_var[length][position][v],
                           -position_var[length][other_position][v]);

    /* Position zero is the smallest selected vertex. */
    for (v = 0; v < current_n; v++)
        for (position = 1; position < length; position++)
            for (u = 0; u < v; u++)
                add_binary(solver,
                           -position_var[length][0][v],
                           -position_var[length][position][u]);

    /* Choose one of the two orientations. */
    for (u = 0; u < current_n; u++)
        for (v = 0; v <= u; v++)
            add_binary(solver,
                       -position_var[length][1][u],
                       -position_var[length][length - 1][v]);

    /* Consecutive selected vertices force their graph edge. */
    for (position = 0; position < length; position++) {
        int next_position = (position + 1) % length;
        for (u = 0; u < current_n; u++) {
            for (v = 0; v < current_n; v++) {
                if (u == v) {
                    add_binary(solver,
                               -position_var[length][position][u],
                               -position_var[length][next_position][v]);
                } else {
                    add_ternary(solver,
                                -position_var[length][position][u],
                                -position_var[length][next_position][v],
                                edge_var[u][v]);
                }
            }
        }
    }
}

static void read_model(CCaDiCaL *solver)
{
    int u, v;
    memset(model_adj, 0, sizeof model_adj);
    for (u = 0; u < current_n; u++) {
        for (v = u + 1; v < current_n; v++) {
            if (ccadical_val(solver, edge_var[u][v]) > 0) {
                model_adj[u] |= 1 << v;
                model_adj[v] |= 1 << u;
            }
        }
    }
}

static void add_forbidden_cycle_clause(CCaDiCaL *solver, int length)
{
    int i;
    for (i = 0; i < length; i++) {
        int u = cycle_path[i];
        int v = cycle_path[(i + 1) % length];
        ccadical_add(solver, -edge_var[u][v]);
    }
    ccadical_add(solver, 0);
    model_cuts++;
}

/* Enumerate cycles of one requested length.  For a required length, stop at
 * the first cycle.  For a forbidden length, add a clause for every cycle in
 * the current model. */
static void enumerate_model_cycles(CCaDiCaL *solver, int start,
                                   int depth, unsigned used)
{
    int last = cycle_path[depth - 1];
    int neighbours;

    if (scan_found && !scan_forbidden) return;
    if (depth == scan_length) {
        if (((model_adj[last] >> start) & 1) && cycle_path[1] < last) {
            scan_found = 1;
            enumerated_spectrum |= 1 << scan_length;
            active_progress->cycles_scanned++;
            if (scan_forbidden)
                add_forbidden_cycle_clause(solver, scan_length);
            if ((active_progress->cycles_scanned % ENUM_REPORT_INTERVAL) == 0)
                maybe_report(active_progress, "checking model");
        }
        return;
    }

    neighbours = model_adj[last] & ~((1 << (start + 1)) - 1) &
                 ~((int) used);
    while (neighbours != 0) {
        int v = __builtin_ctz((unsigned int) neighbours);
        neighbours &= neighbours - 1;
        cycle_path[depth] = v;
        enumerate_model_cycles(solver, start, depth + 1,
                               used | (1u << v));
    }
}

static int inspect_model_and_add_cuts(CCaDiCaL *solver)
{
    int L, start;

    enumerated_spectrum = 0;
    model_cuts = 0;
    for (L = 3; L <= current_n; L++) {
        scan_length = L;
        scan_forbidden = !((target >> L) & 1);
        scan_found = 0;
        for (start = 0; start + L <= current_n; start++) {
            cycle_path[0] = start;
            enumerate_model_cycles(solver, start, 1, 1u << start);
            if (scan_found && !scan_forbidden) break;
        }
        if (!scan_forbidden && !scan_found) {
            fprintf(stderr,
                    "internal error: SAT model misses required length %d\n", L);
            exit(EXIT_FAILURE);
        }
    }
    return model_cuts == 0;
}

static int realizable(int set, int n, Progress *progress)
{
    CCaDiCaL *solver;
    int maximum;
    int edge_bound = 0;
    int L;

    if (set == 0) return 1;
    maximum = 31 - __builtin_clz((unsigned int) set);
    if (maximum > n) return 0;
    /* A chordless cycle realizes every singleton spectrum. */
    if (set == (1 << maximum)) return 1;

    current_n = n;
    target = set;
    memset(position_var, 0, sizeof position_var);

    solver = ccadical_init();
    if (solver == NULL) {
        fprintf(stderr, "failed to initialize CaDiCaL\n");
        exit(EXIT_FAILURE);
    }
    ccadical_set_option(solver, "quiet", 1);
    ccadical_set_terminate(solver, progress, terminate_callback);

    initialize_edge_variables(solver);
    for (L = 3; L <= n; L++)
        if ((set >> L) & 1) edge_bound += L;
    add_edge_bound(solver, edge_bound);
    add_fixed_maximum_cycle(solver, maximum);
    for (L = 3; L <= n; L++)
        if (L != maximum && ((set >> L) & 1))
            add_required_cycle(solver, L);

    for (;;) {
        int status;
        progress->rounds++;
        status = ccadical_solve(solver);
        if (status == 20) {
            ccadical_release(solver);
            return 0;
        }
        if (status != 10) {
            fprintf(stderr, "CaDiCaL returned unexpected status %d\n", status);
            ccadical_release(solver);
            exit(EXIT_FAILURE);
        }

        read_model(solver);
        if (inspect_model_and_add_cuts(solver)) {
            ccadical_release(solver);
            return 1;
        }
        if (model_cuts == 0) {
            fprintf(stderr,
                    "internal error: nonfinal model produced no lazy clause\n");
            ccadical_release(solver);
            exit(EXIT_FAILURE);
        }
        progress->cuts += model_cuts;
        maybe_report(progress, "added cycle clauses");
    }
}

/* This byte-for-byte format is shared with 399654_01.c. */
static int write_spectra_file(const char *path, int n, const int *known,
                              int count)
{
    FILE *stream;
    int index;
    int total = n >= 3 ? (1 << (n - 2)) - 1 : 0;

    stream = fopen(path, "wx");
    if (stream == NULL) {
        fprintf(stderr, "cannot create output file %s: %s\n",
                path, strerror(errno));
        return 0;
    }
    if (fprintf(stream, "n=%d\ncount=%d\n", n, count) < 0) goto failure;
    for (index = 0; index <= total; index++) {
        int L;
        int first = 1;
        if (!known[index]) continue;
        if (fprintf(stream, "%d\t{", index) < 0) goto failure;
        for (L = 3; L <= n; L++) {
            if (!(((index << 3) >> L) & 1)) continue;
            if (fprintf(stream, first ? "%d" : ",%d", L) < 0)
                goto failure;
            first = 0;
        }
        if (fputs("}\n", stream) == EOF) goto failure;
    }
    if (fclose(stream) != 0) {
        fprintf(stderr, "cannot finish output file %s: %s\n",
                path, strerror(errno));
        return 0;
    }
    return 1;

failure:
    fprintf(stderr, "cannot write output file %s: %s\n",
            path, strerror(errno));
    fclose(stream);
    return 0;
}

static void usage(const char *program)
{
    fprintf(stderr, "usage: %s [-o FILE] [nmax]\n", program);
    fprintf(stderr, "  0 <= nmax <= %d (default 10)\n", MAXN);
    fprintf(stderr, "  -o FILE writes the realizable spectra for nmax\n");
}

int main(int argc, char **argv)
{
    int nmax = 10;
    int known[1 << (MAXN - 2)] = {0};
    const char *output_path = NULL;
    int n;
    int arg;

    for (arg = 1; arg < argc; arg++) {
        if (strcmp(argv[arg], "-o") == 0) {
            if (++arg >= argc || argv[arg][0] == '\0') {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            output_path = argv[arg];
        } else {
            char *end = NULL;
            long value;
            errno = 0;
            value = strtol(argv[arg], &end, 10);
            if (errno != 0 || end == argv[arg] || *end != '\0' ||
                value < 0 || value > MAXN) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            nmax = (int) value;
        }
    }
    if (output_path && access(output_path, F_OK) == 0) {
        fprintf(stderr, "output file already exists: %s\n", output_path);
        return EXIT_FAILURE;
    }

    known[0] = 1;
    for (n = 0; n <= nmax; n++) {
        double n_start = wall_seconds();
        int total = n >= 3 ? (1 << (n - 2)) - 1 : 0;
        int count = 1;
        int index;

        current_n = n;
        for (index = 1; index <= total; index++) {
            Progress progress;
            int set = index << 3;

            memset(&progress, 0, sizeof progress);
            progress.n = n;
            progress.index = index;
            progress.total = total;
            progress.target = set;
            progress.start = wall_seconds();
            progress.next_report = progress.start + REPORT_INTERVAL_SECONDS;
            active_progress = &progress;

            if (!known[index] && realizable(set, n, &progress))
                known[index] = 1;
            if (known[index]) count++;

            if (index % SPECTRUM_PROGRESS_INTERVAL == 0 || index == total) {
                fprintf(stderr,
                        "  [n=%d: completed spectra %d/%d (%.1f%%), "
                        "realized=%d, %.1f s]\n",
                        n, index, total,
                        100.0 * (double) index / (double) total,
                        count, wall_seconds() - n_start);
            }
        }
        printf("%d %d\n", n, count);
        fflush(stdout);
        fprintf(stderr, "  [n=%d: %.2f s]\n", n, wall_seconds() - n_start);
        if (n == nmax && output_path &&
            !write_spectra_file(output_path, n, known, count))
            return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
