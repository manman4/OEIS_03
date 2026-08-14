/*
 * A398911 -- bounded memoized and parallel exact search.
 *
 * Count partitions of {1,...,3*n} into unordered triples whose sums are all
 * different.  A state is the complete pair
 *
 *                 (unused element mask, used sum mask).
 *
 * The smallest unused element is placed with every admissible unordered pair
 * of other unused elements.  Completed state values are stored in a bounded
 * direct-mapped memo table.  Every key word is compared before a cached value
 * is used.  A hash collision merely replaces an older entry and can only
 * cause recomputation; it cannot change the answer.
 *
 * Root triples containing element 1 are distributed among pthread workers.
 * Each worker owns a private memo table, so the memo tables have no data
 * races.  Every count addition is checked.  The total number of unrestricted
 * triple partitions is checked to fit U128 before searching; this bounds the
 * answer and every subproblem value.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic -pthread \
 *       398911_02.c -o 398911_02
 *
 * Usage:
 *   ./398911_02 --term 7 --threads 4 --cache-mib 256
 *   ./398911_02 --upto 7 --start 7 --threads 4 --cache-mib 256
 *   ./398911_02 --check 6 --threads 4 --cache-mib 64
 *
 * A positional N is shorthand for --upto N.  --upto writes b398911_2.txt
 * beside the executable through interruption-safe b398911_2_part.txt.
 * --term does not change the b-file.  --start S copies the verified built-in
 * prefix n<S and computes only S..N.  Progress is written to stderr every
 * 30 seconds by default; --progress SEC changes the interval, and zero
 * disables progress reporting.
 */

#define _POSIX_C_SOURCE 200809L

#include <errno.h>
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

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#if !defined(__SIZEOF_INT128__)
#error "398911_02 requires unsigned __int128"
#endif

__extension__ typedef unsigned __int128 U128;

#define MIN_N 0
#define MAX_N 14
#define KNOWN_MAX_N 6
#define DEFAULT_CHECK_N 5
#define DEFAULT_THREADS 4
#define MAX_THREADS 32
#define DEFAULT_CACHE_MIB 256
#define MIN_CACHE_MIB 1
#define MAX_CACHE_MIB 8192
#define MIN_MEMO_CAPACITY 1024
#define DEFAULT_PROGRESS_SECONDS 30
#define MAX_PROGRESS_SECONDS 3600
#define PROGRESS_CALL_STRIDE UINT64_C(1048576)

typedef enum {
    MODE_TERM,
    MODE_UPTO,
    MODE_CHECK
} RunMode;

typedef struct {
    uint64_t unused;
    uint64_t sums_low;
    uint64_t sums_high;
} StateKey;

/* unused_tag is key.unused+1, so zero denotes an empty slot. */
typedef struct {
    uint64_t unused_tag;
    uint64_t sums_low;
    uint64_t sums_high;
    uint64_t value_low;
    uint64_t value_high;
} MemoEntry;

typedef struct {
    MemoEntry *entries;
    size_t capacity;
    size_t mask;
} Memo;

typedef struct {
    uint64_t calls;
    uint64_t hits;
    uint64_t stores;
    uint64_t replacements;
    uint64_t candidate_triples;
    uint64_t duplicate_sum_rejections;
} SearchStats;

typedef struct {
    _Atomic uint64_t calls;
    _Atomic unsigned completed_branches;
} Progress;

typedef struct {
    Memo *memo;
    SearchStats stats;
    Progress *progress;
    uint64_t published_calls;
} Search;

typedef struct {
    uint64_t removed_bits;
    U128 sum_bit;
    U128 value;
} RootTask;

typedef struct {
    uint64_t full_mask;
    unsigned task_count;
    _Atomic unsigned next_task;
    RootTask *tasks;
} RootQueue;

typedef struct {
    Search search;
    RootQueue *queue;
} Worker;

static const char *const known_terms[KNOWN_MAX_N + 1] = {
    "1",
    "1",
    "10",
    "246",
    "12184",
    "995001",
    "122605952"
};

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

static int parse_integer(const char *text, int minimum, int maximum,
                         const char *label)
{
    char *end = NULL;
    errno = 0;
    long value = strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' ||
        value < minimum || value > maximum) {
        fprintf(stderr, "error: %s must be in %d..%d: %s\n",
                label, minimum, maximum, text);
        exit(EXIT_FAILURE);
    }
    return (int)value;
}

