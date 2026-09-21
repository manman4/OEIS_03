/*
 * A327845 -- exact forbidden-value-mask search, specialized for n<=16.
 *
 * This is deliberately different from 327845_01.c.  The _01 search appends
 * a candidate and then compares every newly formed difference with its row.
 * Here, before branching, all row equalities are solved for the next value.
 *
 * If a prefix has length d and x is appended, its new right-edge differences
 * satisfy new[0]=x and new[k]=new[k-1]-last[k-1].  Hence
 *
 *     new[k] = x + offset[k],
 *     offset[0]=0, offset[k]=offset[k-1]-last[k-1].
 *
 * Equality with an old value s in difference row k is therefore equivalent
 * to x=s-offset[k].  All such x in 0..n-1 are collected in one uint32_t mask.
 * The DFS branches only over unused values outside that exact forbidden mask.
 * Thus row comparisons are performed once per state, not once per candidate.
 *
 * Complementing values x -> n-1-x negates every positive-order difference.
 * For n>=2 it pairs permutations without fixed points.  Prefix tasks keep the
 * lexicographically smaller complement representative and the final sum is
 * doubled.  No other symmetry or heuristic pruning is used.
 *
 * The supported maximum is intentionally 16: a(n)<=n! and 16! fits uint64_t.
 * Finite differences fit int64_t by the bound (n-1)*2^k, and every signed
 * subtraction is checked.  An independent checker enumerates all permutations
 * for n<=10 and constructs the full difference triangle.
 *
 * Per-task results are atomically checkpointed every minute.  Sequence terms
 * go to stdout, progress to stderr, and b327845_02.txt is installed atomically.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic -pthread \
 *       327845_02.c -o 327845_02
 *
 * Usage:
 *   A327845_02_THREADS=8 ./327845_02 N [FROM]
 */

#ifdef __APPLE__
#define _DARWIN_C_SOURCE
#endif
#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <limits.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#include <sys/sysctl.h>
#endif

#define MAX_N 16
#define VERIFIED_MAX_N 13
#define FIRST_UNVERIFIED_N 14
#define DIRECT_CHECK_MAX_N 10
#define PREFIX_LENGTH 5
#define MAX_THREADS 64
#define PROGRESS_INTERVAL_SECONDS 60.0
#define WORKER_STACK_KIB ((size_t)256)

static const uint64_t verified_terms[VERIFIED_MAX_N + 1] = {
    0, 1, 2, 4, 12, 40, 132, 428, 1668, 7628, 36924,
    199000, 1161824, 7231332
};

typedef struct {
    uint8_t values[PREFIX_LENGTH];
} PrefixTask;

typedef struct {
    uint64_t value;
    uint8_t done;
    uint8_t reserved[7];
} TaskResult;

typedef struct {
    char magic[8];
    uint32_t version;
    uint32_t n;
    uint32_t prefix_length;
    uint32_t reserved;
    uint64_t task_count;
    uint64_t task_hash;
    uint64_t results_hash;
} CheckpointHeader;

typedef struct {
    int n;
    uint32_t full_values;
    int prefix_length;
    PrefixTask *tasks;
    TaskResult *results;
    size_t task_count;
    size_t next_task;
    size_t completed_tasks;
    uint64_t representative_total;
    uint64_t task_hash;
    const char *checkpoint_path;
    bool finished;
    pthread_mutex_t mutex;
    pthread_cond_t condition;
    double started;
    double last_progress;
} ParallelSearch;

typedef struct {
    ParallelSearch *parallel;
    int64_t seen[MAX_N][MAX_N];
    uint8_t prefix[PREFIX_LENGTH];
} TaskBuilder;

static _Noreturn void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static double monotonic_seconds(void)
{
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) != 0) {
        die("clock_gettime failed");
    }
    return (double)now.tv_sec + (double)now.tv_nsec / 1000000000.0;
}

static void *checked_malloc(size_t count, size_t size)
{
    if (size != 0 && count > SIZE_MAX / size) {
        die("allocation size overflow");
    }
    void *memory = malloc(count * size);
    if (memory == NULL && count != 0) {
        die("memory allocation failed");
    }
    return memory;
}

static void *checked_calloc(size_t count, size_t size)
{
    if (size != 0 && count > SIZE_MAX / size) {
        die("allocation size overflow");
    }
    void *memory = calloc(count, size);
    if (memory == NULL && count != 0) {
        die("memory allocation failed");
    }
    return memory;
}

