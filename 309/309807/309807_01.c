/*
 * A309807 -- number of permutations sigma of [n] such that
 *
 *     sigma(k)/k > sigma(k+1)/(k+1)  (1 <= k < n).
 *
 * This program computes requested terms; known terms are verification data
 * only and are never returned instead of a calculation.
 *
 * Exact reduction used here
 * -------------------------
 * Weak permutations s of [m],
 *
 *     s(k)/k >= s(k+1)/(k+1),
 *
 * are in bijection with strict permutations of [m+1]:
 *
 *     s -> (s(1)+1, ..., s(m)+1, 1).
 *
 * Indeed, adding 1 changes each cross-multiplied difference by +1.
 * Conversely a strict permutation must end in 1 (otherwise the entry after
 * 1 would have to be a second positive integer smaller than (k+1)/k).
 * Thus a(n) is counted using weak permutations of [n-1].
 *
 * For m>1 a weak permutation is either the identity or ends in 1.  If 1 is
 * first, weak monotonicity successively forces 2,3,...; if 1 occurs at any
 * other nonfinal position, the next entry would have to be another 1.
 * Hence the identity is counted separately and the final 1 is fixed.  This
 * leaves two fewer free variables than a direct strict search.
 *
 * Safe pruning and memory use
 * ---------------------------
 * Suppose q is unassigned and the nearest assigned positions around it are
 * (l,x) and (r,y).  A completion must give q a still-unused integer v in
 *
 *     ceil(q*y/r) <= v <= floor(q*x/l),                 (1)
 *
 * with upper bound m when there is no assigned position to the left.
 * The search branches at the position with the smallest domain.
 *
 * It also tests whether all interval domains (1) have distinct
 * representatives: process intervals by increasing upper endpoint, assigning
 * each the least available value not below its lower endpoint.  This greedy
 * test is exact for interval domains.  A failure therefore proves that no
 * completion exists.  Passing is only a necessary test for the ratio
 * conditions, which are still enforced by the complete recursive search.
 *
 * The frontier is divided among POSIX threads.  No subset-DP table is stored:
 * memory is O(tasks*n + threads*n), not O(2^n).  For MAX_N=33 the weak
 * permutation size is at most 32, and 32! < 2^118, so the checked 128-bit
 * limb count is sufficient.  Time is still exponential for large n.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic -Werror -pthread \
 *       309807_01.c -o 309807_01
 *
 * Usage:
 *   ./309807_01
 *   ./309807_01 --upto 33 --threads 16
 *   ./309807_01 --term 25 --threads 16 --verbose
 *   ./309807_01 --check
 *
 * Default/--upto writes b309807_01.txt by atomic replacement.  --term and
 * --check do not change it.  Long terms report progress about once per minute.
 */

#ifdef __APPLE__
#define _DARWIN_C_SOURCE
#endif
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
#include <sys/stat.h>
#ifdef __APPLE__
#include <sys/sysctl.h>
#endif
#include <time.h>
#include <unistd.h>

#define MAX_N 33
#define MAX_M (MAX_N - 1)
#define DEFAULT_UPTO 20
#define DIRECT_CHECK_MAX_N 9
#define MAX_THREADS 256
#define TASKS_PER_THREAD 64U
#define NODE_FLUSH UINT64_C(1048576)
#define BFILE_NAME "b309807_01.txt"
#define BFILE_TEMP_TEMPLATE BFILE_NAME ".tmp.XXXXXX"
#define BFILE_LOCK_NAME BFILE_NAME ".lock"
#define COUNT_LIMBS 4U
#define DECIMAL_BASE UINT32_C(1000000000)
#define DECIMAL_CHUNKS 5U

