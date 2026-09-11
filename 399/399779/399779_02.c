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
 * Independent exact computation for Erdos problem #793 by minimum hitting
 * set search.
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
 * Unit propagation and a greedy packing of disjoint unhit hyperedges give
 * rigorous lower bounds on the number of further removals.
 *
 * This file intentionally uses a different optimization algorithm from
 * 399779_01.c.  The optional --check mode also scans all subsets for n <= 18
 * and tests the original divisibility condition directly.
 *
 * Compile:
 *   cc -O3 -march=native -std=c17 -Wall -Wextra -Wpedantic \
 *      -o 399779_02 399779_02.c
 *   ./399779_02 --progress 60
 *   ./399779_02 --check --no-bfile 18
 *
 * A normal run writes b-file lines both to standard output and, atomically,
 * to b399779_02.txt.  Existing output is preserved unless --force is given.
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

#define PROGRAM_NAME "399779_02"
#define DEFAULT_BFILE "b399779_02.txt"
#define DEFAULT_MAX_N 20
#define MAX_N 127
#define DIRECT_CHECK_MAX_N 18
#define HEARTBEAT_NODE_MASK ((UINT64_C(1) << 20) - UINT64_C(1))

__extension__ typedef unsigned __int128 Mask;

typedef struct {
    Mask mask;
} Constraint;

typedef struct {
    int n;
    Mask full_mask;
    Constraint *constraints;
    size_t constraint_count;
    unsigned degree[MAX_N];

    int best_removed;
    Mask best_removed_mask;
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

static void search(Solver *solver, Mask removed, Mask kept,
                   int removed_count)
{
    if (solver->nodes != UINT64_MAX) {
        ++solver->nodes;
    }
    report_progress(solver);

    if (removed_count >= solver->best_removed) {
        return;
    }

    /* If all but one removable vertex of an unhit edge have been declared
     * kept, its final removable vertex is forced into the hitting set. */
    for (;;) {
        Mask forced = 0;
        for (size_t i = 0; i < solver->constraint_count; ++i) {
            const Mask edge = solver->constraints[i].mask;
            if ((edge & removed) != 0) {
                continue;
            }
            const Mask candidates = edge & ~kept;
            const int count = popcount_mask(candidates);
            if (count == 0) {
                return;
            }
            if (count == 1) {
                forced = candidates;
                break;
            }
        }
        if (forced == 0) {
            break;
        }
        removed |= forced;
        ++removed_count;
        if (removed_count >= solver->best_removed) {
            return;
        }
    }

    Mask chosen = 0;
    int chosen_count = INT_MAX;
    int chosen_score = -1;
    Mask packed = 0;
    int lower_bound = 0;

    for (size_t i = 0; i < solver->constraint_count; ++i) {
        const Mask edge = solver->constraints[i].mask;
        if ((edge & removed) != 0) {
            continue;
        }
        const Mask candidates = edge & ~kept;
        const int count = popcount_mask(candidates);
        if (count == 0) {
            return;
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
        solver->best_removed = removed_count;
        solver->best_removed_mask = removed;
        if (solver->progress) {
            fprintf(stderr,
                    "%s: improved n=%d F(n)=%d removed=%d nodes=%" PRIu64
                    "\n",
                    PROGRAM_NAME, solver->n, solver->n - removed_count,
                    removed_count, solver->nodes);
        }
        return;
    }

    if (removed_count + lower_bound >= solver->best_removed) {
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

    /* The branches classify every solution by the first removed vertex of
     * the chosen edge; earlier vertices are therefore safe to mark kept. */
    Mask earlier_kept = kept;
    for (int i = 0; i < count; ++i) {
        const Mask bit = (Mask)1 << vertices[i];
        search(solver, removed | bit, earlier_kept, removed_count + 1);
        earlier_kept |= bit;
    }
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

static int solve_one(int n, bool progress, Mask *witness,
                     uint64_t *nodes, size_t *constraint_count,
                     double *elapsed)
{
    Solver solver;
    build_constraints(&solver, n);
    solver.progress = progress;
    seed_upper_bound(&solver);
    solver.nodes = 0;
    solver.started = now_seconds();
    solver.next_report = 10.0;

    if (solver.best_removed > 0) {
        search(&solver, 0, 0, 0);
    }

    const Mask selected = solver.full_mask ^ solver.best_removed_mask;
    const int answer = n - solver.best_removed;
    if (popcount_mask(selected) != answer ||
        !valid_by_constraints(&solver, selected) ||
        !valid_direct(selected, n)) {
        free(solver.constraints);
        die("internal witness verification failed");
    }

    *witness = selected;
    *nodes = solver.nodes;
    *constraint_count = solver.constraint_count;
    *elapsed = now_seconds() - solver.started;
    free(solver.constraints);
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

    for (int n = options.min_n; n <= options.max_n; ++n) {
        Mask witness = 0;
        uint64_t nodes = 0;
        size_t constraints = 0;
        double elapsed = 0.0;
        const int answer = solve_one(n, options.progress, &witness, &nodes,
                                     &constraints, &elapsed);
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
    }
    finish_output(output, &options);
    return EXIT_SUCCESS;
}
