/*
 * A332800 -- bounded-memory depth-first dynamic programming.
 *
 * Count permutations sigma of [n] for which
 *
 *   sigma(k) mod sigma(k+1) <= sigma(k+1) mod sigma(k+2).
 *
 * For a used set S ending in x with previous remainder r, let C(S,x,r)
 * count valid completions.  Appending y outside S is allowed exactly when
 *
 *   r <= x mod y,
 *
 * and gives C(S union {y}, y, x mod y).  A singleton starts with r=0;
 * this imposes no restriction because remainders are nonnegative.
 *
 * This program evaluates that exact recurrence depth first.  Independent
 * pthread workers dynamically take the n top-level first-value contributions
 * and share one fixed-size cache.  Cache entries use atomic double-checked
 * reads and nonblocking compare-and-swap publication.  Each published key
 * carries a generation tag, preventing an A->B->A replacement from fooling a
 * reader; a slot is frozen instead of allowing its generation to wrap.  A
 * writer that loses a race simply skips that cache store, and a later miss
 * merely recomputes the state.  Cached counts use 32 bits; a larger exact
 * count is not truncated but simply left uncached.  Thus --threads changes
 * neither the
 * cache budget nor correctness, cache capacity affects time but never
 * correctness, and memory does not grow with the number of reached states.
 * --no-cache gives the same recurrence using only O(n) stack per worker.
 *
 * Put h=floor(n/2).  If a mod b > h, then a<b and a mod b=a>h.  Weak increase
 * forces every later value to be larger than its predecessor.  Hence, when
 * x>h and every unused value is greater than x, their increasing order is the
 * unique completion.  If some unused value is at most x, an appended value
 * greater than x would start such a high-remainder suffix while leaving that
 * smaller value unusable, so those candidates are discarded.  This is an
 * exact shortcut, not heuristic pruning.  Primality supplies no analogous
 * restriction and is not used.
 *
 * Known OEIS values are verification data only and are never returned instead
 * of a computed value.  --check also compares with direct permutation
 * enumeration and with a cache-free run.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic -pthread \
 *       332800_04.c -o 332800_04
 *
 * Usage:
 *   ./332800_04
 *   ./332800_04 --upto 22 --memory-mb 256 --verbose
 *   ./332800_04 --term 32 --memory-mb 1024 --threads 16 --verbose
 *   ./332800_04 --term 15 --no-cache --verbose
 *   ./332800_04 --term 32 --progress-seconds 60
 *   ./332800_04 --check
 *
 * The default and --upto print and flush each completed "n a(n)" line and
 * atomically checkpoint b332800_04.txt after every completed term.  --term
 * and --check do not modify the b-file.
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
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define MAX_N 32
#define DEFAULT_MAX_N 22
#define KNOWN_MAX_N 32
#define CHECK_MAX_N 17
#define DIRECT_CHECK_MAX_N 10
#define CACHE_FREE_CHECK_MAX_N 12
#define DEFAULT_MEMORY_MB 256
#define MIN_MEMORY_MB 1
#define MAX_MEMORY_MB 65536
#ifndef DEFAULT_THREADS
#define DEFAULT_THREADS 16
#endif
#define MAX_THREADS 32
#define KEY_AUX_BITS 10
#define CACHE_WAYS 2
#define CACHE_PAYLOAD_BITS (MAX_N + KEY_AUX_BITS + 1)
#define CACHE_GENERATION_BITS (63 - CACHE_PAYLOAD_BITS)
#define CACHE_PAYLOAD_MASK ((UINT64_C(1) << CACHE_PAYLOAD_BITS) - 1U)
#define CACHE_GENERATION_MAX \
    ((UINT64_C(1) << CACHE_GENERATION_BITS) - 1U)
#define CACHE_BUSY_BIT (UINT64_C(1) << 63)
#if defined(PACKED_CACHE)
#define PACKED_COUNT_BITS (64 - CACHE_PAYLOAD_BITS)
#define PACKED_COUNT_MAX ((UINT64_C(1) << PACKED_COUNT_BITS) - 1U)
#endif
#ifndef PROGRAM_LABEL
#define PROGRAM_LABEL "332800_04"
#endif
#ifndef BFILE_NAME
#define BFILE_NAME "b332800_04.txt"
#endif
#ifndef BFILE_TEMP_TEMPLATE
#define BFILE_TEMP_TEMPLATE BFILE_NAME ".tmp.XXXXXX"
#endif
#ifndef BFILE_LOCK_NAME
#define BFILE_LOCK_NAME BFILE_NAME ".lock"
#endif

typedef uint64_t mask_t;

_Static_assert(MAX_N <= 32, "the state mask reserves 32 bits");
_Static_assert(2 * 5 <= KEY_AUX_BITS,
               "the state key needs five bits each for last and remainder");
_Static_assert(CACHE_PAYLOAD_BITS < 63,
               "tagged cache keys need generation bits below bit 63");
_Static_assert(CACHE_GENERATION_BITS >= 16,
               "cache generation counter is unexpectedly small");
#if defined(PACKED_CACHE)
_Static_assert(PACKED_COUNT_BITS >= 20,
               "packed cache count field is unexpectedly small");
#endif
_Static_assert((CACHE_WAYS & (CACHE_WAYS - 1)) == 0,
               "cache ways must be a power of two");

static const uint64_t known[KNOWN_MAX_N + 1] = {
    UINT64_C(1),            UINT64_C(1),            UINT64_C(2),
    UINT64_C(4),            UINT64_C(9),            UINT64_C(21),
    UINT64_C(44),           UINT64_C(109),          UINT64_C(241),
    UINT64_C(530),          UINT64_C(1176),         UINT64_C(3180),
    UINT64_C(6456),         UINT64_C(14835),        UINT64_C(34672),
    UINT64_C(81877),        UINT64_C(179434),       UINT64_C(479275),
    UINT64_C(977224),       UINT64_C(2503363),      UINT64_C(5339049),
    UINT64_C(11207391),     UINT64_C(28379591),     UINT64_C(82473713),
    UINT64_C(166689486),    UINT64_C(370775384),    UINT64_C(877910547),
    UINT64_C(2150475950),   UINT64_C(4608590865),   UINT64_C(12146671367),
    UINT64_C(24620749285),  UINT64_C(64137229920),  UINT64_C(143062854926)
};

#if defined(PACKED_CACHE)
typedef struct {
    _Atomic uint64_t word[CACHE_WAYS];
} CacheBucket;

_Static_assert(sizeof(CacheBucket) == 16, "unexpected packed bucket size");
#else
typedef struct {
    _Atomic uint64_t key_plus_one[CACHE_WAYS];
    _Atomic uint32_t count[CACHE_WAYS];
} CacheBucket;

_Static_assert(sizeof(CacheBucket) == 24, "unexpected cache bucket size");
#endif

typedef struct {
    CacheBucket *bucket;
    size_t bucket_count;
    size_t bytes;
    bool enabled;
} Cache;

typedef struct {
    uint64_t calls;
    uint64_t transitions;
    uint64_t cache_lookups;
    uint64_t cache_hits;
    uint64_t cache_stores;
    uint64_t cache_replacements;
    uint64_t shortcut_completions;
    uint64_t shortcut_rejections;
    unsigned maximum_depth;
    unsigned threads;
    size_t cache_bytes;
} Statistics;

typedef struct ParallelTask ParallelTask;

typedef struct {
    int n;
    unsigned half;
    mask_t universe;
    uint8_t remainder[MAX_N + 1][MAX_N + 1];
    mask_t equal_mask[MAX_N + 1][MAX_N];
    mask_t at_least_mask[MAX_N + 1][MAX_N];
    Cache *cache;
    Statistics statistics;
    ParallelTask *task;
    uint64_t published_calls;
    uint64_t published_cache_hits;
    uint64_t published_cache_lookups;
} Search;

struct ParallelTask {
    int n;
    unsigned worker_count;
    unsigned progress_seconds;
    double progress_started;
    double next_progress;
    pthread_mutex_t progress_mutex;
    _Atomic unsigned next_root;
    _Atomic unsigned completed_roots;
    _Atomic uint64_t partial_sum;
    _Atomic uint64_t published_calls;
    _Atomic uint64_t published_cache_hits;
    _Atomic uint64_t published_cache_lookups;
    Cache cache;
    uint64_t root_values[MAX_N];
};

typedef struct {
    Search search;
    ParallelTask *task;
} Worker;

typedef enum { MODE_UPTO, MODE_TERM } OutputMode;

static char bfile_temp_name[] = BFILE_TEMP_TEMPLATE;
static int bfile_lock_descriptor = -1;
static bool bfile_temp_active = false;

static _Noreturn void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static double monotonic_seconds(void)
{
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) != 0)
        die("clock_gettime failed");
    return (double)now.tv_sec + (double)now.tv_nsec / 1000000000.0;
}

static int parse_integer(const char *text, const char *label,
                         int minimum, int maximum)
{
    errno = 0;
    char *end = NULL;
    intmax_t value = strtoimax(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' ||
        value < minimum || value > maximum) {
        fprintf(stderr, "error: %s must be in %d..%d: %s\n",
                label, minimum, maximum, text);
        exit(EXIT_FAILURE);
    }
    return (int)value;
}

static void increment_statistic(uint64_t *value, const char *message)
{
    if (*value == UINT64_MAX) die(message);
    ++*value;
}

static uint64_t hash_u64(uint64_t value)
{
    value ^= value >> 30;
    value *= UINT64_C(0xbf58476d1ce4e5b9);
    value ^= value >> 27;
    value *= UINT64_C(0x94d049bb133111eb);
    return value ^ (value >> 31);
}

static size_t cache_bucket_index(uint64_t hash, size_t bucket_count)
{
#if defined(__SIZEOF_INT128__) && SIZE_MAX == UINT64_MAX
    __extension__ typedef unsigned __int128 uint128_t;
    return (size_t)(((uint128_t)hash * (uint128_t)bucket_count) >> 64);
#else
    return (size_t)(hash % (uint64_t)bucket_count);
#endif
}

static int first_value(mask_t mask)
{
#if defined(__clang__) || defined(__GNUC__)
    return (int)__builtin_ctzll(mask) + 1;
#else
    int value = 1;
    while ((mask & 1U) == 0U) { mask >>= 1U; ++value; }
    return value;
#endif
}

static mask_t value_bit(int value)
{
    return UINT64_C(1) << (value - 1);
}

static mask_t full_mask(int n)
{
    return n == 0 ? 0U : (UINT64_C(1) << n) - 1U;
}

static bool all_values_greater(mask_t values, int boundary)
{
    return (values & full_mask(boundary)) == 0U;
}

static uint64_t pack_key(mask_t used, int last, unsigned remainder)
{
    if (last < 1 || last > MAX_N || remainder >= MAX_N)
        die("cache key component out of range");
    if (used > UINT64_MAX >> KEY_AUX_BITS)
        die("cache key mask overflow");
    uint64_t key = (used << KEY_AUX_BITS) |
                   ((uint64_t)(unsigned)(last - 1) << 5) | remainder;
    if (key == UINT64_MAX) die("cache key overflow");
    return key;
}

static void cache_init(Cache *cache, size_t memory_limit, bool enabled)
{
    memset(cache, 0, sizeof(*cache));
    cache->enabled = enabled;
    if (!enabled) return;

    size_t bucket_count = memory_limit / sizeof(CacheBucket);
    if (bucket_count == 0U || bucket_count > SIZE_MAX / sizeof(CacheBucket))
        die("cache size overflow");
    size_t bytes = bucket_count * sizeof(CacheBucket);
    cache->bucket = calloc(bucket_count, sizeof(*cache->bucket));
    if (cache->bucket == NULL) {
        fprintf(stderr, "error: could not allocate %.1f MiB cache\n",
                (double)bytes / 1048576.0);
        exit(EXIT_FAILURE);
    }
    cache->bucket_count = bucket_count;
    cache->bytes = bytes;
#if defined(PACKED_CACHE)
    if (!atomic_is_lock_free(&cache->bucket[0].word[0]))
#else
    if (!atomic_is_lock_free(&cache->bucket[0].key_plus_one[0]) ||
        !atomic_is_lock_free(&cache->bucket[0].count[0]))
#endif
        die("cache atomics are not lock-free on this platform");
}

static void cache_destroy(Cache *cache)
{
    free(cache->bucket);
    memset(cache, 0, sizeof(*cache));
}

static bool cache_lookup(Search *search, uint64_t key, uint64_t hash,
                         uint64_t *count)
{
    Cache *cache = search->cache;
    if (!cache->enabled) return false;
    increment_statistic(&search->statistics.cache_lookups,
                        "cache lookup statistics overflow");
    uint64_t stored = key + 1U;
    if ((stored & ~CACHE_PAYLOAD_MASK) != 0U)
        die("cache key payload overflow");
    CacheBucket *bucket = &cache->bucket[
        cache_bucket_index(hash, cache->bucket_count)];
#if defined(PACKED_CACHE)
    for (unsigned way = 0; way < CACHE_WAYS; ++way) {
        uint64_t word = atomic_load_explicit(&bucket->word[way],
                                              memory_order_acquire);
        if ((word & CACHE_PAYLOAD_MASK) == stored) {
            increment_statistic(&search->statistics.cache_hits,
                                "cache hit statistics overflow");
            *count = word >> CACHE_PAYLOAD_BITS;
            return true;
        }
    }
#else
    for (unsigned way = 0; way < CACHE_WAYS; ++way) {
        uint64_t first_key = atomic_load_explicit(
            &bucket->key_plus_one[way], memory_order_acquire);
        if ((first_key & CACHE_BUSY_BIT) == 0U &&
            (first_key & CACHE_PAYLOAD_MASK) == stored) {
            uint32_t value = atomic_load_explicit(&bucket->count[way],
                                                   memory_order_relaxed);
            uint64_t second_key = atomic_load_explicit(
                &bucket->key_plus_one[way], memory_order_acquire);
            if (second_key != first_key) continue;
            increment_statistic(&search->statistics.cache_hits,
                                "cache hit statistics overflow");
            *count = value;
            return true;
        }
    }
#endif
    return false;
}

static void cache_store(Search *search, uint64_t key, uint64_t hash,
                        uint64_t count)
{
    Cache *cache = search->cache;
    if (!cache->enabled) return;
#if defined(PACKED_CACHE)
    if (count > PACKED_COUNT_MAX) return;
#else
    if (count > UINT32_MAX) return;
#endif
    uint64_t stored = key + 1U;
    if ((stored & ~CACHE_PAYLOAD_MASK) != 0U)
        die("cache key payload overflow");
    size_t bucket_index = cache_bucket_index(hash, cache->bucket_count);
    CacheBucket *bucket = &cache->bucket[bucket_index];
#if defined(PACKED_CACHE)
    uint64_t observed[CACHE_WAYS];
    unsigned selected = CACHE_WAYS;
    for (unsigned way = 0; way < CACHE_WAYS; ++way) {
        observed[way] = atomic_load_explicit(&bucket->word[way],
                                              memory_order_acquire);
        if ((observed[way] & CACHE_PAYLOAD_MASK) == stored) return;
        if (selected == CACHE_WAYS && observed[way] == 0U)
            selected = way;
    }
    if (selected == CACHE_WAYS)
        selected = (unsigned)((hash >> 32) & (CACHE_WAYS - 1U));
    uint64_t word = stored | (count << CACHE_PAYLOAD_BITS);
    atomic_store_explicit(&bucket->word[selected], word,
                          memory_order_release);
    if (observed[selected] != 0U)
        increment_statistic(&search->statistics.cache_replacements,
                            "cache replacement statistics overflow");
    increment_statistic(&search->statistics.cache_stores,
                        "cache store statistics overflow");
#else
    uint64_t observed[CACHE_WAYS];
    unsigned selected = CACHE_WAYS;
    for (unsigned way = 0; way < CACHE_WAYS; ++way) {
        observed[way] = atomic_load_explicit(
            &bucket->key_plus_one[way], memory_order_acquire);
        if ((observed[way] & CACHE_PAYLOAD_MASK) == stored) return;
        if (selected == CACHE_WAYS && observed[way] == 0U)
            selected = way;
    }
    if (selected == CACHE_WAYS) {
        selected = (unsigned)((hash >> 32) & (CACHE_WAYS - 1U));
        if ((observed[selected] & CACHE_BUSY_BIT) != 0U) return;
    }

    uint64_t generation =
        (observed[selected] >> CACHE_PAYLOAD_BITS) & CACHE_GENERATION_MAX;
    if (generation == CACHE_GENERATION_MAX) return;
    ++generation;
    uint64_t tagged = stored | (generation << CACHE_PAYLOAD_BITS);
    uint64_t expected = observed[selected];
    uint64_t busy = tagged | CACHE_BUSY_BIT;
    if (!atomic_compare_exchange_strong_explicit(
            &bucket->key_plus_one[selected], &expected, busy,
            memory_order_acq_rel, memory_order_acquire))
        return;
    if (observed[selected] != 0U)
        increment_statistic(&search->statistics.cache_replacements,
                            "cache replacement statistics overflow");
    atomic_store_explicit(&bucket->count[selected], (uint32_t)count,
                          memory_order_relaxed);
    atomic_store_explicit(&bucket->key_plus_one[selected], tagged,
                          memory_order_release);
    increment_statistic(&search->statistics.cache_stores,
                        "cache store statistics overflow");
#endif
}

static void atomic_add_counter(_Atomic uint64_t *destination, uint64_t add,
                               const char *message)
{
    uint64_t old = atomic_load_explicit(destination, memory_order_relaxed);
    for (;;) {
        if (UINT64_MAX - old < add) die(message);
        if (atomic_compare_exchange_weak_explicit(
                destination, &old, old + add,
                memory_order_relaxed, memory_order_relaxed))
            return;
    }
}

static void publish_worker_statistics(Search *search)
{
    ParallelTask *task = search->task;
    uint64_t calls = search->statistics.calls - search->published_calls;
    uint64_t hits = search->statistics.cache_hits -
                    search->published_cache_hits;
    uint64_t lookups = search->statistics.cache_lookups -
                       search->published_cache_lookups;
    if (calls != 0U)
        atomic_add_counter(&task->published_calls, calls,
                           "published call statistics overflow");
    if (hits != 0U)
        atomic_add_counter(&task->published_cache_hits, hits,
                           "published cache hit statistics overflow");
    if (lookups != 0U)
        atomic_add_counter(&task->published_cache_lookups, lookups,
                           "published cache lookup statistics overflow");
    search->published_calls = search->statistics.calls;
    search->published_cache_hits = search->statistics.cache_hits;
    search->published_cache_lookups = search->statistics.cache_lookups;
}

static void maybe_print_progress(Search *search, bool force_check)
{
    ParallelTask *task = search->task;
    if (!force_check &&
        (search->statistics.calls & UINT64_C(0xfffff)) != 0U)
        return;
    publish_worker_statistics(search);
    if (task->progress_seconds == 0U) return;

    double now = monotonic_seconds();
    int error = pthread_mutex_lock(&task->progress_mutex);
    if (error != 0) {
        fprintf(stderr, "error: pthread_mutex_lock: %s\n", strerror(error));
        exit(EXIT_FAILURE);
    }
    if (now >= task->next_progress) {
        double elapsed = now - task->progress_started;
        uint64_t calls = atomic_load_explicit(&task->published_calls,
                                               memory_order_relaxed);
        uint64_t hits = atomic_load_explicit(&task->published_cache_hits,
                                              memory_order_relaxed);
        uint64_t lookups = atomic_load_explicit(
            &task->published_cache_lookups, memory_order_relaxed);
        unsigned completed = atomic_load_explicit(&task->completed_roots,
                                                   memory_order_relaxed);
        unsigned assigned = atomic_load_explicit(&task->next_root,
                                                  memory_order_relaxed);
        if (assigned > (unsigned)task->n) assigned = (unsigned)task->n;
        uint64_t partial = atomic_load_explicit(&task->partial_sum,
                                                 memory_order_relaxed);
        double rate = elapsed > 0.0 ? (double)calls / elapsed : 0.0;
        double hit_rate = lookups == 0U ? 0.0 :
            100.0 * (double)hits / (double)lookups;
        if (fprintf(stderr,
                    PROGRAM_LABEL
                    " progress: n=%d, roots=%u/%d, assigned=%u/%d, "
                    "partial=%" PRIu64 ", calls=%" PRIu64
                    ", rate=%.2f M/s, cache_hit=%.1f%%, threads=%u, "
                    "elapsed=%.1f min\n",
                    task->n, completed, task->n, assigned, task->n, partial,
                    calls, rate / 1000000.0, hit_rate, task->worker_count,
                    elapsed / 60.0) < 0 || fflush(stderr) != 0)
            die("cannot write progress output");
        do {
            task->next_progress += (double)task->progress_seconds;
        } while (task->next_progress <= now);
    }
    error = pthread_mutex_unlock(&task->progress_mutex);
    if (error != 0) {
        fprintf(stderr, "error: pthread_mutex_unlock: %s\n", strerror(error));
        exit(EXIT_FAILURE);
    }
}

static void atomic_add_partial(ParallelTask *task, uint64_t add)
{
    atomic_add_counter(&task->partial_sum, add,
                       "progress partial sum exceeds uint64_t");
}

static uint64_t count_completions(Search *search, mask_t used, int last,
                                  unsigned minimum_remainder,
                                  unsigned depth)
{
    increment_statistic(&search->statistics.calls,
                        "call statistics overflow");
    maybe_print_progress(search, false);
    if (search->statistics.maximum_depth < depth)
        search->statistics.maximum_depth = depth;

    mask_t last_bit = value_bit(last);
    if ((used & last_bit) == 0U) die("state does not contain its last value");
    mask_t unused = search->universe ^ used;
    if (unused == 0U) return 1U;
    if (minimum_remainder >= (unsigned)last)
        die("previous remainder is not below the current last value");

    mask_t available = unused;
    if ((unsigned)last > search->half) {
        if (all_values_greater(unused, last)) {
            increment_statistic(&search->statistics.shortcut_completions,
                                "shortcut statistics overflow");
            return 1U;
        }
        /* A larger next value would start an impossible high suffix. */
        mask_t rejected = available & ~full_mask(last);
        if (rejected != 0U)
            increment_statistic(&search->statistics.shortcut_rejections,
                                "shortcut statistics overflow");
        available &= full_mask(last);
    }

    while (minimum_remainder < MAX_N &&
           (available & search->equal_mask[last][minimum_remainder]) == 0U)
        ++minimum_remainder;
    if (minimum_remainder >= MAX_N) return 0U;
    mask_t candidates = available &
                        search->at_least_mask[last][minimum_remainder];
    if (candidates == 0U) return 0U;

    uint64_t key = pack_key(used, last, minimum_remainder);
    uint64_t hash = hash_u64(key);
    uint64_t cached;
    if (cache_lookup(search, key, hash, &cached)) return cached;

    uint64_t total = 0U;
    while (candidates != 0U) {
        int appended = first_value(candidates);
        candidates &= candidates - 1U;
        increment_statistic(&search->statistics.transitions,
                            "transition statistics overflow");
        unsigned next_remainder = search->remainder[last][appended];
        if (next_remainder < minimum_remainder)
            die("eligibility mask admitted an invalid transition");
        if (next_remainder > search->half)
            die("high-remainder transition escaped the shortcut");
        uint64_t add = count_completions(search, used | value_bit(appended),
                                         appended, next_remainder, depth + 1U);
        if (UINT64_MAX - total < add) die("completion count exceeds uint64_t");
        total += add;
    }

    cache_store(search, key, hash, total);
    return total;
}