static int parse_int_range(const char *text, int minimum, int maximum,
                           const char *name)
{
    char *end = NULL;
    errno = 0;
    long value = strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' ||
        value < minimum || value > maximum) {
        fprintf(stderr, "error: %s must be in %d..%d: %s\n",
                name, minimum, maximum, text);
        exit(EXIT_FAILURE);
    }
    return (int)value;
}

static int parse_thread_count(void)
{
    long detected = 1;
#ifdef __APPLE__
    int logical_cpus = 1;
    size_t size = sizeof(logical_cpus);
    if (sysctlbyname("hw.logicalcpu", &logical_cpus, &size, NULL, 0) == 0 &&
        logical_cpus > 0) {
        detected = logical_cpus;
    }
#elif defined(_SC_NPROCESSORS_ONLN)
    detected = sysconf(_SC_NPROCESSORS_ONLN);
#endif
    int count = detected > 0 && detected <= MAX_THREADS ? (int)detected : 1;
    const char *text = getenv("A327845_02_THREADS");
    if (text != NULL && *text != '\0') {
        count = parse_int_range(text, 1, MAX_THREADS,
                                "A327845_02_THREADS");
    }
    return count;
}

static char *path_beside_executable(const char *argv0,
                                    const char *filename)
{
    char executable[PATH_MAX];
    char resolved[PATH_MAX];
    bool found = false;
#ifdef __APPLE__
    uint32_t size = (uint32_t)sizeof(executable);
    if (_NSGetExecutablePath(executable, &size) == 0) {
        found = true;
    }
#elif defined(__linux__)
    ssize_t length = readlink("/proc/self/exe", executable,
                              sizeof(executable) - 1);
    if (length >= 0) {
        executable[length] = '\0';
        found = true;
    }
#endif
    if (!found) {
        size_t length = strlen(argv0);
        if (length >= sizeof(executable)) {
            die("executable path is too long");
        }
        memcpy(executable, argv0, length + 1);
    }
    const char *base = realpath(executable, resolved);
    if (base == NULL) {
        base = executable;
    }
    const char *slash = strrchr(base, '/');
    const char *directory = slash == NULL ? "." : base;
    size_t directory_length = slash == NULL ? 1 : (size_t)(slash - base);
    size_t filename_length = strlen(filename);
    if (directory_length > SIZE_MAX - filename_length - 2) {
        die("output path length overflow");
    }
    char *path = checked_malloc(directory_length + filename_length + 2, 1);
    memcpy(path, directory, directory_length);
    path[directory_length] = '/';
    memcpy(path + directory_length + 1, filename, filename_length + 1);
    return path;
}

static int acquire_output_lock(const char *path)
{
    int descriptor = open(path, O_RDWR | O_CREAT, 0600);
    if (descriptor < 0) {
        fprintf(stderr, "error: could not open %s: %s\n",
                path, strerror(errno));
        exit(EXIT_FAILURE);
    }
    struct flock lock = {0};
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    if (fcntl(descriptor, F_SETLK, &lock) != 0) {
        fprintf(stderr, "error: another A327845_02 writer is active (%s)\n",
                path);
        close(descriptor);
        exit(EXIT_FAILURE);
    }
    return descriptor;
}

static uint64_t hash_u64(uint64_t hash, uint64_t value)
{
    for (int byte = 0; byte < 8; ++byte) {
        hash ^= value & UINT64_C(255);
        hash *= UINT64_C(1099511628211);
        value >>= 8;
    }
    return hash;
}

static uint64_t hash_results(const TaskResult *results, size_t count)
{
    uint64_t hash = UINT64_C(1469598103934665603);
    for (size_t i = 0; i < count; ++i) {
        hash = hash_u64(hash, results[i].value);
        hash = hash_u64(hash, results[i].done);
    }
    return hash;
}

/* Solve every possible row collision for the next value x and return the
 * exact set of forbidden x in 0..n-1. */
