/*
 * A397767 -- fast deterministic exact-cover counter.
 *
 * Count partitions of {1,...,4*n} into unordered four-element subsets that
 * can each be split into two pairs with the same prime sum.  Write a block as
 * a < b < c < d.  Two pairings are strictly unequal,
 *
 *     a+b < c+d,             a+c < b+d,
 *
 * so the block is valid exactly when a+d = b+c is prime.  The equal-sum split
 * is therefore unique.  generate_rows() lists each valid four-set exactly once
 * by choosing a<b<c, setting d=b+c-a, and applying an exact sieve of
 * Eratosthenes to a+d.
 *
 * These rows form an exact-cover matrix with columns 1..4*n.  At a remaining
 * mask M, minimum_column() chooses a value occurring in the fewest compatible
 * rows.  Every completion contains exactly one such row.  Trying all of them
 * and recurring on M\row counts every unordered partition exactly once by
 * induction on popcount(M)/4.  A column with zero compatible rows is a proven
 * dead end; there is no heuristic cutoff.
 *
 * A subproblem is completely determined by its full remaining mask.  Each
 * worker has a bounded direct-mapped cache from the 64-bit mask to its exact
 * U128 count.  Lookup compares the complete key.  A hash collision merely
 * replaces an older entry and may cause recomputation; it can never return a
 * value for the wrong mask.  Thus hashing is deterministic and affects only
 * speed, not correctness.  Worker-private caches also avoid data races.
 *
 * The first two exact-cover choices are expanded into dynamically scheduled
 * tasks.  Different branches that reach the same residual mask are coalesced
 * while retaining their exact multiplicity.  The final multiplicity-weighted
 * sum is independent of task order and thread count.  Reflection symmetry is
 * deliberately NOT used: i -> 4*n+1-i changes the common sum, and does not in
 * general preserve primality.
 *
 * MAX_N=12 uses 48 mask bits.  Every answer is at most the unrestricted count
 * (4*n)!/(24^n*n!), whose value at n=12 has 120 bits.  U128 is therefore
 * sufficient throughout the advertised range, and every addition and
 * multiplicity product is checked as an additional guard.
 *
 * Build:
 *   clang -O3 -march=native -std=c11 -Wall -Wextra -Wpedantic -pthread \
 *       397767_02.c -o 397767_02
 *
 * OEIS offset is 1.  A positional N computes a(1),...,a(N).  Completed terms
 * are atomically recorded in b397767_02.txt.  --term N is rejected before the
 * calculation when an earlier b-file term is missing; --no-bfile disables all
 * b-file access.  No interrupted or failed computation is recorded.
 *   ./397767_02 8 --threads 8
 *   ./397767_02 --term 9 --threads 8 --hash-power 19 --no-bfile
 *   ./397767_02 --check --threads 8 --no-bfile
 *
 * Cache memory is approximately 24*2^P bytes per active worker.  P=19 and
 * eight workers use about 96 MiB.  Lower P is always exact and safe, but can
 * be slower because fewer states remain cached.
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
#error "397767_02.c requires unsigned __int128"
#endif
__extension__ typedef unsigned __int128 U128;

#define MAX_N 12
#define MAX_VALUES (4 * MAX_N)
#define MIN_N 1
#define MAX_THREADS 64
#define DEFAULT_N 8
#define DEFAULT_THREADS 4
#define DEFAULT_HASH_POWER 19
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
    uint64_t multiplicity;
} RootTask;

typedef struct {
    uint64_t *keys;
    U128 *values;
    size_t mask;
} Memo;

typedef struct {
    uint64_t calls;
    uint64_t hits;
    uint64_t dead_ends;
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
static const char *output_path = "b397767_02.txt";
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

static bool multiply_u128_u64(U128 value, uint64_t factor, U128 *product)
{
    const U128 maximum = ~(U128)0;
    if (value != 0 && (U128)factor > maximum / value) return false;
    *product = value * (U128)factor;
    return true;
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

static int lock_bfile(void)
{
    char *path = malloc(strlen(output_path) + 6U);
    if (path == NULL) die("cannot allocate lock path");
    sprintf(path, "%s.lock", output_path);
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
        if (errno == ENOENT) return MIN_N;
        die("cannot read b-file");
    }
    char line[256];
    int next = MIN_N;
    while (fgets(line, sizeof(line), input) != NULL) {
        char *p = line;
        while (*p == ' ' || *p == '\t') ++p;
        if (*p == '\0' || *p == '\n' || *p == '#') continue;
        char number[40], extra;
        int index;
        if (sscanf(p, "%d %39s %c", &index, number, &extra) != 2 ||
            index != next || index < MIN_N || index > MAX_N) {
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

    char *temporary = malloc(strlen(output_path) + 32U);
    if (temporary == NULL) die("cannot allocate temporary path");
    sprintf(temporary, "%s.tmp.%ld", output_path, (long)getpid());
    FILE *output = fopen(temporary, "w");
    if (output == NULL) die("cannot create temporary b-file");
    bool failed = false;
    for (int i = MIN_N; i < prefix; ++i) {
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
    fprintf(stderr, "397767_02: recorded computed term n=%d in %s\n",
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
    uint64_t bits = edge;
    while (bits != 0) {
        const unsigned value = (unsigned)__builtin_ctzll(bits);
        bits &= bits - 1U;
        adjacency_add(&adjacency[value], row_count);
    }
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
    const unsigned maximum = 4U * (unsigned)n;
    bool prime[2 * MAX_VALUES + 1];
    const unsigned sieve_max = 2U * MAX_VALUES;
    for (unsigned value = 0; value <= sieve_max; ++value) {
        prime[value] = value >= 2U;
    }
    for (unsigned divisor = 2; divisor <= sieve_max / divisor; ++divisor) {
        if (!prime[divisor]) continue;
        for (unsigned multiple = divisor * divisor;
             multiple <= sieve_max; multiple += divisor) {
            prime[multiple] = false;
        }
    }
    for (unsigned a = 1; a <= maximum; ++a) {
        for (unsigned b = a + 1U; b <= maximum; ++b) {
            for (unsigned c = b + 1U; c <= maximum; ++c) {
                const unsigned d = b + c - a;
                if (d > maximum) break;
                if (!prime[a + d]) continue;
                const uint64_t edge = (UINT64_C(1) << (a - 1U)) |
                                      (UINT64_C(1) << (b - 1U)) |
                                      (UINT64_C(1) << (c - 1U)) |
                                      (UINT64_C(1) << (d - 1U));
                add_row(edge);
            }
        }
    }
}

static int minimum_column(uint64_t mask, uint32_t *compatible_count)
{
    int best = -1;
    uint32_t best_count = UINT32_MAX;
    uint64_t bits = mask;
    while (bits != 0) {
        const unsigned value = (unsigned)__builtin_ctzll(bits);
        bits &= bits - 1U;
        const Adjacency *list = &adjacency[value];
        uint32_t count = 0;
        for (uint32_t i = 0; i < list->count; ++i) {
            const uint64_t edge = rows[list->items[i]].edge;
            count += (edge & mask) == edge;
        }
        if (count < best_count) {
            best = (int)value;
            best_count = count;
            if (count == 0) break;
        }
    }
    *compatible_count = best_count;
    return best;
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

    uint32_t compatible;
    const int column = minimum_column(mask, &compatible);
    if (column < 0 || compatible == 0) {
        if (stats->dead_ends == UINT64_MAX) die("dead-end counter overflow");
        ++stats->dead_ends;
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
    if (schedule->count == UINT32_MAX) die("too many root branches");
    schedule->tasks[schedule->count].mask = mask;
    schedule->tasks[schedule->count].multiplicity = 1;
    ++schedule->count;
}

static int compare_tasks(const void *left, const void *right)
{
    const RootTask *a = left, *b = right;
    return a->mask < b->mask ? -1 : a->mask != b->mask;
}

static void coalesce_tasks(RootSchedule *schedule)
{
    if (schedule->count < 2) return;
    qsort(schedule->tasks, schedule->count, sizeof(*schedule->tasks),
          compare_tasks);
    uint32_t output = 0;
    for (uint32_t input = 0; input < schedule->count; ++input) {
        if (output != 0 &&
            schedule->tasks[output - 1U].mask == schedule->tasks[input].mask) {
            if (schedule->tasks[output - 1U].multiplicity == UINT64_MAX) {
                die("root multiplicity overflow");
            }
            ++schedule->tasks[output - 1U].multiplicity;
        } else {
            schedule->tasks[output++] = schedule->tasks[input];
        }
    }
    schedule->count = output;
}

static void build_root_tasks(uint64_t full, RootSchedule *schedule)
{
    uint32_t capacity = 0, root_count;
    const int root_column = minimum_column(full, &root_count);
    if (root_column < 0 || root_count == 0) return;
    const Adjacency *first = &adjacency[root_column];
    for (uint32_t i = 0; i < first->count; ++i) {
        const uint64_t edge1 = rows[first->items[i]].edge;
        if ((edge1 & full) != edge1) continue;
        const uint64_t child = full ^ edge1;
        if (child == 0) {
            add_task(schedule, &capacity, child);
            continue;
        }
        uint32_t second_count;
        const int second_column = minimum_column(child, &second_count);
        if (second_column < 0 || second_count == 0) continue;
        const Adjacency *second = &adjacency[second_column];
        for (uint32_t j = 0; j < second->count; ++j) {
            const uint64_t edge2 = rows[second->items[j]].edge;
            if ((edge2 & child) == edge2) {
                add_task(schedule, &capacity, child ^ edge2);
            }
        }
    }
}

static void *worker_main(void *argument)
{
    Worker *worker = argument;
    for (;;) {
        const uint32_t index = atomic_fetch_add_explicit(
            &worker->schedule->next, 1U, memory_order_relaxed);
        if (index >= worker->schedule->count) break;
        const RootTask task = worker->schedule->tasks[index];
        const U128 count = count_state(task.mask, &worker->memo,
                                       &worker->stats);
        U128 addend;
        if (!multiply_u128_u64(count, task.multiplicity, &addend) ||
            !add_u128(&worker->answer, addend)) {
            die("worker U128 overflow");
        }
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
                    "397767_02: heartbeat n=%d, tasks=%u/%u, "
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
    const unsigned active_values = 4U * (unsigned)n;
    generate_rows(n);
    const uint64_t full = (UINT64_C(1) << active_values) - 1U;
    RootSchedule schedule = {0};
    atomic_init(&schedule.next, 0U);
    build_root_tasks(full, &schedule);
    if (schedule.count == 0) {
        free(schedule.tasks);
        free_rows();
        return 0;
    }
    const uint32_t root_branches = schedule.count;
    coalesce_tasks(&schedule);

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
    uint64_t calls = 1, hits = 0, dead_ends = 0;
    for (int id = 0; id < threads; ++id) {
        error = pthread_join(ids[id], NULL);
        if (error != 0) die("cannot join worker thread");
        if (!add_u128(&answer, workers[id].answer)) die("final U128 overflow");
        if (UINT64_MAX - calls < workers[id].stats.calls ||
            UINT64_MAX - hits < workers[id].stats.hits ||
            UINT64_MAX - dead_ends < workers[id].stats.dead_ends) {
            die("statistics overflow");
        }
        calls += workers[id].stats.calls;
        hits += workers[id].stats.hits;
        dead_ends += workers[id].stats.dead_ends;
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
            "397767_02: n=%d, prime-sum exact-cover min-column, rows=%u, "
            "root-branches=%u, tasks=%u, calls=%" PRIu64 ", hits=%" PRIu64
            ", dead-ends=%" PRIu64 ", threads=%d, memo=2^%u/worker, "
            "%.3f s\n",
            n, row_count, root_branches, schedule.count, calls, hits,
            dead_ends, threads, requested_hash_power, now_seconds() - started);
    free_rows();
    return answer;
}

static const char *const known[MAX_N + 1] = {
    NULL, "1", "2", "24", "204", "949", "11710", "481040",
    "5803706"
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
    if (n < MIN_N || n > MAX_N || known[n] == NULL) return;
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
            "       %s --check [--threads T] [--hash-power P] "
            "[--output FILE|--no-bfile]\n"
            "N prints a(1)..a(N); --term prints only a(N).\n",
            program, program, program);
}

int main(int argc, char **argv)
{
    int n = DEFAULT_N;
    bool term_only = false, check = false, positional = false;
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
            if (++i == argc || argv[i][0] == '\0') die("--output needs a file");
            output_path = argv[i];
            write_bfile = true;
        } else if (strcmp(argv[i], "--no-bfile") == 0) {
            write_bfile = false;
        } else if (strcmp(argv[i], "--term") == 0) {
            if (term_only || positional || ++i == argc) die("invalid --term");
            term_only = true;
            n = parse_bounded(argv[i], MIN_N, MAX_N, "N");
        } else if (strcmp(argv[i], "--check") == 0) {
            if (check || term_only || positional) die("invalid --check");
            check = true;
            n = 8;
        } else if (argv[i][0] == '-') {
            usage(argv[0]);
            return EXIT_FAILURE;
        } else {
            if (positional || term_only || check) die("multiple N arguments");
            positional = true;
            n = parse_bounded(argv[i], MIN_N, MAX_N, "N");
        }
    }

    if (term_only) {
        require_recordable(n);
        const U128 value = sequence_term(n);
        verify_known(n, value);
        print_term(n, value);
        record_term(n, value);
        return EXIT_SUCCESS;
    }
    for (int k = MIN_N; k <= n; ++k) {
        require_recordable(k);
        const U128 value = sequence_term(k);
        verify_known(k, value);
        print_term(k, value);
        record_term(k, value);
    }
    if (check) fprintf(stderr, "397767_02: self-check passed\n");
    return EXIT_SUCCESS;
}