static void search_init(Search *search, int n, ParallelTask *task)
{
    memset(search, 0, sizeof(*search));
    search->n = n;
    search->half = (unsigned)n / 2U;
    search->universe = full_mask(n);
    search->task = task;
    search->cache = &task->cache;

    for (int last = 1; last <= n; ++last) {
        for (int appended = 1; appended <= n; ++appended) {
            unsigned r = (unsigned)(last % appended);
            search->remainder[last][appended] = (uint8_t)r;
            search->equal_mask[last][r] |= value_bit(appended);
        }
        for (unsigned threshold = 0; threshold < MAX_N; ++threshold) {
            mask_t eligible = 0U;
            for (int appended = 1; appended <= n; ++appended)
                if (search->remainder[last][appended] >= threshold)
                    eligible |= value_bit(appended);
            search->at_least_mask[last][threshold] = eligible;
        }
    }
}

static void *worker_main(void *argument)
{
    Worker *worker = argument;
    ParallelTask *task = worker->task;
    for (;;) {
        unsigned root = atomic_fetch_add_explicit(&task->next_root, 1U,
                                                   memory_order_relaxed);
        if (root >= (unsigned)task->n) break;
        int first = (int)root + 1;
        uint64_t value = count_completions(&worker->search, value_bit(first),
                                           first, 0U, 1U);
        task->root_values[root] = value;
        atomic_add_partial(task, value);
        (void)atomic_fetch_add_explicit(&task->completed_roots, 1U,
                                        memory_order_relaxed);
        maybe_print_progress(&worker->search, true);
    }
    publish_worker_statistics(&worker->search);
    return NULL;
}

