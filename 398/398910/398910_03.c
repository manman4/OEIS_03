/*
 * A398910 -- fast exact counter for partitions of {1,...,3*n} into
 * unordered three-element subsets {x,y,z} satisfying x+y<z.
 *
 * For positive distinct elements, x+y<z forces z to be the largest
 * element.  generate_rows() lists each valid set once as x<y<z.
 *
 * This differs from the smallest-element recurrence in 398910_02.c.  At each
 * state let z be the largest remaining value.  It must be the largest element
 * of its triple: if it were x or y, that triple would need an even larger
 * remaining value as its largest element.  The recurrence therefore tries
 * exactly the unused pairs x<y with x+y<z, removes {x,y,z}, and recurses.
 * Every completed partition has one and only one such triple containing z,
 * so induction proves that every unordered partition is counted exactly once.
 * Rows are indexed only by their largest element, avoiding a column scan.
 *
 * The remaining-value mask completely determines a subproblem.  Each worker
 * has a bounded direct-mapped cache of exact U128 results.  A hash collision
 * replaces an old entry and merely causes recomputation.  Thus hashing can
 * affect time but never the answer: this is deterministic exact counting,
 * not probabilistic hashing.  One maximum-first step is expanded before the
 * dynamically scheduled parallel work queue.
 *
 * The feasibility pruning uses only proved necessary conditions.  For a
 * remaining set of 3*k values, any solution can be changed so that its k
 * largest values are the z-values.  If a larger non-z value q and a smaller
 * z-value r are exchanged, the triple receiving q remains valid, while
 * replacing q by r in the other addend pair only decreases that pair's sum.
 * Hence the other 2*k values must have total sum strictly less than the sum
 * of the k largest values.  The two smallest addends must have sum strictly
 * below the smallest capacity.  Also all addends must be pairable with every
 * pair sum strictly below the largest capacity; pairing smallest with largest
 * tests whether such a pairing is possible.  These are necessary conditions
 * only.  Failure certifies that the state has no completion; passing never
 * contributes without the exact-cover recursion.
 *
 * MAX_N=17 uses 51 mask bits.  Every answer is bounded by the unrestricted
 * number of partitions into triples,
 *
 *                    (3*n)! / (6^n*n!).
 *
 * At n=17 this is
 * 257635246075526315926272895552000000000 < 2^128, so U128 is sufficient.
 * Every answer and worker accumulation is checked for overflow as a second
 * guard.  MAX_N is a mask/arithmetic limit, not a running-time promise.
 *
 * Build:
 *   cc -O3 -mcpu=native -std=c11 -Wall -Wextra -Wpedantic -pthread \
 *       398910_03.c -o 398910_03
 *
 * Usage:
 *   ./398910_03 10
 *   ./398910_03 --term 10 --hash-power 22
 *   ./398910_03 --check
 *
 * Completed terms are atomically recorded in b398910_03.txt.  --term N is
 * rejected before calculation if an earlier b-file term is missing.
 * Interrupted work never records an unfinished term.  --no-bfile suppresses
 * b-file access; --output FILE selects another path.
 *
 * Cache memory is approximately 24*2^P bytes per active worker.  The default
 * P=22 and one worker use about 96 MiB.  A smaller cache is always exact but
 * can cause more recomputation.  Multiple workers can duplicate subproblems,
 * so --threads 1 is intentionally the default; extra threads are optional.
 */

#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
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
#error "398910_03.c requires unsigned __int128"
#endif
__extension__ typedef unsigned __int128 U128;

#define MAX_N 17
#define MAX_VALUES (3 * MAX_N)
#define MAX_THREADS 64
#define DEFAULT_N 10
#define DEFAULT_THREADS 1
#define DEFAULT_HASH_POWER 22
#define MIN_HASH_POWER 16
#define MAX_HASH_POWER 22

_Static_assert(MAX_VALUES < 64, "search mask must fit in uint64_t");

typedef struct {
    uint64_t edge;
} Row;

typedef struct {
    uint32_t *items;
    uint32_t count;
    uint32_t capacity;
} Adjacency;