static uint32_t forbidden_value_mask(int n, int depth,
                                     const int64_t last[MAX_N],
                                     const int64_t seen[MAX_N][MAX_N])
{
    uint32_t forbidden = 0;
    int64_t offset = 0;
    for (int order = 1; order < depth; ++order) {
        if (__builtin_sub_overflow(offset, last[order - 1], &offset)) {
            die("signed difference offset overflow");
        }
        int old_count = depth - order;
        for (int i = 0; i < old_count; ++i) {
            int64_t candidate;
            if (__builtin_sub_overflow(seen[order][i], offset,
                                       &candidate)) {
                die("forbidden-value calculation overflow");
            }
            if (candidate >= 0 && candidate < n) {
                forbidden |= UINT32_C(1) << (unsigned)candidate;
            }
        }
    }
    return forbidden;
}

/* Called only after x survived forbidden_value_mask, so no duplicate scan is
 * needed here. */
static void append_value(int depth, int next,
                         const int64_t last[MAX_N],
                         int64_t seen[MAX_N][MAX_N],
                         int64_t child[MAX_N])
{
    child[0] = next;
    for (int order = 1; order <= depth; ++order) {
        if (__builtin_sub_overflow(child[order - 1], last[order - 1],
                                   &child[order])) {
            die("signed difference overflow");
        }
        seen[order][depth - order] = child[order];
    }
}

static bool is_complement_representative(const uint8_t *prefix,
                                         int length, int n)
{
    for (int i = 0; i < length; ++i) {
        int complement = n - 1 - prefix[i];
        if (prefix[i] != complement) {
            return prefix[i] < complement;
        }
    }
    die("distinct prefix was fixed by complementation");
}

static void generate_tasks(TaskBuilder *builder, int depth,
                           uint32_t used_values,
                           const int64_t last[MAX_N])
{
    ParallelSearch *parallel = builder->parallel;
    if (depth == parallel->prefix_length) {
        if (!is_complement_representative(builder->prefix, depth,
                                          parallel->n)) {
            return;
        }
        PrefixTask *task = &parallel->tasks[parallel->task_count++];
        memcpy(task->values, builder->prefix, (size_t)depth);
        return;
    }
    uint32_t forbidden = depth == 0
                             ? 0
                             : forbidden_value_mask(parallel->n, depth,
                                                    last, builder->seen);
    uint32_t candidates = parallel->full_values & ~used_values & ~forbidden;
    while (candidates != 0) {
        int next = __builtin_ctz(candidates);
        uint32_t bit = UINT32_C(1) << next;
        candidates &= candidates - 1;
        int64_t child[MAX_N];
        if (depth == 0) {
            child[0] = next;
        } else {
            append_value(depth, next, last, builder->seen, child);
        }
        builder->prefix[depth] = (uint8_t)next;
        generate_tasks(builder, depth + 1, used_values | bit, child);
    }
}

static uint64_t count_completions(const ParallelSearch *parallel,
                                  int64_t seen[MAX_N][MAX_N],
                                  const int64_t last[MAX_N],
                                  uint32_t used_values, int depth)
{
    if (depth == parallel->n) {
        return 1;
    }
    uint32_t forbidden = forbidden_value_mask(parallel->n, depth,
                                              last, seen);
    uint32_t candidates = parallel->full_values & ~used_values & ~forbidden;
    uint64_t total = 0;
    while (candidates != 0) {
        int next = __builtin_ctz(candidates);
        uint32_t bit = UINT32_C(1) << next;
        candidates &= candidates - 1;
        int64_t child[MAX_N];
        append_value(depth, next, last, seen, child);
        uint64_t add = count_completions(parallel, seen, child,
                                         used_values | bit, depth + 1);
        if (total > UINT64_MAX - add) {
            die("uint64_t count overflow");
        }
        total += add;
    }
    return total;
}

static void rebuild_task(const ParallelSearch *parallel,
                         const PrefixTask *task,
                         int64_t seen[MAX_N][MAX_N],
                         int64_t last[MAX_N], uint32_t *used_values)
{
    *used_values = 0;
    for (int depth = 0; depth < parallel->prefix_length; ++depth) {
        int next = task->values[depth];
        uint32_t bit = UINT32_C(1) << next;
        if ((*used_values & bit) != 0) {
            die("internal prefix repeats a value");
        }
        if (depth != 0) {
            uint32_t forbidden = forbidden_value_mask(
                parallel->n, depth, last, (const int64_t (*)[MAX_N])seen);
            if ((forbidden & bit) != 0) {
                die("internal prefix violates a difference condition");
            }
        }
        int64_t child[MAX_N];
        if (depth == 0) {
            child[0] = next;
        } else {
            append_value(depth, next, last, seen, child);
        }
        memcpy(last, child, (size_t)(depth + 1) * sizeof(*last));
        *used_values |= bit;
    }
}