static void add_statistic(uint64_t *destination, uint64_t source,
                          const char *message)
{
    if (UINT64_MAX - *destination < source) die(message);
    *destination += source;
}

static uint64_t compute_term(int n, size_t memory_limit, bool use_cache,
                             unsigned progress_seconds,
                             unsigned requested_threads,
                             Statistics *statistics)
{
    memset(statistics, 0, sizeof(*statistics));
    if (n == 0) return 1U;

    unsigned worker_count = requested_threads;
    if (worker_count > (unsigned)n) worker_count = (unsigned)n;
    if (worker_count == 0U) worker_count = 1U;
    statistics->threads = worker_count;
    ParallelTask task;
    memset(&task, 0, sizeof(task));
    task.n = n;
    task.worker_count = worker_count;
    task.progress_seconds = progress_seconds;
    atomic_init(&task.next_root, 0U);
    atomic_init(&task.completed_roots, 0U);
    atomic_init(&task.partial_sum, 0U);
    atomic_init(&task.published_calls, 0U);
    atomic_init(&task.published_cache_hits, 0U);
    atomic_init(&task.published_cache_lookups, 0U);
    cache_init(&task.cache, memory_limit, use_cache);
    int error = pthread_mutex_init(&task.progress_mutex, NULL);
    if (error != 0) {
        fprintf(stderr, "error: pthread_mutex_init: %s\n", strerror(error));
        exit(EXIT_FAILURE);
    }

    Worker workers[MAX_THREADS];
    pthread_t threads[MAX_THREADS - 1];
    memset(workers, 0, sizeof(workers));
    for (unsigned i = 0; i < worker_count; ++i) {
        workers[i].task = &task;
        search_init(&workers[i].search, n, &task);
    }
    task.progress_started = monotonic_seconds();
    task.next_progress = task.progress_started + (double)progress_seconds;

    for (unsigned i = 1; i < worker_count; ++i) {
        error = pthread_create(&threads[i - 1], NULL, worker_main,
                               &workers[i]);
        if (error != 0) {
            fprintf(stderr, "error: pthread_create: %s\n", strerror(error));
            exit(EXIT_FAILURE);
        }
    }
    (void)worker_main(&workers[0]);
    for (unsigned i = 1; i < worker_count; ++i) {
        error = pthread_join(threads[i - 1], NULL);
        if (error != 0) {
            fprintf(stderr, "error: pthread_join: %s\n", strerror(error));
            exit(EXIT_FAILURE);
        }
    }
    uint64_t answer = 0U;
    for (int root = 0; root < n; ++root) {
        if (UINT64_MAX - answer < task.root_values[root])
            die("A332800 term exceeds uint64_t");
        answer += task.root_values[root];
    }
    for (unsigned i = 0; i < worker_count; ++i) {
        Statistics *source = &workers[i].search.statistics;
        add_statistic(&statistics->calls, source->calls,
                      "call statistics overflow");
        add_statistic(&statistics->transitions, source->transitions,
                      "transition statistics overflow");
        add_statistic(&statistics->cache_lookups, source->cache_lookups,
                      "cache lookup statistics overflow");
        add_statistic(&statistics->cache_hits, source->cache_hits,
                      "cache hit statistics overflow");
        add_statistic(&statistics->cache_stores, source->cache_stores,
                      "cache store statistics overflow");
        add_statistic(&statistics->cache_replacements,
                      source->cache_replacements,
                      "cache replacement statistics overflow");
        add_statistic(&statistics->shortcut_completions,
                      source->shortcut_completions,
                      "shortcut statistics overflow");
        add_statistic(&statistics->shortcut_rejections,
                      source->shortcut_rejections,
                      "shortcut statistics overflow");
        if (statistics->maximum_depth < source->maximum_depth)
            statistics->maximum_depth = source->maximum_depth;
    }
    statistics->cache_bytes = task.cache.bytes;
    cache_destroy(&task.cache);
    error = pthread_mutex_destroy(&task.progress_mutex);
    if (error != 0) {
        fprintf(stderr, "error: pthread_mutex_destroy: %s\n", strerror(error));
        exit(EXIT_FAILURE);
    }
    return answer;
}