static const uint64_t known[MAX_N + 1] = {
    UINT64_C(1), UINT64_C(1), UINT64_C(1), UINT64_C(2),
    UINT64_C(3), UINT64_C(6), UINT64_C(9), UINT64_C(19),
    UINT64_C(30), UINT64_C(60), UINT64_C(108), UINT64_C(222),
    UINT64_C(388), UINT64_C(874), UINT64_C(1601), UINT64_C(3244),
    UINT64_C(6437), UINT64_C(14056), UINT64_C(26545), UINT64_C(57326),
    UINT64_C(109333), UINT64_C(232751), UINT64_C(481137),
    UINT64_C(1002039), UINT64_C(1911740), UINT64_C(4261276),
    UINT64_C(8678424), UINT64_C(17734328), UINT64_C(36186279),
    UINT64_C(77402058), UINT64_C(154454851), UINT64_C(340848002),
    UINT64_C(691228119), UINT64_C(1460761640)
};

typedef struct { uint32_t limb[COUNT_LIMBS]; } Count;
_Static_assert(sizeof(Count) == 16U, "Count must have 128 bits");

typedef struct {
    uint64_t used;
    uint8_t remaining;
    uint8_t value[MAX_M + 1];
} State;

typedef struct {
    State *data;
    size_t length;
    size_t capacity;
} StateVector;

typedef struct {
    int position;
    uint64_t domain;
} Branch;

typedef struct {
    int n;
    int m;
    State *tasks;
    size_t task_count;
    size_t worker_count;
    atomic_size_t next_task;
    atomic_size_t finished_tasks;
    atomic_uint_fast64_t nodes;
    atomic_uint_fast64_t prunes;
    atomic_bool diagnostic_overflow;
    atomic_bool stop;
    pthread_mutex_t total_mutex;
    Count total;
    pthread_mutex_t done_mutex;
    pthread_cond_t done_condition;
    size_t active_workers;
} Pool;

typedef struct {
    uint64_t pending_nodes;
    uint64_t pending_prunes;
    Count count;
} Local;

typedef struct {
    uint64_t nodes;
    uint64_t prunes;
    size_t tasks;
    size_t bytes;
    size_t threads;
    double seconds;
    bool diagnostic_overflow;
} Statistics;

typedef enum { MODE_UPTO, MODE_TERM, MODE_CHECK } Mode;

static _Noreturn void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static double now_seconds(void)
{
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) != 0)
        die("clock_gettime failed");
    return (double)now.tv_sec + (double)now.tv_nsec / 1000000000.0;
}

static int parse_int(const char *text, const char *label, int low, int high)
{
    errno = 0;
    char *end = NULL;
    const intmax_t value = strtoimax(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' ||
        value < low || value > high) {
        fprintf(stderr, "error: %s must be in %d..%d: %s\n",
                label, low, high, text);
        exit(EXIT_FAILURE);
    }
    return (int)value;
}

static Count count_u64(uint64_t value)
{
    Count result = { { 0U, 0U, 0U, 0U } };
    result.limb[0] = (uint32_t)value;
    result.limb[1] = (uint32_t)(value >> 32);
    return result;
}

static bool count_zero(const Count *value)
{
    for (size_t i = 0U; i < COUNT_LIMBS; ++i)
        if (value->limb[i] != 0U)
            return false;
    return true;
}

static bool count_equal(const Count *a, const Count *b)
{
    return memcmp(a, b, sizeof(*a)) == 0;
}

static void count_add(Count *a, const Count *b)
{
    uint64_t carry = 0U;
    for (size_t i = 0U; i < COUNT_LIMBS; ++i) {
        const uint64_t sum = (uint64_t)a->limb[i] + b->limb[i] + carry;
        a->limb[i] = (uint32_t)sum;
        carry = sum >> 32;
    }
    if (carry != 0U)
        die("count exceeds 128 bits");
}

static void count_inc(Count *value)
{
    uint64_t carry = 1U;
    for (size_t i = 0U; i < COUNT_LIMBS && carry != 0U; ++i) {
        const uint64_t sum = (uint64_t)value->limb[i] + carry;
        value->limb[i] = (uint32_t)sum;
        carry = sum >> 32;
    }
    if (carry != 0U)
        die("count exceeds 128 bits");
}

static uint32_t count_div_base(Count *value)
{
    uint64_t remainder = 0U;
    for (size_t i = COUNT_LIMBS; i > 0U;) {
        --i;
        const uint64_t part = (remainder << 32) | value->limb[i];
        value->limb[i] = (uint32_t)(part / DECIMAL_BASE);
        remainder = part % DECIMAL_BASE;
    }
    return (uint32_t)remainder;
}

