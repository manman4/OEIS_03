/*
 * A399206: Number of permutations p of [n] such that the n-1 products
 *          p(i)*p(i+1) are all distinct.
 *
 * Version 04 parallelizes the exact inclusion-exclusion search of version 02
 * with a dynamic pthread task queue.  The first color classes are expanded
 * into independent linear-forest states; workers then process those subtrees.
 * No approximation or probabilistic state merging is used.
 *
 * Build:
 *   cc -O3 -std=c11 -Wall -Wextra -Wpedantic 399206_04.c \
 *      -o 399206_04 -pthread
 *
 * Examples:
 *   ./399206_04                 # n=0..23; also updates b399206_04.txt
 *   ./399206_04 23 --threads 8 --stats
 *   ./399206_04 --target 23 --no-bfile --threads 8
 *   ./399206_04 23 --output FILE
 *   ./399206_04 --self-test 10  # compare with direct permutation search
 *
 * The supported limit is 23 and the practical default is also 23.
 * Every arithmetic operation that can exceed the signed 128-bit accumulator
 * is checked.
 *
 * Exact inclusion-exclusion
 * -------------------------
 * Regard {u,v} as an edge of K_n, colored by its product u*v.  A permutation
 * is an oriented Hamilton path, and the required condition says that its
 * edges have distinct colors.
 *
 * For one color, suppose a path contains k edges of that color.  For a
 * selected subset of s such edges use the coefficient
 *
 *        w_0=1, w_1=0, w_s=(-1)^(s-1)*(s-1) for s>=2.
 *
 * Binomial inversion gives
 *
 *        sum_{s=0}^k binomial(k,s)*w_s = 1 if k<=1, and 0 otherwise.
 *
 * Multiplying this identity over all repeated colors and expanding therefore
 * counts exactly the paths whose edge colors are distinct.
 *
 * A selected set F of required edges can occur in a Hamilton path only when
 * F is a linear forest: every degree is at most 2 and F has no cycle.  This
 * is the only search pruning used.  It is exact because every subgraph of a
 * path is a linear forest.  Conversely, if F is a linear forest with m edges
 * and c nontrivial path components, contracting its path components gives
 *
 *                 2^c * (n-m)!
 *
 * oriented Hamilton paths containing F.  Each component has two orientations
 * and the n-m resulting blocks may be ordered arbitrarily.
 *
 * Finally, distinct edges with the same product cannot share an endpoint:
 * uv=uw with u>0 implies v=w.  Thus every color class is a matching.  This
 * justifies the subset generation used below; edges are nevertheless added
 * one at a time because two new matching edges can jointly close a cycle
 * through previously selected edges.
 *
 * --self-test uses a separate, definition-level permutation recursion.  It
 * shares neither the inclusion-exclusion formula nor its linear-forest prune.
 *
 * Each completed term is checked against the existing contiguous b-file
 * prefix and saved under an exclusive lock.  The replacement file is flushed
 * and fsync'ed before rename, so interruption or concurrent runs cannot make
 * duplicate indices, gaps, or partially written lines.
 */

#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#if !defined(__SIZEOF_INT128__)
#error "399206_04.c requires unsigned __int128"
#endif

__extension__ typedef unsigned __int128 u128;
__extension__ typedef __int128 s128;

#define MAX_N 23U
#define DEFAULT_N 23U
#define DEFAULT_SELF_TEST_N 10U
#define MAX_SELF_TEST_N 11U
#define MAX_THREADS 64U
#define DEFAULT_THREAD_CAP 8U
#define TASK_SPLIT_DEPTH 5U
#define MAX_PRODUCT (MAX_N * MAX_N)
#define MAX_CLASS_EDGES (MAX_N / 2U)
#define MAX_CLASSES (MAX_PRODUCT + 1U)

static const char *output_path = "b399206_04.txt";
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
    uint8_t child;
    uint8_t root;
    uint8_t old_root_size;
    uint8_t u;
    uint8_t v;
    int8_t component_delta;
} Change;