static void verify_known(uint64_t value, int n)
{
    if (n <= KNOWN_MAX_N && value != known[n]) {
        fprintf(stderr, "error: A332800 mismatch at n=%d: got %" PRIu64
                        ", expected %" PRIu64 "\n",
                n, value, known[n]);
        exit(EXIT_FAILURE);
    }
}

static bool next_permutation(int values[MAX_N], int n)
{
    int i = n - 2;
    while (i >= 0 && values[i] >= values[i + 1]) --i;
    if (i < 0) return false;
    int j = n - 1;
    while (values[j] <= values[i]) --j;
    int temporary = values[i]; values[i] = values[j]; values[j] = temporary;
    for (int left = i + 1, right = n - 1; left < right; ++left, --right) {
        temporary = values[left]; values[left] = values[right];
        values[right] = temporary;
    }
    return true;
}

static uint64_t direct_term(int n)
{
    if (n == 0) return 1U;
    int values[MAX_N];
    for (int i = 0; i < n; ++i) values[i] = i + 1;
    uint64_t count = 0U;
    do {
        bool valid = true;
        for (int i = 0; i + 2 < n; ++i) {
            if (values[i] % values[i + 1] >
                values[i + 1] % values[i + 2]) {
                valid = false;
                break;
            }
        }
        if (valid) ++count;
    } while (next_permutation(values, n));
    return count;
}