static void save_checkpoint(const ParallelSearch *parallel)
{
    CheckpointHeader header = {{0}, 1, (uint32_t)parallel->n,
                               (uint32_t)parallel->prefix_length, 0,
                               (uint64_t)parallel->task_count,
                               parallel->task_hash,
                               hash_results(parallel->results,
                                            parallel->task_count)};
    memcpy(header.magic, "A3278452", 8);
    size_t length = strlen(parallel->checkpoint_path);
    if (length > SIZE_MAX - 6) {
        die("checkpoint path length overflow");
    }
    char *temporary = checked_malloc(length + 6, 1);
    memcpy(temporary, parallel->checkpoint_path, length);
    memcpy(temporary + length, ".part", 6);
    FILE *file = fopen(temporary, "wb");
    if (file == NULL || fwrite(&header, sizeof(header), 1, file) != 1 ||
        fwrite(parallel->results, sizeof(*parallel->results),
               parallel->task_count, file) != parallel->task_count ||
        fflush(file) != 0 || fsync(fileno(file)) != 0 ||
        fclose(file) != 0 || rename(temporary, parallel->checkpoint_path) != 0) {
        fprintf(stderr, "error: could not save checkpoint %s: %s\n",
                parallel->checkpoint_path, strerror(errno));
        free(temporary);
        exit(EXIT_FAILURE);
    }
    free(temporary);
}

static void load_checkpoint(ParallelSearch *parallel)
{
    FILE *file = fopen(parallel->checkpoint_path, "rb");
    if (file == NULL) {
        if (errno == ENOENT) {
            return;
        }
        fprintf(stderr, "error: could not open checkpoint %s: %s\n",
                parallel->checkpoint_path, strerror(errno));
        exit(EXIT_FAILURE);
    }
    CheckpointHeader header;
    if (fread(&header, sizeof(header), 1, file) != 1 ||
        memcmp(header.magic, "A3278452", 8) != 0 || header.version != 1 ||
        header.n != (uint32_t)parallel->n ||
        header.prefix_length != (uint32_t)parallel->prefix_length ||
        header.task_count != (uint64_t)parallel->task_count ||
        header.task_hash != parallel->task_hash ||
        fread(parallel->results, sizeof(*parallel->results),
              parallel->task_count, file) != parallel->task_count ||
        fgetc(file) != EOF || ferror(file)) {
        fclose(file);
        die("checkpoint is truncated or incompatible");
    }
    if (hash_results(parallel->results, parallel->task_count) !=
        header.results_hash) {
        fclose(file);
        die("checkpoint checksum disagrees");
    }
    if (fclose(file) != 0) {
        die("could not close checkpoint");
    }
    for (size_t i = 0; i < parallel->task_count; ++i) {
        if (parallel->results[i].done > 1) {
            die("checkpoint contains an invalid completion flag");
        }
        if (parallel->results[i].done) {
            if (parallel->representative_total >
                UINT64_MAX - parallel->results[i].value) {
                die("checkpoint total overflow");
            }
            parallel->representative_total += parallel->results[i].value;
            ++parallel->completed_tasks;
        }
    }
    fprintf(stderr, "A327845_02 n=%d resumed tasks=%zu/%zu from %s\n",
            parallel->n, parallel->completed_tasks, parallel->task_count,
            parallel->checkpoint_path);
}

static void *worker(void *argument)
{
    ParallelSearch *parallel = argument;
    int64_t seen[MAX_N][MAX_N] = {{0}};
    int64_t last[MAX_N];
    for (;;) {
        if (pthread_mutex_lock(&parallel->mutex) != 0) {
            die("pthread_mutex_lock failed");
        }
        while (parallel->next_task < parallel->task_count &&
               parallel->results[parallel->next_task].done) {
            ++parallel->next_task;
        }
        if (parallel->next_task == parallel->task_count) {
            if (pthread_mutex_unlock(&parallel->mutex) != 0) {
                die("pthread_mutex_unlock failed");
            }
            break;
        }
        size_t index = parallel->next_task++;
        PrefixTask task = parallel->tasks[index];
        if (pthread_mutex_unlock(&parallel->mutex) != 0) {
            die("pthread_mutex_unlock failed");
        }

        uint32_t used_values;
        rebuild_task(parallel, &task, seen, last, &used_values);
        uint64_t value = count_completions(parallel, seen, last, used_values,
                                           parallel->prefix_length);

        if (pthread_mutex_lock(&parallel->mutex) != 0) {
            die("pthread_mutex_lock failed");
        }
        if (parallel->representative_total > UINT64_MAX - value) {
            die("uint64_t count overflow");
        }
        parallel->representative_total += value;
        parallel->results[index].value = value;
        parallel->results[index].done = 1;
        ++parallel->completed_tasks;
        if (pthread_mutex_unlock(&parallel->mutex) != 0) {
            die("pthread_mutex_unlock failed");
        }
    }
    return NULL;
}