static unsigned parse_unsigned(const char *text, unsigned minimum,
                               unsigned maximum, const char *label)
{
    char *end = NULL;
    errno = 0;
    unsigned long value = strtoul(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' ||
        value < minimum || value > maximum) {
        fprintf(stderr, "error: %s must be in %u..%u: %s\n",
                label, minimum, maximum, text);
        exit(EXIT_FAILURE);
    }
    return (unsigned)value;
}

static bool parse_u128(const char *text, U128 *result)
{
    U128 value = 0;
    const U128 maximum = ~(U128)0;
    if (*text == '\0') {
        return false;
    }
    for (; *text != '\0'; ++text) {
        if (*text < '0' || *text > '9') {
            return false;
        }
        unsigned digit = (unsigned)(*text - '0');
        if (value > (maximum - digit) / 10) {
            return false;
        }
        value = value * 10 + digit;
    }
    *result = value;
    return true;
}

static int fprint_u128(FILE *stream, U128 value)
{
    char digits[40];
    size_t length = 0;
    do {
        digits[length++] = (char)('0' + (unsigned)(value % 10));
        value /= 10;
    } while (value != 0);
    while (length != 0) {
        if (fputc(digits[--length], stream) == EOF) {
            return -1;
        }
    }
    return 0;
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
    size_t directory_length =
        slash == NULL ? 1 : (size_t)(slash - base);
    if (slash != NULL && directory_length == 0) {
        directory_length = 1;
    }
    size_t filename_length = strlen(filename);
    if (directory_length > SIZE_MAX - filename_length - 2) {
        die("b-file output path length overflow");
    }
    char *path = malloc(directory_length + filename_length + 2);
    if (path == NULL) {
        die("could not allocate a b-file output path");
    }
    if (slash == NULL) {
        path[0] = '.';
    } else if (slash == base) {
        path[0] = '/';
    } else {
        memcpy(path, base, directory_length);
    }
    path[directory_length] = '/';
    memcpy(path + directory_length + 1, filename, filename_length + 1);
    return path;
}

static void increment_stat(uint64_t *value)
{
    if (*value != UINT64_MAX) {
        ++*value;
    }
}

static void add_stat_saturating(uint64_t *destination, uint64_t addend)
{
    *destination = *destination > UINT64_MAX - addend
                       ? UINT64_MAX
                       : *destination + addend;
}

static void atomic_add_saturating(_Atomic uint64_t *destination,
                                  uint64_t addend)
{
    uint64_t old = atomic_load_explicit(destination, memory_order_relaxed);
    for (;;) {
        uint64_t updated = old > UINT64_MAX - addend
                               ? UINT64_MAX
                               : old + addend;
        if (atomic_compare_exchange_weak_explicit(
                destination, &old, updated, memory_order_relaxed,
                memory_order_relaxed)) {
            return;
        }
    }
}

static void publish_call_progress(Search *search)
{
    uint64_t calls = search->stats.calls;
    uint64_t delta = calls - search->published_calls;
    if (delta != 0) {
        atomic_add_saturating(&search->progress->calls, delta);
        search->published_calls = calls;
    }
}

static uint64_t mix64(uint64_t value)
{
    value ^= value >> 30;
    value *= UINT64_C(0xbf58476d1ce4e5b9);
    value ^= value >> 27;
    value *= UINT64_C(0x94d049bb133111eb);
    return value ^ (value >> 31);
}

static uint64_t key_hash(StateKey key)
{
    uint64_t hash = mix64(key.unused + UINT64_C(0x9e3779b97f4a7c15));
    hash ^= mix64(key.sums_low + UINT64_C(0xd1b54a32d192ed03));
    hash ^= mix64(key.sums_high + UINT64_C(0x94d049bb133111eb));
    return mix64(hash);
}

static StateKey make_key(uint64_t unused, U128 used_sums)
{
    StateKey key = {
        .unused = unused,
        .sums_low = (uint64_t)used_sums,
        .sums_high = (uint64_t)(used_sums >> 64)
    };
    return key;
}