static void release_bfile_lock(void)
{
    if (bfile_lock_descriptor < 0) return;
    const struct flock lock = {.l_type = F_UNLCK, .l_whence = SEEK_SET,
                               .l_start = 0, .l_len = 0};
    (void)fcntl(bfile_lock_descriptor, F_SETLK, &lock);
    (void)close(bfile_lock_descriptor);
    bfile_lock_descriptor = -1;
}

static void cleanup_bfile(void)
{
    if (bfile_temp_active) {
        (void)unlink(bfile_temp_name);
        bfile_temp_active = false;
    }
    release_bfile_lock();
}

static FILE *open_bfile(void)
{
    if (bfile_temp_active) die("temporary b-file is already active");
    memcpy(bfile_temp_name, BFILE_TEMP_TEMPLATE,
           sizeof(bfile_temp_name));
    bfile_lock_descriptor = open(BFILE_LOCK_NAME, O_CREAT | O_RDWR, 0666);
    if (bfile_lock_descriptor < 0) die("cannot open b-file lock");
    const struct flock lock = {.l_type = F_WRLCK, .l_whence = SEEK_SET,
                               .l_start = 0, .l_len = 0};
    if (fcntl(bfile_lock_descriptor, F_SETLK, &lock) != 0)
        die("another writer holds the b-file lock");

    int descriptor = mkstemp(bfile_temp_name);
    if (descriptor < 0) die("cannot create temporary b-file");
    bfile_temp_active = true;

    struct stat existing;
    if (stat(BFILE_NAME, &existing) == 0) {
        if (fchmod(descriptor, existing.st_mode & 0777) != 0) {
            (void)close(descriptor);
            die("cannot preserve b-file permissions");
        }
    } else if (errno != ENOENT) {
        (void)close(descriptor);
        die("cannot inspect existing b-file");
    }
    FILE *stream = fdopen(descriptor, "w");
    if (stream == NULL) {
        (void)close(descriptor);
        die("cannot open temporary b-file stream");
    }
    return stream;
}