static void *progress_monitor(void *argument)
{
    ParallelSearch *parallel = argument;
    if (pthread_mutex_lock(&parallel->mutex) != 0) {
        die("pthread_mutex_lock failed");
    }
    while (!parallel->finished) {
        struct timespec deadline;
        if (clock_gettime(CLOCK_REALTIME, &deadline) != 0) {
            die("clock_gettime failed");
        }
        ++deadline.tv_sec;
        int result = pthread_cond_timedwait(&parallel->condition,
                                            &parallel->mutex, &deadline);
        if (result != 0 && result != ETIMEDOUT) {
            die("pthread_cond_timedwait failed");
        }
        double now = monotonic_seconds();
        if (!parallel->finished &&
            now - parallel->last_progress >= PROGRESS_INTERVAL_SECONDS) {
            save_checkpoint(parallel);
            fprintf(stderr,
                    "A327845_02 n=%d progress tasks=%zu/%zu (%.1f%%) "
                    "elapsed=%.0fs checkpoint=saved\n",
                    parallel->n, parallel->completed_tasks,
                    parallel->task_count,
                    100.0 * (double)parallel->completed_tasks /
                        (double)parallel->task_count,
                    now - parallel->started);
            fflush(stderr);
            parallel->last_progress = now;
        }
    }
    if (pthread_mutex_unlock(&parallel->mutex) != 0) {
        die("pthread_mutex_unlock failed");
    }
    return NULL;
}