static int print_count(FILE *stream, const Count *value)
{
    if (count_zero(value))
        return fputc('0', stream) == EOF ? -1 : 0;
    Count copy = *value;
    uint32_t chunk[DECIMAL_CHUNKS] = { 0U };
    size_t used = 0U;
    while (!count_zero(&copy)) {
        if (used == DECIMAL_CHUNKS)
            die("decimal buffer is too small");
        chunk[used++] = count_div_base(&copy);
    }
    if (fprintf(stream, "%" PRIu32, chunk[used - 1U]) < 0)
        return -1;
    while (--used > 0U)
        if (fprintf(stream, "%09" PRIu32, chunk[used - 1U]) < 0)
            return -1;
    return 0;
}

static uint64_t interval_mask(int low, int high)
{
    if (low > high)
        return 0U;
    return ((UINT64_C(1) << (unsigned)(high + 1)) - 1U) &
           ~((UINT64_C(1) << (unsigned)low) - 1U);
}

/*
 * 1: branch returned, 0: complete, -1: impossible.
 * The final greedy loop is the exact matching test for interval domains.
 */
static int choose_branch(const State *state, int m, Branch *branch)
{
    if (state->remaining == 0U)
        return 0;

    uint8_t rpos[MAX_M + 1] = { 0U };
    uint8_t rval[MAX_M + 1] = { 0U };
    uint8_t low[MAX_M + 1] = { 0U };
    uint8_t high[MAX_M + 1] = { 0U };
    int next[MAX_M + 1] = { 0 };
    int head[MAX_M + 1];
    for (int h = 0; h <= m; ++h)
        head[h] = -1;

    int rp = 0, rv = 0;
    for (int q = m; q >= 1; --q) {
        if (state->value[q] != 0U) {
            rp = q;
            rv = state->value[q];
        } else {
            rpos[q] = (uint8_t)rp;
            rval[q] = (uint8_t)rv;
        }
    }

    const uint64_t all = (UINT64_C(1) << (unsigned)(m + 1)) - 1U;
    const uint64_t available = all & ~state->used;
    unsigned best = UINT32_MAX;
    int lp = 0, lv = 0;

    for (int q = 1; q < m; ++q) {
        if (state->value[q] != 0U) {
            lp = q;
            lv = state->value[q];
            continue;
        }
        const int right_p = rpos[q];
        const int right_v = rval[q];
        if (right_p == 0)
            die("invalid internal search state");
        int lo = (int)(((uint64_t)(unsigned)q * (unsigned)right_v +
                        (unsigned)right_p - 1U) / (unsigned)right_p);
        int hi = lp == 0 ? m :
            (int)(((uint64_t)(unsigned)q * (unsigned)lv) / (unsigned)lp);
        if (lo < 2)
            lo = 2;
        if (hi > m)
            hi = m;
        if (lo > hi)
            return -1;

        low[q] = (uint8_t)lo;
        high[q] = (uint8_t)hi;
        next[q] = head[hi];
        head[hi] = q;

        const uint64_t domain = available & interval_mask(lo, hi);
        const unsigned size = (unsigned)__builtin_popcountll(domain);
        if (size == 0U)
            return -1;
        if (size < best) {
            best = size;
            branch->position = q;
            branch->domain = domain;
        }
    }

    uint64_t matching_available = available;
    for (int hi = 2; hi <= m; ++hi) {
        for (int q = head[hi]; q >= 0; q = next[q]) {
            const uint64_t candidates = matching_available &
                interval_mask(low[q], high[q]);
            if (candidates == 0U)
                return -1;
            matching_available ^= candidates & (~candidates + 1U);
        }
    }
    return 1;
}

static void vector_push(StateVector *vector, const State *state)
{
    if (vector->length == vector->capacity) {
        size_t capacity = vector->capacity == 0U ? 16U :
            vector->capacity * 2U;
        if (capacity < vector->capacity ||
            capacity > SIZE_MAX / sizeof(*vector->data))
            die("task frontier size overflow");
        State *const grown = realloc(vector->data,
                                     capacity * sizeof(*grown));
        if (grown == NULL)
            die("cannot allocate task frontier");
        vector->data = grown;
        vector->capacity = capacity;
    }
    vector->data[vector->length++] = *state;
}

