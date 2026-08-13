/* A397415 --
 * Exact count of partitions of {1,...,5*n} into unordered five-element
 * blocks satisfying x+y+z=w+v.  Intended especially for n=7.
 *
 * Each valid block has a unique two-element side {w,v}: if two different
 * pairs in the same positive five-set had the same sum, disjoint pairs
 * would already use the whole required total and leave a positive fifth
 * value, while intersecting pairs would force repeated values.
 *
 * The valid five-sets are the rows of an exact-cover instance.  A recursive
 * state is only the remaining-value mask S, so equal subproblems share one
 * exact U128 count in a concurrent hash table.  The next value is chosen by
 * Knuth's minimum-column (S) heuristic.
 *
 * For |S|=5*m and total T, the union P of all two-element sides in any
 * completion has |P|=2*m and sum(P)=T/2.  Therefore the sum L of the 2*m
 * smallest remaining values and the sum H of the 2*m largest must satisfy
 * L<=T/2<=H.  Equality fixes P to the corresponding end block.  Near the
 * root an exact cardinality-aware subset-sum bitset verifies that such P
 * exists.  All pruning is necessary-condition pruning; no random or
 * probabilistic step is used.
 *
 * Build:
 *   clang -O3 -march=native -std=c11 -Wall -Wextra -Wpedantic -pthread \
 *       397415_01.c -o 397415_01
 *
 * Run:
 *   ./397415_01 7 --threads 8 --hash-power 25
 *
 * For n=7 the first two exact-cover rows are expanded before threads start,
 * avoiding a few exceptionally heavy root jobs.  Approximate memo allocation
 * is 25*2^P bytes: P=24 about 400 MiB, P=25 about 800 MiB.  At 60% occupancy
 * new insertions stop while existing entries remain usable; calculation then
 * continues exactly, so a full cache never aborts the run.  Completed terms
 * are atomically recorded in b397415_01.txt by default.  Use --no-bfile to
 * disable this, or --through N to compute and record the contiguous prefix.
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
#error unsigned __int128 is required
#endif
__extension__ typedef unsigned __int128 U128;

#define MAX_N 7
#define MAX_VALUES (5 * MAX_N)
#define DEFAULT_THREADS 8
#define DEFAULT_HASH_POWER 25
#define MAX_THREADS 64
#define MIN_HASH_POWER 23
#define MAX_HASH_POWER 27

typedef struct { uint64_t edge, pair; } Row;
typedef struct { uint32_t *items, count, capacity; } Adjacency;
typedef struct { uint64_t mask; } RootTask;
typedef struct { U128 answer; } Worker;

static Row *rows;
static uint32_t row_count, row_capacity;
static Adjacency adjacency[MAX_VALUES];

static _Atomic uint64_t *memo_keys;
static U128 *memo_values;
static _Atomic unsigned char *memo_ready;
static _Atomic size_t memo_used;
static _Atomic uint64_t calls, hits, pruned;
static _Atomic int memo_frozen;
static size_t memo_size, memo_mask;
static int requested_threads = DEFAULT_THREADS;
static unsigned requested_hash_power = DEFAULT_HASH_POWER;
static const char *output_path = "b397415_01.txt";
static bool write_bfile = true;

static RootTask *root_tasks;
static uint32_t root_count, root_capacity;
static _Atomic uint32_t next_root, completed_root;
static double start_time;
static pthread_mutex_t monitor_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t monitor_condition = PTHREAD_COND_INITIALIZER;
static int monitor_finished;

static _Noreturn void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static double now_seconds(void)
{
    struct timespec t;
    if (clock_gettime(CLOCK_MONOTONIC, &t) != 0) {
        die("clock_gettime failed");
    }
    return (double)t.tv_sec + (double)t.tv_nsec / 1e9;
}

static void add_adjacency(Adjacency *a, uint32_t row)
{
    if (a->count == a->capacity) {
        a->capacity = a->capacity ? 2U * a->capacity : 1024U;
        a->items = realloc(a->items, (size_t)a->capacity * sizeof(*a->items));
        if (a->items == NULL) die("cannot allocate adjacency list");
    }
    a->items[a->count++] = row;
}

static void add_row(uint64_t edge, uint64_t pair)
{
    if (row_count == row_capacity) {
        row_capacity = row_capacity ? 2U * row_capacity : 65536U;
        rows = realloc(rows, (size_t)row_capacity * sizeof(*rows));
        if (rows == NULL) die("cannot allocate rows");
    }
    rows[row_count] = (Row){edge, pair};
    for (uint64_t t = edge; t != 0; t &= t - 1) {
        add_adjacency(&adjacency[__builtin_ctzll(t)], row_count);
    }
    ++row_count;
}

static void generate_rows(int n)
{
    const int maximum = 5 * n;
    for (int a = 1; a <= maximum; ++a) {
        for (int b = a + 1; b <= maximum; ++b) {
            const int sum = a + b;
            for (int x = 1; x <= maximum; ++x) {
                if (x == a || x == b) continue;
                for (int y = x + 1; y <= maximum; ++y) {
                    if (y == a || y == b) continue;
                    const int z = sum - x - y;
                    if (z <= y || z > maximum || z == a || z == b) continue;
                    const uint64_t pair = (UINT64_C(1) << (a - 1)) |
                                          (UINT64_C(1) << (b - 1));
                    const uint64_t edge = pair |
                        (UINT64_C(1) << (x - 1)) |
                        (UINT64_C(1) << (y - 1)) |
                        (UINT64_C(1) << (z - 1));
                    add_row(edge, pair);
                }
            }
        }
    }
}

static uint64_t mix64(uint64_t x)
{
    x ^= x >> 30;
    x *= UINT64_C(0xbf58476d1ce4e5b9);
    x ^= x >> 27;
    x *= UINT64_C(0x94d049bb133111eb);
    return x ^ (x >> 31);
}

/* Return 1 for a hit, 0 when the caller owns *slot, and -1 for an uncached
   miss after insertion has been frozen.  Freezing affects speed only. */