static uint64_t calculate_term(int n, const char *checkpoint_path,
                               size_t *tasks_out, uint64_t *memory_out,
                               double *seconds_out, int *threads_out)
{
    if (n == 1) {
        *tasks_out = 1;
        *memory_out = 0;
        *seconds_out = 0;
        *threads_out = 1;
        return 1;
    }
    ParallelSearch parallel;
    memset(&parallel, 0, sizeof(parallel));
    parallel.n = n;
    parallel.full_values = (UINT32_C(1) << n) - 1;
    parallel.prefix_length = n < PREFIX_LENGTH ? n : PREFIX_LENGTH;
    parallel.checkpoint_path = checkpoint_path;

    size_t maximum_tasks = 1;
    for (int i = 0; i < parallel.prefix_length; ++i) {
        maximum_tasks *= (size_t)(n - i);
    }
    parallel.tasks = checked_malloc(maximum_tasks, sizeof(*parallel.tasks));
    TaskBuilder builder;
    memset(&builder, 0, sizeof(builder));
    builder.parallel = &parallel;
    int64_t empty_last[MAX_N] = {0};
    generate_tasks(&builder, 0, 0, empty_last);
    if (parallel.task_count == 0) {
        die("prefix task generation produced no tasks");
    }
    parallel.task_hash = UINT64_C(1469598103934665603);
    for (size_t i = 0; i < parallel.task_count; ++i) {
        for (int j = 0; j < parallel.prefix_length; ++j) {
            parallel.task_hash = hash_u64(parallel.task_hash,
                                           parallel.tasks[i].values[j]);
        }
    }
    parallel.results = checked_calloc(parallel.task_count,
                                      sizeof(*parallel.results));
    load_checkpoint(&parallel);

    int thread_count = parse_thread_count();
    if ((size_t)thread_count > parallel.task_count) {
        thread_count = (int)parallel.task_count;
    }
    pthread_t *threads = checked_malloc((size_t)thread_count,
                                        sizeof(*threads));
    size_t stack_size = WORKER_STACK_KIB * (size_t)1024;
#ifdef PTHREAD_STACK_MIN
    if (stack_size < (size_t)PTHREAD_STACK_MIN) {
        stack_size = (size_t)PTHREAD_STACK_MIN;
    }
#endif
    uint64_t controlled_memory =
        (uint64_t)maximum_tasks * sizeof(PrefixTask) +
        (uint64_t)parallel.task_count * sizeof(TaskResult) +
        (uint64_t)(size_t)(thread_count + 1) * sizeof(pthread_t) +
        (uint64_t)(size_t)(thread_count + 1) * stack_size;
    if (pthread_mutex_init(&parallel.mutex, NULL) != 0 ||
        pthread_cond_init(&parallel.condition, NULL) != 0) {
        die("pthread synchronization initialization failed");
    }
    pthread_attr_t attributes;
    if (pthread_attr_init(&attributes) != 0 ||
        pthread_attr_setstacksize(&attributes, stack_size) != 0) {
        die("pthread attribute initialization failed");
    }
    parallel.started = monotonic_seconds();
    parallel.last_progress = parallel.started;
    pthread_t monitor;
    if (pthread_create(&monitor, &attributes, progress_monitor, &parallel) != 0) {
        die("pthread_create failed for progress monitor");
    }
    for (int i = 0; i < thread_count; ++i) {
        if (pthread_create(&threads[i], &attributes, worker, &parallel) != 0) {
            die("pthread_create failed");
        }
    }
    if (pthread_attr_destroy(&attributes) != 0) {
        die("pthread_attr_destroy failed");
    }
    for (int i = 0; i < thread_count; ++i) {
        if (pthread_join(threads[i], NULL) != 0) {
            die("pthread_join failed");
        }
    }
    if (pthread_mutex_lock(&parallel.mutex) != 0) {
        die("pthread_mutex_lock failed");
    }
    parallel.finished = true;
    if (pthread_cond_signal(&parallel.condition) != 0 ||
        pthread_mutex_unlock(&parallel.mutex) != 0) {
        die("could not stop progress monitor");
    }
    if (pthread_join(monitor, NULL) != 0) {
        die("pthread_join failed for progress monitor");
    }
    double seconds = monotonic_seconds() - parallel.started;
    if (pthread_cond_destroy(&parallel.condition) != 0 ||
        pthread_mutex_destroy(&parallel.mutex) != 0) {
        die("pthread synchronization destruction failed");
    }
    free(threads);
    save_checkpoint(&parallel);
    free(parallel.results);
    free(parallel.tasks);

    if (parallel.representative_total > UINT64_MAX / 2) {
        die("uint64_t count overflow");
    }
    *tasks_out = parallel.task_count;
    *memory_out = controlled_memory;
    *seconds_out = seconds;
    *threads_out = thread_count;
    return 2 * parallel.representative_total;
}

static bool next_permutation(int *values, int n)
{
    int pivot = n - 2;
    while (pivot >= 0 && values[pivot] >= values[pivot + 1]) {
        --pivot;
    }
    if (pivot < 0) {
        return false;
    }
    int successor = n - 1;
    while (values[successor] <= values[pivot]) {
        --successor;
    }
    int temporary = values[pivot];
    values[pivot] = values[successor];
    values[successor] = temporary;
    for (int left = pivot + 1, right = n - 1; left < right;
         ++left, --right) {
        temporary = values[left];
        values[left] = values[right];
        values[right] = temporary;
    }
    return true;
}

static uint64_t direct_count(int n)
{
    int permutation[DIRECT_CHECK_MAX_N];
    for (int i = 0; i < n; ++i) {
        permutation[i] = i;
    }
    uint64_t count = 0;
    do {
        int64_t row[DIRECT_CHECK_MAX_N];
        for (int i = 0; i < n; ++i) {
            row[i] = permutation[i];
        }
        bool valid = true;
        for (int order = 1; order < n && valid; ++order) {
            int length = n - order;
            for (int i = 0; i < length; ++i) {
                row[i] = row[i + 1] - row[i];
            }
            for (int i = 0; i < length && valid; ++i) {
                for (int j = i + 1; j < length; ++j) {
                    if (row[i] == row[j]) {
                        valid = false;
                        break;
                    }
                }
            }
        }
        if (valid) {
            ++count;
        }
    } while (next_permutation(permutation, n));
    return count;
}