static StateVector make_tasks(int m, size_t thread_count, Count *early,
                              uint64_t *nodes, uint64_t *prunes)
{
    StateVector current = { NULL, 0U, 0U };
    State root;
    memset(&root, 0, sizeof(root));
    root.value[m] = 1U;
    root.used = UINT64_C(1) << 1U;
    root.remaining = (uint8_t)(m - 1);
    vector_push(&current, &root);

    if (thread_count > SIZE_MAX / TASKS_PER_THREAD)
        die("task count overflow");
    const size_t target = thread_count * TASKS_PER_THREAD;

    while (current.length != 0U && current.length < target) {
        StateVector next_level = { NULL, 0U, 0U };
        for (size_t i = 0U; i < current.length; ++i) {
            if (*nodes == UINT64_MAX)
                die("task-builder node counter overflow");
            ++*nodes;
            Branch branch = { 0, 0U };
            const int status = choose_branch(&current.data[i], m, &branch);
            if (status < 0) {
                if (*prunes == UINT64_MAX)
                    die("task-builder prune counter overflow");
                ++*prunes;
            } else if (status == 0) {
                count_inc(early);
            } else {
                uint64_t choices = branch.domain;
                while (choices != 0U) {
                    const unsigned value = (unsigned)__builtin_ctzll(choices);
                    choices &= choices - 1U;
                    State child = current.data[i];
                    child.value[branch.position] = (uint8_t)value;
                    child.used |= UINT64_C(1) << value;
                    --child.remaining;
                    vector_push(&next_level, &child);
                }
            }
        }
        free(current.data);
        current = next_level;
    }
    return current;
}

static void atomic_add_saturating(atomic_uint_fast64_t *target,
                                  uint64_t increment,
                                  atomic_bool *overflow)
{
    uint_fast64_t old = atomic_load_explicit(target, memory_order_relaxed);
    for (;;) {
        if (old == UINT_FAST64_MAX) {
            atomic_store_explicit(overflow, true, memory_order_relaxed);
            return;
        }
        const uint_fast64_t room = UINT_FAST64_MAX - old;
        const uint_fast64_t next = increment > room ? UINT_FAST64_MAX :
            old + (uint_fast64_t)increment;
        if (atomic_compare_exchange_weak_explicit(
                target, &old, next, memory_order_relaxed,
                memory_order_relaxed)) {
            if (increment > room)
                atomic_store_explicit(overflow, true, memory_order_relaxed);
            return;
        }
    }
}

static void flush_local(Pool *pool, Local *local)
{
    if (local->pending_nodes != 0U) {
        atomic_add_saturating(&pool->nodes, local->pending_nodes,
                              &pool->diagnostic_overflow);
        local->pending_nodes = 0U;
    }
    if (local->pending_prunes != 0U) {
        atomic_add_saturating(&pool->prunes, local->pending_prunes,
                              &pool->diagnostic_overflow);
        local->pending_prunes = 0U;
    }
}

static void search(Pool *pool, State *state, Local *local)
{
    ++local->pending_nodes;
    if (local->pending_nodes == NODE_FLUSH)
        flush_local(pool, local);
    if (atomic_load_explicit(&pool->stop, memory_order_relaxed))
        return;

    Branch branch = { 0, 0U };
    const int status = choose_branch(state, pool->m, &branch);
    if (status < 0) {
        ++local->pending_prunes;
        return;
    }
    if (status == 0) {
        count_inc(&local->count);
        return;
    }

    uint64_t choices = branch.domain;
    while (choices != 0U) {
        const unsigned value = (unsigned)__builtin_ctzll(choices);
        choices &= choices - 1U;
        state->value[branch.position] = (uint8_t)value;
        state->used |= UINT64_C(1) << value;
        --state->remaining;
        search(pool, state, local);
        ++state->remaining;
        state->used &= ~(UINT64_C(1) << value);
        state->value[branch.position] = 0U;
        if (atomic_load_explicit(&pool->stop, memory_order_relaxed))
            return;
    }
}

