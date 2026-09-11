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
 * Exact computation for Erdos problem #793.
 *
 * F(n) is the maximum size of A subseteq {1,...,n} such that
 *
 *   a does not divide b*c
 *
 * whenever a,b,c are in A, a != b, and a != c.  The two factors b and c
 * are allowed to be equal.  Thus {a,b} is already forbidden when a | b^2
 * or b | a^2.  This is the strongly 2-primitive convention; it is different
 * from the newer 2-primitive convention in which b and c must be distinct.
 *
 * A set is invalid precisely when it contains one of the following:
 *
 *   {a,b},       if a | b^2 or b | a^2;
 *   {a,b,c},     if the three numbers are distinct and one divides the
 *                product of the other two.
 *
 * Redundant triples containing a forbidden pair are omitted.  The program
 * regards pair-compatible numbers as adjacent in a graph and searches for a
 * maximum clique, rejecting the forbidden triples during the search.  Greedy
 * graph coloring gives a rigorous clique upper bound.
 *
 * The optional --check mode exhaustively scans all subsets for n <= 18 and
 * tests the divisibility condition directly, independently of the generated
 * constraints and the branch-and-bound proof of optimality.  The separate
 * 399779_02.c program implements an independent minimum-hitting-set search.
 *
 * References:
 *   Erdos Problem 793: https://www.erdosproblems.com/793
 *   P. Chojecki, The Second Term for Strongly 2-Primitive Sets (2026):
 *     https://www.ulam.ai/research/erdos793.pdf
 *   Related distinct-factor convention: T. H. Chan, J. D. Lichtman, and
 *   C. Pomerance, Combinatorica 42 (2022), 729-747:
 *     https://math.dartmouth.edu/~carlp/4695pomerance.pdf
 *   Earlier work on the general distinct-factor problem: T. H. Chan,
 *   European J. Combin. 32 (2011), 443-447:
 *     https://doi.org/10.1016/j.ejc.2010.11.010
 *
 * Compile:
 *   cc -O3 -march=native -std=c17 -Wall -Wextra -Wpedantic \
 *      -o 399779_01 399779_01.c
 *
 * Examples:
 *   ./399779_01 30
 *   ./399779_01 --witness --progress 35
 *   ./399779_01 --check --no-bfile 18
 *   ./399779_01 --progress 105
 *   ./399779_01 --force --progress 105
 *
 * A normal run writes b-file lines both to standard output and, atomically,
 * to b399779_01.txt.  Existing output is preserved unless --force is given.
 * Use --output FILE to choose another path or --no-bfile to suppress the
 * file while retaining standard output.
 *
 * The hard limit 127 comes from the unsigned 128-bit representation.  The
 * program therefore requires GCC or Clang.  The running time is exponential;
 * the limit is not a claim that n=127 is practical.
 */

#define PROGRAM_NAME "399779_01"
#define DEFAULT_BFILE "b399779_01.txt"
#define DEFAULT_MAX_N 20
#define MAX_N 127
#define DIRECT_CHECK_MAX_N 18
#define HEARTBEAT_NODE_MASK ((UINT64_C(1) << 20) - UINT64_C(1))

__extension__ typedef unsigned __int128 Mask;

typedef struct {
    Mask mask;
    uint8_t size;
} Constraint;

typedef struct {
    int n;
    Mask full_mask;
    Constraint *constraints;
    size_t constraint_count;
    unsigned degree[MAX_N];
    Mask compatible[MAX_N];
    Mask *triple_block;

    int best_size;
    Mask best_selected_mask;
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
    return popcount64((uint64_t)value) + popcount64((uint64_t)(value >> 64));
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
    const size_t pairs = nn * (nn - 1U) / 2U;
    const size_t triples = nn * (nn - 1U) * (nn - 2U) / 6U;
    return pairs + triples;
}

static void add_constraint(Solver *solver, size_t capacity, Mask mask,
                           uint8_t size)
{
    if (solver->constraint_count >= capacity) {
        die("internal constraint-capacity overflow");
    }
    Constraint *constraint = &solver->constraints[solver->constraint_count++];
    constraint->mask = mask;
    constraint->size = size;

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
                               bit_for(a) | bit_for(b), 2);
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
                                   bit_for(a) | bit_for(b) | bit_for(c), 3);
                }
            }
        }
    }

    for (int vertex = 0; vertex < n; ++vertex) {
        solver->compatible[vertex] =
            solver->full_mask & ~((Mask)1 << vertex);
    }
    const size_t table_entries = (size_t)n * (size_t)n;
    solver->triple_block = calloc(table_entries == 0 ? 1U : table_entries,
                                  sizeof(*solver->triple_block));
    if (solver->triple_block == NULL) {
        free(solver->constraints);
        die("unable to allocate the triple-conflict table");
    }

    for (size_t i = 0; i < solver->constraint_count; ++i) {
        int vertices[3];
        int count = 0;
        Mask work = solver->constraints[i].mask;
        while (work != 0) {
            vertices[count++] = ctz_mask(work);
            work &= work - (Mask)1;
        }
        if (solver->constraints[i].size == 2) {
            const int a = vertices[0];
            const int b = vertices[1];
            solver->compatible[a] &= ~((Mask)1 << b);
            solver->compatible[b] &= ~((Mask)1 << a);
        } else {
            const int a = vertices[0];
            const int b = vertices[1];
            const int c = vertices[2];
            solver->triple_block[(size_t)a * (size_t)n + (size_t)b] |=
                (Mask)1 << c;
            solver->triple_block[(size_t)b * (size_t)n + (size_t)a] |=
                (Mask)1 << c;
            solver->triple_block[(size_t)a * (size_t)n + (size_t)c] |=
                (Mask)1 << b;
            solver->triple_block[(size_t)c * (size_t)n + (size_t)a] |=
                (Mask)1 << b;
            solver->triple_block[(size_t)b * (size_t)n + (size_t)c] |=
                (Mask)1 << a;
            solver->triple_block[(size_t)c * (size_t)n + (size_t)b] |=
                (Mask)1 << a;
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

/* This deliberately checks the original quantified condition, not the
 * generated pair/triple table. */
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
    const int size = popcount_mask(selected);
    if (size > solver->best_size) {
        solver->best_size = size;
        solver->best_selected_mask = selected;
    }
}

static void seed_lower_bound(Solver *solver)
{
    Mask primes = 0;
    for (int value = 1; value <= solver->n; ++value) {
        if (is_prime(value)) {
            primes |= bit_for(value);
        }
    }
    solver->best_size = popcount_mask(primes);
    solver->best_selected_mask = primes;

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
            PROGRAM_NAME, solver->n, solver->best_size,
            solver->nodes, elapsed);
    solver->next_report = elapsed + 10.0;
}