static void verify_bounds(void)
{
    uint64_t factorial = 1;
    for (uint64_t n = 2; n <= MAX_N; ++n) {
        if (factorial > UINT64_MAX / n) {
            die("MAX_N factorial does not fit uint64_t");
        }
        factorial *= n;
    }
    uint64_t difference_bound =
        (uint64_t)(MAX_N - 1) << (MAX_N - 1);
    if (difference_bound > (uint64_t)INT64_MAX) {
        die("MAX_N difference bound does not fit int64_t");
    }
}

static void write_term(FILE *file, const char *path, int n, uint64_t value)
{
    if (fprintf(file, "%d %" PRIu64 "\n", n, value) < 0 ||
        fflush(file) != 0 || fsync(fileno(file)) != 0) {
        fprintf(stderr, "error: could not update %s: %s\n",
                path, strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (printf("%d %" PRIu64 "\n", n, value) < 0 || fflush(stdout) != 0) {
        die("could not write stdout");
    }
}

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s N [FROM]\n"
            "  write exact A327845 terms 1..N, with N <= %d\n"
            "  1 <= FROM <= min(N,%d)\n",
            program, MAX_N, FIRST_UNVERIFIED_N);
}

int main(int argc, char **argv)
{
    if (argc != 2 && argc != 3) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    int maximum_n = parse_int_range(argv[1], 1, MAX_N, "N");
    int maximum_from = maximum_n < FIRST_UNVERIFIED_N
                           ? maximum_n
                           : FIRST_UNVERIFIED_N;
    int first_n = argc == 3
                      ? parse_int_range(argv[2], 1, maximum_from, "FROM")
                      : 1;
    verify_bounds();

    char *part_path =
        path_beside_executable(argv[0], "b327845_02_part.txt");
    char *final_path =
        path_beside_executable(argv[0], "b327845_02.txt");
    char *lock_path =
        path_beside_executable(argv[0], "b327845_02.lock");
    int lock_descriptor = acquire_output_lock(lock_path);
    FILE *bfile = fopen(part_path, "w");
    if (bfile == NULL) {
        fprintf(stderr, "error: could not open %s: %s\n",
                part_path, strerror(errno));
        close(lock_descriptor);
        free(lock_path);
        free(final_path);
        free(part_path);
        return EXIT_FAILURE;
    }

    for (int n = 1; n < first_n; ++n) {
        write_term(bfile, part_path, n, verified_terms[n]);
        fprintf(stderr, "A327845_02 n=%d loaded from verified prefix\n", n);
    }
    for (int n = first_n; n <= maximum_n; ++n) {
        char checkpoint_name[64];
        int length = snprintf(checkpoint_name, sizeof(checkpoint_name),
                              "a327845_02_n%d.checkpoint", n);
        if (length < 0 || (size_t)length >= sizeof(checkpoint_name)) {
            die("checkpoint filename overflow");
        }
        char *checkpoint_path =
            path_beside_executable(argv[0], checkpoint_name);
        size_t tasks = 0;
        uint64_t memory = 0;
        double seconds = 0;
        int threads = 0;
        uint64_t value = calculate_term(n, checkpoint_path, &tasks, &memory,
                                        &seconds, &threads);
        free(checkpoint_path);
        if (n <= DIRECT_CHECK_MAX_N && value != direct_count(n)) {
            die("independent full-permutation check disagrees");
        }
        if (n <= VERIFIED_MAX_N && value != verified_terms[n]) {
            die("computed value disagrees with the OEIS check term");
        }
        fprintf(stderr,
                "A327845_02 n=%d tasks=%zu threads=%d time=%.3fs "
                "controlled=%.3f MiB%s%s\n",
                n, tasks, threads, seconds,
                (double)memory / (1024.0 * 1024.0),
                n <= DIRECT_CHECK_MAX_N ? " direct-check=ok" : "",
                n <= VERIFIED_MAX_N ? " known-term=ok" : "");
        write_term(bfile, part_path, n, value);
    }

    if (fclose(bfile) != 0) {
        die("could not close b-file");
    }
    if (rename(part_path, final_path) != 0) {
        fprintf(stderr, "error: could not rename %s to %s: %s\n",
                part_path, final_path, strerror(errno));
        close(lock_descriptor);
        free(lock_path);
        free(final_path);
        free(part_path);
        return EXIT_FAILURE;
    }
    fprintf(stderr, "wrote %s (n=1..%d)\n", final_path, maximum_n);
    close(lock_descriptor);
    free(lock_path);
    free(final_path);
    free(part_path);
    return EXIT_SUCCESS;
}