static int memo_claim(uint64_t key, size_t *slot, U128 *value)
{
    size_t p = (size_t)mix64(key) & memo_mask;
    for (;;) {
        uint64_t found = atomic_load_explicit(&memo_keys[p], memory_order_acquire);
        if (found == key) {
            while (!atomic_load_explicit(&memo_ready[p], memory_order_acquire)) {
                sched_yield();
            }
            *value = memo_values[p];
            atomic_fetch_add_explicit(&hits, 1, memory_order_relaxed);
            return 1;
        }
        if (found == 0) {
            if (atomic_load_explicit(&memo_frozen, memory_order_relaxed)) {
                return -1;
            }
            uint64_t expected = 0;
            if (atomic_compare_exchange_strong_explicit(
                    &memo_keys[p], &expected, key,
                    memory_order_acq_rel, memory_order_acquire)) {
                const size_t used = atomic_fetch_add_explicit(
                    &memo_used, 1, memory_order_relaxed) + 1U;
                if (used * 10U >= memo_size * 6U) {
                    atomic_store_explicit(&memo_frozen, 1,
                                          memory_order_relaxed);
                }
                *slot = p;
                return 0;
            }
            continue;
        }
        p = (p + 1U) & memo_mask;
    }
}

static void memo_publish(size_t slot, U128 value)
{
    memo_values[slot] = value;
    atomic_store_explicit(&memo_ready[slot], 1, memory_order_release);
}

/* Necessary condition: choose exactly need values whose sum is target.
   target<=315, hence five 64-bit words suffice. */
static int role_sum_possible(uint64_t mask, int need, int target)
{
    uint64_t dp[15][5] = {{0}};
    dp[0][0] = 1;
    for (uint64_t t = mask; t != 0; t &= t - 1) {
        const int value = __builtin_ctzll(t) + 1;
        const int word_shift = value >> 6;
        const int bit_shift = value & 63;
        for (int count = need; count > 0; --count) {
            for (int word = 4; word >= word_shift; --word) {
                uint64_t shifted = dp[count - 1][word - word_shift] << bit_shift;
                if (bit_shift != 0 && word > word_shift) {
                    shifted |= dp[count - 1][word - word_shift - 1] >>
                               (64 - bit_shift);
                }
                dp[count][word] |= shifted;
            }
        }
    }
    return (int)((dp[need][target >> 6] >> (target & 63)) & 1U);
}