static void *worker_main(void *argument)
{
    Pool *const pool = argument;
    Local local;
    memset(&local, 0, sizeof(local));

    for (;;) {
        if (atomic_load_explicit(&pool->stop, memory_order_relaxed))
            break;
        const size_t task = atomic_fetch_add_explicit(
            &pool->next_task, 1U, memory_order_relaxed);
        if (task >= pool->task_count)
            break;

        State state = pool->tasks[task];
        search(pool, &state, &local);
        flush_local(pool, &local);

        if (pthread_mutex_lock(&pool->total_mutex) != 0)
            abort();
        count_add(&pool->total, &local.count);
        memset(&local.count, 0, sizeof(local.count));
        if (pthread_mutex_unlock(&pool->total_mutex) != 0)
            abort();
        (void)atomic_fetch_add_explicit(&pool->finished_tasks, 1U,
                                        memory_order_relaxed);
    }
    flush_local(pool, &local);

    if (pthread_mutex_lock(&pool->done_mutex) != 0)
        abort();
    if (pool->active_workers == 0U)
        abort();
    --pool->active_workers;
    if (pthread_cond_broadcast(&pool->done_condition) != 0)
        abort();
    if (pthread_mutex_unlock(&pool->done_mutex) != 0)
        abort();
    return NULL;
}

static void report_progress(Pool *pool, double start)
{
    Count partial;
    if (pthread_mutex_lock(&pool->total_mutex) != 0)
        die("cannot lock progress count");
    partial = pool->total;
    if (pthread_mutex_unlock(&pool->total_mutex) != 0)
        die("cannot unlock progress count");

    const double elapsed = now_seconds() - start;
    const uint64_t nodes = (uint64_t)atomic_load_explicit(
        &pool->nodes, memory_order_relaxed);
    const size_t done = atomic_load_explicit(&pool->finished_tasks,
                                             memory_order_relaxed);
    fprintf(stderr, "309807_01 progress: n=%d, tasks=%zu/%zu, partial=",
            pool->n, done, pool->task_count);
    if (print_count(stderr, &partial) != 0)
        die("cannot print progress count");
    fprintf(stderr,
            ", nodes=%" PRIu64 ", rate=%.2f M/s, threads=%zu, "
            "elapsed=%.1f min%s\n",
            nodes, elapsed > 0.0 ? (double)nodes / elapsed / 1000000.0 : 0.0,
            pool->worker_count, elapsed / 60.0,
            atomic_load_explicit(&pool->diagnostic_overflow,
                                 memory_order_relaxed) ?
                ", diagnostic-counter-saturated" : "");
}

static void verify_known(int n, const Count *result)
{
    const Count expected = count_u64(known[n]);
    if (!count_equal(result, &expected)) {
        fprintf(stderr, "error: computed result disagrees with the known "
                "A309807 term at n=%d\n", n);
        exit(EXIT_FAILURE);
    }
}