static void finish_bfile(FILE *stream)
{
    if (fflush(stream) != 0) die("cannot flush temporary b-file");
    int descriptor = fileno(stream);
    if (descriptor < 0 || fsync(descriptor) != 0)
        die("cannot sync temporary b-file");
    if (fclose(stream) != 0) die("cannot close temporary b-file");
    if (rename(bfile_temp_name, BFILE_NAME) != 0)
        die("cannot atomically replace b-file");
    bfile_temp_active = false;
    release_bfile_lock();
}

static void checkpoint_bfile(const uint64_t values[MAX_N + 1], int limit)
{
    FILE *bfile = open_bfile();
    for (int n = 0; n <= limit; ++n) {
        if (fprintf(bfile, "%d %" PRIu64 "\n", n, values[n]) < 0)
            die("cannot write temporary b-file");
    }
    finish_bfile(bfile);
}

static void print_statistics(int n, uint64_t value,
                             const Statistics *statistics, double elapsed)
{
    double hit_rate = statistics->cache_lookups == 0U ? 0.0 :
        100.0 * (double)statistics->cache_hits /
        (double)statistics->cache_lookups;
    fprintf(stderr,
            PROGRAM_LABEL ": n=%d, a(n)=%" PRIu64
            ", calls=%" PRIu64 ", transitions=%" PRIu64
            ", cache_hits=%" PRIu64 "/%" PRIu64 " (%.1f%%)"
            ", stores=%" PRIu64 ", replacements=%" PRIu64
            ", shortcut_ones=%" PRIu64 ", shortcut_rejects=%" PRIu64
            ", max_depth=%u, threads=%u, cache=%.1f MiB, %.6f s\n",
            n, value, statistics->calls, statistics->transitions,
            statistics->cache_hits, statistics->cache_lookups, hit_rate,
            statistics->cache_stores, statistics->cache_replacements,
            statistics->shortcut_completions,
            statistics->shortcut_rejections, statistics->maximum_depth,
            statistics->threads,
            (double)statistics->cache_bytes / 1048576.0, elapsed);
}

