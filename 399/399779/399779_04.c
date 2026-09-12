#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <limits.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

/*
 * Experimental exact SAT computation for OEIS A399779 / Erdos problem #793.
 *
 * F(n) is the maximum size of A subseteq {1,...,n} such that a does not
 * divide b*c whenever a,b,c are in A, a != b, and a != c.  The factors b
 * and c may be equal.
 *
 * Put x_i = true exactly when i is selected.  Invalid sets are excluded by
 * the following clauses:
 *
 *   not x_a or not x_b,
 *     if a divides b^2 or b divides a^2;
 *
 *   not x_a or not x_b or not x_c,
 *     for distinct a,b,c if one divides the product of the other two.
 *
 * Triples containing an already forbidden pair are redundant and omitted.
 * A dynamic-programming sequential counter encodes sum(x_i) >= k exactly.
 * If s(i,j) means that at least j of x_1,...,x_i are true, then
 *
 *   s(i,j) <-> s(i-1,j) or (s(i-1,j-1) and x_i).
 *
 * For consecutive n, the rigorous inequality
 *
 *   F(n-1) <= F(n) <= F(n-1) + 1
 *
 * means that only the question F(n) >= F(n-1)+1 has to be sent to SAT.
 * If --min-n skips the preceding term, increasingly strong cardinality
 * questions are asked until the first UNSAT answer, starting from the valid
 * set of primes.  Thus --min-n does not assume any unverified earlier value.
 *
 * The SAT solver is run by fork/exec rather than through a shell.  Every SAT
 * witness is checked again against all original forbidden hyperedges in this
 * program.  An UNSAT answer necessarily relies on the external SAT solver;
 * compare the resulting b-file with 399779_01.c and 399779_03.c.
 *
 * Requires CaDiCaL (or another command-line solver accepting -q, -w FILE,
 * DIMACS_FILE and returning exit status 10 for SAT or 20 for UNSAT).
 *
 * Compile and run:
 *   cc -O3 -std=c17 -Wall -Wextra -Wpedantic -o 399779_04 399779_04.c
 *   /usr/bin/time -p ./399779_04 --progress 127
 *   ./399779_04 --check --no-bfile 18
 *
 * A normal run writes b-file lines both to standard output and, atomically,
 * to b399779_04.txt.  Existing output is preserved unless --force is given.
 */

#define PROGRAM_NAME "399779_04"
#define DEFAULT_BFILE "b399779_04.txt"
#define DEFAULT_SOLVER "cadical"
#define DEFAULT_MAX_N 20
#define MAX_N 255
#define DIRECT_CHECK_MAX_N 18

typedef struct {
    uint16_t vertex[3];
    uint8_t size;
} Hyperedge;

typedef struct {
    Hyperedge *data;
    size_t count;
    size_t capacity;
} Hypergraph;

typedef struct {
    int literal[3];
    uint8_t size;
} Clause;

typedef struct {
    Clause *data;
    size_t count;
    size_t capacity;
    int variable_count;
} Formula;

typedef enum {
    SAT_RESULT_ERROR = 0,
    SAT_RESULT_SAT,
    SAT_RESULT_UNSAT
} SatResult;

typedef struct {
    int min_n;
    int max_n;
    bool witness;
    bool progress;
    bool check;
    bool write_bfile;
    bool force;
    const char *output_path;
    const char *solver_path;
} Options;

typedef struct {
    SatResult result;
    bool *selected;
    int variable_count;
    size_t clause_count;
    double elapsed;
} SatAnswer;

static char *active_output_path = NULL;
static char active_cnf_path[PATH_MAX] = "";
static char active_solution_path[PATH_MAX] = "";
static volatile sig_atomic_t active_solver_pid = 0;

static void die_errno(const char *operation, const char *path);