static Memo make_memo(unsigned cache_mib_per_worker)
{
    uint64_t bytes = (uint64_t)cache_mib_per_worker *
                     UINT64_C(1024) * UINT64_C(1024);
    uint64_t slot_limit = bytes / sizeof(MemoEntry);
    size_t capacity = 1;
    while (capacity <= SIZE_MAX / 2 &&
           (uint64_t)(capacity * 2) <= slot_limit) {
        capacity *= 2;
    }
    if (capacity < MIN_MEMO_CAPACITY) {
        capacity = MIN_MEMO_CAPACITY;
    }
    if (capacity > SIZE_MAX / sizeof(MemoEntry)) {
        die("memo table allocation size overflow");
    }
    MemoEntry *entries = calloc(capacity, sizeof(*entries));
    if (entries == NULL) {
        die("cannot allocate memo table; reduce --cache-mib or --threads");
    }
    Memo memo = {
        .entries = entries,
        .capacity = capacity,
        .mask = capacity - 1
    };
    return memo;
}

static void destroy_memo(Memo *memo)
{
    free(memo->entries);
    memset(memo, 0, sizeof(*memo));
}

static bool memo_get(const Memo *memo, StateKey key, U128 *value)
{
    const MemoEntry *entry =
        &memo->entries[key_hash(key) & memo->mask];
    if (entry->unused_tag == key.unused + 1 &&
        entry->sums_low == key.sums_low &&
        entry->sums_high == key.sums_high) {
        *value = (U128)entry->value_low |
                 ((U128)entry->value_high << 64);
        return true;
    }
    return false;
}

static void memo_put(Memo *memo, StateKey key, U128 value,
                     SearchStats *stats)
{
    MemoEntry *entry = &memo->entries[key_hash(key) & memo->mask];
    if (entry->unused_tag != 0 &&
        (entry->unused_tag != key.unused + 1 ||
         entry->sums_low != key.sums_low ||
         entry->sums_high != key.sums_high)) {
        increment_stat(&stats->replacements);
    }
    entry->sums_low = key.sums_low;
    entry->sums_high = key.sums_high;
    entry->value_low = (uint64_t)value;
    entry->value_high = (uint64_t)(value >> 64);
    entry->unused_tag = key.unused + 1;
    increment_stat(&stats->stores);
}

static U128 count_state(Search *search, uint64_t unused,
                        U128 used_sums)
{
    increment_stat(&search->stats.calls);
    if (search->stats.calls - search->published_calls >=
        PROGRESS_CALL_STRIDE) {
        publish_call_progress(search);
    }
    if (unused == 0) {
        return 1;
    }

    uint64_t first_bit = unused & (UINT64_C(0) - unused);
    unsigned first = (unsigned)__builtin_ctzll(first_bit) + 1U;
    uint64_t second_choices = unused ^ first_bit;
    uint64_t after_lowest = second_choices & (second_choices - 1);

    /* Exactly one triple remains. */
    if (after_lowest != 0 &&
        (after_lowest & (after_lowest - 1)) == 0) {
        uint64_t second_bit =
            second_choices & (UINT64_C(0) - second_choices);
        uint64_t third_bit = second_choices ^ second_bit;
        unsigned second =
            (unsigned)__builtin_ctzll(second_bit) + 1U;
        unsigned third =
            (unsigned)__builtin_ctzll(third_bit) + 1U;
        U128 sum_bit = (U128)1 << (first + second + third);
        increment_stat(&search->stats.candidate_triples);
        if ((used_sums & sum_bit) != 0) {
            increment_stat(&search->stats.duplicate_sum_rejections);
            return 0;
        }
        return 1;
    }

    StateKey key = make_key(unused, used_sums);
    U128 cached = 0;
    if (memo_get(search->memo, key, &cached)) {
        increment_stat(&search->stats.hits);
        return cached;
    }

    U128 total = 0;
    while (second_choices != 0) {
        uint64_t second_bit =
            second_choices & (UINT64_C(0) - second_choices);
        unsigned second =
            (unsigned)__builtin_ctzll(second_bit) + 1U;
        uint64_t third_choices = second_choices ^ second_bit;
        while (third_choices != 0) {
            uint64_t third_bit =
                third_choices & (UINT64_C(0) - third_choices);
            unsigned third =
                (unsigned)__builtin_ctzll(third_bit) + 1U;
            U128 sum_bit = (U128)1 << (first + second + third);
            increment_stat(&search->stats.candidate_triples);
            if ((used_sums & sum_bit) != 0) {
                increment_stat(&search->stats.duplicate_sum_rejections);
            } else {
                U128 addend = count_state(
                    search,
                    unused ^ first_bit ^ second_bit ^ third_bit,
                    used_sums | sum_bit);
                if (total > ~(U128)0 - addend) {
                    die("A398911 count overflowed U128");
                }
                total += addend;
            }
            third_choices ^= third_bit;
        }
        second_choices ^= second_bit;
    }
    memo_put(search->memo, key, total, &search->stats);
    return total;
}