typedef struct {
    unsigned n;
    ColorClass classes[MAX_CLASSES];
    unsigned class_count;
    uint8_t parent[MAX_N];
    uint8_t component_size[MAX_N];
    uint8_t degree[MAX_N];
    unsigned selected_edges;
    unsigned path_components;
    s128 factorial[MAX_N + 1U];
    s128 total;
    uint64_t recursion_nodes;
    uint64_t surviving_leaves;
} Search;

typedef struct {
    unsigned n;
    uint64_t used_vertices;
    bool used_product[MAX_PRODUCT + 1U];
    u128 count;
} DirectSearch;

typedef struct {
    uint8_t parent[MAX_N];
    uint8_t component_size[MAX_N];
    uint8_t degree[MAX_N];
    unsigned selected_edges;
    unsigned path_components;
    unsigned class_index;
    s128 coefficient;
} Task;

typedef struct {
    Task *items;
    size_t count;
    size_t capacity;
    uint64_t generation_nodes;
} TaskList;

typedef struct {
    const Search *prototype;
    const Task *tasks;
    size_t task_count;
    _Atomic size_t next_task;
} WorkQueue;

typedef struct {
    WorkQueue *queue;
    s128 total;
    uint64_t recursion_nodes;
    uint64_t surviving_leaves;
} Worker;

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

static uint64_t checked_add_u64(uint64_t a, uint64_t b)
{
    if (a > UINT64_MAX - b)
        die("64-bit statistic overflow");
    return a + b;
}