typedef struct {
    uint64_t mask;
} RootTask;

typedef struct {
    uint64_t *keys;
    U128 *values;
    size_t mask;
} Memo;

typedef struct {
    uint64_t calls;
    uint64_t hits;
    uint64_t feasibility_prunes;
} SearchStats;

typedef struct RootSchedule RootSchedule;
typedef struct {
    RootSchedule *schedule;
    Memo memo;
    U128 answer;
    SearchStats stats;
} Worker;

struct RootSchedule {
    _Atomic uint32_t next;
    uint32_t count;
    RootTask *tasks;
};

static Row *rows;
static uint32_t row_count, row_capacity;
static Adjacency adjacency[MAX_VALUES];
static int requested_threads = DEFAULT_THREADS;
static unsigned requested_hash_power = DEFAULT_HASH_POWER;
static const char *output_path = "b398910_03.txt";
static bool write_bfile = true;

static _Atomic uint64_t live_call_chunks;
static _Atomic uint32_t completed_tasks;
static pthread_mutex_t monitor_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t monitor_condition = PTHREAD_COND_INITIALIZER;
static bool monitor_finished;

static _Noreturn void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static int parse_bounded(const char *text, int low, int high,
                         const char *what)
{
    char *end = NULL;
    errno = 0;
    const long value = strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' ||
        value < low || value > high) {
        fprintf(stderr, "error: %s must be in %d..%d: %s\n",
                what, low, high, text);
        exit(EXIT_FAILURE);
    }
    return (int)value;
}

static double now_seconds(void)
{
    struct timespec value;
    if (clock_gettime(CLOCK_MONOTONIC, &value) != 0) {
        die("clock_gettime failed");
    }
    return (double)value.tv_sec + (double)value.tv_nsec / 1e9;
}

static uint64_t mix64(uint64_t x)
{
    x ^= x >> 30;
    x *= UINT64_C(0xbf58476d1ce4e5b9);
    x ^= x >> 27;
    x *= UINT64_C(0x94d049bb133111eb);
    return x ^ (x >> 31);
}

static bool add_u128(U128 *destination, U128 addend)
{
    const U128 old = *destination;
    *destination += addend;
    return *destination >= old;
}

static int print_u128(FILE *stream, U128 value)
{
    char digits[40];
    size_t length = 0;
    do {
        digits[length++] = (char)('0' + (unsigned)(value % 10));
        value /= 10;
    } while (value != 0);
    while (length != 0) {
        if (fputc(digits[--length], stream) == EOF) return -1;
    }
    return 0;
}

static void print_term(int n, U128 value)
{
    if (fprintf(stdout, "%d ", n) < 0 || print_u128(stdout, value) != 0 ||
        fputc('\n', stdout) == EOF || fflush(stdout) != 0) {
        die("cannot write result");
    }
}

static bool parse_u128(const char *text, U128 *result);

static void u128_text(U128 value, char text[40])
{
    char reverse[40];
    size_t length = 0;
    do {
        reverse[length++] = (char)('0' + (unsigned)(value % 10));
        value /= 10;
    } while (value != 0);
    for (size_t i = 0; i < length; ++i) text[i] = reverse[length - 1U - i];
    text[length] = '\0';
}

static char *path_with_suffix(const char *suffix)
{
    const size_t path_length = strlen(output_path);
    const size_t suffix_length = strlen(suffix);
    if (path_length > SIZE_MAX - suffix_length - 1U) {
        die("output path is too long");
    }
    char *path = malloc(path_length + suffix_length + 1U);
    if (path == NULL) die("cannot allocate derived output path");
    memcpy(path, output_path, path_length);
    memcpy(path + path_length, suffix, suffix_length + 1U);
    return path;
}

static int lock_bfile(void)
{
    char *path = path_with_suffix(".lock");
    const int fd = open(path, O_RDWR | O_CREAT, 0666);
    free(path);
    if (fd < 0) die("cannot open b-file lock");
    struct flock lock = {.l_type=F_WRLCK, .l_whence=SEEK_SET};
    while (fcntl(fd, F_SETLKW, &lock) != 0) {
        if (errno != EINTR) die("cannot lock b-file");
    }
    return fd;
}