static void *worker_main(void *argument)
{
    Worker *worker = argument;
    RootQueue *queue = worker->queue;
    for (;;) {
        unsigned task_index = atomic_fetch_add_explicit(
            &queue->next_task, 1U, memory_order_relaxed);
        if (task_index >= queue->task_count) {
            break;
        }
        RootTask *task = &queue->tasks[task_index];
        task->value = count_state(
            &worker->search,
            queue->full_mask ^ task->removed_bits,
            task->sum_bit);
        publish_call_progress(&worker->search);
        atomic_fetch_add_explicit(
            &worker->search.progress->completed_branches, 1U,
            memory_order_release);
    }
    return NULL;
}

static void report_progress(int n, const Progress *progress,
                            unsigned branch_count, double start,
                            uint64_t *previous_calls,
                            double *previous_time, bool terminal,
                            bool final)
{
    unsigned completed = atomic_load_explicit(
        &progress->completed_branches, memory_order_acquire);
    uint64_t calls = atomic_load_explicit(
        &progress->calls, memory_order_relaxed);
    double now = monotonic_seconds();
    double interval = now - *previous_time;
    double rate = interval > 0.0
                      ? (double)(calls - *previous_calls) /
                            interval / 1000000.0
                      : 0.0;
    double percent = branch_count == 0
                         ? 100.0
                         : 100.0 * (double)completed /
                               (double)branch_count;
    fprintf(stderr,
            terminal ? "\r398911_02: n=%d progress roots=%u/%u "
                       "(%5.1f%%), calls=%" PRIu64
                       ", rate=%.2f M/s, elapsed=%.1f s"
                     : "398911_02: n=%d progress roots=%u/%u "
                       "(%5.1f%%), calls=%" PRIu64
                       ", rate=%.2f M/s, elapsed=%.1f s\n",
            n, completed, branch_count, percent, calls, rate,
            now - start);
    if (terminal && final) {
        fputc('\n', stderr);
    }
    fflush(stderr);
    *previous_calls = calls;
    *previous_time = now;
}

static U128 partition_upper_bound(int n)
{
    U128 bound = 1;
    const U128 maximum = ~(U128)0;
    for (unsigned triples = 1; triples <= (unsigned)n; ++triples) {
        U128 a = 3U * triples - 1U;
        U128 b = 3U * triples - 2U;
        U128 factor = (a * b) / 2U;
        if (bound > maximum / factor) {
            die("triple-partition bound exceeds U128");
        }
        bound *= factor;
    }
    return bound;
}

static RootTask *make_root_tasks(int n, uint64_t full_mask,
                                 unsigned *task_count)
{
    unsigned other_count = 3U * (unsigned)n - 1U;
    *task_count = other_count * (other_count - 1U) / 2U;
    RootTask *tasks = calloc(*task_count, sizeof(*tasks));
    if (tasks == NULL) {
        die("cannot allocate root tasks");
    }

    unsigned index = 0;
    uint64_t second_choices = full_mask ^ UINT64_C(1);
    while (second_choices != 0) {
        uint64_t second_bit =
            second_choices & (UINT64_C(0) - second_choices);
        unsigned second =
            (unsigned)__builtin_ctzll(second_bit) + 1U;
        uint64_t third_choices = second_choices ^ second_bit;
        while (third_choices != 0) {
            uint64_t third_bit =
                third_choices & (UINT64_C(0) - third_choices);
            unsigned third =
                (unsigned)__builtin_ctzll(third_bit) + 1U;
            tasks[index].removed_bits =
                UINT64_C(1) | second_bit | third_bit;
            tasks[index].sum_bit =
                (U128)1 << (1U + second + third);
            ++index;
            third_choices ^= third_bit;
        }
        second_choices ^= second_bit;
    }
    if (index != *task_count) {
        die("internal root-task count mismatch");
    }
    return tasks;
}