static U128 count_state(uint64_t mask)
{
    if (mask == 0) return 1;
    atomic_fetch_add_explicit(&calls, 1, memory_order_relaxed);

    size_t slot = 0;
    U128 cached;
    const int memo_status = memo_claim(mask, &slot, &cached);
    if (memo_status == 1) return cached;
    const int memoized = memo_status == 0;

    const int m = __builtin_popcountll(mask) / 5;
    int total = 0, lower_sum = 0, upper_sum = 0;
    uint64_t lower_mask = 0, upper_mask = 0;
    for (uint64_t t = mask; t != 0; t &= t - 1) {
        total += __builtin_ctzll(t) + 1;
    }
    uint64_t t = mask;
    for (int i = 0; i < 2 * m; ++i) {
        const uint64_t bit = t & (~t + 1U);
        t ^= bit;
        lower_mask |= bit;
        lower_sum += __builtin_ctzll(bit) + 1;
    }
    t = mask;
    for (int i = 0; i < 2 * m; ++i) {
        const int value = 64 - __builtin_clzll(t);
        const uint64_t bit = UINT64_C(1) << (value - 1);
        t ^= bit;
        upper_mask |= bit;
        upper_sum += value;
    }

    if ((total & 1) || 2 * lower_sum > total || 2 * upper_sum < total ||
        (m >= 6 && !role_sum_possible(mask, 2 * m, total / 2))) {
        atomic_fetch_add_explicit(&pruned, 1, memory_order_relaxed);
        if (memoized) memo_publish(slot, 0);
        return 0;
    }
    const int lower_forced = 2 * lower_sum == total;
    const int upper_forced = 2 * upper_sum == total;

    int best_value = -1;
    uint32_t best_count = UINT32_MAX;
    for (uint64_t values = mask; values != 0; values &= values - 1) {
        const int value = __builtin_ctzll(values);
        const Adjacency *a = &adjacency[value];
        uint32_t compatible = 0;
        for (uint32_t j = 0; j < a->count; ++j) {
            const Row *r = &rows[a->items[j]];
            compatible += (r->edge & mask) == r->edge &&
                (!lower_forced || r->pair == (r->edge & lower_mask)) &&
                (!upper_forced || r->pair == (r->edge & upper_mask));
        }
        if (compatible == 0) {
            if (memoized) memo_publish(slot, 0);
            return 0;
        }
        if (compatible < best_count) {
            best_count = compatible;
            best_value = value;
            if (compatible == 1) break;
        }
    }

    U128 answer = 0;
    const Adjacency *a = &adjacency[best_value];
    for (uint32_t j = 0; j < a->count; ++j) {
        const Row *r = &rows[a->items[j]];
        if ((r->edge & mask) != r->edge ||
            (lower_forced && r->pair != (r->edge & lower_mask)) ||
            (upper_forced && r->pair != (r->edge & upper_mask))) continue;
        const U128 addend = count_state(mask ^ r->edge);
        const U128 next = answer + addend;
        if (next < answer) die("answer overflowed unsigned __int128");
        answer = next;
    }
    if (memoized) memo_publish(slot, answer);
    return answer;
}

static void add_root_task(uint64_t mask)
{
    if (root_count == root_capacity) {
        root_capacity = root_capacity ? 2U * root_capacity : 65536U;
        root_tasks = realloc(root_tasks,
                             (size_t)root_capacity * sizeof(*root_tasks));
        if (root_tasks == NULL) die("cannot allocate split root tasks");
    }
    root_tasks[root_count++].mask = mask;
}

static int minimum_column(uint64_t mask, uint32_t *compatible_count)
{
    int best_value = -1;
    uint32_t best = UINT32_MAX;
    for (uint64_t values = mask; values != 0; values &= values - 1) {
        const int value = __builtin_ctzll(values);
        const Adjacency *a = &adjacency[value];
        uint32_t count = 0;
        for (uint32_t j = 0; j < a->count; ++j) {
            count += (rows[a->items[j]].edge & mask) == rows[a->items[j]].edge;
        }
        if (count < best) {
            best = count;
            best_value = value;
            if (count == 0) break;
        }
    }
    *compatible_count = best;
    return best_value;
}