static void unlock_bfile(int fd)
{
    struct flock lock = {.l_type=F_UNLCK, .l_whence=SEEK_SET};
    if (fcntl(fd, F_SETLK, &lock) != 0 || close(fd) != 0) {
        die("cannot unlock b-file");
    }
}

/* Read a strict contiguous b-file prefix.  No uncomputed built-in values are
   inserted: only record_term() can extend this prefix. */
static int read_bfile(char values[MAX_N + 1][40])
{
    FILE *input = fopen(output_path, "r");
    if (input == NULL) {
        if (errno == ENOENT) return 0;
        die("cannot read b-file");
    }
    char line[256];
    int next = 0;
    while (fgets(line, sizeof(line), input) != NULL) {
        char *p = line;
        while (*p == ' ' || *p == '\t') ++p;
        if (*p == '\0' || *p == '\n' || *p == '#') continue;
        char number[40], extra;
        int index;
        if (sscanf(p, "%d %39s %c", &index, number, &extra) != 2 ||
            index != next || index < 0 || index > MAX_N) {
            fclose(input);
            die("b-file is malformed or has a gap");
        }
        U128 parsed;
        if (!parse_u128(number, &parsed)) {
            fclose(input);
            die("b-file contains an invalid value");
        }
        strcpy(values[next++], number);
    }
    if (ferror(input) || fclose(input) != 0) die("cannot finish reading b-file");
    return next;
}

static void require_recordable(int n)
{
    if (!write_bfile) return;
    char values[MAX_N + 1][40];
    const int fd = lock_bfile();
    const int prefix = read_bfile(values);
    unlock_bfile(fd);
    if (n > prefix) {
        fprintf(stderr,
                "error: %s currently ends before n=%d; compute the missing "
                "prefix first, or use --no-bfile\n", output_path, n);
        exit(EXIT_FAILURE);
    }
}

static void record_term(int n, U128 value)
{
    if (!write_bfile) return;
    char values[MAX_N + 1][40], text[40];
    u128_text(value, text);
    const int lock_fd = lock_bfile();
    int prefix = read_bfile(values);
    if (n < prefix) {
        if (strcmp(values[n], text) != 0) die("computed value disagrees with b-file");
        unlock_bfile(lock_fd);
        return;
    }
    if (n != prefix) die("b-file gap detected while recording");
    strcpy(values[prefix++], text);

    char *temporary = path_with_suffix(".tmp.XXXXXX");
    const int temporary_fd = mkstemp(temporary);
    if (temporary_fd < 0) die("cannot create temporary b-file");
    FILE *output = fdopen(temporary_fd, "w");
    if (output == NULL) {
        close(temporary_fd);
        unlink(temporary);
        free(temporary);
        die("cannot open temporary b-file stream");
    }
    bool failed = false;
    for (int i = 0; i < prefix; ++i) {
        if (fprintf(output, "%d %s\n", i, values[i]) < 0) failed = true;
    }
    if (!failed && fflush(output) != 0) failed = true;
    if (!failed && fsync(fileno(output)) != 0) failed = true;
    if (fclose(output) != 0) failed = true;
    if (failed || rename(temporary, output_path) != 0) {
        unlink(temporary);
        free(temporary);
        die("cannot atomically update b-file");
    }
    free(temporary);
    unlock_bfile(lock_fd);
    fprintf(stderr, "398910_03: recorded computed term n=%d in %s\n",
            n, output_path);
}

static void adjacency_add(Adjacency *list, uint32_t row)
{
    if (list->count == list->capacity) {
        const uint32_t next = list->capacity ? 2U * list->capacity : 64U;
        if (next < list->capacity ||
            (size_t)next > SIZE_MAX / sizeof(*list->items)) {
            die("adjacency capacity overflow");
        }
        uint32_t *items = realloc(list->items,
                                  (size_t)next * sizeof(*items));
        if (items == NULL) die("cannot allocate adjacency list");
        list->items = items;
        list->capacity = next;
    }
    list->items[list->count++] = row;
}