static U128 compute_term(int n, int requested_threads,
                         unsigned total_cache_mib,
                         unsigned progress_seconds,
                         SearchStats *combined_stats,
                         size_t *combined_cache_bytes,
                         int *actual_workers)
{
    if (n < MIN_N || n > MAX_N || 3 * n >= 64 ||
        9 * n - 3 >= 128) {
        die("unsupported dimension in memoized triple search");
    }
    (void)partition_upper_bound(n);
    memset(combined_stats, 0, sizeof(*combined_stats));
    *combined_cache_bytes = 0;
    *actual_workers = 1;
    if (n == 0) {
        combined_stats->calls = 1;
        return 1;
    }

    uint64_t full_mask =
        (UINT64_C(1) << (3 * n)) - UINT64_C(1);
    unsigned task_count = 0;
    RootTask *tasks = make_root_tasks(n, full_mask, &task_count);
    int workers = requested_threads;
    if (workers > (int)task_count) {
        workers = (int)task_count;
    }
    if (workers < 1) {
        workers = 1;
    }
    unsigned cache_per_worker = total_cache_mib / (unsigned)workers;
    if (cache_per_worker < MIN_CACHE_MIB) {
        cache_per_worker = MIN_CACHE_MIB;
    }

    Worker *worker = calloc((size_t)workers, sizeof(*worker));
    pthread_t *threads = calloc((size_t)workers, sizeof(*threads));
    if (worker == NULL || threads == NULL) {
        free(threads);
        free(worker);
        free(tasks);
        die("cannot allocate root search data");
    }

    RootQueue queue = {
        .full_mask = full_mask,
        .task_count = task_count,
        .next_task = 0,
        .tasks = tasks
    };
    Progress progress;
    atomic_init(&progress.calls, 0);
    atomic_init(&progress.completed_branches, 0);
    for (int index = 0; index < workers; ++index) {
        worker[index].search.memo = malloc(sizeof(Memo));
        if (worker[index].search.memo == NULL) {
            die("cannot allocate a worker memo descriptor");
        }
        *worker[index].search.memo = make_memo(cache_per_worker);
        worker[index].search.progress = &progress;
        worker[index].queue = &queue;
        size_t worker_cache_bytes =
            worker[index].search.memo->capacity * sizeof(MemoEntry);
        if (*combined_cache_bytes > SIZE_MAX - worker_cache_bytes) {
            die("combined memo size overflow");
        }
        *combined_cache_bytes += worker_cache_bytes;
    }

    for (int index = 0; index < workers; ++index) {
        if (pthread_create(&threads[index], NULL, worker_main,
                           &worker[index]) != 0) {
            die("pthread_create failed");
        }
    }

    bool terminal = isatty(fileno(stderr)) != 0;
    bool progress_printed = false;
    double progress_start = monotonic_seconds();
    double previous_time = progress_start;
    double next_report = progress_start + (double)progress_seconds;
    uint64_t previous_calls = 0;
    while (atomic_load_explicit(&progress.completed_branches,
                                memory_order_acquire) < task_count) {
        double now = monotonic_seconds();
        if (progress_seconds != 0 && now >= next_report) {
            report_progress(n, &progress, task_count, progress_start,
                            &previous_calls, &previous_time, terminal,
                            false);
            progress_printed = true;
            next_report = now + (double)progress_seconds;
        }
        struct timespec pause = {.tv_sec = 0, .tv_nsec = 20000000L};
        while (nanosleep(&pause, &pause) != 0 && errno == EINTR) {
            /* Resume the short progress-monitoring sleep. */
        }
    }
    if (progress_printed) {
        report_progress(n, &progress, task_count, progress_start,
                        &previous_calls, &previous_time, terminal, true);
    }

    for (int index = 0; index < workers; ++index) {
        if (pthread_join(threads[index], NULL) != 0) {
            die("pthread_join failed");
        }
    }

    U128 answer = 0;
    for (unsigned task = 0; task < task_count; ++task) {
        if (answer > ~(U128)0 - tasks[task].value) {
            die("root sum overflowed U128");
        }
        answer += tasks[task].value;
    }
    for (int index = 0; index < workers; ++index) {
        add_stat_saturating(&combined_stats->calls,
                            worker[index].search.stats.calls);
        add_stat_saturating(&combined_stats->hits,
                            worker[index].search.stats.hits);
        add_stat_saturating(&combined_stats->stores,
                            worker[index].search.stats.stores);
        add_stat_saturating(&combined_stats->replacements,
                            worker[index].search.stats.replacements);
        add_stat_saturating(
            &combined_stats->candidate_triples,
            worker[index].search.stats.candidate_triples);
        add_stat_saturating(
            &combined_stats->duplicate_sum_rejections,
            worker[index].search.stats.duplicate_sum_rejections);
        destroy_memo(worker[index].search.memo);
        free(worker[index].search.memo);
    }
    free(threads);
    free(worker);
    free(tasks);
    *actual_workers = workers;
    return answer;
}

