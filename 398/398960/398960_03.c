/*
 * A398960 -- sparse dancing-links exact-cover search.
 *
 * Count pairings {b(i),c(i)} of {1,...,2*n}, b(i)<c(i), such that all 2*n
 * positive values b(i)+c(i)-1 and c(i)-b(i) are different.
 *
 * A candidate pair is a four-node exact-cover row:
 *
 *     vertex b, vertex c, value b+c-1, value c-b.
 *
 * Vertex columns are primary and must be covered exactly once.  Shifted sums
 * and differences share one family of secondary columns and may be covered at
 * most once.  Sparse Algorithm X maintains column sizes with dancing links;
 * the smallest primary column is selected without rescanning edge bitsets.
 *
 * Two-row root branches are dynamically shared by pthread workers.  Each
 * worker owns a complete mutable DLX structure, while the problem and root
 * tasks are immutable.  Only task and progress counters are atomic.
 *
 * Every answer addition is checked.  The unrestricted count (2*n-1)!! fits
 * unsigned __int128 through n=28, which is the hard limit.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic -pthread \
 *       398960_03.c -o 398960_03
 *
 * Usage:
 *   ./398960_03 --term 13 --threads 4
 *   ./398960_03 --upto 13 --start 13 --threads 4
 *   ./398960_03 --check 13 --threads 4
 *
 * A positional N is shorthand for --upto N.  --upto writes
 * b398960_03.txt through b398960_03_part.txt unless --no-bfile is given.
 * --start S copies the verified built-in prefix n<S to the b-file and only
 * calculates and prints S..N.  --term and --check do not change the b-file.
 * A file lock serializes simultaneous b-file writers.  Progress is written
 * to stderr every 30 seconds by default; --progress SEC changes the interval,
 * and zero disables progress reporting.
 */

#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <limits.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#if !defined(__SIZEOF_INT128__)
#error "398960_03.c requires unsigned __int128"
#endif

__extension__ typedef unsigned __int128 U128;

#define MIN_N 0
#define MAX_N 28
#define KNOWN_MAX_N 13
#define DEFAULT_THREADS 4
#define MAX_THREADS 64
#define DEFAULT_PROGRESS_SECONDS 30
#define MAX_PROGRESS_SECONDS 3600
#define NODE_FLUSH_INTERVAL UINT64_C(65536)
#define MAX_VERTICES (2 * MAX_N)
#define MAX_ROWS (MAX_VERTICES * (MAX_VERTICES - 1) / 2)
#define MAX_VALUES (4 * MAX_N - 2)
#define MAX_COLUMNS (MAX_VERTICES + MAX_VALUES)
#define MAX_DLX_NODES (1 + MAX_COLUMNS + 4 * MAX_ROWS)

#define BFILE_NAME "b398960_03.txt"
#define PART_FILE_NAME "b398960_03_part.txt"
#define LOCK_FILE_NAME "b398960_03.txt.lock"

typedef enum {
    MODE_NONE,
    MODE_TERM,
    MODE_UPTO,
    MODE_CHECK
} RunMode;

typedef struct {
    unsigned x;
    unsigned y;
    unsigned shifted_sum;
    unsigned difference;
} Edge;

typedef struct {
    unsigned n;
    unsigned vertex_count;
    unsigned maximum_value;
    unsigned column_count;
    unsigned row_count;
    unsigned node_capacity;
    Edge *edges;
} Problem;

typedef struct {
    int left;
    int right;
    int up;
    int down;
    unsigned column;
    unsigned row;
} DlxNode;

typedef struct {
    const Problem *problem;
    DlxNode *nodes;
    unsigned *column_sizes;
    int *row_heads;
    unsigned node_count;
} Dlx;

typedef struct {
    unsigned first_row;
    unsigned second_row;
} RootTask;

typedef struct {
    const Problem *problem;
    const RootTask *root_tasks;
    unsigned root_count;
    _Atomic unsigned next_root;
    _Atomic unsigned completed_roots;
    _Atomic uint64_t completed_nodes;
    bool done;
    pthread_mutex_t progress_mutex;
    pthread_cond_t progress_condition;
    double start_time;
    unsigned progress_seconds;
} TaskQueue;