static void add_row(uint64_t edge)
{
    if (row_count == row_capacity) {
        const uint32_t next = row_capacity ? 2U * row_capacity : 256U;
        if (next < row_capacity || (size_t)next > SIZE_MAX / sizeof(*rows)) {
            die("row capacity overflow");
        }
        Row *next_rows = realloc(rows, (size_t)next * sizeof(*rows));
        if (next_rows == NULL) die("cannot allocate rows");
        rows = next_rows;
        row_capacity = next;
    }
    rows[row_count].edge = edge;
    const unsigned maximum = 63U - (unsigned)__builtin_clzll(edge);
    adjacency_add(&adjacency[maximum], row_count);
    ++row_count;
}

static void free_rows(void)
{
    free(rows);
    rows = NULL;
    row_count = row_capacity = 0;
    for (unsigned i = 0; i < MAX_VALUES; ++i) {
        free(adjacency[i].items);
        adjacency[i] = (Adjacency){0};
    }
}

static void generate_rows(int n)
{
    const unsigned maximum = 3U * (unsigned)n;
    for (unsigned x = 1; x <= maximum; ++x) {
        for (unsigned y = x + 1U; y <= maximum; ++y) {
            for (unsigned z = y + 1U; z <= maximum; ++z) {
                if (x + y >= z) continue;
                const uint64_t edge = (UINT64_C(1) << (x - 1U)) |
                                      (UINT64_C(1) << (y - 1U)) |
                                      (UINT64_C(1) << (z - 1U));
                add_row(edge);
            }
        }
    }
}

/* Sound necessary-condition pruning; see the proof in the file header. */
static bool feasible_mask(uint64_t mask)
{
    const unsigned count = (unsigned)__builtin_popcountll(mask);
    if (count % 3U != 0) {
        die("internal mask cardinality is not a multiple of 3");
    }
    const unsigned k = count / 3U;
    if (k == 0) return true;

    unsigned values[MAX_VALUES];
    unsigned used = 0;
    while (mask != 0) {
        values[used++] = (unsigned)__builtin_ctzll(mask) + 1U;
        mask &= mask - 1U;
    }
    if (used != count) die("internal mask extraction mismatch");

    unsigned addend_sum = 0, capacity_sum = 0;
    for (unsigned i = 0; i < 2U * k; ++i) addend_sum += values[i];
    for (unsigned i = 2U * k; i < 3U * k; ++i) {
        capacity_sum += values[i];
    }
    if (addend_sum >= capacity_sum) return false;

    /* The smallest capacity needs some pair, whose minimum is values[0]+[1]. */
    if (values[0] + values[1] >= values[2U * k]) return false;

    /*
     * All addends must be pairable below the largest capacity.  Pairing the
     * smallest with the largest recursively minimizes the maximum pair sum.
     */
    const unsigned maximum_capacity = values[3U * k - 1U];
    for (unsigned i = 0; i < k; ++i) {
        if (values[i] + values[2U * k - 1U - i] >= maximum_capacity) {
            return false;
        }
    }

    return true;
}

static int maximum_column(uint64_t mask)
{
    return mask == 0 ? -1
                     : (int)(63U - (unsigned)__builtin_clzll(mask));
}

static bool memo_get(const Memo *memo, uint64_t key, U128 *value)
{
    const size_t slot = (size_t)mix64(key) & memo->mask;
    if (memo->keys[slot] != key) return false;
    *value = memo->values[slot];
    return true;
}

static void memo_put(Memo *memo, uint64_t key, U128 value)
{
    const size_t slot = (size_t)mix64(key) & memo->mask;
    memo->values[slot] = value;
    memo->keys[slot] = key;
}