static void verify_known(U128 value, int n)
{
    if (n < MIN_N || n > KNOWN_MAX_N) {
        return;
    }
    U128 expected = 0;
    if (!parse_u128(known_terms[n], &expected)) {
        die("invalid built-in A398911 term");
    }
    if (value != expected) {
        fprintf(stderr, "error: A398911 mismatch at n=%d: got ", n);
        fprint_u128(stderr, value);
        fprintf(stderr, ", expected %s\n", known_terms[n]);
        exit(EXIT_FAILURE);
    }
}

static U128 compute_checked(int n, int requested_threads,
                            unsigned cache_mib,
                            unsigned progress_seconds, bool report)
{
    double start = monotonic_seconds();
    SearchStats stats;
    size_t cache_bytes = 0;
    int workers = 1;
    U128 value = compute_term(
        n, requested_threads, cache_mib,
        report ? progress_seconds : 0,
        &stats, &cache_bytes, &workers);
    verify_known(value, n);
    if (report) {
        fprintf(stderr,
                "398911_02: n=%d, bounded memo DFS, calls=%" PRIu64
                ", hits=%" PRIu64 ", stores=%" PRIu64
                ", replacements=%" PRIu64 ", %d worker%s, "
                "cache=%.1f MiB, %.3f s\n",
                n, stats.calls, stats.hits, stats.stores,
                stats.replacements, workers,
                workers == 1 ? "" : "s",
                (double)cache_bytes / (1024.0 * 1024.0),
                monotonic_seconds() - start);
    }
    return value;
}