typedef struct {
    TaskQueue *queue;
    Dlx *dlx;
    U128 result;
    uint64_t pending_nodes;
} Worker;

typedef struct {
    RunMode mode;
    int n;
    int start;
    unsigned threads;
    unsigned progress_seconds;
    bool write_bfile;
} Options;

static const uint64_t known_terms[KNOWN_MAX_N + 1] = {
    UINT64_C(1), UINT64_C(1), UINT64_C(0), UINT64_C(1),
    UINT64_C(4), UINT64_C(15), UINT64_C(52), UINT64_C(257),
    UINT64_C(1589), UINT64_C(11417), UINT64_C(75375),
    UINT64_C(616010), UINT64_C(5253278), UINT64_C(49667373)
};

static _Noreturn void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static _Noreturn void die_errno(const char *message)
{
    fprintf(stderr, "error: %s: %s\n", message, strerror(errno));
    exit(EXIT_FAILURE);
}

static double monotonic_seconds(void)
{
    struct timespec value;
    if (clock_gettime(CLOCK_MONOTONIC, &value) != 0) {
        die_errno("clock_gettime failed");
    }
    return (double)value.tv_sec + (double)value.tv_nsec / 1e9;
}

static int parse_integer(const char *text, int minimum, int maximum,
                         const char *name)
{
    char *end = NULL;
    errno = 0;
    const long value = strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' ||
        value < minimum || value > maximum) {
        fprintf(stderr, "error: %s must be in %d..%d: %s\n",
                name, minimum, maximum, text);
        exit(EXIT_FAILURE);
    }
    return (int)value;
}

static unsigned parse_unsigned(const char *text, unsigned minimum,
                               unsigned maximum, const char *name)
{
    char *end = NULL;
    errno = 0;
    const unsigned long value = strtoul(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' ||
        value < minimum || value > maximum) {
        fprintf(stderr, "error: %s must be in %u..%u: %s\n",
                name, minimum, maximum, text);
        exit(EXIT_FAILURE);
    }
    return (unsigned)value;
}

static void print_u128(FILE *stream, U128 value)
{
    char digits[40];
    size_t length = 0;
    do {
        digits[length++] = (char)('0' + (unsigned)(value % 10));
        value /= 10;
    } while (value != 0);
    while (length != 0) {
        if (fputc(digits[--length], stream) == EOF) {
            die("could not write an integer");
        }
    }
}

static void write_term(FILE *stream, int n, U128 value)
{
    if (fprintf(stream, "%d ", n) < 0) {
        die("could not write a term index");
    }
    print_u128(stream, value);
    if (fputc('\n', stream) == EOF || fflush(stream) != 0) {
        die_errno("could not flush a sequence term");
    }
}

static void add_u128(U128 *destination, U128 addend)
{
    const U128 maximum = ~(U128)0;
    if (*destination > maximum - addend) {
        die("answer overflow in unsigned __int128");
    }
    *destination += addend;
}

static U128 pairing_upper_bound(unsigned n)
{
    U128 result = 1;
    for (unsigned odd = 1; odd < 2 * n; odd += 2) {
        const U128 previous = result;
        result *= odd;
        if (result / odd != previous) {
            die("pairing upper bound overflow");
        }
    }
    return result;
}

