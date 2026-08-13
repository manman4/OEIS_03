/*
 * A397767 -- weighted prime-sum permanent, exact deterministic version.
 *
 * Count partitions of {1,...,4*n} into unordered four-element subsets whose
 * elements form two pairs with the same prime sum.  Every prime sum is odd,
 * so each pair contains one odd and one even number.  Conversely, first take
 * a perfect matching between the 2*n odd and 2*n even numbers.  If prime p
 * occurs c_p times among its edge sums, those edges can be grouped into
 * c_p/2 unordered pairs in
 *
 *                       (c_p - 1)!!
 *
 * ways when c_p is even, and in zero ways when c_p is odd.  Each paired edge
 * pair is exactly one required four-element block.  Thus
 *
 *   a(n) = sum over odd-even perfect matchings M
 *          product_p L(c_p(M)),
 *
 * where L(c)=0 for odd c, L(0)=1, and L(c)=(c-1)!! for positive even c.
 * This is equivalently the permanent of A[o,e]=x_(o+e), followed by the
 * independent moment functional E[x_p^c]=L(c).  It is mathematically
 * independent of the four-set exact-cover search in 397767_02.c.
 *
 * The memoized search state is the complete triple
 *
 *   (used odd mask, used even mask, all current prime multiplicities).
 *
 * At each state it chooses the unused odd vertex having the fewest available
 * prime-sum even partners and tries every such partner.  At the leaf it
 * applies the moment product above.  Induction on the number of unused odd
 * vertices proves that every perfect matching is visited once.  The moment
 * product then groups its edges in every possible valid way, so every desired
 * partition is counted exactly once.
 *
 * A necessary parity test rejects a state only when the number of currently
 * odd multiplicities exceeds the number of matching edges still to choose:
 * every odd multiplicity needs at least one future occurrence.  This is an
 * exact impossibility test, not a heuristic cutoff.
 *
 * Every worker has a bounded direct-mapped memo table.  All three 64-bit key
 * words are compared.  A hash collision merely replaces an older entry and
 * can cause recomputation; it cannot return a wrong value.  Worker caches are
 * private, so thread scheduling affects speed only.  All count arithmetic is
 * checked U128.  For MAX_N=12, the unrestricted number of four-set partitions
 * (4*n)!/(24^n*n!) has 120 bits, so the advertised range fits in U128.
 *
 * Build:
 *   clang -O3 -march=native -std=c11 -Wall -Wextra -Wpedantic -pthread \
 *       397767_04.c -o 397767_04
 *
 * Usage:
 *   ./397767_04 8 --threads 8
 *   ./397767_04 --term 8 --threads 8 --hash-power 15 --no-bfile
 *   ./397767_04 --check --threads 8 --no-bfile
 *
 * A memo entry uses 40 bytes.  The default P=15 uses about 1.25 MiB per
 * active worker (10 MiB for eight workers).  Increasing P is always exact,
 * but this search is often faster with the smaller cache because of locality.
 *
 * OEIS offset is 1.  A positional N computes a(1),...,a(N).  Only completed
 * terms are atomically recorded in b397767_04.txt.  --term requires the
 * preceding b-file prefix unless --no-bfile is used.  An interrupted or
 * failed computation never records its unfinished term.
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
#error "397767_04.c requires unsigned __int128"
#endif
__extension__ typedef unsigned __int128 U128;

#define MAX_N 12
#define MAX_SIDE (2 * MAX_N)
#define MAX_PRIMES 24
#define MIN_N 1
#define MAX_THREADS 64
#define DEFAULT_N 8
#define DEFAULT_THREADS 4
#define DEFAULT_HASH_POWER 15
#define MIN_HASH_POWER 15
#define MAX_HASH_POWER 22
#ifndef ROOT_SPLIT_LEVELS
#define ROOT_SPLIT_LEVELS 3
#endif

_Static_assert(MAX_SIDE == 24, "packed-key layout assumes 24 vertices/side");
_Static_assert(5 * (MAX_PRIMES - 15) <= 64,
               "prime counts do not fit packed key");

typedef struct {
    uint64_t word[3];
} StateKey;

typedef struct {
    StateKey *keys;
    U128 *values;
    size_t mask;
} Memo;

typedef struct {
    uint64_t calls;
    uint64_t hits;
    uint64_t parity_prunes;
    uint64_t dead_ends;
} SearchStats;

typedef struct {
    StateKey key;
    unsigned depth;
    unsigned odd_count;
} RootTask;

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

static int8_t edge_prime[MAX_SIDE][MAX_SIDE];
static unsigned side_size;
static unsigned prime_count;
static unsigned prime_values[MAX_PRIMES];
static int requested_threads = DEFAULT_THREADS;
static unsigned requested_hash_power = DEFAULT_HASH_POWER;
static const char *output_path = "b397767_04.txt";
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
        int index;
        char number[40], extra;
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
    fprintf(stderr, "397767_04: recorded computed term n=%d in %s\n",
            n, output_path);
}

static uint64_t mix64(uint64_t x)
{
    x ^= x >> 30;
    x *= UINT64_C(0xbf58476d1ce4e5b9);
    x ^= x >> 27;
    x *= UINT64_C(0x94d049bb133111eb);
    return x ^ (x >> 31);
}

static uint64_t key_hash(StateKey key)
{
    return mix64(key.word[0] ^ mix64(key.word[1]) ^
                 (mix64(key.word[2]) + UINT64_C(0x9e3779b97f4a7c15)));
}

static bool key_equal(StateKey a, StateKey b)
{
    return a.word[0] == b.word[0] && a.word[1] == b.word[1] &&
           a.word[2] == b.word[2];
}

static uint32_t key_odd_mask(StateKey key)
{
    return (uint32_t)(key.word[0] & UINT64_C(0xffffff));
}

static uint32_t key_even_mask(StateKey key)
{
    return (uint32_t)((key.word[0] >> 24) & UINT64_C(0xffffff));
}

static unsigned key_prime_count(StateKey key, unsigned prime)
{
    if (prime < 3U) return (unsigned)((key.word[0] >> (48U + 5U * prime)) & 31U);
    if (prime < 15U) return (unsigned)((key.word[1] >> (5U * (prime - 3U))) & 31U);
    return (unsigned)((key.word[2] >> (5U * (prime - 15U))) & 31U);
}

static StateKey key_increment_prime(StateKey key, unsigned prime)
{
    if (key_prime_count(key, prime) >= MAX_SIDE) die("prime multiplicity overflow");
    if (prime < 3U) key.word[0] += UINT64_C(1) << (48U + 5U * prime);
    else if (prime < 15U) key.word[1] += UINT64_C(1) << (5U * (prime - 3U));
    else key.word[2] += UINT64_C(1) << (5U * (prime - 15U));
    return key;
}

static StateKey key_add_edge(StateKey key, unsigned odd, unsigned even,
                             unsigned prime)
{
    key.word[0] |= UINT64_C(1) << odd;
    key.word[0] |= UINT64_C(1) << (24U + even);
    return key_increment_prime(key, prime);
}

static bool memo_get(const Memo *memo, StateKey key, U128 *value)
{
    const size_t slot = (size_t)key_hash(key) & memo->mask;
    if (memo->keys[slot].word[0] == UINT64_MAX ||
        !key_equal(memo->keys[slot], key)) return false;
    *value = memo->values[slot];
    return true;
}

static void memo_put(Memo *memo, StateKey key, U128 value)
{
    const size_t slot = (size_t)key_hash(key) & memo->mask;
    memo->values[slot] = value;
    memo->keys[slot] = key;
}

static void initialize_problem(int n)
{
    side_size = 2U * (unsigned)n;
    const unsigned maximum_sum = 8U * (unsigned)n - 1U;
    bool prime[8 * MAX_N];
    for (unsigned value = 0; value < 8U * MAX_N; ++value) {
        prime[value] = value >= 2U;
    }
    for (unsigned divisor = 2; divisor <= maximum_sum / divisor; ++divisor) {
        if (!prime[divisor]) continue;
        for (unsigned multiple = divisor * divisor;
             multiple <= maximum_sum; multiple += divisor) {
            prime[multiple] = false;
        }
    }
    prime_count = 0;
    for (unsigned value = 3; value <= maximum_sum; value += 2U) {
        if (!prime[value]) continue;
        if (prime_count == MAX_PRIMES) die("too many prime sums for packed key");
        prime_values[prime_count++] = value;
    }
    for (unsigned odd = 0; odd < side_size; ++odd) {
        for (unsigned even = 0; even < side_size; ++even) {
            const unsigned sum = (2U * odd + 1U) + (2U * even + 2U);
            int index = -1;
            for (unsigned p = 0; p < prime_count; ++p) {
                if (prime_values[p] == sum) { index = (int)p; break; }
            }
            edge_prime[odd][even] = (int8_t)index;
        }
    }
}

static int choose_odd(StateKey key, uint32_t *available, unsigned *degree)
{
    const uint32_t odd_used = key_odd_mask(key);
    const uint32_t even_used = key_even_mask(key);
    int best = -1;
    unsigned best_degree = UINT32_MAX;
    uint32_t best_available = 0;
    for (unsigned odd = 0; odd < side_size; ++odd) {
        if ((odd_used & (UINT32_C(1) << odd)) != 0) continue;
        uint32_t candidates = 0;
        unsigned count = 0;
        for (unsigned even = 0; even < side_size; ++even) {
            if ((even_used & (UINT32_C(1) << even)) != 0 ||
                edge_prime[odd][even] < 0) continue;
            candidates |= UINT32_C(1) << even;
            ++count;
        }
        if (count < best_degree) {
            best = (int)odd;
            best_degree = count;
            best_available = candidates;
            if (count == 0) break;
        }
    }
    *available = best_available;
    *degree = best_degree;
    return best;
}

static U128 leaf_weight(StateKey key)
{
    U128 weight = 1;
    for (unsigned prime = 0; prime < prime_count; ++prime) {
        const unsigned count = key_prime_count(key, prime);
        if ((count & 1U) != 0) return 0;
        for (unsigned factor = 1; factor < count; factor += 2U) {
            U128 product;
            if (!multiply_u128_u64(weight, factor, &product)) {
                die("leaf pairing weight overflowed U128");
            }
            weight = product;
        }
    }
    return weight;
}

static U128 count_state(StateKey key, unsigned depth, unsigned odd_count,
                        Memo *memo, SearchStats *stats)
{
    if (depth == side_size) return leaf_weight(key);
    if (stats->calls == UINT64_MAX) die("call counter overflow");
    ++stats->calls;
    if ((stats->calls & UINT64_C(65535)) == 0) {
        atomic_fetch_add_explicit(&live_call_chunks, UINT64_C(65536),
                                  memory_order_relaxed);
    }
    U128 cached;
    if (memo_get(memo, key, &cached)) {
        if (stats->hits == UINT64_MAX) die("hit counter overflow");
        ++stats->hits;
        return cached;
    }

    uint32_t available;
    unsigned degree;
    const int odd = choose_odd(key, &available, &degree);
    if (odd < 0 || degree == 0) {
        if (stats->dead_ends == UINT64_MAX) die("dead-end counter overflow");
        ++stats->dead_ends;
        memo_put(memo, key, 0);
        return 0;
    }

    U128 answer = 0;
    while (available != 0) {
        const unsigned even = (unsigned)__builtin_ctz(available);
        available &= available - 1U;
        const unsigned prime = (unsigned)edge_prime[odd][even];
        const unsigned old_count = key_prime_count(key, prime);
        const unsigned next_odd_count = (old_count & 1U) == 0 ?
            odd_count + 1U : odd_count - 1U;
        const unsigned remaining = side_size - depth - 1U;
        if (next_odd_count > remaining) {
            if (stats->parity_prunes == UINT64_MAX) {
                die("parity-prune counter overflow");
            }
            ++stats->parity_prunes;
            continue;
        }
        const StateKey child = key_add_edge(
            key, (unsigned)odd, even, prime);
        const U128 addend = count_state(child, depth + 1U,
                                        next_odd_count, memo, stats);
        if (!add_u128(&answer, addend)) die("answer overflowed U128");
    }
    memo_put(memo, key, answer);
    return answer;
}

static void add_root_task(RootSchedule *schedule, uint32_t *capacity,
                          RootTask task)
{
    if (schedule->count == *capacity) {
        const uint32_t next = *capacity ? 2U * *capacity : 256U;
        if (next < *capacity ||
            (size_t)next > SIZE_MAX / sizeof(*schedule->tasks)) {
            die("root task capacity overflow");
        }
        RootTask *tasks = realloc(schedule->tasks,
                                  (size_t)next * sizeof(*tasks));
        if (tasks == NULL) die("cannot allocate root tasks");
        schedule->tasks = tasks;
        *capacity = next;
    }
    schedule->tasks[schedule->count++] = task;
}

static void expand_root(StateKey key, unsigned depth, unsigned odd_count,
                        unsigned levels, RootSchedule *schedule,
                        uint32_t *capacity)
{
    if (levels == 0 || depth == side_size) {
        add_root_task(schedule, capacity,
                      (RootTask){key, depth, odd_count});
        return;
    }
    uint32_t available;
    unsigned degree;
    const int odd = choose_odd(key, &available, &degree);
    if (odd < 0 || degree == 0) return;
    while (available != 0) {
        const unsigned even = (unsigned)__builtin_ctz(available);
        available &= available - 1U;
        const unsigned prime = (unsigned)edge_prime[odd][even];
        const unsigned old_count = key_prime_count(key, prime);
        const unsigned next_odd_count = (old_count & 1U) == 0 ?
            odd_count + 1U : odd_count - 1U;
        const unsigned remaining = side_size - depth - 1U;
        if (next_odd_count > remaining) continue;
        expand_root(key_add_edge(key, (unsigned)odd, even, prime),
                    depth + 1U, next_odd_count, levels - 1U,
                    schedule, capacity);
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
        const U128 addend = count_state(task.key, task.depth, task.odd_count,
                                        &worker->memo, &worker->stats);
        if (!add_u128(&worker->answer, addend)) die("worker answer overflow");
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
                    "397767_04: heartbeat n=%d, tasks=%u/%u, calls>=%" PRIu64
                    ", %.1f s\n", monitor->n,
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
    initialize_problem(n);
    RootSchedule schedule = {0};
    atomic_init(&schedule.next, 0U);
    uint32_t task_capacity = 0;
    const StateKey empty = {{0, 0, 0}};
    expand_root(empty, 0, 0, ROOT_SPLIT_LEVELS,
                &schedule, &task_capacity);
    if (schedule.count == 0) return 0;

    int threads = requested_threads;
    if ((uint32_t)threads > schedule.count) threads = (int)schedule.count;
    Worker *workers = calloc((size_t)threads, sizeof(*workers));
    pthread_t *ids = calloc((size_t)threads, sizeof(*ids));
    if (workers == NULL || ids == NULL) {
        free(workers); free(ids); die("cannot allocate workers");
    }
    const size_t memo_size = (size_t)1U << requested_hash_power;
    if (memo_size > SIZE_MAX / sizeof(StateKey) ||
        memo_size > SIZE_MAX / sizeof(U128)) {
        die("memo allocation size overflow");
    }
    for (int worker = 0; worker < threads; ++worker) {
        workers[worker].schedule = &schedule;
        workers[worker].memo.keys = malloc(memo_size * sizeof(StateKey));
        workers[worker].memo.values = malloc(memo_size * sizeof(U128));
        workers[worker].memo.mask = memo_size - 1U;
        if (workers[worker].memo.keys == NULL ||
            workers[worker].memo.values == NULL) {
            die("cannot allocate per-worker memo");
        }
        for (size_t slot = 0; slot < memo_size; ++slot) {
            workers[worker].memo.keys[slot].word[0] = UINT64_MAX;
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
    for (int worker = 0; worker < threads; ++worker) {
        error = pthread_create(&ids[worker], NULL, worker_main,
                               &workers[worker]);
        if (error != 0) die("cannot create worker thread");
    }

    U128 answer = 0;
    uint64_t calls = 0, hits = 0, parity_prunes = 0, dead_ends = 0;
    for (int worker = 0; worker < threads; ++worker) {
        if (pthread_join(ids[worker], NULL) != 0) die("cannot join worker");
        if (!add_u128(&answer, workers[worker].answer)) {
            die("final answer overflowed U128");
        }
        if (UINT64_MAX - calls < workers[worker].stats.calls ||
            UINT64_MAX - hits < workers[worker].stats.hits ||
            UINT64_MAX - parity_prunes < workers[worker].stats.parity_prunes ||
            UINT64_MAX - dead_ends < workers[worker].stats.dead_ends) {
            die("statistics overflow");
        }
        calls += workers[worker].stats.calls;
        hits += workers[worker].stats.hits;
        parity_prunes += workers[worker].stats.parity_prunes;
        dead_ends += workers[worker].stats.dead_ends;
    }
    if (pthread_mutex_lock(&monitor_mutex) != 0) die("monitor lock failed");
    monitor_finished = true;
    if (pthread_cond_signal(&monitor_condition) != 0) die("monitor signal failed");
    if (pthread_mutex_unlock(&monitor_mutex) != 0) die("monitor unlock failed");
    if (pthread_join(monitor_id, NULL) != 0) die("cannot join monitor");

    for (int worker = 0; worker < threads; ++worker) {
        free(workers[worker].memo.keys);
        free(workers[worker].memo.values);
    }
    free(workers);
    free(ids);
    free(schedule.tasks);

    fprintf(stderr,
            "397767_04: n=%d, weighted prime-sum permanent, primes=%u, "
            "root-tasks=%u, calls=%" PRIu64 ", hits=%" PRIu64
            ", parity-prunes=%" PRIu64 ", dead-ends=%" PRIu64
            ", threads=%d, memo=2^%u/worker (%.2f MiB total), %.3f s\n",
            n, prime_count, schedule.count, calls, hits, parity_prunes,
            dead_ends, threads, requested_hash_power,
            (double)(memo_size * (sizeof(StateKey) + sizeof(U128)) *
                     (size_t)threads) / 1048576.0,
            now_seconds() - started);
    return answer;
}

static const char *const known[MAX_N + 1] = {
    NULL, "1", "2", "24", "204", "949", "11710", "481040",
    "5803706", "292492048", "12461183880"
};

static void verify_known(int n, U128 value)
{
    if (n < MIN_N || n > MAX_N || known[n] == NULL) return;
    U128 expected;
    if (!parse_u128(known[n], &expected)) die("invalid built-in known term");
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
    bool positional = false, term_only = false, check = false;
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
            if (term_only || positional || check || ++i == argc) {
                die("invalid --term usage");
            }
            term_only = true;
            n = parse_bounded(argv[i], MIN_N, MAX_N, "N");
        } else if (strcmp(argv[i], "--check") == 0) {
            if (term_only || positional || check) die("invalid --check usage");
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
    if (check) fprintf(stderr, "397767_04: self-check passed\n");
    return EXIT_SUCCESS;
}