static Count compute_term(int n, int requested_threads, Statistics *statistics,
                          bool verbose)
{
    const double start = now_seconds();
    memset(statistics, 0, sizeof(*statistics));
    if (n <= 2) {
        const Count result = count_u64(1U);
        verify_known(n, &result);
        statistics->seconds = now_seconds() - start;
        return result;
    }

    const int m = n - 1;
    Count early = count_u64(1U); /* weak identity */
    uint64_t builder_nodes = 0U, builder_prunes = 0U;
    StateVector tasks = make_tasks(m, (size_t)requested_threads, &early,
                                   &builder_nodes, &builder_prunes);
    if (tasks.length == 0U) {
        verify_known(n, &early);
        statistics->nodes = builder_nodes;
        statistics->prunes = builder_prunes;
        statistics->seconds = now_seconds() - start;
        return early;
    }

    size_t worker_count = (size_t)requested_threads;
    if (worker_count > tasks.length)
        worker_count = tasks.length;
    if (tasks.length > SIZE_MAX / sizeof(*tasks.data))
        die("task memory size overflow");

    Pool pool;
    memset(&pool, 0, sizeof(pool));
    pool.n = n;
    pool.m = m;
    pool.tasks = tasks.data;
    pool.task_count = tasks.length;
    pool.worker_count = worker_count;
    pool.total = early;
    pool.active_workers = worker_count;
    atomic_init(&pool.next_task, 0U);
    atomic_init(&pool.finished_tasks, 0U);
    atomic_init(&pool.nodes, builder_nodes);
    atomic_init(&pool.prunes, builder_prunes);
    atomic_init(&pool.diagnostic_overflow, false);
    atomic_init(&pool.stop, false);

    bool total_mutex_ok = false, done_mutex_ok = false, condition_ok = false;
    if (pthread_mutex_init(&pool.total_mutex, NULL) == 0)
        total_mutex_ok = true;
    if (total_mutex_ok && pthread_mutex_init(&pool.done_mutex, NULL) == 0)
        done_mutex_ok = true;
    if (done_mutex_ok &&
        pthread_cond_init(&pool.done_condition, NULL) == 0)
        condition_ok = true;
    if (!condition_ok) {
        if (done_mutex_ok)
            (void)pthread_mutex_destroy(&pool.done_mutex);
        if (total_mutex_ok)
            (void)pthread_mutex_destroy(&pool.total_mutex);
        free(tasks.data);
        die("cannot initialize thread synchronization");
    }

    if (worker_count > SIZE_MAX / sizeof(pthread_t))
        die("worker array size overflow");
    pthread_t *const worker = calloc(worker_count, sizeof(*worker));
    if (worker == NULL)
        die("cannot allocate worker array");

    size_t created = 0U;
    for (; created < worker_count; ++created) {
        const int code = pthread_create(&worker[created], NULL,
                                        worker_main, &pool);
        if (code != 0) {
            atomic_store_explicit(&pool.stop, true, memory_order_relaxed);
            if (pthread_mutex_lock(&pool.done_mutex) != 0)
                abort();
            pool.active_workers -= worker_count - created;
            if (pthread_mutex_unlock(&pool.done_mutex) != 0)
                abort();
            for (size_t i = 0U; i < created; ++i)
                (void)pthread_join(worker[i], NULL);
            free(worker);
            free(tasks.data);
            errno = code;
            die("cannot create worker thread");
        }
    }

    if (pthread_mutex_lock(&pool.done_mutex) != 0)
        die("cannot lock worker completion mutex");
    while (pool.active_workers != 0U) {
        struct timespec deadline;
        if (clock_gettime(CLOCK_REALTIME, &deadline) != 0)
            die("clock_gettime failed");
        if (deadline.tv_sec > (time_t)(INT64_MAX - 60))
            die("progress deadline overflow");
        deadline.tv_sec += 60;
        const int code = pthread_cond_timedwait(
            &pool.done_condition, &pool.done_mutex, &deadline);
        if (code == ETIMEDOUT && pool.active_workers != 0U) {
            if (pthread_mutex_unlock(&pool.done_mutex) != 0)
                die("cannot unlock completion mutex");
            report_progress(&pool, start);
            if (pthread_mutex_lock(&pool.done_mutex) != 0)
                die("cannot relock completion mutex");
        } else if (code != 0) {
            errno = code;
            die("cannot wait for worker completion");
        }
    }
    if (pthread_mutex_unlock(&pool.done_mutex) != 0)
        die("cannot unlock worker completion mutex");

    for (size_t i = 0U; i < worker_count; ++i) {
        const int code = pthread_join(worker[i], NULL);
        if (code != 0) {
            errno = code;
            die("cannot join worker thread");
        }
    }

    const Count result = pool.total;
    statistics->nodes = (uint64_t)atomic_load_explicit(
        &pool.nodes, memory_order_relaxed);
    statistics->prunes = (uint64_t)atomic_load_explicit(
        &pool.prunes, memory_order_relaxed);
    statistics->tasks = tasks.length;
    statistics->bytes = tasks.length * sizeof(*tasks.data);
    statistics->threads = worker_count;
    statistics->diagnostic_overflow = atomic_load_explicit(
        &pool.diagnostic_overflow, memory_order_relaxed);
    statistics->seconds = now_seconds() - start;

    (void)pthread_cond_destroy(&pool.done_condition);
    (void)pthread_mutex_destroy(&pool.done_mutex);
    (void)pthread_mutex_destroy(&pool.total_mutex);
    free(worker);
    free(tasks.data);

    verify_known(n, &result);
    if (verbose) {
        fprintf(stderr,
                "309807_01 completed: n=%d, weak-size=%d, tasks=%zu, "
                "nodes=%" PRIu64 ", prunes=%" PRIu64 ", threads=%zu, "
                "task-memory=%.3f MiB, elapsed=%.3f s%s\n",
                n, m, statistics->tasks, statistics->nodes,
                statistics->prunes, statistics->threads,
                (double)statistics->bytes / (1024.0 * 1024.0),
                statistics->seconds,
                statistics->diagnostic_overflow ?
                    ", diagnostic counters saturated" : "");
    }
    return result;
}