static Problem *make_problem(unsigned n)
{
    Problem *problem = calloc(1, sizeof(*problem));
    if (problem == NULL) {
        die("could not allocate the problem descriptor");
    }
    problem->n = n;
    problem->vertex_count = 2 * n;
    problem->maximum_value = 4 * n - 2;
    problem->column_count =
        problem->vertex_count + problem->maximum_value;
    problem->row_count =
        problem->vertex_count * (problem->vertex_count - 1) / 2;
    problem->node_capacity =
        1 + problem->column_count + 4 * problem->row_count;
    if (problem->vertex_count > MAX_VERTICES ||
        problem->maximum_value > MAX_VALUES ||
        problem->column_count > MAX_COLUMNS ||
        problem->row_count > MAX_ROWS ||
        problem->node_capacity > MAX_DLX_NODES ||
        problem->node_capacity > (unsigned)INT_MAX) {
        die("internal DLX dimensions are out of range");
    }
    problem->edges = calloc(problem->row_count, sizeof(*problem->edges));
    if (problem->edges == NULL) {
        die("could not allocate the edge table");
    }
    unsigned row_index = 0;
    for (unsigned x = 0; x < problem->vertex_count; ++x) {
        for (unsigned y = x + 1; y < problem->vertex_count; ++y) {
            if (row_index >= problem->row_count) {
                die("internal edge-table overflow");
            }
            Edge *edge = &problem->edges[row_index++];
            edge->x = x;
            edge->y = y;
            edge->shifted_sum = x + y + 1;
            edge->difference = y - x;
            if (edge->shifted_sum == edge->difference ||
                edge->shifted_sum == 0 ||
                edge->difference == 0 ||
                edge->shifted_sum > problem->maximum_value ||
                edge->difference > problem->maximum_value) {
                die("internal edge value is out of range");
            }
        }
    }
    if (row_index != problem->row_count) {
        die("internal edge-count mismatch");
    }
    return problem;
}

static void free_problem(Problem *problem)
{
    if (problem == NULL) {
        return;
    }
    free(problem->edges);
    free(problem);
}

static Dlx *make_dlx(const Problem *problem)
{
    Dlx *dlx = calloc(1, sizeof(*dlx));
    if (dlx == NULL) {
        die("could not allocate the DLX descriptor");
    }
    dlx->problem = problem;
    dlx->nodes = calloc(problem->node_capacity, sizeof(*dlx->nodes));
    dlx->column_sizes = calloc(
        (size_t)problem->column_count + 1, sizeof(*dlx->column_sizes));
    dlx->row_heads = malloc(
        (size_t)problem->row_count * sizeof(*dlx->row_heads));
    if (dlx->nodes == NULL || dlx->column_sizes == NULL ||
        dlx->row_heads == NULL) {
        die("could not allocate the DLX tables");
    }

    dlx->nodes[0] = (DlxNode) {
        .left = (int)problem->vertex_count,
        .right = problem->vertex_count == 0 ? 0 : 1,
        .up = 0,
        .down = 0,
        .column = 0,
        .row = UINT_MAX
    };
    for (unsigned column = 1; column <= problem->column_count; ++column) {
        const int header = (int)column;
        dlx->nodes[header] = (DlxNode) {
            .left = header,
            .right = header,
            .up = header,
            .down = header,
            .column = column,
            .row = UINT_MAX
        };
        if (column <= problem->vertex_count) {
            dlx->nodes[header].left = column == 1 ? 0 : header - 1;
            dlx->nodes[header].right =
                column == problem->vertex_count ? 0 : header + 1;
        }
    }

    unsigned next_node = 1 + problem->column_count;
    for (unsigned row_index = 0; row_index < problem->row_count;
         ++row_index) {
        const Edge *edge = &problem->edges[row_index];
        const unsigned columns[4] = {
            edge->x + 1,
            edge->y + 1,
            problem->vertex_count + edge->shifted_sum,
            problem->vertex_count + edge->difference
        };
        int row_nodes[4];
        for (unsigned offset = 0; offset < 4; ++offset) {
            if (columns[offset] == 0 ||
                columns[offset] > problem->column_count ||
                next_node >= problem->node_capacity) {
                die("internal DLX node is out of range");
            }
            const int node = (int)next_node++;
            const int column = (int)columns[offset];
            const int previous = dlx->nodes[column].up;
            dlx->nodes[node] = (DlxNode) {
                .left = node,
                .right = node,
                .up = previous,
                .down = column,
                .column = columns[offset],
                .row = row_index
            };
            dlx->nodes[previous].down = node;
            dlx->nodes[column].up = node;
            ++dlx->column_sizes[column];
            row_nodes[offset] = node;
        }
        for (unsigned offset = 0; offset < 4; ++offset) {
            dlx->nodes[row_nodes[offset]].left =
                row_nodes[(offset + 3) % 4];
            dlx->nodes[row_nodes[offset]].right =
                row_nodes[(offset + 1) % 4];
        }
        dlx->row_heads[row_index] = row_nodes[0];
    }
    if (next_node != problem->node_capacity) {
        die("internal DLX node-count mismatch");
    }
    dlx->node_count = next_node;
    return dlx;
}

