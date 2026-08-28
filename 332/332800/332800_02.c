/*
 * A332800 -- parallel fixed-table evaluation.
 *
 * Count permutations sigma of [n] for which
 *
 *   sigma(k) mod sigma(k+1) <= sigma(k+1) mod sigma(k+2).
 *
 * For a nonempty set S, x in S, and t >= 0, let F(S,x,t) count valid
 * orderings of S ending in x whose last remainder is at most t.  Then
 *
 *   F({x},x,t) = 1,
 *   F(S,x,t) = sum F(S\{x},y,y mod x),
 *
 * over y in S\{x} with y mod x <= t.  This is the same proved recurrence
 * used to verify 332800_01, but the execution algorithm is different:
 * 02 allocates one fixed-capacity concurrent transposition table and evaluates
 * the n top-level last-value contributions with pthread workers.
 *
 * A memo key is first installed with MEMO_BUSY_BIT.  The owner writes the
 * count and then publishes the completed key with release semantics; readers
 * use acquire loads.  Every dependency removes one set bit, so wait cycles are
 * mathematically impossible.  The table never resizes while workers run.
 * Capacity exhaustion, arithmetic overflow, and all file errors stop the
 * program without publishing a partial b-file.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic -pthread \
 *       332800_02.c -o 332800_02
 *
 * Usage:
 *   ./332800_02
 *   ./332800_02 --upto 22 --threads 3 --memory-mb 1024 --verbose
 *   ./332800_02 --term 22 --threads 8 --memory-mb 1024 --verbose
 *   ./332800_02 --check --threads 4
 *
 * The default and --upto atomically replace b332800_02.txt.  --term and
 * --check do not modify it.
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
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define MAX_N 32
#define DEFAULT_MAX_N 22
#define KNOWN_MAX_N 32
#define CHECK_MAX_N 17
#define DIRECT_CHECK_MAX_N 10
#define DEFAULT_MEMORY_MB 1024
#define MIN_MEMORY_MB 32
#define MAX_MEMORY_MB 65536
#define DEFAULT_THREADS 3
#define MAX_THREADS 64
#define INITIAL_CAPACITY 1024
#define LOAD_NUMERATOR 7
#define LOAD_DENOMINATOR 10
#define KEY_AUX_BITS 10
#define MEMO_BUSY_BIT (UINT64_C(1) << 63)
#define BFILE_NAME "b332800_02.txt"
#define BFILE_TEMP_TEMPLATE BFILE_NAME ".tmp.XXXXXX"
#define BFILE_LOCK_NAME BFILE_NAME ".lock"

typedef uint64_t mask_t;

_Static_assert(MAX_N <= 32, "packed mask has 32 bits");
_Static_assert(2 * 5 <= KEY_AUX_BITS,
               "packed key needs five bits each for last and threshold");
_Static_assert(MAX_N + KEY_AUX_BITS < 63,
               "packed key must not overlap busy bit");

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

typedef struct {
    _Atomic uint64_t key_plus_one;
    uint64_t count;
} MemoEntry;

_Static_assert(sizeof(MemoEntry) == 16, "unexpected atomic memo entry size");

typedef struct {
    MemoEntry *entry;
    size_t capacity;
    _Atomic size_t used;
} MemoTable;

typedef struct {
    int universe_n;
    uint8_t remainder[MAX_N + 1][MAX_N + 1];
    mask_t equal_mask[MAX_N + 1][MAX_N];
    mask_t at_most_mask[MAX_N + 1][MAX_N];
    MemoTable memo;
    size_t memory_limit;
} Search;

typedef struct {
    uint64_t calls;
    uint64_t memo_hits;
    uint64_t transitions;
    uint64_t waits;
} WorkerStatistics;

typedef struct {
    size_t states;
    size_t memory;
    uint64_t calls;
    uint64_t memo_hits;
    uint64_t transitions;
    uint64_t waits;
    unsigned threads;
} Statistics;

typedef struct {
    Search *search;
    int n;
    _Atomic unsigned next_root;
    uint64_t roots[MAX_N];
} ParallelTask;

typedef struct {
    ParallelTask *task;
    WorkerStatistics statistics;
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

static uint64_t hash_u64(uint64_t value)
{
    value ^= value >> 30;
    value *= UINT64_C(0xbf58476d1ce4e5b9);
    value ^= value >> 27;
    value *= UINT64_C(0x94d049bb133111eb);
    return value ^ (value >> 31);
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

static uint64_t pack_key(mask_t set, int last, unsigned threshold)
{
    if (last < 1 || last > MAX_N || threshold >= MAX_N)
        die("memo key component out of range");
    if (set > UINT64_MAX >> KEY_AUX_BITS)
        die("memo key mask overflow");
    return (set << KEY_AUX_BITS) |
           ((uint64_t)(unsigned)(last - 1) << 5) | threshold;
}

static size_t capacity_for(int n, size_t memory_limit)
{
    size_t wanted = INITIAL_CAPACITY;
    if (n >= 9) {
        unsigned shift = (unsigned)n + (n < 15 ? 3U : 4U);
        if (shift < (unsigned)(sizeof(size_t) * 8U))
            wanted = (size_t)1U << shift;
    }
    if (wanted <= SIZE_MAX / sizeof(MemoEntry) &&
        wanted * sizeof(MemoEntry) <= memory_limit)
        return wanted;

    size_t capacity = INITIAL_CAPACITY;
    while (capacity <= SIZE_MAX / 2U &&
           capacity * 2U <= SIZE_MAX / sizeof(MemoEntry) &&
           capacity * 2U * sizeof(MemoEntry) <= memory_limit)
        capacity *= 2U;
    return capacity;
}

static void search_init(Search *search, int n, size_t memory_limit)
{
    memset(search, 0, sizeof(*search));
    search->universe_n = n;
    search->memory_limit = memory_limit;

    for (int last = 1; last <= n; ++last) {
        for (int value = 1; value <= n; ++value) {
            unsigned r = (unsigned)(value % last);
            search->remainder[value][last] = (uint8_t)r;
            search->equal_mask[last][r] |= value_bit(value);
        }
        mask_t accumulated = 0U;
        for (unsigned r = 0; r < (unsigned)n; ++r) {
            accumulated |= search->equal_mask[last][r];
            search->at_most_mask[last][r] = accumulated;
        }
    }

    size_t capacity = capacity_for(n, memory_limit);
    if (capacity > SIZE_MAX / sizeof(MemoEntry))
        die("memo allocation size overflow");
    size_t bytes = capacity * sizeof(MemoEntry);
    search->memo.entry = calloc(capacity, sizeof(*search->memo.entry));
    if (search->memo.entry == NULL) {
        fprintf(stderr, "error: could not allocate %.1f MiB for n=%d\n",
                (double)bytes / 1048576.0, n);
        exit(EXIT_FAILURE);
    }
    search->memo.capacity = capacity;
    atomic_init(&search->memo.used, 0U);
}

static void search_free(Search *search)
{
    free(search->memo.entry);
    search->memo.entry = NULL;
    search->memo.capacity = 0U;
}

static void wait_for_publication(MemoEntry *entry, uint64_t stored,
                                 WorkerStatistics *statistics)
{
    uint64_t busy = stored | MEMO_BUSY_BIT;
    unsigned spins = 0U;
    for (;;) {
        uint64_t observed = atomic_load_explicit(&entry->key_plus_one,
                                                 memory_order_acquire);
        if (observed == stored) return;
        if (observed != busy) die("concurrent memo reservation changed key");
        ++statistics->waits;
        if (++spins >= 64U) {
            spins = 0U;
            (void)sched_yield();
        }
    }
}

static uint64_t count_state(Search *search, WorkerStatistics *statistics,
                            mask_t set, int last, unsigned threshold)
{
    ++statistics->calls;
    mask_t bit = value_bit(last);
    if ((set & bit) == 0U) die("state does not contain its last value");
    mask_t rest = set ^ bit;
    if (rest == 0U) return 1U;

    if (threshold >= (unsigned)last) threshold = (unsigned)last - 1U;
    while ((rest & search->equal_mask[last][threshold]) == 0U) {
        if (threshold == 0U) return 0U;
        --threshold;
    }

    uint64_t key = pack_key(set, last, threshold);
    uint64_t stored = key + 1U;
    if (stored == 0U || (stored & MEMO_BUSY_BIT) != 0U)
        die("memo stored-key overflow");
    uint64_t hash = hash_u64(key);
    size_t slot = (size_t)hash & (search->memo.capacity - 1U);
    MemoEntry *entry = NULL;

    for (size_t probes = 0; probes < search->memo.capacity; ++probes) {
        entry = &search->memo.entry[slot];
        uint64_t observed = atomic_load_explicit(&entry->key_plus_one,
                                                 memory_order_acquire);
        if (observed == 0U) {
            uint64_t expected = 0U;
            uint64_t busy = stored | MEMO_BUSY_BIT;
            if (atomic_compare_exchange_strong_explicit(
                    &entry->key_plus_one, &expected, busy,
                    memory_order_acq_rel, memory_order_acquire)) {
                size_t used = atomic_fetch_add_explicit(&search->memo.used, 1U,
                                                        memory_order_relaxed) +
                              1U;
                if (used * LOAD_DENOMINATOR >=
                    search->memo.capacity * LOAD_NUMERATOR) {
                    fprintf(stderr,
                            "error: fixed memo table full at n=%d: "
                            "states=%zu, capacity=%zu, memory=%.1f MiB; "
                            "increase --memory-mb\n",
                            search->universe_n, used, search->memo.capacity,
                            (double)(search->memo.capacity * sizeof(MemoEntry)) /
                                1048576.0);
                    exit(EXIT_FAILURE);
                }
                break;
            }
            observed = expected;
        }

        if ((observed & ~MEMO_BUSY_BIT) == stored) {
            if ((observed & MEMO_BUSY_BIT) != 0U)
                wait_for_publication(entry, stored, statistics);
            ++statistics->memo_hits;
            return entry->count;
        }
        slot = (slot + 1U) & (search->memo.capacity - 1U);
    }
    if (entry == NULL ||
        atomic_load_explicit(&entry->key_plus_one, memory_order_relaxed) !=
            (stored | MEMO_BUSY_BIT))
        die("concurrent memo insertion failed");

    mask_t candidates = rest & search->at_most_mask[last][threshold];
    uint64_t total = 0U;
    while (candidates != 0U) {
        int penultimate = first_value(candidates);
        candidates &= candidates - 1U;
        ++statistics->transitions;
        unsigned next_threshold = search->remainder[penultimate][last];
        uint64_t add = count_state(search, statistics, rest, penultimate,
                                   next_threshold);
        if (UINT64_MAX - total < add) die("state count exceeds uint64_t");
        total += add;
    }

    entry->count = total;
    atomic_store_explicit(&entry->key_plus_one, stored, memory_order_release);
    return total;
}

static void *worker_main(void *argument)
{
    Worker *worker = argument;
    ParallelTask *task = worker->task;
    for (;;) {
        unsigned root = atomic_fetch_add_explicit(&task->next_root, 1U,
                                                  memory_order_relaxed);
        if (root >= (unsigned)task->n) break;
        int last = (int)root + 1;
        task->roots[root] = count_state(task->search, &worker->statistics,
                                        full_mask(task->n), last,
                                        (unsigned)task->n - 1U);
    }
    return NULL;
}

static uint64_t compute_largest(Search *search, int n, unsigned thread_count,
                                Statistics *statistics)
{
    memset(statistics, 0, sizeof(*statistics));
    if (n == 0) return 1U;
    if (thread_count > (unsigned)n) thread_count = (unsigned)n;
    if (thread_count == 0U) thread_count = 1U;

    ParallelTask task;
    memset(&task, 0, sizeof(task));
    task.search = search;
    task.n = n;
    atomic_init(&task.next_root, 0U);

    Worker workers[MAX_THREADS];
    pthread_t threads[MAX_THREADS - 1];
    memset(workers, 0, sizeof(workers));
    for (unsigned i = 0; i < thread_count; ++i) workers[i].task = &task;

    for (unsigned i = 1; i < thread_count; ++i) {
        int error = pthread_create(&threads[i - 1], NULL, worker_main,
                                   &workers[i]);
        if (error != 0) {
            fprintf(stderr, "error: pthread_create: %s\n", strerror(error));
            exit(EXIT_FAILURE);
        }
    }
    (void)worker_main(&workers[0]);
    for (unsigned i = 1; i < thread_count; ++i) {
        int error = pthread_join(threads[i - 1], NULL);
        if (error != 0) {
            fprintf(stderr, "error: pthread_join: %s\n", strerror(error));
            exit(EXIT_FAILURE);
        }
    }

    uint64_t answer = 0U;
    for (int last = 0; last < n; ++last) {
        if (UINT64_MAX - answer < task.roots[last])
            die("A332800 term exceeds uint64_t");
        answer += task.roots[last];
    }
    for (unsigned i = 0; i < thread_count; ++i) {
        WorkerStatistics *source = &workers[i].statistics;
        if (UINT64_MAX - statistics->calls < source->calls ||
            UINT64_MAX - statistics->memo_hits < source->memo_hits ||
            UINT64_MAX - statistics->transitions < source->transitions ||
            UINT64_MAX - statistics->waits < source->waits)
            die("statistics overflow");
        statistics->calls += source->calls;
        statistics->memo_hits += source->memo_hits;
        statistics->transitions += source->transitions;
        statistics->waits += source->waits;
    }
    statistics->states = atomic_load_explicit(&search->memo.used,
                                               memory_order_relaxed);
    statistics->memory = search->memo.capacity * sizeof(MemoEntry);
    statistics->threads = thread_count;
    return answer;
}

static uint64_t compute_cached_prefix(Search *search, int n,
                                      Statistics *statistics)
{
    memset(statistics, 0, sizeof(*statistics));
    if (n == 0) return 1U;
    size_t before = atomic_load_explicit(&search->memo.used,
                                         memory_order_relaxed);
    WorkerStatistics local = {0};
    uint64_t answer = 0U;
    for (int last = 1; last <= n; ++last) {
        uint64_t add = count_state(search, &local, full_mask(n), last,
                                   (unsigned)n - 1U);
        if (UINT64_MAX - answer < add) die("A332800 term exceeds uint64_t");
        answer += add;
    }
    size_t after = atomic_load_explicit(&search->memo.used,
                                        memory_order_relaxed);
    if (after != before)
        die("largest-term memo did not contain a prefix root");
    statistics->states = after;
    statistics->memory = search->memo.capacity * sizeof(MemoEntry);
    statistics->calls = local.calls;
    statistics->memo_hits = local.memo_hits;
    statistics->transitions = local.transitions;
    statistics->waits = local.waits;
    statistics->threads = 1U;
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

static void print_statistics(int n, uint64_t value,
                             const Statistics *statistics, double elapsed)
{
    double hit_rate = statistics->calls == 0U ? 0.0 :
        100.0 * (double)statistics->memo_hits / (double)statistics->calls;
    fprintf(stderr,
            "332800_02: n=%d, a(n)=%" PRIu64
            ", states=%zu, calls=%" PRIu64 ", memo_hits=%" PRIu64
            " (%.1f%%), transitions=%" PRIu64 ", waits=%" PRIu64
            ", table=%.1f MiB, threads=%u, %.6f s\n",
            n, value, statistics->states, statistics->calls,
            statistics->memo_hits, hit_rate, statistics->transitions,
            statistics->waits, (double)statistics->memory / 1048576.0,
            statistics->threads, elapsed);
}

static void usage(FILE *stream, const char *program)
{
    fprintf(stream,
            "Usage: %s [--upto N | --term N | --check] "
            "[--threads T] [--memory-mb MiB] [--verbose]\n",
            program);
}

int main(int argc, char **argv)
{
    OutputMode mode = MODE_UPTO;
    int limit = DEFAULT_MAX_N;
    unsigned thread_count = DEFAULT_THREADS;
    unsigned memory_mb = DEFAULT_MEMORY_MB;
    bool check = false, verbose = false, mode_seen = false;

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
        } else if (strcmp(argv[i], "--threads") == 0) {
            if (i + 1 == argc) { usage(stderr, argv[0]); return EXIT_FAILURE; }
            thread_count = (unsigned)parse_integer(argv[++i], "threads", 1,
                                                    MAX_THREADS);
        } else if (strcmp(argv[i], "--memory-mb") == 0) {
            if (i + 1 == argc) { usage(stderr, argv[0]); return EXIT_FAILURE; }
            memory_mb = (unsigned)parse_integer(argv[++i], "memory-mb",
                                                 MIN_MEMORY_MB,
                                                 MAX_MEMORY_MB);
        } else if (strcmp(argv[i], "--verbose") == 0) {
            verbose = true;
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
    values[0] = 1U;

    if (limit > 0) {
        Search search;
        search_init(&search, limit, memory_limit);
        double started = monotonic_seconds();
        values[limit] = compute_largest(&search, limit, thread_count,
                                        &statistics[limit]);
        elapsed[limit] = monotonic_seconds() - started;
        verify_known(values[limit], limit);

        if (mode == MODE_UPTO) {
            for (int n = limit - 1; n >= 1; --n) {
                started = monotonic_seconds();
                values[n] = compute_cached_prefix(&search, n, &statistics[n]);
                elapsed[n] = monotonic_seconds() - started;
                verify_known(values[n], n);
            }
        }
        search_free(&search);
    }

    if (mode == MODE_TERM) {
        verify_known(values[limit], limit);
        if (verbose)
            print_statistics(limit, values[limit], &statistics[limit],
                             elapsed[limit]);
        printf("%" PRIu64 "\n", values[limit]);
        return EXIT_SUCCESS;
    }

    verify_known(values[0], 0);
    if (check) {
        for (int n = 0; n <= DIRECT_CHECK_MAX_N; ++n) {
            uint64_t direct = direct_term(n);
            if (direct != values[n]) {
                fprintf(stderr, "error: direct mismatch at n=%d\n", n);
                return EXIT_FAILURE;
            }
        }
    }
    if (verbose)
        for (int n = 0; n <= limit; ++n)
            print_statistics(n, values[n], &statistics[n], elapsed[n]);

    FILE *bfile = check ? NULL : open_bfile();
    for (int n = 0; n <= limit; ++n) {
        if (bfile != NULL &&
            fprintf(bfile, "%d %" PRIu64 "\n", n, values[n]) < 0)
            die("cannot write temporary b-file");
        printf("%s%" PRIu64, n == 0 ? "" : ", ", values[n]);
    }
    putchar('\n');
    if (fflush(stdout) != 0) die("cannot flush stdout");
    if (bfile != NULL) finish_bfile(bfile);
    if (check)
        fprintf(stderr,
                "check passed: A332800(0..%d), direct permutations "
                "through n=%d\n", CHECK_MAX_N, DIRECT_CHECK_MAX_N);
    return EXIT_SUCCESS;
}