/* Greedily color the pair-compatibility graph induced by candidates.  Each
 * color class is independent, so the number of colors bounds the size of any
 * clique, and hence of any valid extension. */
static int color_sort(const Solver *solver, Mask candidates,
                      int order[MAX_N], int bounds[MAX_N])
{
    Mask uncolored = candidates;
    int count = 0;
    int color = 0;

    while (uncolored != 0) {
        ++color;
        Mask available = uncolored;
        while (available != 0) {
            const int vertex = ctz_mask(available);
            const Mask bit = (Mask)1 << vertex;
            order[count] = vertex;
            bounds[count] = color;
            ++count;
            uncolored &= ~bit;
            available &= ~bit;
            available &= ~solver->compatible[vertex];
        }
    }
    return count;
}

static void clique_search(Solver *solver, Mask selected, Mask candidates,
                          int selected_count)
{
    if (solver->nodes != UINT64_MAX) {
        ++solver->nodes;
    }
    report_progress(solver);

    if (candidates == 0) {
        if (selected_count > solver->best_size) {
            solver->best_size = selected_count;
            solver->best_selected_mask = selected;
        }
        return;
    }

    int order[MAX_N];
    int bounds[MAX_N];
    const int count = color_sort(solver, candidates, order, bounds);

    for (int i = count - 1; i >= 0; --i) {
        if (selected_count + bounds[i] <= solver->best_size) {
            return;
        }

        const int vertex = order[i];
        const Mask bit = (Mask)1 << vertex;
        Mask next = candidates & solver->compatible[vertex];
        Mask selected_work = selected;
        while (selected_work != 0) {
            const int old = ctz_mask(selected_work);
            next &= ~solver->triple_block[
                (size_t)vertex * (size_t)solver->n + (size_t)old];
            selected_work &= selected_work - (Mask)1;
        }

        const Mask with_vertex = selected | bit;
        if (next == 0) {
            if (selected_count + 1 > solver->best_size) {
                solver->best_size = selected_count + 1;
                solver->best_selected_mask = with_vertex;
                if (solver->progress) {
                    fprintf(stderr,
                            "%s: improved n=%d F(n)=%d nodes=%" PRIu64
                            "\n",
                            PROGRAM_NAME, solver->n, solver->best_size,
                            solver->nodes);
                }
            }
        } else {
            clique_search(solver, with_vertex, next, selected_count + 1);
        }
        candidates &= ~bit;
    }
}

static int direct_maximum(int n, uint64_t *witness)
{
    const uint64_t end = UINT64_C(1) << n;
    int best = 0;
    uint64_t best_mask = 0;
    for (uint64_t selected = 0; selected < end; ++selected) {
        const int size = popcount64(selected);
        if (size <= best) {
            continue;
        }
        if (valid_direct(selected, n)) {
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
    seed_lower_bound(&solver);
    solver.nodes = 0;
    solver.started = now_seconds();
    solver.next_report = 10.0;
    if (solver.best_size < n) {
        clique_search(&solver, 0, solver.full_mask, 0);
    }

    const Mask selected = solver.best_selected_mask;
    if (popcount_mask(selected) != solver.best_size ||
        !valid_by_constraints(&solver, selected) ||
        !valid_direct(selected, n)) {
        free(solver.constraints);
        free(solver.triple_block);
        die("internal witness verification failed");
    }

    *witness = selected;
    *nodes = solver.nodes;
    *constraint_count = solver.constraint_count;
    *elapsed = now_seconds() - solver.started;
    const int answer = solver.best_size;
    free(solver.constraints);
    free(solver.triple_block);
    return answer;
}

static void print_witness(FILE *stream, Mask witness, int n)
{
    fprintf(stream, "{");
    bool first = true;
    for (int value = 1; value <= n; ++value) {
        if ((witness & bit_for(value)) == 0) {
            continue;
        }
        fprintf(stream, "%s%d", first ? "" : ",", value);
        first = false;
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
            if (direct != answer || !valid_direct(direct_witness, n)) {
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