static void free_dlx(Dlx *dlx)
{
    if (dlx == NULL) {
        return;
    }
    free(dlx->row_heads);
    free(dlx->column_sizes);
    free(dlx->nodes);
    free(dlx);
}

static inline void cover_column(Dlx *dlx, int column)
{
    DlxNode *nodes = dlx->nodes;
    nodes[nodes[column].right].left = nodes[column].left;
    nodes[nodes[column].left].right = nodes[column].right;
    for (int row = nodes[column].down; row != column;
         row = nodes[row].down) {
        for (int node = nodes[row].right; node != row;
             node = nodes[node].right) {
            nodes[nodes[node].down].up = nodes[node].up;
            nodes[nodes[node].up].down = nodes[node].down;
            const unsigned other_column = nodes[node].column;
            if (dlx->column_sizes[other_column] == 0) {
                die("internal DLX column-size underflow");
            }
            --dlx->column_sizes[other_column];
        }
    }
}

static inline void uncover_column(Dlx *dlx, int column)
{
    DlxNode *nodes = dlx->nodes;
    for (int row = nodes[column].up; row != column; row = nodes[row].up) {
        for (int node = nodes[row].left; node != row;
             node = nodes[node].left) {
            const unsigned other_column = nodes[node].column;
            if (dlx->column_sizes[other_column] >=
                dlx->problem->row_count) {
                die("internal DLX column-size overflow");
            }
            ++dlx->column_sizes[other_column];
            nodes[nodes[node].down].up = node;
            nodes[nodes[node].up].down = node;
        }
    }
    nodes[nodes[column].right].left = column;
    nodes[nodes[column].left].right = column;
}

static inline int choose_primary_column(const Dlx *dlx)
{
    const DlxNode *nodes = dlx->nodes;
    int best = nodes[0].right;
    if (best == 0) {
        return 0;
    }
    unsigned best_size = dlx->column_sizes[best];
    for (int column = nodes[best].right; column != 0;
         column = nodes[column].right) {
        const unsigned size = dlx->column_sizes[column];
        if (size < best_size) {
            best = column;
            best_size = size;
            if (best_size <= 1) {
                break;
            }
        }
    }
    return best;
}

static inline void record_node(Worker *worker)
{
    ++worker->pending_nodes;
    if (worker->pending_nodes == NODE_FLUSH_INTERVAL) {
        atomic_fetch_add_explicit(&worker->queue->completed_nodes,
                                  worker->pending_nodes,
                                  memory_order_relaxed);
        worker->pending_nodes = 0;
    }
}

static void flush_nodes(Worker *worker)
{
    if (worker->pending_nodes != 0) {
        atomic_fetch_add_explicit(&worker->queue->completed_nodes,
                                  worker->pending_nodes,
                                  memory_order_relaxed);
        worker->pending_nodes = 0;
    }
}

static U128 search(Worker *worker)
{
    record_node(worker);
    Dlx *dlx = worker->dlx;
    const int column = choose_primary_column(dlx);
    if (column == 0) {
        return 1;
    }
    if (dlx->column_sizes[column] == 0) {
        return 0;
    }
    U128 total = 0;
    cover_column(dlx, column);
    for (int row = dlx->nodes[column].down; row != column;
         row = dlx->nodes[row].down) {
        for (int node = dlx->nodes[row].right; node != row;
             node = dlx->nodes[node].right) {
            cover_column(dlx, (int)dlx->nodes[node].column);
        }
        add_u128(&total, search(worker));
        for (int node = dlx->nodes[row].left; node != row;
             node = dlx->nodes[node].left) {
            uncover_column(dlx, (int)dlx->nodes[node].column);
        }
    }
    uncover_column(dlx, column);
    return total;
}