static U128 count_state(uint64_t mask, Memo *memo, SearchStats *stats)
{
    if (mask == 0) return 1;
    if (stats->calls == UINT64_MAX) die("call counter overflow");
    ++stats->calls;
    if ((stats->calls & UINT64_C(65535)) == 0) {
        atomic_fetch_add_explicit(&live_call_chunks, UINT64_C(65536),
                                  memory_order_relaxed);
    }

    U128 cached;
    if (memo_get(memo, mask, &cached)) {
        if (stats->hits == UINT64_MAX) die("hit counter overflow");
        ++stats->hits;
        return cached;
    }

    if (!feasible_mask(mask)) {
        if (stats->feasibility_prunes == UINT64_MAX) {
            die("prune counter overflow");
        }
        ++stats->feasibility_prunes;
        memo_put(memo, mask, 0);
        return 0;
    }

    const int column = maximum_column(mask);
    if (column < 0) {
        memo_put(memo, mask, 0);
        return 0;
    }

    U128 answer = 0;
    const Adjacency *list = &adjacency[column];
    for (uint32_t i = 0; i < list->count; ++i) {
        const uint64_t edge = rows[list->items[i]].edge;
        if ((edge & mask) != edge) continue;
        const U128 addend = count_state(mask ^ edge, memo, stats);
        if (!add_u128(&answer, addend)) die("answer overflowed U128");
    }
    memo_put(memo, mask, answer);
    return answer;
}

static void add_task(RootSchedule *schedule, uint32_t *capacity,
                     uint64_t mask)
{
    if (schedule->count == *capacity) {
        const uint32_t next = *capacity ? 2U * *capacity : 256U;
        if (next < *capacity ||
            (size_t)next > SIZE_MAX / sizeof(*schedule->tasks)) {
            die("task capacity overflow");
        }
        RootTask *tasks = realloc(schedule->tasks,
                                  (size_t)next * sizeof(*tasks));
        if (tasks == NULL) die("cannot allocate root tasks");
        schedule->tasks = tasks;
        *capacity = next;
    }
    schedule->tasks[schedule->count++].mask = mask;
}

static void build_root_tasks(uint64_t full, RootSchedule *schedule)
{
    uint32_t capacity = 0;
    const int root_column = maximum_column(full);
    if (root_column < 0) return;
    const Adjacency *first = &adjacency[root_column];
    for (uint32_t i = 0; i < first->count; ++i) {
        const uint64_t edge1 = rows[first->items[i]].edge;
        if ((edge1 & full) != edge1) continue;
        add_task(schedule, &capacity, full ^ edge1);
    }
}

static void *worker_main(void *argument)
{
    Worker *worker = argument;
    for (;;) {
        const uint32_t index = atomic_fetch_add_explicit(
            &worker->schedule->next, 1U, memory_order_relaxed);
        if (index >= worker->schedule->count) break;
        const U128 addend = count_state(worker->schedule->tasks[index].mask,
                                       &worker->memo, &worker->stats);
        if (!add_u128(&worker->answer, addend)) die("worker U128 overflow");
        atomic_fetch_add_explicit(&completed_tasks, 1U, memory_order_relaxed);
    }
    return NULL;
}

typedef struct {
    int n;
    uint32_t task_count;
    double started;
} MonitorArgument;

static void *monitor_main(void *argument)
{
    const MonitorArgument *monitor = argument;
    if (pthread_mutex_lock(&monitor_mutex) != 0) die("monitor lock failed");
    while (!monitor_finished) {
        struct timespec deadline;
        if (clock_gettime(CLOCK_REALTIME, &deadline) != 0) {
            die("monitor clock failed");
        }
        deadline.tv_sec += 10;
        const int error = pthread_cond_timedwait(
            &monitor_condition, &monitor_mutex, &deadline);
        if (error != 0 && error != ETIMEDOUT) die("monitor wait failed");
        if (!monitor_finished && error == ETIMEDOUT) {
            fprintf(stderr,
                    "398910_03: heartbeat n=%d, tasks=%u/%u, "
                    "calls>=%" PRIu64 ", %.1f s\n",
                    monitor->n,
                    atomic_load_explicit(&completed_tasks,
                                         memory_order_relaxed),
                    monitor->task_count,
                    atomic_load_explicit(&live_call_chunks,
                                         memory_order_relaxed),
                    now_seconds() - monitor->started);
        }
    }
    if (pthread_mutex_unlock(&monitor_mutex) != 0) die("monitor unlock failed");
    return NULL;
}

