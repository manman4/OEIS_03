/*
 * A398904 -- fast exact counter for partitions of {1,...,3*n} into
 * unordered three-element subsets {x,y,z}, where z is the largest element,
 * satisfying x+y>=z.
 *
 * generate_rows() lists each valid set once in the canonical order x<y<z.
 * Thus z denotes the largest element by definition; this ordering is needed
 * because the inequality is not symmetric in all three displayed variables.
 *
 * At each state let z be the largest remaining value.  Since it is also the
 * largest member of whichever triple contains it, the recurrence tries
 * exactly the unused pairs x<y with x+y>=z, removes {x,y,z}, and recurses.
 * Every completed partition has exactly one triple containing z, so induction
 * proves that every unordered partition is counted exactly once.  Rows are
 * indexed only by their largest element, avoiding a minimum-column scan.
 *
 * The remaining-value mask completely determines a subproblem.  All workers
 * share one fixed-capacity open-addressing table which retains every claimed
 * state: collisions are resolved by probing and entries are never evicted.
 * A slot tag distinguishes a state being computed from a completed state.
 * Publishing the exact U128 value uses release/acquire synchronization, so a
 * second worker either reads the completed value or waits for its unique
 * owner.  Such waits cannot deadlock: every recursive dependency removes
 * three mask bits, so the wait-for relation is strictly decreasing.  Keys are
 * compared in full; this is deterministic exact counting, not probabilistic
 * hashing.  If the configured table reaches 80% load, the
 * program stops with an error before returning an answer and asks for a
 * larger --hash-power.  One maximum-first step is expanded before the
 * dynamically scheduled parallel work queue.
 *
 * The feasibility pruning uses only two proved necessary conditions.  If the
 * current maximum z exceeds the sum of the next two largest values, no pair
 * remaining in the mask can accompany z.  Also, if values v[0]<...<v[3*k-1]
 * can be partitioned, the sum of the k triple maxima is at least
 * v[2]+v[5]+...+v[3*k-1]; this is the minimum possible sum of k group maxima.
 * The triangle inequalities imply that twice the actual maxima sum is at
 * most the total value sum.  Failure of either test therefore certifies zero.
 * Passing a test never contributes without the exact recurrence.  In
 * particular, the tempting rule that consecutive triples in sorted order
 * must work is deliberately not used: {1,2,4,5,6,7} is a counterexample,
 * since {1,6,7} and {2,4,5} form a valid partition.
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
 *       398904_03.c -o 398904_03
 *
 * Usage:
 *   ./398904_03 10
 *   ./398904_03 --term 10 --hash-power 22
 *   ./398904_03 --check
 *
 * Completed terms are atomically recorded in b398904_03.txt.  --term N is
 * rejected before calculation if an earlier b-file term is missing.
 * Interrupted work never records an unfinished term.  --no-bfile suppresses
 * b-file access; --output FILE selects another path.
 *
 * Shared memo memory is approximately 24*2^P bytes in total, independent of
 * the thread count.  The default P=22 uses about 96 MiB and can retain about
 * 3.35 million states at the enforced 80% load limit.  Unlike 398904_02, a
 * smaller table never evicts and recomputes: it fails explicitly if full.
 * --threads 1 remains the conservative default; extra threads share results.
 */

#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <pthread.h>
#include <sched.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#if !defined(__SIZEOF_INT128__)
#error "398904_03.c requires unsigned __int128"
#endif
__extension__ typedef unsigned __int128 U128;

#define MAX_N 17
#define MAX_VALUES (3 * MAX_N)
#define MAX_THREADS 64
#define DEFAULT_N 10
#define DEFAULT_THREADS 1
#define DEFAULT_HASH_POWER 22
#define MIN_HASH_POWER 16
#define MAX_HASH_POWER 26
#define MEMO_LOAD_NUMERATOR 4U
#define MEMO_LOAD_DENOMINATOR 5U
#define MEMO_DONE_BIT (UINT64_C(1) << 63)

_Static_assert(MAX_VALUES < 63, "search mask must not use the memo tag bit");

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
    _Atomic uint64_t *tags;
    U128 *values;
    size_t mask;
    size_t capacity;
    size_t load_limit;
    _Atomic size_t used;
} Memo;

typedef struct {
    uint64_t calls;
    uint64_t hits;
    uint64_t waits;
    uint64_t probes;
    uint64_t feasibility_prunes;
} SearchStats;