static void *worker_main(void *argument)
{
    Worker *worker = argument;
    for (;;) {
        const uint32_t index = atomic_fetch_add_explicit(
            &next_root, 1, memory_order_relaxed);
        if (index >= root_count) break;
        const U128 addend = count_state(root_tasks[index].mask);
        const U128 next = worker->answer + addend;
        if (next < worker->answer) die("worker answer overflow");
        worker->answer = next;
        const uint32_t done = atomic_fetch_add_explicit(
            &completed_root, 1, memory_order_relaxed) + 1U;
        uint32_t step = root_count / 20U;
        if (step == 0) step = 1;
        if (done % step == 0 || done == root_count) {
            fprintf(stderr,
                    "progress: %u/%u tasks, states=%zu, calls=%" PRIu64
                    ", %.1f s\n",
                    done, root_count,
                    atomic_load_explicit(&memo_used, memory_order_relaxed),
                    atomic_load_explicit(&calls, memory_order_relaxed),
                    now_seconds() - start_time);
        }
    }
    return NULL;
}

static void *monitor_main(void *argument)
{
    (void)argument;
    if (pthread_mutex_lock(&monitor_mutex) != 0) {
        die("cannot lock progress monitor");
    }
    while (!monitor_finished) {
        struct timespec deadline;
        if (clock_gettime(CLOCK_REALTIME, &deadline) != 0) {
            die("clock_gettime failed in progress monitor");
        }
        deadline.tv_sec += 10;
        const int error = pthread_cond_timedwait(
            &monitor_condition, &monitor_mutex, &deadline);
        if (monitor_finished) break;
        if (error != 0 && error != ETIMEDOUT) {
            die("progress monitor wait failed");
        }
        fprintf(stderr,
                "heartbeat: tasks=%u/%u, states=%zu%s, calls=%" PRIu64
                ", hits=%" PRIu64 ", pruned=%" PRIu64 ", %.1f s\n",
                atomic_load_explicit(&completed_root, memory_order_relaxed),
                root_count,
                atomic_load_explicit(&memo_used, memory_order_relaxed),
                atomic_load_explicit(&memo_frozen, memory_order_relaxed) ?
                    " (cache frozen)" : "",
                atomic_load_explicit(&calls, memory_order_relaxed),
                atomic_load_explicit(&hits, memory_order_relaxed),
                atomic_load_explicit(&pruned, memory_order_relaxed),
                now_seconds() - start_time);
    }
    if (pthread_mutex_unlock(&monitor_mutex) != 0) {
        die("cannot unlock progress monitor");
    }
    return NULL;
}

static void reset_calculation_storage(void)
{
    free(rows);
    rows = NULL;
    row_count = row_capacity = 0;
    for (int i = 0; i < MAX_VALUES; ++i) {
        free(adjacency[i].items);
        adjacency[i] = (Adjacency){0};
    }
    free(root_tasks);
    root_tasks = NULL;
    root_count = root_capacity = 0;
    free(memo_keys);
    free(memo_values);
    free(memo_ready);
    memo_keys = NULL;
    memo_values = NULL;
    memo_ready = NULL;
    memo_size = memo_mask = 0;
    atomic_store(&memo_used, 0);
    atomic_store(&calls, 0);
    atomic_store(&hits, 0);
    atomic_store(&pruned, 0);
    atomic_store(&memo_frozen, 0);
    atomic_store(&next_root, 0);
    atomic_store(&completed_root, 0);
    monitor_finished = 0;
}