static U128 sequence_term(int n)
{
    if (n == 0) return 1;
    generate_rows(n);
    const uint64_t full = (UINT64_C(1) << (3U * (unsigned)n)) - 1U;
    RootSchedule schedule = {0};
    atomic_init(&schedule.next, 0U);
    build_root_tasks(full, &schedule);
    if (schedule.count == 0) {
        free(schedule.tasks);
        free_rows();
        return 0;
    }

    int threads = requested_threads;
    if ((uint32_t)threads > schedule.count) threads = (int)schedule.count;
    Worker *workers = calloc((size_t)threads, sizeof(*workers));
    pthread_t *ids = calloc((size_t)threads, sizeof(*ids));
    if (workers == NULL || ids == NULL) {
        free(workers); free(ids); die("cannot allocate workers");
    }
    const size_t memo_size = (size_t)1U << requested_hash_power;
    for (int id = 0; id < threads; ++id) {
        workers[id].schedule = &schedule;
        workers[id].memo.keys = calloc(memo_size, sizeof(uint64_t));
        workers[id].memo.values = malloc(memo_size * sizeof(U128));
        workers[id].memo.mask = memo_size - 1U;
        if (workers[id].memo.keys == NULL || workers[id].memo.values == NULL) {
            die("cannot allocate per-worker memo cache");
        }
    }

    const double started = now_seconds();
    atomic_store_explicit(&live_call_chunks, 0, memory_order_relaxed);
    atomic_store_explicit(&completed_tasks, 0, memory_order_relaxed);
    if (pthread_mutex_lock(&monitor_mutex) != 0) die("monitor lock failed");
    monitor_finished = false;
    if (pthread_mutex_unlock(&monitor_mutex) != 0) die("monitor unlock failed");
    const MonitorArgument monitor_argument = {n, schedule.count, started};
    pthread_t monitor_id;
    int error = pthread_create(&monitor_id, NULL, monitor_main,
                               (void *)&monitor_argument);
    if (error != 0) die("cannot create monitor thread");
    for (int id = 0; id < threads; ++id) {
        error = pthread_create(&ids[id], NULL, worker_main, &workers[id]);
        if (error != 0) die("cannot create worker thread");
    }

    U128 answer = 0;
    uint64_t calls = 1, hits = 0, feasibility_prunes = 0;
    for (int id = 0; id < threads; ++id) {
        error = pthread_join(ids[id], NULL);
        if (error != 0) die("cannot join worker thread");
        if (!add_u128(&answer, workers[id].answer)) die("final U128 overflow");
        if (UINT64_MAX - calls < workers[id].stats.calls ||
            UINT64_MAX - hits < workers[id].stats.hits ||
            UINT64_MAX - feasibility_prunes <
                workers[id].stats.feasibility_prunes) {
            die("statistics overflow");
        }
        calls += workers[id].stats.calls;
        hits += workers[id].stats.hits;
        feasibility_prunes += workers[id].stats.feasibility_prunes;
    }
    if (pthread_mutex_lock(&monitor_mutex) != 0) die("monitor lock failed");
    monitor_finished = true;
    if (pthread_cond_signal(&monitor_condition) != 0) die("monitor signal failed");
    if (pthread_mutex_unlock(&monitor_mutex) != 0) die("monitor unlock failed");
    if (pthread_join(monitor_id, NULL) != 0) die("cannot join monitor thread");

    for (int id = 0; id < threads; ++id) {
        free(workers[id].memo.keys);
        free(workers[id].memo.values);
    }
    free(workers);
    free(ids);
    free(schedule.tasks);

    fprintf(stderr,
            "398910_03: n=%d, sparse DP, maximum-first, rows=%u, "
            "tasks=%u, calls=%" PRIu64 ", hits=%" PRIu64
            ", feasibility-prunes=%" PRIu64
            ", threads=%d, memo=2^%u/worker, %.3f s\n",
            n, row_count, schedule.count, calls, hits,
            feasibility_prunes, threads,
            requested_hash_power, now_seconds() - started);
    free_rows();
    return answer;
}