static void select_fixed_row(Dlx *dlx, unsigned row_index)
{
    if (row_index >= dlx->problem->row_count) {
        die("internal fixed row is out of range");
    }
    const int head = dlx->row_heads[row_index];
    int node = head;
    do {
        if (dlx->nodes[dlx->nodes[node].up].down != node ||
            dlx->nodes[dlx->nodes[node].down].up != node) {
            die("internal fixed row is not active");
        }
        node = dlx->nodes[node].right;
    } while (node != head);
    node = head;
    do {
        cover_column(dlx, (int)dlx->nodes[node].column);
        node = dlx->nodes[node].right;
    } while (node != head);
}

static void unselect_fixed_row(Dlx *dlx, unsigned row_index)
{
    if (row_index >= dlx->problem->row_count) {
        die("internal fixed row is out of range during restoration");
    }
    const int head = dlx->row_heads[row_index];
    int node = dlx->nodes[head].left;
    for (;;) {
        uncover_column(dlx, (int)dlx->nodes[node].column);
        if (node == head) {
            break;
        }
        node = dlx->nodes[node].left;
    }
}

static void *worker_main(void *argument)
{
    Worker *worker = argument;
    TaskQueue *queue = worker->queue;
    worker->dlx = make_dlx(queue->problem);
    for (;;) {
        const unsigned task = atomic_fetch_add_explicit(
            &queue->next_root, 1, memory_order_relaxed);
        if (task >= queue->root_count) {
            break;
        }
        const RootTask *root_task = &queue->root_tasks[task];
        select_fixed_row(worker->dlx, root_task->first_row);
        select_fixed_row(worker->dlx, root_task->second_row);
        add_u128(&worker->result, search(worker));
        unselect_fixed_row(worker->dlx, root_task->second_row);
        unselect_fixed_row(worker->dlx, root_task->first_row);
        flush_nodes(worker);
        atomic_fetch_add_explicit(&queue->completed_roots, 1,
                                  memory_order_relaxed);
    }
    flush_nodes(worker);
    free_dlx(worker->dlx);
    worker->dlx = NULL;
    return NULL;
}

static void *progress_main(void *argument)
{
    TaskQueue *queue = argument;
    if (pthread_mutex_lock(&queue->progress_mutex) != 0) {
        die("could not lock the progress mutex");
    }
    while (!queue->done) {
        struct timespec deadline;
        if (clock_gettime(CLOCK_REALTIME, &deadline) != 0) {
            die_errno("clock_gettime failed for progress reporting");
        }
        deadline.tv_sec += (time_t)queue->progress_seconds;
        int wait_result = 0;
        while (!queue->done && wait_result != ETIMEDOUT) {
            wait_result = pthread_cond_timedwait(
                &queue->progress_condition, &queue->progress_mutex,
                &deadline);
            if (wait_result != 0 && wait_result != ETIMEDOUT) {
                die("progress condition wait failed");
            }
        }
        if (!queue->done && wait_result == ETIMEDOUT) {
            const double now = monotonic_seconds();
            const unsigned roots = atomic_load_explicit(
                &queue->completed_roots, memory_order_relaxed);
            const uint64_t nodes = atomic_load_explicit(
                &queue->completed_nodes, memory_order_relaxed);
            fprintf(stderr,
                    "progress: n=%u roots=%u/%u nodes=%" PRIu64
                    " elapsed=%.1f s\n",
                    queue->problem->n, roots, queue->root_count, nodes,
                    now - queue->start_time);
        }
    }
    if (pthread_mutex_unlock(&queue->progress_mutex) != 0) {
        die("could not unlock the progress mutex");
    }
    return NULL;
}