static void *xrealloc(void *old, size_t count, size_t size)
{
    if (size != 0U && count > SIZE_MAX / size)
        die("allocation size overflow");
    void *memory = realloc(old, count * size);
    if (memory == NULL)
        die("out of memory");
    return memory;
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

static void make_color_classes(Search *search)
{
    ColorClass buckets[MAX_PRODUCT + 1U];
    memset(buckets, 0, sizeof(buckets));

    for (unsigned u = 0U; u < search->n; ++u) {
        for (unsigned v = u + 1U; v < search->n; ++v) {
            const unsigned product = (u + 1U) * (v + 1U);
            ColorClass *class_ = &buckets[product];
            if (class_->count == MAX_CLASS_EDGES)
                die("a product class exceeds the proved matching bound");
            class_->edges[class_->count++] = (Edge){(uint8_t)u, (uint8_t)v};
            class_->vertex_mask |= (UINT64_C(1) << u) |
                                   (UINT64_C(1) << v);
        }
    }

    for (unsigned product = 1U; product <= search->n * search->n;
         ++product) {
        const ColorClass *source = &buckets[product];
        if (source->count <= 1U)
            continue;
        if (popcount_u64(source->vertex_mask) != 2U * source->count)
            die("equal-product edges unexpectedly share an endpoint");
        search->classes[search->class_count++] = *source;
    }

    for (unsigned i = 0U; i < search->class_count; ++i) {
        unsigned score = 0U;
        for (unsigned j = 0U; j < search->class_count; ++j)
            score += popcount_u64(search->classes[i].vertex_mask &
                                  search->classes[j].vertex_mask);
        search->classes[i].conflict_score = score;
    }
    qsort(search->classes, search->class_count, sizeof(search->classes[0]),
          compare_classes);
}

static unsigned find_root(const Search *search, unsigned vertex)
{
    while (search->parent[vertex] != vertex)
        vertex = search->parent[vertex];
    return vertex;
}

/* Add one edge to the current linear forest, recording an exact rollback. */
static bool add_edge(Search *search, Edge edge, Change *change)
{
    const unsigned u = edge.u;
    const unsigned v = edge.v;
    if (search->degree[u] == 2U || search->degree[v] == 2U)
        return false;

    unsigned root_u = find_root(search, u);
    unsigned root_v = find_root(search, v);
    if (root_u == root_v)
        return false; /* This edge would close a cycle. */

    if (search->component_size[root_u] < search->component_size[root_v]) {
        const unsigned temporary = root_u;
        root_u = root_v;
        root_v = temporary;
    }

    int delta = 0;
    if (search->degree[u] == 0U && search->degree[v] == 0U)
        delta = 1;
    else if (search->degree[u] != 0U && search->degree[v] != 0U)
        delta = -1;
    if (delta < 0 && search->path_components < 2U)
        die("invalid path-component accounting before union");

    *change = (Change){
        .child = (uint8_t)root_v,
        .root = (uint8_t)root_u,
        .old_root_size = search->component_size[root_u],
        .u = (uint8_t)u,
        .v = (uint8_t)v,
        .component_delta = (int8_t)delta
    };
    search->parent[root_v] = (uint8_t)root_u;
    search->component_size[root_u] =
        (uint8_t)(search->component_size[root_u] +
                  search->component_size[root_v]);
    ++search->degree[u];
    ++search->degree[v];
    ++search->selected_edges;
    search->path_components =
        (unsigned)((int)search->path_components + delta);
    return true;
}

static void rollback_edge(Search *search, const Change *change)
{
    --search->degree[change->u];
    --search->degree[change->v];
    --search->selected_edges;
    search->path_components =
        (unsigned)((int)search->path_components - change->component_delta);
    search->parent[change->child] = change->child;
    search->component_size[change->root] = change->old_root_size;
}

static void accumulate_leaf(Search *search, s128 coefficient)
{
    if (search->selected_edges >= search->n && search->n != 0U)
        die("a linear forest has too many selected edges");
    if (search->path_components > search->selected_edges)
        die("invalid path-component count");
    const unsigned blocks = search->n - search->selected_edges;
    s128 containing_paths = search->factorial[blocks];
    for (unsigned i = 0U; i < search->path_components; ++i)
        containing_paths = checked_multiply(containing_paths, 2);
    const s128 contribution = checked_multiply(coefficient, containing_paths);
    search->total = checked_add(search->total, contribution);
    if (search->surviving_leaves == UINT64_MAX)
        die("surviving-leaf statistic overflow");
    ++search->surviving_leaves;
}

static void inclusion_exclusion(Search *search, unsigned class_index,
                                s128 coefficient)
{
    if (search->recursion_nodes == UINT64_MAX)
        die("recursion-node statistic overflow");
    ++search->recursion_nodes;

    if (class_index == search->class_count) {
        accumulate_leaf(search, coefficient);
        return;
    }

    /* Select no edge of this color.  Selecting one has coefficient zero. */
    inclusion_exclusion(search, class_index + 1U, coefficient);

    const ColorClass *class_ = &search->classes[class_index];
    const uint64_t subset_count = UINT64_C(1) << class_->count;
    for (uint64_t subset = 0U; subset < subset_count; ++subset) {
        const unsigned selected = popcount_u64(subset);
        if (selected < 2U)
            continue;

        Change changes[MAX_CLASS_EDGES];
        unsigned added = 0U;
        bool possible = true;
        for (unsigned edge_index = 0U; edge_index < class_->count;
             ++edge_index) {
            if ((subset & (UINT64_C(1) << edge_index)) == 0U)
                continue;
            if (!add_edge(search, class_->edges[edge_index],
                          &changes[added])) {
                possible = false;
                break;
            }
            ++added;
        }

        if (possible) {
            const s128 weight = (selected & 1U)
                ? (s128)(selected - 1U)
                : -(s128)(selected - 1U);
            inclusion_exclusion(
                search, class_index + 1U,
                checked_multiply(coefficient, weight));
        }

        while (added != 0U)
            rollback_edge(search, &changes[--added]);
    }
}

static void append_task(TaskList *list, const Search *search,
                        unsigned class_index, s128 coefficient)
{
    if (list->count == list->capacity) {
        if (list->capacity > SIZE_MAX / 2U)
            die("task-list capacity overflow");
        list->capacity = list->capacity == 0U ? 128U : 2U * list->capacity;
        list->items = xrealloc(list->items, list->capacity,
                               sizeof(*list->items));
    }
    Task *task = &list->items[list->count++];
    memcpy(task->parent, search->parent, sizeof(task->parent));
    memcpy(task->component_size, search->component_size,
           sizeof(task->component_size));
    memcpy(task->degree, search->degree, sizeof(task->degree));
    task->selected_edges = search->selected_edges;
    task->path_components = search->path_components;
    task->class_index = class_index;
    task->coefficient = coefficient;
}

static void generate_tasks(Search *search, unsigned class_index,
                           unsigned split_depth, s128 coefficient,
                           TaskList *list)
{
    if (list->generation_nodes == UINT64_MAX)
        die("task-generation statistic overflow");
    ++list->generation_nodes;
    if (class_index == split_depth) {
        append_task(list, search, class_index, coefficient);
        return;
    }

    generate_tasks(search, class_index + 1U, split_depth, coefficient, list);
    const ColorClass *class_ = &search->classes[class_index];
    const uint64_t subset_count = UINT64_C(1) << class_->count;
    for (uint64_t subset = 0U; subset < subset_count; ++subset) {
        const unsigned selected = popcount_u64(subset);
        if (selected < 2U)
            continue;
        Change changes[MAX_CLASS_EDGES];
        unsigned added = 0U;
        bool possible = true;
        for (unsigned edge_index = 0U; edge_index < class_->count;
             ++edge_index) {
            if ((subset & (UINT64_C(1) << edge_index)) == 0U)
                continue;
            if (!add_edge(search, class_->edges[edge_index],
                          &changes[added])) {
                possible = false;
                break;
            }
            ++added;
        }
        if (possible) {
            const s128 weight = (selected & 1U)
                ? (s128)(selected - 1U)
                : -(s128)(selected - 1U);
            generate_tasks(search, class_index + 1U, split_depth,
                           checked_multiply(coefficient, weight), list);
        }
        while (added != 0U)
            rollback_edge(search, &changes[--added]);
    }
}

static void restore_task(Search *search, const Task *task)
{
    memcpy(search->parent, task->parent, sizeof(task->parent));
    memcpy(search->component_size, task->component_size,
           sizeof(task->component_size));
    memcpy(search->degree, task->degree, sizeof(task->degree));
    search->selected_edges = task->selected_edges;
    search->path_components = task->path_components;
    search->total = 0;
    search->recursion_nodes = 0U;
    search->surviving_leaves = 0U;
}

static void verify_task_restored(const Search *search, const Task *task)
{
    if (search->selected_edges != task->selected_edges ||
        search->path_components != task->path_components ||
        memcmp(search->parent, task->parent, sizeof(task->parent)) != 0 ||
        memcmp(search->component_size, task->component_size,
               sizeof(task->component_size)) != 0 ||
        memcmp(search->degree, task->degree, sizeof(task->degree)) != 0)
        die("a worker did not restore its task state");
}

static void *worker_main(void *argument)
{
    Worker *worker = argument;
    Search search = *worker->queue->prototype;
    for (;;) {
        const size_t index = atomic_fetch_add_explicit(
            &worker->queue->next_task, 1U, memory_order_relaxed);
        if (index >= worker->queue->task_count)
            break;
        const Task *task = &worker->queue->tasks[index];
        restore_task(&search, task);
        inclusion_exclusion(&search, task->class_index, task->coefficient);
        verify_task_restored(&search, task);
        worker->total = checked_add(worker->total, search.total);
        worker->recursion_nodes = checked_add_u64(
            worker->recursion_nodes, search.recursion_nodes);
        worker->surviving_leaves = checked_add_u64(
            worker->surviving_leaves, search.surviving_leaves);
    }
    return NULL;
}

static u128 calculate(unsigned n, unsigned requested_threads,
                      uint64_t *nodes, uint64_t *leaves,
                      unsigned *repeated_colors)
{
    Search search;
    memset(&search, 0, sizeof(search));
    search.n = n;
    search.factorial[0] = 1;
    for (unsigned i = 1U; i <= n; ++i)
        search.factorial[i] =
            checked_multiply(search.factorial[i - 1U], (s128)i);
    for (unsigned i = 0U; i < n; ++i) {
        search.parent[i] = (uint8_t)i;
        search.component_size[i] = 1U;
    }

    make_color_classes(&search);
    const unsigned split_depth = search.class_count < TASK_SPLIT_DEPTH
        ? search.class_count : TASK_SPLIT_DEPTH;
    TaskList tasks = {0};
    generate_tasks(&search, 0U, split_depth, 1, &tasks);
    if (tasks.count == 0U)
        die("task generation produced no work");

    unsigned thread_count = requested_threads;
    if ((size_t)thread_count > tasks.count)
        thread_count = (unsigned)tasks.count;
    WorkQueue queue = {
        .prototype = &search,
        .tasks = tasks.items,
        .task_count = tasks.count,
        .next_task = 0U
    };
    Worker *workers = calloc(thread_count, sizeof(*workers));
    pthread_t *threads = calloc(thread_count, sizeof(*threads));
    if (workers == NULL || threads == NULL)
        die("cannot allocate worker metadata");
    for (unsigned i = 0U; i < thread_count; ++i) {
        workers[i].queue = &queue;
        if (pthread_create(&threads[i], NULL, worker_main, &workers[i]) != 0)
            die("cannot create a worker thread");
    }
    search.total = 0;
    if (tasks.generation_nodes < tasks.count)
        die("invalid task-generation statistics");
    search.recursion_nodes = tasks.generation_nodes - tasks.count;
    search.surviving_leaves = 0U;
    for (unsigned i = 0U; i < thread_count; ++i) {
        if (pthread_join(threads[i], NULL) != 0)
            die("cannot join a worker thread");
        search.total = checked_add(search.total, workers[i].total);
        search.recursion_nodes = checked_add_u64(
            search.recursion_nodes, workers[i].recursion_nodes);
        search.surviving_leaves = checked_add_u64(
            search.surviving_leaves, workers[i].surviving_leaves);
    }
    free(threads);
    free(workers);
    free(tasks.items);
    if (search.selected_edges != 0U || search.path_components != 0U)
        die("inclusion-exclusion rollback did not restore the root state");
    for (unsigned i = 0U; i < n; ++i) {
        if (search.degree[i] != 0U || search.parent[i] != i)
            die("linear-forest rollback did not restore a vertex");
    }
    if (search.total < 0 || search.total > search.factorial[n])
        die("inclusion-exclusion result is outside 0..n!");

    *nodes = search.recursion_nodes;
    *leaves = search.surviving_leaves;
    *repeated_colors = search.class_count;
    return (u128)search.total;
}

/* Independent definition-level oracle used only by --self-test. */
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
    struct flock lock = {
        .l_type = F_WRLCK,
        .l_whence = SEEK_SET
    };
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
    struct flock lock = {
        .l_type = F_UNLCK,
        .l_whence = SEEK_SET
    };
    if (fcntl(descriptor, F_SETLK, &lock) != 0 || close(descriptor) != 0)
        die("cannot unlock the b-file");
}