static void cleanup_temporary_files(void)
{
    if (active_output_path != NULL) {
        (void)unlink(active_output_path);
    }
    if (active_cnf_path[0] != '\0') {
        (void)unlink(active_cnf_path);
    }
    if (active_solution_path[0] != '\0') {
        (void)unlink(active_solution_path);
    }
}

static void handle_termination_signal(int signal_number)
{
    const pid_t solver = (pid_t)active_solver_pid;
    if (solver > 0) {
        (void)kill(solver, signal_number);
    }
    if (active_output_path != NULL) {
        (void)unlink(active_output_path);
    }
    if (active_cnf_path[0] != '\0') {
        (void)unlink(active_cnf_path);
    }
    if (active_solution_path[0] != '\0') {
        (void)unlink(active_solution_path);
    }
    _exit(128 + signal_number);
}

static void install_signal_handlers(void)
{
    const struct sigaction action = {
        .sa_handler = handle_termination_signal,
        .sa_flags = 0
    };
    struct sigaction configured = action;
    if (sigemptyset(&configured.sa_mask) != 0 ||
        sigaction(SIGINT, &configured, NULL) != 0 ||
        sigaction(SIGTERM, &configured, NULL) != 0 ||
        sigaction(SIGHUP, &configured, NULL) != 0) {
        die_errno("cannot install signal handlers", NULL);
    }
}

static void die(const char *message)
{
    fprintf(stderr, "%s: %s\n", PROGRAM_NAME, message);
    exit(EXIT_FAILURE);
}

static void die_errno(const char *operation, const char *path)
{
    fprintf(stderr, "%s: %s %s: %s\n", PROGRAM_NAME, operation,
            path == NULL ? "" : path, strerror(errno));
    exit(EXIT_FAILURE);
}

static double now_seconds(void)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        die_errno("clock_gettime", NULL);
    }
    return (double)ts.tv_sec + 1.0e-9 * (double)ts.tv_nsec;
}