static U128 compute_term(unsigned n, unsigned requested_threads,
                         unsigned progress_seconds)
{
    if (n <= 1) {
        return 1;
    }
    Problem *problem = make_problem(n);
    const unsigned root_capacity =
        (problem->vertex_count - 1) * (problem->vertex_count - 3);
    RootTask *root_tasks = malloc(
        (size_t)root_capacity * sizeof(*root_tasks));
    if (root_tasks == NULL) {
        die("could not allocate the root task list");
    }
    Dlx *root_dlx = make_dlx(problem);
    unsigned root_count = 0;
    const int first_column = 1;
    cover_column(root_dlx, first_column);
    for (int first_node = root_dlx->nodes[first_column].down;
         first_node != first_column;
         first_node = root_dlx->nodes[first_node].down) {
        const unsigned first_row = root_dlx->nodes[first_node].row;
        for (int node = root_dlx->nodes[first_node].right;
             node != first_node; node = root_dlx->nodes[node].right) {
            cover_column(root_dlx,
                         (int)root_dlx->nodes[node].column);
        }
        const int second_column = choose_primary_column(root_dlx);
        if (second_column != 0 &&
            root_dlx->column_sizes[second_column] != 0) {
            cover_column(root_dlx, second_column);
            for (int second_node = root_dlx->nodes[second_column].down;
                 second_node != second_column;
                 second_node = root_dlx->nodes[second_node].down) {
                if (root_count >= root_capacity) {
                    die("internal root-task overflow");
                }
                root_tasks[root_count++] = (RootTask) {
                    .first_row = first_row,
                    .second_row = root_dlx->nodes[second_node].row
                };
            }
            uncover_column(root_dlx, second_column);
        }
        for (int node = root_dlx->nodes[first_node].left;
             node != first_node; node = root_dlx->nodes[node].left) {
            uncover_column(root_dlx,
                           (int)root_dlx->nodes[node].column);
        }
    }
    uncover_column(root_dlx, first_column);
    free_dlx(root_dlx);
    if (root_count == 0) {
        free(root_tasks);
        free_problem(problem);
        return 0;
    }
    const unsigned thread_count = requested_threads < root_count
        ? requested_threads : root_count;
    TaskQueue queue = {
        .problem = problem,
        .root_tasks = root_tasks,
        .root_count = root_count,
        .next_root = 0,
        .completed_roots = 0,
        .completed_nodes = 0,
        .done = false,
        .start_time = monotonic_seconds(),
        .progress_seconds = progress_seconds
    };
    Worker *workers = calloc(thread_count, sizeof(*workers));
    pthread_t *threads = calloc(thread_count, sizeof(*threads));
    if (workers == NULL || threads == NULL) {
        die("could not allocate worker descriptors");
    }

    pthread_t progress_thread;
    bool progress_started = false;
    if (progress_seconds != 0) {
        if (pthread_mutex_init(&queue.progress_mutex, NULL) != 0 ||
            pthread_cond_init(&queue.progress_condition, NULL) != 0) {
            die("could not initialize progress synchronization");
        }
        if (pthread_create(&progress_thread, NULL, progress_main, &queue) != 0) {
            die("could not create the progress thread");
        }
        progress_started = true;
    }

    for (unsigned index = 0; index < thread_count; ++index) {
        workers[index].queue = &queue;
        if (pthread_create(&threads[index], NULL, worker_main,
                           &workers[index]) != 0) {
            die("could not create a search worker");
        }
    }

    U128 answer = 0;
    for (unsigned index = 0; index < thread_count; ++index) {
        if (pthread_join(threads[index], NULL) != 0) {
            die("could not join a search worker");
        }
        add_u128(&answer, workers[index].result);
    }
    if (progress_started) {
        if (pthread_mutex_lock(&queue.progress_mutex) != 0) {
            die("could not lock the progress mutex at completion");
        }
        queue.done = true;
        if (pthread_cond_signal(&queue.progress_condition) != 0) {
            die("could not signal progress completion");
        }
        if (pthread_mutex_unlock(&queue.progress_mutex) != 0) {
            die("could not unlock the progress mutex at completion");
        }
        if (pthread_join(progress_thread, NULL) != 0) {
            die("could not join the progress thread");
        }
        if (pthread_cond_destroy(&queue.progress_condition) != 0 ||
            pthread_mutex_destroy(&queue.progress_mutex) != 0) {
            die("could not destroy progress synchronization");
        }
    }

    if (answer > pairing_upper_bound(n)) {
        die("answer exceeds the unrestricted pairing bound");
    }
    free(threads);
    free(workers);
    free(root_tasks);
    free_problem(problem);
    return answer;
}