static U128 calculate(int n)
{
    generate_rows(n);
    const int values = 5 * n;
    const uint64_t full = (UINT64_C(1) << values) - 1U;

    int best_value = -1;
    uint32_t best_count = UINT32_MAX;
    for (int value = 0; value < values; ++value) {
        if (adjacency[value].count < best_count) {
            best_count = adjacency[value].count;
            best_value = value;
        }
    }
    const int split_levels = n >= 7 ? 2 : 1;
    fprintf(stderr, "building %d-level task split...\n", split_levels);
    for (uint32_t i = 0; i < best_count; ++i) {
        const uint64_t child =
            full ^ rows[adjacency[best_value].items[i]].edge;
        if (split_levels == 1) {
            add_root_task(child);
            continue;
        }
        uint32_t second_count;
        const int second_value = minimum_column(child, &second_count);
        if (second_count == 0) continue;
        const Adjacency *a = &adjacency[second_value];
        for (uint32_t j = 0; j < a->count; ++j) {
            const uint64_t edge = rows[a->items[j]].edge;
            if ((edge & child) == edge) add_root_task(child ^ edge);
        }
    }
    if (root_count == 0) {
        reset_calculation_storage();
        return 0;
    }
    /* Keep tasks with the same first row adjacent: their descendants overlap
       heavily, so concurrent workers obtain far more shared-cache hits. */

    memo_size = (size_t)1 << requested_hash_power;
    memo_mask = memo_size - 1U;
    memo_keys = calloc(memo_size, sizeof(*memo_keys));
    memo_values = calloc(memo_size, sizeof(*memo_values));
    memo_ready = calloc(memo_size, sizeof(*memo_ready));
    if (memo_keys == NULL || memo_values == NULL || memo_ready == NULL) {
        die("cannot allocate shared memo table");
    }

    int threads = requested_threads;
    if ((uint32_t)threads > root_count) threads = (int)root_count;
    Worker *workers = calloc((size_t)threads, sizeof(*workers));
    pthread_t *ids = calloc((size_t)threads, sizeof(*ids));
    if (workers == NULL || ids == NULL) die("cannot allocate workers");
    start_time = now_seconds();
    fprintf(stderr,
            "n=%d, rows=%u, root-column=%d, split-levels=%d, tasks=%u, "
            "threads=%d, memo=2^%u\n",
            n, row_count, best_value + 1, split_levels, root_count, threads,
            requested_hash_power);
    for (int i = 0; i < threads; ++i) {
        const int error = pthread_create(&ids[i], NULL, worker_main, &workers[i]);
        if (error != 0) die("pthread_create failed");
    }
    pthread_t monitor_id;
    if (pthread_create(&monitor_id, NULL, monitor_main, NULL) != 0) {
        die("cannot create progress monitor");
    }
    U128 answer = 0;
    for (int i = 0; i < threads; ++i) {
        if (pthread_join(ids[i], NULL) != 0) die("pthread_join failed");
        const U128 next = answer + workers[i].answer;
        if (next < answer) die("answer overflow");
        answer = next;
    }
    free(ids);
    free(workers);
    if (pthread_mutex_lock(&monitor_mutex) != 0) {
        die("cannot lock completed progress monitor");
    }
    monitor_finished = 1;
    if (pthread_cond_signal(&monitor_condition) != 0 ||
        pthread_mutex_unlock(&monitor_mutex) != 0) {
        die("cannot stop progress monitor");
    }
    if (pthread_join(monitor_id, NULL) != 0) {
        die("cannot join progress monitor");
    }
    fprintf(stderr,
            "complete: states=%zu, calls=%" PRIu64 ", hits=%" PRIu64
            ", pruned=%" PRIu64 ", %.3f s\n",
            atomic_load(&memo_used), atomic_load(&calls), atomic_load(&hits),
            atomic_load(&pruned), now_seconds() - start_time);
    reset_calculation_storage();
    return answer;
}

static void print_u128(U128 value)
{
    char digits[40];
    int count = 0;
    do {
        digits[count++] = (char)('0' + value % 10);
        value /= 10;
    } while (value != 0);
    while (count != 0) putchar(digits[--count]);
}

static bool parse_u128(const char *text, U128 *result)
{
    const U128 maximum = ~(U128)0;
    U128 value = 0;
    if (*text == '\0') return false;
    while (*text != '\0') {
        if (*text < '0' || *text > '9') return false;
        const unsigned digit = (unsigned)(*text++ - '0');
        if (value > (maximum - digit) / 10U) return false;
        value = value * 10U + digit;
    }
    *result = value;
    return true;
}

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