/* Independent strict-definition enumeration used only by --check. */
static void brute_rec(int n, int position, int previous, uint64_t used,
                      uint64_t *count)
{
    if (position > n) {
        if (*count == UINT64_MAX)
            die("direct-check counter overflow");
        ++*count;
        return;
    }
    for (int value = 1; value <= n; ++value) {
        const uint64_t bit = UINT64_C(1) << (unsigned)value;
        if ((used & bit) != 0U)
            continue;
        if (position > 1 &&
            (uint64_t)(unsigned)position * (unsigned)previous <=
            (uint64_t)(unsigned)(position - 1) * (unsigned)value)
            continue;
        brute_rec(n, position + 1, value, used | bit, count);
    }
}

static Count brute_count(int n)
{
    if (n == 0)
        return count_u64(1U);
    uint64_t result = 0U;
    brute_rec(n, 1, 0, 0U, &result);
    return count_u64(result);
}

static void run_check(int threads, bool verbose)
{
    for (int n = 0; n <= DIRECT_CHECK_MAX_N; ++n) {
        Statistics statistics;
        const Count optimized = compute_term(n, threads, &statistics, verbose);
        const Count direct = brute_count(n);
        if (!count_equal(&optimized, &direct)) {
            fprintf(stderr, "error: optimized weak search and independent "
                    "strict enumeration disagree at n=%d\n", n);
            exit(EXIT_FAILURE);
        }
    }
    fprintf(stderr, "check passed: optimized weak search equals independent "
            "strict enumeration for n=0..%d; every result also matches the "
            "known A309807 term\n", DIRECT_CHECK_MAX_N);
}

static int acquire_bfile_lock(void)
{
    const int descriptor = open(BFILE_LOCK_NAME, O_RDWR | O_CREAT, 0666);
    if (descriptor < 0)
        die("cannot open b-file lock");
    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    if (fcntl(descriptor, F_SETLKW, &lock) < 0) {
        const int saved = errno;
        (void)close(descriptor);
        errno = saved;
        die("cannot acquire b-file lock");
    }
    return descriptor;
}

static void write_bfile(const Count *terms, int upto)
{
    const int lock_descriptor = acquire_bfile_lock();
    char temporary[] = BFILE_TEMP_TEMPLATE;
    const int descriptor = mkstemp(temporary);
    if (descriptor < 0) {
        (void)close(lock_descriptor);
        die("cannot create temporary b-file");
    }

    const mode_t old_mask = umask(0);
    (void)umask(old_mask);
    if (fchmod(descriptor, (mode_t)(0666 & ~old_mask)) != 0) {
        const int saved = errno;
        (void)close(descriptor);
        (void)unlink(temporary);
        (void)close(lock_descriptor);
        errno = saved;
        die("cannot set temporary b-file permissions");
    }
    FILE *const stream = fdopen(descriptor, "w");
    if (stream == NULL) {
        const int saved = errno;
        (void)close(descriptor);
        (void)unlink(temporary);
        (void)close(lock_descriptor);
        errno = saved;
        die("cannot open temporary b-file stream");
    }

    bool failed = false;
    for (int n = 0; n <= upto && !failed; ++n) {
        if (fprintf(stream, "%d ", n) < 0 ||
            print_count(stream, &terms[n]) != 0 ||
            fputc('\n', stream) == EOF)
            failed = true;
    }
    if (!failed && fflush(stream) != 0)
        failed = true;
    if (!failed && fsync(descriptor) != 0)
        failed = true;
    if (fclose(stream) != 0)
        failed = true;

    if (failed) {
        const int saved = errno;
        (void)unlink(temporary);
        (void)close(lock_descriptor);
        errno = saved;
        die("cannot write temporary b-file");
    }
    if (rename(temporary, BFILE_NAME) != 0) {
        const int saved = errno;
        (void)unlink(temporary);
        (void)close(lock_descriptor);
        errno = saved;
        die("cannot replace b-file");
    }
    if (close(lock_descriptor) != 0)
        die("cannot close b-file lock");
}