static void usage(FILE *stream, const char *program)
{
    fprintf(stream,
            "usage:\n"
            "  %s --term N [--threads T] [--progress SEC]\n"
            "  %s --upto N [--start S] [--threads T] [--progress SEC]\n"
            "  %s N [--start S] [--threads T] [--progress SEC]\n"
            "  %s --check N [--threads T] [--progress SEC]\n",
            program, program, program, program);
}

static Options parse_options(int argc, char **argv)
{
    Options options = {
        .mode = MODE_NONE,
        .n = -1,
        .start = 0,
        .threads = DEFAULT_THREADS,
        .progress_seconds = DEFAULT_PROGRESS_SECONDS,
        .write_bfile = true
    };
    bool start_seen = false;
    for (int index = 1; index < argc; ++index) {
        const char *argument = argv[index];
        if (strcmp(argument, "--help") == 0 ||
            strcmp(argument, "-h") == 0) {
            usage(stdout, argv[0]);
            exit(EXIT_SUCCESS);
        } else if (strcmp(argument, "--term") == 0 ||
                   strcmp(argument, "--upto") == 0 ||
                   strcmp(argument, "--check") == 0) {
            if (options.mode != MODE_NONE || index + 1 >= argc) {
                usage(stderr, argv[0]);
                exit(EXIT_FAILURE);
            }
            options.mode = strcmp(argument, "--term") == 0
                ? MODE_TERM
                : (strcmp(argument, "--upto") == 0
                    ? MODE_UPTO : MODE_CHECK);
            options.n = parse_integer(argv[++index], MIN_N,
                                      options.mode == MODE_CHECK
                                          ? KNOWN_MAX_N : MAX_N,
                                      "N");
        } else if (strcmp(argument, "--start") == 0) {
            if (start_seen || index + 1 >= argc) {
                die("--start requires exactly one argument");
            }
            options.start = parse_integer(argv[++index], MIN_N,
                                          MAX_N, "start");
            start_seen = true;
        } else if (strcmp(argument, "--threads") == 0) {
            if (index + 1 >= argc) {
                die("--threads requires an argument");
            }
            options.threads = parse_unsigned(
                argv[++index], 1, MAX_THREADS, "threads");
        } else if (strcmp(argument, "--progress") == 0) {
            if (index + 1 >= argc) {
                die("--progress requires an argument");
            }
            options.progress_seconds = parse_unsigned(
                argv[++index], 0, MAX_PROGRESS_SECONDS, "progress seconds");
        } else if (strcmp(argument, "--no-bfile") == 0) {
            options.write_bfile = false;
        } else if (argument[0] != '-') {
            if (options.mode != MODE_NONE) {
                die("more than one N or run mode was specified");
            }
            options.mode = MODE_UPTO;
            options.n = parse_integer(argument, MIN_N, MAX_N, "N");
        } else {
            fprintf(stderr, "error: unknown option: %s\n", argument);
            usage(stderr, argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    if (options.mode == MODE_NONE) {
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }
    if (start_seen && options.mode != MODE_UPTO) {
        die("--start is only valid with --upto or positional N");
    }
    if (options.start > options.n) {
        die("start must not exceed N");
    }
    if (options.start > KNOWN_MAX_N + 1) {
        die("start would require unknown prefix terms");
    }
    if (options.mode != MODE_UPTO && !options.write_bfile) {
        die("--no-bfile is only meaningful with --upto or positional N");
    }
    return options;
}

static int acquire_bfile_lock(void)
{
    const int descriptor = open(LOCK_FILE_NAME, O_RDWR | O_CREAT, 0666);
    if (descriptor < 0) {
        die_errno("could not open the b-file lock");
    }
    struct flock lock = {
        .l_type = F_WRLCK,
        .l_whence = SEEK_SET,
        .l_start = 0,
        .l_len = 0
    };
    int result;
    do {
        result = fcntl(descriptor, F_SETLKW, &lock);
    } while (result != 0 && errno == EINTR);
    if (result != 0) {
        close(descriptor);
        die_errno("could not lock the b-file");
    }
    return descriptor;
}

static void release_bfile_lock(int descriptor)
{
    struct flock lock = {
        .l_type = F_UNLCK,
        .l_whence = SEEK_SET,
        .l_start = 0,
        .l_len = 0
    };
    const int unlock_result = fcntl(descriptor, F_SETLK, &lock);
    const int close_result = close(descriptor);
    if (unlock_result != 0 || close_result != 0) {
        die_errno("could not release the b-file lock");
    }
}

static FILE *open_partial_bfile(void)
{
    FILE *stream = fopen(PART_FILE_NAME, "w");
    if (stream == NULL) {
        die_errno("could not open the partial b-file");
    }
    if (setvbuf(stream, NULL, _IOLBF, 0) != 0) {
        fclose(stream);
        die("could not make the partial b-file line buffered");
    }
    return stream;
}

static void finish_bfile(FILE *stream)
{
    if (fflush(stream) != 0) {
        die_errno("could not flush the partial b-file");
    }
    const int descriptor = fileno(stream);
    if (descriptor < 0 || fsync(descriptor) != 0) {
        die_errno("could not synchronize the partial b-file");
    }
    if (fclose(stream) != 0) {
        die_errno("could not close the partial b-file");
    }
    if (rename(PART_FILE_NAME, BFILE_NAME) != 0) {
        die_errno("could not publish the b-file");
    }
    const int directory = open(".", O_RDONLY);
    if (directory < 0) {
        die_errno("could not open the output directory");
    }
    const int sync_result = fsync(directory);
    const int close_result = close(directory);
    if (sync_result != 0 || close_result != 0) {
        die_errno("could not synchronize the output directory");
    }
}

int main(int argc, char **argv)
{
    const Options options = parse_options(argc, argv);
    if (options.mode == MODE_TERM) {
        const U128 answer = compute_term(
            (unsigned)options.n, options.threads, options.progress_seconds);
        write_term(stdout, options.n, answer);
        return EXIT_SUCCESS;
    }
    if (options.mode == MODE_CHECK) {
        for (int n = 0; n <= options.n; ++n) {
            const U128 answer = compute_term(
                (unsigned)n, options.threads, options.progress_seconds);
            if (answer != known_terms[n]) {
                fprintf(stderr, "error: check failed at n=%d\n", n);
                return EXIT_FAILURE;
            }
        }
        fprintf(stderr, "check passed through n=%d\n", options.n);
        return EXIT_SUCCESS;
    }

    const int lock_descriptor = options.write_bfile
        ? acquire_bfile_lock() : -1;
    FILE *bfile = options.write_bfile ? open_partial_bfile() : NULL;
    if (bfile != NULL) {
        for (int n = 0; n < options.start; ++n) {
            write_term(bfile, n, known_terms[n]);
        }
    }
    for (int n = options.start; n <= options.n; ++n) {
        const U128 answer = compute_term(
            (unsigned)n, options.threads, options.progress_seconds);
        write_term(stdout, n, answer);
        if (bfile != NULL) {
            write_term(bfile, n, answer);
        }
    }
    if (bfile != NULL) {
        finish_bfile(bfile);
        release_bfile_lock(lock_descriptor);
    }
    return EXIT_SUCCESS;
}