static bool is_prime(int value)
{
    if (value < 2) {
        return false;
    }
    for (int divisor = 2; divisor <= value / divisor; ++divisor) {
        if (value % divisor == 0) {
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
    const uint64_t aa = (uint64_t)a * (uint64_t)a;
    const uint64_t bb = (uint64_t)b * (uint64_t)b;
    return bb % (uint64_t)a == 0 || aa % (uint64_t)b == 0;
}

static bool forbidden_triple(int a, int b, int c)
{
    return ((uint64_t)b * (uint64_t)c) % (uint64_t)a == 0 ||
           ((uint64_t)a * (uint64_t)c) % (uint64_t)b == 0 ||
           ((uint64_t)a * (uint64_t)b) % (uint64_t)c == 0;
}

static void *checked_realloc(void *pointer, size_t count, size_t size)
{
    if (size != 0 && count > SIZE_MAX / size) {
        die("allocation-size overflow");
    }
    void *result = realloc(pointer, count * size);
    if (result == NULL) {
        die("memory allocation failed");
    }
    return result;
}

static void add_hyperedge(Hypergraph *graph, int a, int b, int c, int size)
{
    if (graph->count == graph->capacity) {
        size_t capacity = graph->capacity == 0 ? 1024U : graph->capacity * 2U;
        if (capacity < graph->capacity) {
            die("hyperedge-capacity overflow");
        }
        graph->data = checked_realloc(graph->data, capacity,
                                      sizeof(*graph->data));
        graph->capacity = capacity;
    }
    Hyperedge *edge = &graph->data[graph->count++];
    edge->vertex[0] = (uint16_t)a;
    edge->vertex[1] = (uint16_t)b;
    edge->vertex[2] = (uint16_t)c;
    edge->size = (uint8_t)size;
}

static Hypergraph build_hypergraph(int n)
{
    Hypergraph graph = {0};
    const size_t side = (size_t)n + 1U;
    if (side > SIZE_MAX / side) {
        die("pair-table size overflow");
    }
    uint8_t *pair = calloc(side * side, sizeof(*pair));
    if (pair == NULL) {
        die("unable to allocate pair table");
    }

    for (int a = 1; a <= n; ++a) {
        for (int b = a + 1; b <= n; ++b) {
            if (forbidden_pair(a, b)) {
                pair[(size_t)a * side + (size_t)b] = 1;
                add_hyperedge(&graph, a, b, 0, 2);
            }
        }
    }
    for (int a = 1; a <= n; ++a) {
        for (int b = a + 1; b <= n; ++b) {
            if (pair[(size_t)a * side + (size_t)b] != 0) {
                continue;
            }
            for (int c = b + 1; c <= n; ++c) {
                if (pair[(size_t)a * side + (size_t)c] != 0 ||
                    pair[(size_t)b * side + (size_t)c] != 0) {
                    continue;
                }
                if (forbidden_triple(a, b, c)) {
                    add_hyperedge(&graph, a, b, c, 3);
                }
            }
        }
    }
    free(pair);
    return graph;
}

static void free_hypergraph(Hypergraph *graph)
{
    free(graph->data);
    memset(graph, 0, sizeof(*graph));
}

static bool valid_by_hypergraph(const Hypergraph *graph,
                                const bool *selected)
{
    for (size_t index = 0; index < graph->count; ++index) {
        const Hyperedge *edge = &graph->data[index];
        bool contained = true;
        for (int position = 0; position < edge->size; ++position) {
            contained = contained && selected[edge->vertex[position]];
        }
        if (contained) {
            return false;
        }
    }
    return true;
}

static bool valid_direct_mask(uint64_t mask, int n)
{
    for (int a = 1; a <= n; ++a) {
        if ((mask & (UINT64_C(1) << (a - 1))) == 0) {
            continue;
        }
        for (int b = 1; b <= n; ++b) {
            if (a == b || (mask & (UINT64_C(1) << (b - 1))) == 0) {
                continue;
            }
            for (int c = 1; c <= n; ++c) {
                if (a == c || (mask & (UINT64_C(1) << (c - 1))) == 0) {
                    continue;
                }
                if (((uint64_t)b * (uint64_t)c) % (uint64_t)a == 0) {
                    return false;
                }
            }
        }
    }
    return true;
}

static int popcount64(uint64_t value)
{
    return __builtin_popcountll((unsigned long long)value);
}

static int direct_maximum(int n)
{
    const uint64_t end = UINT64_C(1) << n;
    int best = 0;
    for (uint64_t mask = 0; mask < end; ++mask) {
        const int size = popcount64(mask);
        if (size > best && valid_direct_mask(mask, n)) {
            best = size;
        }
    }
    return best;
}

static void add_clause(Formula *formula, int a, int b, int c, int size)
{
    if (size < 1 || size > 3) {
        die("invalid internal clause size");
    }
    if (formula->count == formula->capacity) {
        size_t capacity = formula->capacity == 0 ? 2048U
                                                  : formula->capacity * 2U;
        if (capacity < formula->capacity) {
            die("clause-capacity overflow");
        }
        formula->data = checked_realloc(formula->data, capacity,
                                        sizeof(*formula->data));
        formula->capacity = capacity;
    }
    Clause *clause = &formula->data[formula->count++];
    clause->literal[0] = a;
    clause->literal[1] = b;
    clause->literal[2] = c;
    clause->size = (uint8_t)size;
}

static int new_variable(Formula *formula)
{
    if (formula->variable_count == INT_MAX) {
        die("DIMACS variable-number overflow");
    }
    return ++formula->variable_count;
}

/* Encode s <-> a OR x. */
static void encode_or(Formula *formula, int s, int a, int x)
{
    add_clause(formula, -a, s, 0, 2);
    add_clause(formula, -x, s, 0, 2);
    add_clause(formula, -s, a, x, 3);
}

/* Encode s <-> b AND x. */
static void encode_and(Formula *formula, int s, int b, int x)
{
    add_clause(formula, -s, b, 0, 2);
    add_clause(formula, -s, x, 0, 2);
    add_clause(formula, -b, -x, s, 3);
}

/* Encode s <-> a OR (b AND x). */
static void encode_step(Formula *formula, int s, int a, int b, int x)
{
    add_clause(formula, -a, s, 0, 2);
    add_clause(formula, -b, -x, s, 3);
    add_clause(formula, -s, a, b, 3);
    add_clause(formula, -s, a, x, 3);
}

static Formula build_formula(const Hypergraph *graph, int n, int target)
{
    if (target < 1 || target > n) {
        die("invalid cardinality target");
    }
    Formula formula = {.variable_count = n};
    for (size_t index = 0; index < graph->count; ++index) {
        const Hyperedge *edge = &graph->data[index];
        add_clause(&formula, -(int)edge->vertex[0],
                   -(int)edge->vertex[1],
                   edge->size == 3 ? -(int)edge->vertex[2] : 0,
                   edge->size);
    }

    const size_t rows = (size_t)n + 1U;
    const size_t columns = (size_t)target + 1U;
    if (rows > SIZE_MAX / columns) {
        die("sequential-counter table size overflow");
    }
    int *counter = calloc(rows * columns, sizeof(*counter));
    if (counter == NULL) {
        die("unable to allocate sequential counter");
    }
#define COUNTER(i, j) counter[(size_t)(i) * columns + (size_t)(j)]
    for (int i = 1; i <= n; ++i) {
        const int last = i < target ? i : target;
        for (int j = 1; j <= last; ++j) {
            const int s = new_variable(&formula);
            COUNTER(i, j) = s;
            if (i == 1 && j == 1) {
                /* s(1,1) <-> x_1. */
                add_clause(&formula, -s, 1, 0, 2);
                add_clause(&formula, -1, s, 0, 2);
            } else if (j == 1) {
                encode_or(&formula, s, COUNTER(i - 1, 1), i);
            } else if (j == i) {
                encode_and(&formula, s, COUNTER(i - 1, j - 1), i);
            } else {
                encode_step(&formula, s, COUNTER(i - 1, j),
                            COUNTER(i - 1, j - 1), i);
            }
        }
    }
    add_clause(&formula, COUNTER(n, target), 0, 0, 1);
#undef COUNTER
    free(counter);
    return formula;
}

static void free_formula(Formula *formula)
{
    free(formula->data);
    memset(formula, 0, sizeof(*formula));
}

static void make_temporary_file(char path[PATH_MAX], const char *stem,
                                int *descriptor)
{
    const int written = snprintf(path, PATH_MAX, "/tmp/%s.%ld.XXXXXX", stem,
                                 (long)getpid());
    if (written < 0 || written >= PATH_MAX) {
        die("temporary path is too long");
    }
    *descriptor = mkstemp(path);
    if (*descriptor < 0) {
        die_errno("cannot create temporary file", path);
    }
}

static void write_dimacs(const Formula *formula, int descriptor,
                         const char *path)
{
    FILE *stream = fdopen(descriptor, "w");
    if (stream == NULL) {
        (void)close(descriptor);
        die_errno("fdopen failed for", path);
    }
    if (fprintf(stream, "p cnf %d %zu\n", formula->variable_count,
                formula->count) < 0) {
        die_errno("cannot write", path);
    }
    for (size_t index = 0; index < formula->count; ++index) {
        const Clause *clause = &formula->data[index];
        for (int position = 0; position < clause->size; ++position) {
            if (fprintf(stream, "%d ", clause->literal[position]) < 0) {
                die_errno("cannot write", path);
            }
        }
        if (fprintf(stream, "0\n") < 0) {
            die_errno("cannot write", path);
        }
    }
    if (fclose(stream) != 0) {
        die_errno("cannot close", path);
    }
}

static int wait_for_solver(pid_t child, bool progress, int n, int target,
                           double started)
{
    int status = 0;
    double next_report = 10.0;
    for (;;) {
        const int flags = progress ? WNOHANG : 0;
        const pid_t result = waitpid(child, &status, flags);
        if (result == child) {
            break;
        }
        if (result < 0) {
            if (errno == EINTR) {
                continue;
            }
            die_errno("waitpid failed", NULL);
        }
        const double elapsed = now_seconds() - started;
        if (elapsed >= next_report) {
            fprintf(stderr,
                    "[n=%d: SAT target=%d still running, %.1f s]\n",
                    n, target, elapsed);
            next_report += 10.0;
        }
        struct timespec pause = {.tv_sec = 0, .tv_nsec = 200000000L};
        while (nanosleep(&pause, &pause) != 0 && errno == EINTR) {
        }
    }
    return status;
}

static SatResult parse_solution(const char *path, int variables,
                                int selected_variables, bool *selected,
                                bool expect_sat)
{
    FILE *stream = fopen(path, "r");
    if (stream == NULL) {
        die_errno("cannot open SAT result", path);
    }
    int8_t *assignment = calloc((size_t)variables + 1U,
                                sizeof(*assignment));
    if (assignment == NULL) {
        fclose(stream);
        die("unable to allocate SAT assignment");
    }
    SatResult parsed = SAT_RESULT_ERROR;
    char line[8192];
    while (fgets(line, sizeof(line), stream) != NULL) {
        if (line[0] == 's') {
            if (strstr(line, "UNSATISFIABLE") != NULL) {
                parsed = SAT_RESULT_UNSAT;
            } else if (strstr(line, "SATISFIABLE") != NULL) {
                parsed = SAT_RESULT_SAT;
            }
        } else if (line[0] == 'v') {
            char *cursor = line + 1;
            for (;;) {
                while (*cursor == ' ' || *cursor == '\t' ||
                       *cursor == '\r' || *cursor == '\n') {
                    ++cursor;
                }
                if (*cursor == '\0') {
                    break;
                }
                errno = 0;
                char *end = NULL;
                const long literal = strtol(cursor, &end, 10);
                if (errno != 0 || end == cursor ||
                    literal < -variables || literal > variables) {
                    free(assignment);
                    fclose(stream);
                    die("malformed literal in SAT result");
                }
                cursor = end;
                if (literal == 0) {
                    continue;
                }
                const int variable = literal < 0 ? -(int)literal
                                                 : (int)literal;
                const int8_t value = literal > 0 ? 1 : -1;
                if (assignment[variable] != 0 &&
                    assignment[variable] != value) {
                    free(assignment);
                    fclose(stream);
                    die("contradictory SAT assignment");
                }
                assignment[variable] = value;
            }
        }
    }
    if (ferror(stream)) {
        free(assignment);
        fclose(stream);
        die_errno("cannot read SAT result", path);
    }
    if (fclose(stream) != 0) {
        free(assignment);
        die_errno("cannot close SAT result", path);
    }
    if (parsed == SAT_RESULT_ERROR ||
        (expect_sat && parsed != SAT_RESULT_SAT) ||
        (!expect_sat && parsed != SAT_RESULT_UNSAT)) {
        free(assignment);
        die("SAT result file disagrees with solver exit status");
    }
    if (parsed == SAT_RESULT_SAT) {
        for (int value = 1;
             selected != NULL && value <= selected_variables; ++value) {
            if (assignment[value] == 0) {
                free(assignment);
                die("SAT result omits a required variable assignment");
            }
            selected[value] = assignment[value] > 0;
        }
    }
    free(assignment);
    return parsed;
}

static SatAnswer run_sat(const Formula *formula, int n, int target,
                         const char *solver_path, bool progress)
{
    int cnf_descriptor = -1;
    int solution_descriptor = -1;
    make_temporary_file(active_cnf_path, "399779_04_cnf", &cnf_descriptor);
    make_temporary_file(active_solution_path, "399779_04_solution",
                        &solution_descriptor);
    if (close(solution_descriptor) != 0) {
        die_errno("cannot close", active_solution_path);
    }
    write_dimacs(formula, cnf_descriptor, active_cnf_path);

    const double started = now_seconds();
    const pid_t child = fork();
    if (child < 0) {
        die_errno("fork failed", NULL);
    }
    if (child == 0) {
        const int devnull = open("/dev/null", O_WRONLY);
        if (devnull < 0 || dup2(devnull, STDOUT_FILENO) < 0) {
            _exit(126);
        }
        if (devnull != STDOUT_FILENO) {
            (void)close(devnull);
        }
        char *const arguments[] = {
            (char *)solver_path,
            (char *)"-q",
            (char *)"-w",
            active_solution_path,
            active_cnf_path,
            NULL
        };
        execvp(solver_path, arguments);
        _exit(127);
    }
    active_solver_pid = (sig_atomic_t)child;
    const int status = wait_for_solver(child, progress, n, target, started);
    active_solver_pid = 0;
    const double elapsed = now_seconds() - started;
    if (!WIFEXITED(status)) {
        die("SAT solver terminated abnormally");
    }
    const int exit_code = WEXITSTATUS(status);
    if (exit_code == 126) {
        die("SAT solver child could not redirect its output");
    }
    if (exit_code == 127) {
        fprintf(stderr,
                "%s: cannot execute SAT solver '%s'; install CaDiCaL or "
                "pass --solver PATH\n",
                PROGRAM_NAME, solver_path);
        exit(EXIT_FAILURE);
    }
    if (exit_code != 10 && exit_code != 20) {
        fprintf(stderr, "%s: SAT solver returned unexpected status %d\n",
                PROGRAM_NAME, exit_code);
        exit(EXIT_FAILURE);
    }

    SatAnswer answer = {
        .result = exit_code == 10 ? SAT_RESULT_SAT : SAT_RESULT_UNSAT,
        .selected = NULL,
        .variable_count = formula->variable_count,
        .clause_count = formula->count,
        .elapsed = elapsed
    };
    if (answer.result == SAT_RESULT_SAT) {
        answer.selected = calloc((size_t)n + 1U, sizeof(*answer.selected));
        if (answer.selected == NULL) {
            die("unable to allocate selected-set result");
        }
    }
    (void)parse_solution(active_solution_path, formula->variable_count, n,
                         answer.selected,
                         answer.result == SAT_RESULT_SAT);

    if (unlink(active_cnf_path) != 0) {
        die_errno("cannot remove", active_cnf_path);
    }
    active_cnf_path[0] = '\0';
    if (unlink(active_solution_path) != 0) {
        die_errno("cannot remove", active_solution_path);
    }
    active_solution_path[0] = '\0';
    return answer;
}

static int selected_count(const bool *selected, int n)
{
    int count = 0;
    for (int value = 1; value <= n; ++value) {
        count += selected[value] ? 1 : 0;
    }
    return count;
}

static void copy_witness(bool *destination, const bool *source, int n)
{
    memcpy(destination, source, ((size_t)n + 1U) * sizeof(*destination));
}

static void print_witness(FILE *stream, const bool *selected, int n)
{
    fprintf(stream, "{");
    bool first = true;
    for (int value = 1; value <= n; ++value) {
        if (selected[value]) {
            fprintf(stream, "%s%d", first ? "" : ",", value);
            first = false;
        }
    }
    fprintf(stream, "}");
}

static int solve_initial(const Hypergraph *graph, int n, const Options *options,
                         bool *witness, int *sat_calls, int *last_variables,
                         size_t *last_clauses, double *sat_seconds)
{
    int answer = 0;
    memset(witness, 0, ((size_t)n + 1U) * sizeof(*witness));
    for (int value = 2; value <= n; ++value) {
        if (is_prime(value)) {
            witness[value] = true;
            ++answer;
        }
    }
    if (!valid_by_hypergraph(graph, witness)) {
        die("internal prime-set seed is invalid");
    }

    while (answer < n) {
        const int target = answer + 1;
        Formula formula = build_formula(graph, n, target);
        SatAnswer sat = run_sat(&formula, n, target, options->solver_path,
                                options->progress);
        ++*sat_calls;
        *last_variables = sat.variable_count;
        *last_clauses = sat.clause_count;
        *sat_seconds += sat.elapsed;
        free_formula(&formula);
        if (sat.result == SAT_RESULT_UNSAT) {
            free(sat.selected);
            break;
        }
        const int size = selected_count(sat.selected, n);
        if (size < target || !valid_by_hypergraph(graph, sat.selected)) {
            free(sat.selected);
            die("SAT solver returned an invalid witness");
        }
        answer = size;
        copy_witness(witness, sat.selected, n);
        free(sat.selected);
    }
    return answer;
}

static int solve_consecutive(const Hypergraph *graph, int n,
                             const Options *options, int previous_answer,
                             bool *witness, int *sat_calls,
                             int *last_variables, size_t *last_clauses,
                             double *sat_seconds, const char **method)
{
    witness[n] = true;
    if (valid_by_hypergraph(graph, witness)) {
        *method = "extension";
        return previous_answer + 1;
    }
    witness[n] = false;

    const int target = previous_answer + 1;
    Formula formula = build_formula(graph, n, target);
    SatAnswer sat = run_sat(&formula, n, target, options->solver_path,
                            options->progress);
    ++*sat_calls;
    *last_variables = sat.variable_count;
    *last_clauses = sat.clause_count;
    *sat_seconds += sat.elapsed;
    free_formula(&formula);

    if (sat.result == SAT_RESULT_UNSAT) {
        *method = "UNSAT";
        free(sat.selected);
        return previous_answer;
    }
    const int size = selected_count(sat.selected, n);
    if (size != target || !valid_by_hypergraph(graph, sat.selected)) {
        free(sat.selected);
        die("SAT witness contradicts the consecutive-term bound");
    }
    copy_witness(witness, sat.selected, n);
    free(sat.selected);
    *method = "SAT";
    return target;
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
            "Usage: %s [--min-n N] [--solver PATH] [--witness] "
            "[--progress] [--check]\n"
            "       [--output FILE | --no-bfile] [--force] [nmax]\n"
            "  --min-n N   start at N (default 1; solved without prior data)\n"
            "  --solver P  SAT solver executable (default %s)\n"
            "  --witness   print one maximum set for every n\n"
            "  --progress  report a running SAT call every about 10 seconds\n"
            "  --check     direct exhaustive cross-check for n <= %d\n"
            "  --output F  save the b-file as F (default %s)\n"
            "  --no-bfile  do not save a b-file\n"
            "  --force     replace an existing output file atomically\n"
            "  nmax        final n (default %d, hard limit %d)\n"
            "B-file lines are also printed to standard output; diagnostics "
            "use standard error.\n",
            PROGRAM_NAME, DEFAULT_SOLVER, DIRECT_CHECK_MAX_N, DEFAULT_BFILE,
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
        .output_path = DEFAULT_BFILE,
        .solver_path = DEFAULT_SOLVER
    };
    bool positional_seen = false;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--min-n") == 0) {
            if (++i >= argc) {
                die("--min-n requires an argument");
            }
            options.min_n = parse_int(argv[i], "minimum n");
        } else if (strcmp(argv[i], "--solver") == 0) {
            if (++i >= argc || argv[i][0] == '\0') {
                die("--solver requires a nonempty path");
            }
            options.solver_path = argv[i];
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
            die_errno("cannot inspect output file", options->output_path);
        }
    }
    const size_t length = strlen(options->output_path) + 48U;
    active_output_path = malloc(length);
    if (active_output_path == NULL) {
        die("unable to allocate temporary output path");
    }
    const int written = snprintf(active_output_path, length, "%s.part.%ld",
                                 options->output_path, (long)getpid());
    if (written < 0 || (size_t)written >= length) {
        die("temporary output path is too long");
    }
    FILE *stream = fopen(active_output_path, "wx");
    if (stream == NULL) {
        die_errno("cannot create temporary output", active_output_path);
    }
    return stream;
}