static int default_threads(void)
{
#ifdef __APPLE__
    int detected = 0;
    size_t size = sizeof(detected);
    if (sysctlbyname("hw.logicalcpu", &detected, &size, NULL, 0) == 0 &&
        detected > 0)
        return detected > MAX_THREADS ? MAX_THREADS : detected;
#elif defined(_SC_NPROCESSORS_ONLN)
    const long detected = sysconf(_SC_NPROCESSORS_ONLN);
    if (detected > 0)
        return detected > MAX_THREADS ? MAX_THREADS : (int)detected;
#endif
    return 1;
}

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s [--upto N | --term N | --check] "
            "[--threads T] [--verbose]\n"
            "       N must be in 0..%d; T must be in 1..%d; "
            "default --upto %d\n",
            program, MAX_N, MAX_THREADS, DEFAULT_UPTO);
}

int main(int argc, char **argv)
{
    Mode mode = MODE_UPTO;
    int requested = DEFAULT_UPTO;
    int threads = default_threads();
    bool verbose = false;
    bool mode_seen = false;

    for (int arg = 1; arg < argc; ++arg) {
        if (strcmp(argv[arg], "--upto") == 0 ||
            strcmp(argv[arg], "--term") == 0) {
            if (mode_seen || arg + 1 >= argc) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            mode = strcmp(argv[arg], "--upto") == 0 ?
                MODE_UPTO : MODE_TERM;
            requested = parse_int(argv[++arg], "n", 0, MAX_N);
            mode_seen = true;
        } else if (strcmp(argv[arg], "--check") == 0) {
            if (mode_seen) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            mode = MODE_CHECK;
            mode_seen = true;
        } else if (strcmp(argv[arg], "--threads") == 0) {
            if (arg + 1 >= argc) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            threads = parse_int(argv[++arg], "threads", 1, MAX_THREADS);
        } else if (strcmp(argv[arg], "--verbose") == 0) {
            verbose = true;
        } else if (strcmp(argv[arg], "--help") == 0) {
            usage(argv[0]);
            return EXIT_SUCCESS;
        } else {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (mode == MODE_CHECK) {
        run_check(threads, verbose);
        return EXIT_SUCCESS;
    }
    if (mode == MODE_TERM) {
        Statistics statistics;
        const Count result = compute_term(requested, threads, &statistics,
                                          verbose);
        if (print_count(stdout, &result) != 0 || fputc('\n', stdout) == EOF ||
            fflush(stdout) != 0)
            die("cannot write standard output");
        return EXIT_SUCCESS;
    }

    const size_t term_count = (size_t)requested + 1U;
    if (term_count > SIZE_MAX / sizeof(Count))
        die("term array size overflow");
    Count *const terms = calloc(term_count, sizeof(*terms));
    if (terms == NULL)
        die("cannot allocate term array");

    for (int n = 0; n <= requested; ++n) {
        Statistics statistics;
        terms[n] = compute_term(n, threads, &statistics, verbose);
        if (fprintf(stdout, "%d ", n) < 0 ||
            print_count(stdout, &terms[n]) != 0 ||
            fputc('\n', stdout) == EOF || fflush(stdout) != 0) {
            free(terms);
            die("cannot write standard output");
        }
    }
    write_bfile(terms, requested);
    free(terms);
    return EXIT_SUCCESS;
}