typedef struct RootSchedule RootSchedule;
typedef struct {
    RootSchedule *schedule;
    Memo *memo;
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
static const char *output_path = "b398904_03.txt";
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
    fprintf(stderr, "398904_03: recorded computed term n=%d in %s\n",
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
                if (x + y < z) continue;
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
    if (count == 0) return true;

    unsigned values[MAX_VALUES];
    unsigned used = 0;
    while (mask != 0) {
        values[used++] = (unsigned)__builtin_ctzll(mask) + 1U;
        mask &= mask - 1U;
    }
    if (used != count) die("internal mask extraction mismatch");

    /* No pair has a larger sum than the two values just below the maximum. */
    if (values[count - 3U] + values[count - 2U] < values[count - 1U]) {
        return false;
    }

    /* Consecutive groups minimize the sum of the k group maxima. */
    unsigned total = 0, minimum_maxima_sum = 0;
    for (unsigned i = 0; i < count; ++i) {
        total += values[i];
        if (i % 3U == 2U) minimum_maxima_sum += values[i];
    }
    return 2U * minimum_maxima_sum <= total;
}

static int maximum_column(uint64_t mask)
{
    return mask == 0 ? -1
                     : (int)(63U - (unsigned)__builtin_clzll(mask));
}

static void memo_init(Memo *memo, unsigned hash_power)
{
    const size_t capacity = (size_t)1U << hash_power;
    if (capacity > SIZE_MAX / sizeof(*memo->tags) ||
        capacity > SIZE_MAX / sizeof(*memo->values)) {
        die("memo allocation size overflow");
    }
    memo->tags = malloc(capacity * sizeof(*memo->tags));
    memo->values = malloc(capacity * sizeof(*memo->values));
    if (memo->tags == NULL || memo->values == NULL) {
        free(memo->tags);
        free(memo->values);
        die("cannot allocate shared memo table");
    }
    for (size_t i = 0; i < capacity; ++i) {
        atomic_init(&memo->tags[i], UINT64_C(0));
    }
    memo->mask = capacity - 1U;
    memo->capacity = capacity;
    memo->load_limit = (capacity / MEMO_LOAD_DENOMINATOR) *
                       MEMO_LOAD_NUMERATOR;
    atomic_init(&memo->used, (size_t)0);
}

static void memo_destroy(Memo *memo)
{
    free(memo->tags);
    free(memo->values);
    *memo = (Memo){0};
}

static void increment_stat(uint64_t *counter, const char *message)
{
    if (*counter == UINT64_MAX) die(message);
    ++*counter;
}

/*
 * Return true with *value when key is already available.  Return false after
 * atomically claiming *owned_slot when this caller must compute the state.
 * A tag equal to key means "computing"; key|MEMO_DONE_BIT means "complete".
 * Entries never move or disappear, so a waiter can safely watch one slot.
 */
static bool memo_get_or_claim(Memo *memo, uint64_t key, size_t *owned_slot,
                              U128 *value, SearchStats *stats)
{
    if (key == 0 || (key & MEMO_DONE_BIT) != 0) die("invalid memo key");
    size_t slot = (size_t)mix64(key) & memo->mask;
    for (size_t visited = 0; visited < memo->capacity;) {
        increment_stat(&stats->probes, "probe counter overflow");
        uint64_t tag = atomic_load_explicit(&memo->tags[slot],
                                            memory_order_acquire);
        if ((tag & ~MEMO_DONE_BIT) == key) {
            increment_stat(&stats->hits, "hit counter overflow");
            if ((tag & MEMO_DONE_BIT) == 0) {
                increment_stat(&stats->waits, "wait counter overflow");
                unsigned spins = 0;
                do {
                    if (++spins == 1024U) {
                        spins = 0;
                        sched_yield();
                    }
                    tag = atomic_load_explicit(&memo->tags[slot],
                                               memory_order_acquire);
                } while (tag == key);
                if (tag != (key | MEMO_DONE_BIT)) {
                    die("memo tag changed unexpectedly");
                }
            }
            *value = memo->values[slot];
            return true;
        }
        if (tag == 0) {
            uint64_t expected = 0;
            if (atomic_compare_exchange_strong_explicit(
                    &memo->tags[slot], &expected, key,
                    memory_order_acq_rel, memory_order_acquire)) {
                const size_t old = atomic_fetch_add_explicit(
                    &memo->used, (size_t)1, memory_order_relaxed);
                if (old >= memo->load_limit) {
                    die("shared memo reached 80% load; increase --hash-power");
                }
                *owned_slot = slot;
                return false;
            }
            continue;
        }
        slot = (slot + 1U) & memo->mask;
        ++visited;
    }
    die("shared memo table is full");
}

static void memo_publish(Memo *memo, size_t slot, uint64_t key, U128 value)
{
    if (slot >= memo->capacity ||
        atomic_load_explicit(&memo->tags[slot], memory_order_relaxed) != key) {
        die("invalid memo publication");
    }
    memo->values[slot] = value;
    atomic_store_explicit(&memo->tags[slot], key | MEMO_DONE_BIT,
                          memory_order_release);
}

static U128 count_state(uint64_t mask, Memo *memo, SearchStats *stats)
{
    if (mask == 0) return 1;
    increment_stat(&stats->calls, "call counter overflow");
    if ((stats->calls & UINT64_C(65535)) == 0) {
        const uint64_t old = atomic_fetch_add_explicit(
            &live_call_chunks, UINT64_C(65536), memory_order_relaxed);
        if (old > UINT64_MAX - UINT64_C(65536)) {
            die("live call counter overflow");
        }
    }

    size_t owned_slot;
    U128 cached;
    if (memo_get_or_claim(memo, mask, &owned_slot, &cached, stats)) {
        return cached;
    }

    if (!feasible_mask(mask)) {
        increment_stat(&stats->feasibility_prunes, "prune counter overflow");
        memo_publish(memo, owned_slot, mask, 0);
        return 0;
    }

    const int column = maximum_column(mask);
    if (column < 0) {
        memo_publish(memo, owned_slot, mask, 0);
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
    memo_publish(memo, owned_slot, mask, answer);
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
                                       worker->memo, &worker->stats);
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
                    "398904_03: heartbeat n=%d, tasks=%u/%u, "
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
    const double started = now_seconds();
    Memo memo = {0};
    memo_init(&memo, requested_hash_power);
    Worker *workers = calloc((size_t)threads, sizeof(*workers));
    pthread_t *ids = calloc((size_t)threads, sizeof(*ids));
    if (workers == NULL || ids == NULL) {
        free(workers); free(ids); die("cannot allocate workers");
    }
    for (int id = 0; id < threads; ++id) {
        workers[id].schedule = &schedule;
        workers[id].memo = &memo;
    }

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
    uint64_t calls = 1, hits = 0, waits = 0, probes = 0;
    uint64_t feasibility_prunes = 0;
    for (int id = 0; id < threads; ++id) {
        error = pthread_join(ids[id], NULL);
        if (error != 0) die("cannot join worker thread");
        if (!add_u128(&answer, workers[id].answer)) die("final U128 overflow");
        if (UINT64_MAX - calls < workers[id].stats.calls ||
            UINT64_MAX - hits < workers[id].stats.hits ||
            UINT64_MAX - waits < workers[id].stats.waits ||
            UINT64_MAX - probes < workers[id].stats.probes ||
            UINT64_MAX - feasibility_prunes <
                workers[id].stats.feasibility_prunes) {
            die("statistics overflow");
        }
        calls += workers[id].stats.calls;
        hits += workers[id].stats.hits;
        waits += workers[id].stats.waits;
        probes += workers[id].stats.probes;
        feasibility_prunes += workers[id].stats.feasibility_prunes;
    }
    if (pthread_mutex_lock(&monitor_mutex) != 0) die("monitor lock failed");
    monitor_finished = true;
    if (pthread_cond_signal(&monitor_condition) != 0) die("monitor signal failed");
    if (pthread_mutex_unlock(&monitor_mutex) != 0) die("monitor unlock failed");
    if (pthread_join(monitor_id, NULL) != 0) die("cannot join monitor thread");

    const size_t memo_states = atomic_load_explicit(&memo.used,
                                                     memory_order_relaxed);
    const size_t memo_capacity = memo.capacity;
    free(workers);
    free(ids);
    free(schedule.tasks);
    memo_destroy(&memo);

    fprintf(stderr,
            "398904_03: n=%d, full sparse DP, maximum-first, rows=%u, "
            "tasks=%u, states=%zu, calls=%" PRIu64 ", hits=%" PRIu64
            ", waits=%" PRIu64 ", probes=%" PRIu64
            ", feasibility-prunes=%" PRIu64
            ", threads=%d, memo=2^%u shared (%.1f%%), %.3f s\n",
            n, row_count, schedule.count, memo_states, calls, hits,
            waits, probes, feasibility_prunes, threads,
            requested_hash_power,
            100.0 * (double)memo_states / (double)memo_capacity,
            now_seconds() - started);
    free_rows();
    return answer;
}

static const char *const known[] = {
    "1", "1", "3", "38", "809", "28253", "1447208", "103513338",
    "9779693367", "1184537484761", "178435066621194"
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
            n = 9;
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
    if (check) fprintf(stderr, "398904_03: self-check passed for n=0..9\n");
    return EXIT_SUCCESS;
}