static void finish_output(FILE *stream, const Options *options)
{
    if (stream == NULL) {
        return;
    }
    if (fclose(stream) != 0) {
        die_errno("cannot close temporary output", active_output_path);
    }
    int status;
    if (options->force) {
        status = rename(active_output_path, options->output_path);
    } else {
        status = link(active_output_path, options->output_path);
        if (status == 0) {
            status = unlink(active_output_path);
        }
    }
    if (status != 0) {
        die_errno("cannot install output file", options->output_path);
    }
    char *const completed_path = active_output_path;
    active_output_path = NULL;
    free(completed_path);
    fprintf(stderr, "saved %s\n", options->output_path);
}

int main(int argc, char **argv)
{
    const Options options = parse_options(argc, argv);
    if (atexit(cleanup_temporary_files) != 0) {
        die("cannot register temporary-file cleanup");
    }
    install_signal_handlers();
    FILE *output = open_temporary_output(&options);
    bool *witness = calloc((size_t)options.max_n + 1U, sizeof(*witness));
    if (witness == NULL) {
        die("unable to allocate witness");
    }
    bool have_previous = false;
    int previous_answer = 0;

    for (int n = options.min_n; n <= options.max_n; ++n) {
        const double started = now_seconds();
        Hypergraph graph = build_hypergraph(n);
        int sat_calls = 0;
        int last_variables = 0;
        size_t last_clauses = 0;
        double sat_seconds = 0.0;
        const char *method = "initial";
        int answer;
        if (!have_previous) {
            answer = solve_initial(&graph, n, &options, witness, &sat_calls,
                                   &last_variables, &last_clauses,
                                   &sat_seconds);
        } else {
            answer = solve_consecutive(&graph, n, &options, previous_answer,
                                       witness, &sat_calls, &last_variables,
                                       &last_clauses, &sat_seconds, &method);
        }
        if (selected_count(witness, n) != answer ||
            !valid_by_hypergraph(&graph, witness)) {
            free_hypergraph(&graph);
            die("internal final-witness verification failed");
        }
        if (options.check && n <= DIRECT_CHECK_MAX_N) {
            const int direct = direct_maximum(n);
            if (direct != answer) {
                free_hypergraph(&graph);
                die("direct exhaustive cross-check failed");
            }
        }

        if (printf("%d %d\n", n, answer) < 0 || fflush(stdout) != 0 ||
            (output != NULL &&
             (fprintf(output, "%d %d\n", n, answer) < 0 ||
              fflush(output) != 0))) {
            free_hypergraph(&graph);
            die("cannot write b-file output");
        }
        const double elapsed = now_seconds() - started;
        fprintf(stderr,
                "[n=%d: F(n)=%d, pi(n)=%d, excess=%d, edges=%zu, "
                "method=%s, SAT calls=%d, last CNF=%d vars/%zu clauses, "
                "SAT %.3f s, total %.3f s",
                n, answer, prime_count(n), answer - prime_count(n),
                graph.count, method, sat_calls, last_variables,
                last_clauses, sat_seconds, elapsed);
        if (options.check && n <= DIRECT_CHECK_MAX_N) {
            fprintf(stderr, ", checked");
        }
        if (options.witness) {
            fprintf(stderr, ", witness=");
            print_witness(stderr, witness, n);
        }
        fprintf(stderr, "]\n");
        free_hypergraph(&graph);
        have_previous = true;
        previous_answer = answer;
    }
    free(witness);
    finish_output(output, &options);
    return EXIT_SUCCESS;
}