static void flush_b_file(FILE *stream, const char *path)
{
    if (fflush(stream) != 0 || fsync(fileno(stream)) != 0) {
        fprintf(stderr, "error: cannot flush %s: %s\n",
                path, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

static void write_known_prefix_term(FILE *stream, int n,
                                    const char *path)
{
    if (n < MIN_N || n > KNOWN_MAX_N) {
        die("requested built-in A398911 prefix term is unavailable");
    }
    if (fprintf(stream, "%d %s\n", n, known_terms[n]) < 0) {
        fprintf(stderr, "error: cannot write %s: %s\n",
                path, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

static void write_result(FILE *stream, int n, U128 value,
                         const char *description)
{
    if (fprintf(stream, "%d ", n) < 0 ||
        fprint_u128(stream, value) != 0 ||
        fputc('\n', stream) == EOF) {
        fprintf(stderr, "error: cannot write %s\n", description);
        exit(EXIT_FAILURE);
    }
}

static void produce_b_file(const char *argv0, int maximum_n, int start_n,
                           int requested_threads, unsigned cache_mib,
                           unsigned progress_seconds)
{
    if (start_n < MIN_N || start_n > maximum_n + 1 ||
        start_n > KNOWN_MAX_N + 1) {
        fprintf(stderr,
                "error: start N must be in %d..min(UPTO_N+1,%d)\n",
                MIN_N, KNOWN_MAX_N + 1);
        exit(EXIT_FAILURE);
    }

    char *final_path = path_beside_executable(argv0, "b398911_2.txt");
    char *part_path =
        path_beside_executable(argv0, "b398911_2_part.txt");
    FILE *output = fopen(part_path, "w");
    if (output == NULL) {
        fprintf(stderr, "error: cannot open %s: %s\n",
                part_path, strerror(errno));
        free(part_path);
        free(final_path);
        exit(EXIT_FAILURE);
    }

    for (int n = MIN_N; n < start_n && n <= maximum_n; ++n) {
        write_known_prefix_term(output, n, part_path);
    }
    flush_b_file(output, part_path);
    if (start_n > MIN_N) {
        fprintf(stderr,
                "398911_02: using built-in verified prefix n=%d..%d\n",
                MIN_N, start_n - 1);
    }

    for (int n = start_n; n <= maximum_n; ++n) {
        U128 value = compute_checked(
            n, requested_threads, cache_mib,
            progress_seconds, true);
        write_result(output, n, value, part_path);
        flush_b_file(output, part_path);
        write_result(stdout, n, value, "result to stdout");
    }

    if (fclose(output) != 0) {
        fprintf(stderr, "error: cannot close %s: %s\n",
                part_path, strerror(errno));
        free(part_path);
        free(final_path);
        exit(EXIT_FAILURE);
    }
    if (rename(part_path, final_path) != 0) {
        fprintf(stderr, "error: cannot rename %s to %s: %s\n",
                part_path, final_path, strerror(errno));
        free(part_path);
        free(final_path);
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "wrote %s (n=%d..%d)\n",
            final_path, MIN_N, maximum_n);
    free(part_path);
    free(final_path);
}

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s N [--start S] [--threads T] [--cache-mib M] "
            "[--progress SEC]\n"
            "       %s --term N [--threads T] [--cache-mib M] "
            "[--progress SEC]\n"
            "       %s --upto N [--start S] [--threads T] "
            "[--cache-mib M] [--progress SEC]\n"
            "       %s --check [N] [--threads T] [--cache-mib M] "
            "[--progress SEC]\n",
            program, program, program, program);
}

int main(int argc, char **argv)
{
    RunMode mode = MODE_TERM;
    bool mode_set = false;
    int target = -1;
    int start_n = MIN_N;
    bool start_set = false;
    int requested_threads = DEFAULT_THREADS;
    unsigned cache_mib = DEFAULT_CACHE_MIB;
    unsigned progress_seconds = DEFAULT_PROGRESS_SECONDS;

    for (int argument = 1; argument < argc; ++argument) {
        const char *text = argv[argument];
        if (strcmp(text, "--term") == 0 ||
            strcmp(text, "--upto") == 0) {
            if (mode_set || target >= 0 || argument + 1 == argc) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            mode = strcmp(text, "--term") == 0
                       ? MODE_TERM
                       : MODE_UPTO;
            mode_set = true;
            target = parse_integer(argv[++argument], MIN_N, MAX_N, "N");
        } else if (strcmp(text, "--check") == 0) {
            if (mode_set || target >= 0) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            mode = MODE_CHECK;
            mode_set = true;
            target = DEFAULT_CHECK_N;
            if (argument + 1 < argc && argv[argument + 1][0] != '-') {
                target = parse_integer(argv[++argument], MIN_N,
                                       KNOWN_MAX_N, "check N");
            }
        } else if (strcmp(text, "--start") == 0) {
            if (start_set || argument + 1 == argc) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            start_n = parse_integer(argv[++argument], MIN_N,
                                    KNOWN_MAX_N + 1, "start N");
            start_set = true;
        } else if (strcmp(text, "--threads") == 0) {
            if (argument + 1 == argc) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            requested_threads = parse_integer(
                argv[++argument], 1, MAX_THREADS, "threads");
        } else if (strcmp(text, "--cache-mib") == 0) {
            if (argument + 1 == argc) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            cache_mib = parse_unsigned(
                argv[++argument], MIN_CACHE_MIB, MAX_CACHE_MIB,
                "cache MiB");
        } else if (strcmp(text, "--progress") == 0) {
            if (argument + 1 == argc) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            progress_seconds = parse_unsigned(
                argv[++argument], 0, MAX_PROGRESS_SECONDS,
                "progress seconds");
        } else if (text[0] != '-' && !mode_set && target < 0) {
            mode = MODE_UPTO;
            mode_set = true;
            target = parse_integer(text, MIN_N, MAX_N, "N");
        } else {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (target < 0) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    if (start_set && mode != MODE_UPTO) {
        fprintf(stderr, "error: --start is valid only with --upto or "
                "a positional upper bound N\n");
        return EXIT_FAILURE;
    }

    if (mode == MODE_TERM) {
        U128 value = compute_checked(
            target, requested_threads, cache_mib,
            progress_seconds, true);
        write_result(stdout, target, value, "result to stdout");
    } else if (mode == MODE_UPTO) {
        produce_b_file(argv[0], target, start_n,
                       requested_threads, cache_mib,
                       progress_seconds);
    } else {
        for (int n = MIN_N; n <= target; ++n) {
            (void)compute_checked(
                n, requested_threads, cache_mib,
                progress_seconds, false);
        }
        printf("ok: bounded memoized search agrees with A398911 for "
               "n=%d..%d\n", MIN_N, target);
    }
    return EXIT_SUCCESS;
}