static void print_term(int n, U128 value)
{
    if (printf("%d ", n) < 0) die("cannot write result");
    print_u128(value);
    if (putchar('\n') == EOF || fflush(stdout) != 0) die("cannot write result");
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
        int index;
        char number[40], extra;
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
    if (ferror(input) || fclose(input) != 0) die("cannot read complete b-file");
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
                "error: %s ends before n=%d; use --through %d first, or "
                "use --no-bfile\n", output_path, n, n);
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
        if (strcmp(values[n], text) != 0) die("value disagrees with b-file");
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
    fprintf(stderr, "397415_01: recorded computed term n=%d in %s\n",
            n, output_path);
}

static U128 sequence_term(int n)
{
    if (n == 0) return 1;
    if (n % 4 != 0 && n % 4 != 3) return 0;
    return calculate(n);
}

static const char *const known[] = {
    "1", "0", "0", "5452", "2085870", "0", "0"
};

static void verify_known(int n, U128 value)
{
    if ((size_t)n >= sizeof(known) / sizeof(known[0])) return;
    U128 expected;
    if (!parse_u128(known[n], &expected)) die("invalid built-in known term");
    if (value != expected) die("computed term disagrees with built-in value");
}

static long parse_long(const char *text, long minimum, long maximum,
                       const char *name)
{
    errno = 0;
    char *end = NULL;
    const long value = strtol(text, &end, 10);
    if (errno || end == text || *end != '\0' ||
        value < minimum || value > maximum) {
        fprintf(stderr, "error: %s must be in %ld..%ld: %s\n",
                name, minimum, maximum, text);
        exit(EXIT_FAILURE);
    }
    return value;
}

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s N [--threads T] [--hash-power P] "
            "[--output FILE|--no-bfile]\n"
            "       %s --term N [options]\n"
            "       %s --through N [options]\n"
            "N computes a(0)..a(N); --term computes only a(N).\n"
            "N=0..%d, T=1..%d, P=%d..%d (default P=%d)\n",
            program, program, program, MAX_N, MAX_THREADS,
            MIN_HASH_POWER, MAX_HASH_POWER, DEFAULT_HASH_POWER);
}

int main(int argc, char **argv)
{
    int n = -1;
    int have_threads = 0, have_hash_power = 0;
    bool through = false;
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")) {
            usage(argv[0]);
            return EXIT_SUCCESS;
        } else if (!strcmp(argv[i], "--threads")) {
            if (have_threads || ++i >= argc) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            requested_threads = (int)parse_long(
                argv[i], 1, MAX_THREADS, "threads");
            have_threads = 1;
        } else if (!strcmp(argv[i], "--hash-power")) {
            if (have_hash_power || ++i >= argc) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            requested_hash_power = (unsigned)parse_long(
                argv[i], MIN_HASH_POWER, MAX_HASH_POWER, "hash power");
            have_hash_power = 1;
        } else if (!strcmp(argv[i], "--output")) {
            if (++i >= argc || argv[i][0] == '\0') {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            output_path = argv[i];
            write_bfile = true;
        } else if (!strcmp(argv[i], "--no-bfile")) {
            write_bfile = false;
        } else if (!strcmp(argv[i], "--term") ||
                   !strcmp(argv[i], "--through")) {
            if (n >= 0 || ++i >= argc) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            through = !strcmp(argv[i - 1], "--through");
            n = (int)parse_long(argv[i], 0, MAX_N, "N");
        } else if (argv[i][0] != '-' && n < 0) {
            n = (int)parse_long(argv[i], 0, MAX_N, "N");
            through = true;
        } else {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
    }
    if (n < 0) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    if (through) {
        for (int k = 0; k <= n; ++k) {
            require_recordable(k);
            const U128 value = sequence_term(k);
            verify_known(k, value);
            print_term(k, value);
            record_term(k, value);
        }
        return EXIT_SUCCESS;
    }
    require_recordable(n);
    const U128 answer = sequence_term(n);
    verify_known(n, answer);
    print_term(n, answer);
    record_term(n, answer);
    return EXIT_SUCCESS;
}