static const char *const known[] = {
    "1", "0", "0", "2", "46", "1413", "63060", "3777584",
    "302149153", "30550415691"
};

static bool parse_u128(const char *text, U128 *result)
{
    const U128 maximum = ~(U128)0;
    U128 value = 0;
    if (*text == '\0') return false;
    while (*text != '\0') {
        if (*text < '0' || *text > '9') return false;
        const unsigned digit = (unsigned)(*text++ - '0');
        if (value > (maximum - digit) / 10U) return false;
        value = 10U * value + digit;
    }
    *result = value;
    return true;
}

static void verify_known(int n, U128 value)
{
    if ((size_t)n >= sizeof(known) / sizeof(known[0])) return;
    U128 expected;
    if (!parse_u128(known[n], &expected)) die("invalid known term");
    if (value != expected) {
        fprintf(stderr, "error: known-term mismatch n=%d: got ", n);
        print_u128(stderr, value);
        fprintf(stderr, ", expected %s\n", known[n]);
        exit(EXIT_FAILURE);
    }
}

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s [N] [--threads T] [--hash-power P] "
            "[--output FILE|--no-bfile]\n"
            "       %s --term N [--threads T] [--hash-power P] "
            "[--output FILE|--no-bfile]\n"
            "       %s --check [--threads T] [--hash-power P]\n"
            "N prints a(0)..a(N); --term prints only a(N).\n",
            program, program, program);
}

int main(int argc, char **argv)
{
    int n = DEFAULT_N;
    bool term_only = false, check = false, positional = false;
    bool output_option = false, no_bfile_option = false;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--threads") == 0) {
            if (++i == argc) die("--threads needs an argument");
            requested_threads = parse_bounded(argv[i], 1, MAX_THREADS,
                                              "threads");
        } else if (strcmp(argv[i], "--hash-power") == 0) {
            if (++i == argc) die("--hash-power needs an argument");
            requested_hash_power = (unsigned)parse_bounded(
                argv[i], MIN_HASH_POWER, MAX_HASH_POWER, "hash-power");
        } else if (strcmp(argv[i], "--output") == 0) {
            if (output_option || ++i == argc || argv[i][0] == '\0') {
                die("invalid --output");
            }
            output_option = true;
            output_path = argv[i];
        } else if (strcmp(argv[i], "--no-bfile") == 0) {
            if (no_bfile_option) die("duplicate --no-bfile");
            no_bfile_option = true;
        } else if (strcmp(argv[i], "--term") == 0) {
            if (term_only || positional || check || ++i == argc) {
                die("invalid --term");
            }
            term_only = true;
            n = parse_bounded(argv[i], 0, MAX_N, "N");
        } else if (strcmp(argv[i], "--check") == 0) {
            if (check || term_only || positional) die("invalid --check");
            check = true;
            n = 8;
        } else if (strcmp(argv[i], "--help") == 0 ||
                   strcmp(argv[i], "-h") == 0) {
            usage(argv[0]);
            return EXIT_SUCCESS;
        } else if (argv[i][0] == '-') {
            usage(argv[0]);
            return EXIT_FAILURE;
        } else {
            if (positional || term_only || check) die("multiple N arguments");
            positional = true;
            n = parse_bounded(argv[i], 0, MAX_N, "N");
        }
    }

    if (output_option && no_bfile_option) {
        die("--output and --no-bfile are mutually exclusive");
    }
    if (check && output_option) die("--check cannot be combined with --output");
    write_bfile = !check && !no_bfile_option;

    if (term_only) {
        require_recordable(n);
        const U128 value = sequence_term(n);
        verify_known(n, value);
        print_term(n, value);
        record_term(n, value);
        return EXIT_SUCCESS;
    }
    for (int k = 0; k <= n; ++k) {
        require_recordable(k);
        const U128 value = sequence_term(k);
        verify_known(k, value);
        print_term(k, value);
        record_term(k, value);
    }
    if (check) fprintf(stderr, "398910_03: self-check passed for n=0..8\n");
    return EXIT_SUCCESS;
}