static void usage(FILE *stream, const char *program)
{
    fprintf(stream,
            "Usage: %s [--upto N | --term N | --check] "
            "[--memory-mb MiB | --no-cache] [--threads T] "
            "[--progress-seconds S] "
            "[--verbose]\n",
            program);
}

int main(int argc, char **argv)
{
    OutputMode mode = MODE_UPTO;
    int limit = DEFAULT_MAX_N;
    unsigned memory_mb = DEFAULT_MEMORY_MB;
    unsigned thread_count = DEFAULT_THREADS;
    unsigned progress_seconds = 60U;
    bool check = false, verbose = false, mode_seen = false;
    bool use_cache = true, cache_option_seen = false;

    if (atexit(cleanup_bfile) != 0) die("could not register cleanup");
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--upto") == 0 ||
            strcmp(argv[i], "--term") == 0) {
            if (mode_seen || i + 1 == argc) {
                usage(stderr, argv[0]); return EXIT_FAILURE;
            }
            mode = strcmp(argv[i], "--term") == 0 ? MODE_TERM : MODE_UPTO;
            limit = parse_integer(argv[++i], "n", 0, MAX_N);
            mode_seen = true;
        } else if (strcmp(argv[i], "--check") == 0) {
            if (mode_seen) { usage(stderr, argv[0]); return EXIT_FAILURE; }
            check = true; limit = CHECK_MAX_N; mode_seen = true;
        } else if (strcmp(argv[i], "--memory-mb") == 0) {
            if (cache_option_seen || i + 1 == argc) {
                usage(stderr, argv[0]); return EXIT_FAILURE;
            }
            memory_mb = (unsigned)parse_integer(argv[++i], "memory-mb",
                                                 MIN_MEMORY_MB,
                                                 MAX_MEMORY_MB);
            use_cache = true;
            cache_option_seen = true;
        } else if (strcmp(argv[i], "--no-cache") == 0) {
            if (cache_option_seen) { usage(stderr, argv[0]); return EXIT_FAILURE; }
            use_cache = false;
            cache_option_seen = true;
        } else if (strcmp(argv[i], "--verbose") == 0) {
            verbose = true;
        } else if (strcmp(argv[i], "--threads") == 0) {
            if (i + 1 == argc) { usage(stderr, argv[0]); return EXIT_FAILURE; }
            thread_count = (unsigned)parse_integer(argv[++i], "threads", 1,
                                                    MAX_THREADS);
        } else if (strcmp(argv[i], "--progress-seconds") == 0) {
            if (i + 1 == argc) { usage(stderr, argv[0]); return EXIT_FAILURE; }
            progress_seconds = (unsigned)parse_integer(
                argv[++i], "progress-seconds", 0, 3600);
        } else if (strcmp(argv[i], "--help") == 0 ||
                   strcmp(argv[i], "-h") == 0) {
            usage(stdout, argv[0]); return EXIT_SUCCESS;
        } else {
            fprintf(stderr, "error: unknown option: %s\n", argv[i]);
            usage(stderr, argv[0]); return EXIT_FAILURE;
        }
    }

    if ((size_t)memory_mb > SIZE_MAX / (1024U * 1024U))
        die("memory limit size overflow");
    size_t memory_limit = (size_t)memory_mb * 1024U * 1024U;

    uint64_t values[MAX_N + 1] = {0U};
    Statistics statistics[MAX_N + 1];
    double elapsed[MAX_N + 1] = {0.0};
    memset(statistics, 0, sizeof(statistics));

    int first_n = mode == MODE_TERM ? limit : 0;
    for (int n = first_n; n <= limit; ++n) {
        double started = monotonic_seconds();
        values[n] = compute_term(n, memory_limit, use_cache,
                                 progress_seconds, thread_count,
                                 &statistics[n]);
        elapsed[n] = monotonic_seconds() - started;
        verify_known(values[n], n);
        if (mode == MODE_UPTO) {
            printf("%d %" PRIu64 "\n", n, values[n]);
            if (fflush(stdout) != 0) die("cannot flush progress output");
            if (verbose)
                print_statistics(n, values[n], &statistics[n], elapsed[n]);
            if (!check) checkpoint_bfile(values, n);
        }
    }

    if (mode == MODE_TERM) {
        if (verbose)
            print_statistics(limit, values[limit], &statistics[limit],
                             elapsed[limit]);
        printf("%" PRIu64 "\n", values[limit]);
        return EXIT_SUCCESS;
    }

    if (check) {
        for (int n = 0; n <= DIRECT_CHECK_MAX_N; ++n) {
            uint64_t direct = direct_term(n);
            if (direct != values[n]) {
                fprintf(stderr,
                        "error: direct enumeration mismatch at n=%d: "
                        "got %" PRIu64 ", expected %" PRIu64 "\n",
                        n, values[n], direct);
                return EXIT_FAILURE;
            }
        }
        for (int n = 0; n <= CACHE_FREE_CHECK_MAX_N; ++n) {
            Statistics cache_free_statistics;
            uint64_t cache_free = compute_term(n, memory_limit, false, 0U, 1U,
                                               &cache_free_statistics);
            if (cache_free != values[n]) {
                fprintf(stderr,
                        "error: cache mismatch at n=%d: got %" PRIu64
                        ", cache-free DFS gives %" PRIu64 "\n",
                        n, values[n], cache_free);
                return EXIT_FAILURE;
            }
        }
    }

    if (check)
        fprintf(stderr,
                "check passed: A332800(0..%d), direct permutations "
                "through n=%d, cache-free DFS through n=%d\n",
                CHECK_MAX_N, DIRECT_CHECK_MAX_N, CACHE_FREE_CHECK_MAX_N);
    return EXIT_SUCCESS;
}