/* Accept comments and blank lines, but require data indices 0,1,2,... . */
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
        if (*cursor < '0' || *cursor > '9') {
            fclose(input);
            die("the b-file contains an invalid index");
        }

        errno = 0;
        char *index_end = NULL;
        const uintmax_t index = strtoumax(cursor, &index_end, 10);
        if (errno == ERANGE || index_end == cursor || index > MAX_N ||
            index != next) {
            fclose(input);
            die("the b-file is malformed or has a gap");
        }
        cursor = index_end;
        if (*cursor != ' ' && *cursor != '\t') {
            fclose(input);
            die("the b-file is malformed or has a gap");
        }
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

static unsigned parse_threads(const char *text)
{
    const unsigned value = parse_n(text, MAX_THREADS, "thread count");
    if (value == 0U)
        die("thread count must be at least 1");
    return value;
}

static unsigned default_thread_count(void)
{
#if defined(_SC_NPROCESSORS_ONLN)
    const long detected = sysconf(_SC_NPROCESSORS_ONLN);
    if (detected <= 0)
        return DEFAULT_THREAD_CAP;
    return detected > (long)DEFAULT_THREAD_CAP
        ? DEFAULT_THREAD_CAP : (unsigned)detected;
#else
    return DEFAULT_THREAD_CAP;
#endif
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

static void run_self_test(unsigned maximum, unsigned thread_count)
{
    s128 factorial = 1;
    for (unsigned n = 0U; n <= maximum; ++n) {
        uint64_t nodes, leaves;
        unsigned colors;
        const u128 fast =
            calculate(n, thread_count, &nodes, &leaves, &colors);
        uint64_t serial_nodes, serial_leaves;
        unsigned serial_colors;
        const u128 serial = calculate(n, 1U, &serial_nodes, &serial_leaves,
                                      &serial_colors);
        const u128 direct = calculate_direct(n);
        if (fast != serial || fast != direct || colors != serial_colors ||
            nodes != serial_nodes || leaves != serial_leaves) {
            fprintf(stderr, "self-test mismatch at n=%u: parallel=",
                    n);
            print_u128(stderr, fast);
            fprintf(stderr, ", serial=");
            print_u128(stderr, serial);
            fprintf(stderr, ", direct=");
            print_u128(stderr, direct);
            fputc('\n', stderr);
            exit(EXIT_FAILURE);
        }
        if (n != 0U)
            factorial = checked_multiply(factorial, (s128)n);
        if (n <= 5U && fast != (u128)factorial)
            die("small-n factorial regression failed");
    }
    printf("self-test passed through n=%u\n", maximum);
}

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s [N] [--threads T] [--stats] "
            "[--output FILE|--no-bfile]\n"
            "       %s --target N [--threads T] [--stats] "
            "[--output FILE|--no-bfile]\n"
            "       %s --self-test [N] [--threads T]\n"
            "N defaults to %u and must be in 0..%u.\n"
            "T defaults to %u (capped to detected processors where available) "
            "and must be in 1..%u.\n"
            "Each completed term is recorded in %s by default.\n",
            program, program, program, DEFAULT_N, MAX_N,
            DEFAULT_THREAD_CAP, MAX_THREADS, output_path);
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
    bool threads_option = false;
    unsigned thread_count = default_thread_count();

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
        if (strcmp(argv[argument], "--threads") == 0) {
            if (threads_option || argument + 1 >= argc)
                die("invalid --threads");
            threads_option = true;
            thread_count = parse_threads(argv[++argument]);
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
        if (output_option)
            die("--output is not used with --self-test");
        if (stats)
            die("--stats is not used with --self-test");
        run_self_test(maximum, thread_count);
        return EXIT_SUCCESS;
    }
    write_bfile = !no_bfile_option;
    validate_bfile();

    const unsigned first = target_only ? maximum : 0U;
    for (unsigned n = first; n <= maximum; ++n) {
        const double started = monotonic_seconds();
        uint64_t nodes, leaves;
        unsigned colors;
        const u128 value = calculate(n, thread_count, &nodes, &leaves,
                                     &colors);
        record_term(n, value);
        print_term(n, value);
        if (fflush(stdout) != 0)
            die("cannot flush result output");
        if (stats) {
            if (fprintf(stderr,
                        "n=%u: %.3f s, repeated colors=%u, "
                        "recursion nodes=%" PRIu64 ", leaves=%" PRIu64
                        ", requested threads=%u\n",
                        n, monotonic_seconds() - started, colors, nodes,
                        leaves, thread_count) < 0)
                die("cannot write statistics");
        }
        if (n == maximum)
            break; /* Avoid unsigned wraparound when maximum is MAX_N. */
    }
    if (write_bfile &&
        fprintf(stderr, "b-file verified/updated: %s\n", output_path) < 0)
        die("cannot write the b-file completion message");
    return EXIT_SUCCESS;
}
