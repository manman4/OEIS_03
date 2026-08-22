/*
 * A398911 -- reflected Gray-code inclusion-exclusion, exact modulo 2^128.
 *
 * Count partitions of V={1,...,3*n} into n unordered triples whose sums are
 * pairwise distinct.  For A subset V let
 *
 *   c_s(A) = #{ {x,y,z} subset V\A : x<y<z, x+y+z=s }.
 *
 * If e_n is the elementary symmetric polynomial of degree n, then
 *
 *   a(n) = sum_{A subset V} (-1)^|A| e_n(c_6(A),...,c_(9*n-3)(A)).
 *
 * Here e_n selects n distinct sum classes and one surviving triple from each.
 * Inclusion-exclusion removes every selection that misses a vertex.  A
 * surviving selection has n triples and covers all 3*n vertices, so its
 * triples are automatically disjoint and form exactly one desired partition.
 *
 * This is deliberately different from 398911_04.c.  It follows the method in
 * OEIS file A398911/a398911.txt: A is traversed in Gray-code order, c_s is
 * updated after one vertex changes, and e_n is rebuilt only when required.
 * Reflection v -> 3*n+1-v maps A to R(A), preserves |A|, and merely permutes
 * the sum classes.  Thus only the representative R(A)>=A is evaluated, with
 * weight two except at fixed points.  If fewer than n counts c_s are nonzero,
 * e_n is zero and its evaluation is skipped.  Time is O(8^n*n^2), memory is
 * O(n) per worker, and chunk slices permit independent distributed runs.
 *
 * Arithmetic contributing to inclusion-exclusion is intentionally performed
 * modulo 2^128.  Unsigned U128 wraparound is therefore required, not an
 * unchecked-overflow bug: polynomial evaluation, signed inclusion-exclusion,
 * worker reduction, and PARTIAL combination are all valid in Z/(2^128).  For
 * every accepted n<=17 the unrestricted triple-partition bound
 *
 *                    (3*n)! / (6^n*n!)
 *
 * is computed without overflow and proved smaller than 2^128.  Since
 * 0<=a(n) is below that bound, the complete residue uniquely equals the
 * ordinary integer a(n).  Complete results are checked against this bound and
 * against the known prefix.  A slice is only a modular partial value; complete
 * slices must cover [0,2^split) exactly once before their hexadecimal values
 * are added modulo 2^128.
 *
 * Workers own all c_s arrays, polynomials, accumulators, and statistics.
 * Shared scheduling and progress fields are atomic.  --check also evaluates
 * n<=6 without reflection and compares the answers, independently checking
 * the symmetry optimization.
 *
 * Build:
 *   cc -O3 -mcpu=native -std=c11 -Wall -Wextra -Wpedantic -pthread \
 *       398911_05.c -o 398911_05
 *
 * Usage:
 *   ./398911_05 --term 9 --threads 8
 *   ./398911_05 --term 12 --threads 8 --split 12
 *   ./398911_05 --term 14 --threads 8 --split 15 --slice 0 8
 *   ./398911_05 --check 7 --threads 8
 *   ./398911_05 --upto 9 --threads 8
 *
 * --term and --check do not modify a b-file.  --upto calculates every term
 * itself (no uncomputed built-in prefix is inserted), writes completed terms
 * to b398911_5_part.txt with fsync, and atomically renames it to
 * b398911_5.txt only after the requested prefix is complete.
 */

#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <fcntl.h>
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
#error "398911_05 requires unsigned __int128"
#endif
__extension__ typedef unsigned __int128 U128;

#define MIN_N 0
#define MAX_N 17
#define KNOWN_MAX_N 9
#define DEFAULT_CHECK_N 7
#define DEFAULT_THREADS 4
#define MAX_THREADS 64
#define DEFAULT_SPLIT 6
#define DEFAULT_PROGRESS_SECONDS 30
#define MAX_PROGRESS_SECONDS 3600
#define MAX_ELEMENTS (3 * MAX_N)
#define MAX_TRIPLE_SUM (9 * MAX_N - 3)

_Static_assert(MAX_ELEMENTS < 64, "element mask must fit uint64_t");

typedef enum {
    MODE_TERM,
    MODE_UPTO,
    MODE_CHECK
} RunMode;

typedef struct {
    uint64_t chunks;
    uint64_t subsets;
    uint64_t pair_updates;
    uint64_t polynomial_evaluations;
    uint64_t nonzero_skips;
    uint64_t reflection_skips;
} SearchStats;

typedef struct {
    int n;
    unsigned element_count;
    unsigned maximum_sum;
    unsigned low_bits;
    uint64_t inner_steps;
    uint64_t chunk_hi;
    uint64_t target_subsets;
    bool use_reflection;
    _Atomic uint64_t next_chunk;
    _Atomic uint64_t completed_subsets;
} TaskQueue;

typedef struct {
    TaskQueue *queue;
    U128 residue;
    SearchStats stats;
} Worker;

static const uint64_t known_terms[KNOWN_MAX_N + 1] = {
    UINT64_C(1),
    UINT64_C(1),
    UINT64_C(10),
    UINT64_C(246),
    UINT64_C(12184),
    UINT64_C(995001),
    UINT64_C(122605952),
    UINT64_C(21102544958),
    UINT64_C(4843191888898),
    UINT64_C(1428055454246576)
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
    const long value = strtol(text, &end, 10);
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
    const unsigned long value = strtoul(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' || text[0] == '-' ||
        value < minimum || value > maximum) {
        fprintf(stderr, "error: %s must be in %u..%u: %s\n",
                label, minimum, maximum, text);
        exit(EXIT_FAILURE);
    }
    return (unsigned)value;
}

static uint64_t parse_u64(const char *text, const char *label)
{
    char *end = NULL;
    errno = 0;
    const unsigned long long value = strtoull(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' || text[0] == '-') {
        fprintf(stderr, "error: invalid %s: %s\n", label, text);
        exit(EXIT_FAILURE);
    }
    return (uint64_t)value;
}

static void add_u64_saturating(uint64_t *destination, uint64_t addend)
{
    *destination = *destination > UINT64_MAX - addend
                       ? UINT64_MAX
                       : *destination + addend;
}

static void merge_stats(SearchStats *destination, const SearchStats *source)
{
    add_u64_saturating(&destination->chunks, source->chunks);
    add_u64_saturating(&destination->subsets, source->subsets);
    add_u64_saturating(&destination->pair_updates, source->pair_updates);
    add_u64_saturating(&destination->polynomial_evaluations,
                       source->polynomial_evaluations);
    add_u64_saturating(&destination->nonzero_skips,
                       source->nonzero_skips);
    add_u64_saturating(&destination->reflection_skips,
                       source->reflection_skips);
}

static int fprint_u128(FILE *stream, U128 value)
{
    char digits[40];
    size_t length = 0;
    do {
        digits[length++] = (char)('0' + (unsigned)(value % 10U));
        value /= 10U;
    } while (value != 0);
    while (length != 0) {
        if (fputc(digits[--length], stream) == EOF) return -1;
    }
    return 0;
}

static void print_term(int n, U128 value)
{
    if (printf("%d ", n) < 0 || fprint_u128(stdout, value) != 0 ||
        putchar('\n') == EOF || fflush(stdout) != 0) {
        die("cannot write standard output");
    }
}

static U128 unrestricted_partition_bound(int n)
{
    U128 bound = 1;
    const U128 maximum = ~(U128)0;
    for (unsigned triples = 1; triples <= (unsigned)n; ++triples) {
        const unsigned a = 3U * triples - 1U;
        const unsigned b = 3U * triples - 2U;
        const unsigned factor = a * b / 2U;
        if (bound > maximum / factor) {
            die("unrestricted triple-partition bound exceeds U128");
        }
        bound *= factor;
    }
    return bound;
}

static void verify_complete_result(int n, U128 value)
{
    const U128 bound = unrestricted_partition_bound(n);
    if (value > bound) {
        die("A398911 result exceeds the unrestricted partition bound");
    }
    if (n <= KNOWN_MAX_N && value != (U128)known_terms[n]) {
        fprintf(stderr, "error: A398911 mismatch at n=%d: got ", n);
        (void)fprint_u128(stderr, value);
        fprintf(stderr, ", expected %" PRIu64 "\n", known_terms[n]);
        exit(EXIT_FAILURE);
    }
}

/* All three helpers below intentionally operate modulo 2^128. */
static U128 residue_add(U128 left, U128 right)
{
    return left + right;
}

static U128 residue_sub(U128 left, U128 right)
{
    return left - right;
}

static U128 residue_scale(unsigned factor, U128 value)
{
    return (U128)factor * value;
}

static uint64_t reflect_mask(uint64_t mask, unsigned width)
{
    uint64_t reflected = 0;
    for (unsigned index = 0; index < width; ++index) {
        if ((mask & (UINT64_C(1) << index)) != 0) {
            reflected |= UINT64_C(1) << (width - 1U - index);
        }
    }
    return reflected;
}

static void initialize_counts(const TaskQueue *queue, uint64_t avoided,
                              uint16_t counts[MAX_TRIPLE_SUM + 1],
                              unsigned *nonzero)
{
    memset(counts, 0, (MAX_TRIPLE_SUM + 1U) * sizeof(*counts));
    *nonzero = 0;
    const unsigned maximum = queue->element_count;
    for (unsigned x = 1; x + 2U <= maximum; ++x) {
        if ((avoided & (UINT64_C(1) << (x - 1U))) != 0) continue;
        for (unsigned y = x + 1U; y + 1U <= maximum; ++y) {
            if ((avoided & (UINT64_C(1) << (y - 1U))) != 0) continue;
            for (unsigned z = y + 1U; z <= maximum; ++z) {
                if ((avoided & (UINT64_C(1) << (z - 1U))) != 0) continue;
                const unsigned sum = x + y + z;
                if (counts[sum] == UINT16_MAX) {
                    die("triple-sum count overflow");
                }
                ++counts[sum];
            }
        }
    }
    for (unsigned sum = 6; sum <= queue->maximum_sum; ++sum) {
        *nonzero += counts[sum] != 0;
    }
}

static U128 evaluate_elementary(const TaskQueue *queue,
                                const uint16_t counts[MAX_TRIPLE_SUM + 1])
{
    U128 elementary[MAX_N + 1] = {0};
    elementary[0] = 1;
    for (unsigned sum = 6; sum <= queue->maximum_sum; ++sum) {
        const unsigned count = counts[sum];
        if (count == 0) continue;
        for (int degree = queue->n; degree >= 1; --degree) {
            elementary[degree] = residue_add(
                elementary[degree],
                residue_scale(count, elementary[degree - 1]));
        }
    }
    return elementary[queue->n];
}

static void flip_avoided_vertex(
    const TaskQueue *queue, uint64_t *avoided, uint64_t *reflected,
    unsigned vertex, uint16_t counts[MAX_TRIPLE_SUM + 1],
    unsigned *nonzero, SearchStats *stats)
{
    const uint64_t vertex_bit = UINT64_C(1) << (vertex - 1U);
    const bool add_to_avoided = (*avoided & vertex_bit) == 0;
    *avoided ^= vertex_bit;
    *reflected ^= UINT64_C(1) << (queue->element_count - vertex);

    const uint64_t full_mask =
        (UINT64_C(1) << queue->element_count) - UINT64_C(1);
    uint64_t first_choices = (~*avoided) & full_mask & ~vertex_bit;
    while (first_choices != 0) {
        const uint64_t first_bit =
            first_choices & (UINT64_C(0) - first_choices);
        const unsigned first =
            (unsigned)__builtin_ctzll(first_bit) + 1U;
        first_choices ^= first_bit;
        uint64_t second_choices = first_choices;
        while (second_choices != 0) {
            const uint64_t second_bit =
                second_choices & (UINT64_C(0) - second_choices);
            const unsigned second =
                (unsigned)__builtin_ctzll(second_bit) + 1U;
            second_choices ^= second_bit;
            const unsigned sum = vertex + first + second;
            const uint16_t old = counts[sum];
            if (add_to_avoided) {
                if (old == 0) die("internal negative triple-sum count");
                counts[sum] = (uint16_t)(old - 1U);
                if (old == 1U) --*nonzero;
            } else {
                if (old == UINT16_MAX) die("triple-sum count overflow");
                counts[sum] = (uint16_t)(old + 1U);
                if (old == 0) ++*nonzero;
            }
            add_u64_saturating(&stats->pair_updates, 1);
        }
    }
}

static void process_chunk(Worker *worker, uint64_t chunk)
{
    TaskQueue *queue = worker->queue;
    uint64_t avoided = chunk << queue->low_bits;
    uint64_t reflected = reflect_mask(avoided, queue->element_count);
    uint16_t counts[MAX_TRIPLE_SUM + 1];
    unsigned nonzero;
    initialize_counts(queue, avoided, counts, &nonzero);

    for (uint64_t step = 0;; ++step) {
        if (nonzero < (unsigned)queue->n) {
            add_u64_saturating(&worker->stats.nonzero_skips, 1);
        } else if (queue->use_reflection && reflected < avoided) {
            add_u64_saturating(&worker->stats.reflection_skips, 1);
        } else {
            const U128 value = evaluate_elementary(queue, counts);
            const unsigned weight =
                queue->use_reflection && reflected != avoided ? 2U : 1U;
            const U128 term = residue_scale(weight, value);
            worker->residue =
                (__builtin_popcountll(avoided) & 1) != 0
                    ? residue_sub(worker->residue, term)
                    : residue_add(worker->residue, term);
            add_u64_saturating(
                &worker->stats.polynomial_evaluations, 1);
        }

        if (step + UINT64_C(1) == queue->inner_steps) break;
        const unsigned vertex =
            (unsigned)__builtin_ctzll(step + UINT64_C(1)) + 1U;
        flip_avoided_vertex(queue, &avoided, &reflected, vertex,
                            counts, &nonzero, &worker->stats);
    }
    add_u64_saturating(&worker->stats.chunks, 1);
    add_u64_saturating(&worker->stats.subsets, queue->inner_steps);
}

static void *worker_main(void *argument)
{
    Worker *worker = argument;
    TaskQueue *queue = worker->queue;
    for (;;) {
        const uint64_t chunk = atomic_fetch_add_explicit(
            &queue->next_chunk, UINT64_C(1), memory_order_relaxed);
        if (chunk >= queue->chunk_hi) break;
        process_chunk(worker, chunk);
        atomic_fetch_add_explicit(&queue->completed_subsets,
                                  queue->inner_steps,
                                  memory_order_release);
    }
    return NULL;
}

static void report_progress(int n, const TaskQueue *queue, double started,
                            uint64_t *previous, double *previous_time,
                            bool terminal, bool final)
{
    const uint64_t completed = atomic_load_explicit(
        &queue->completed_subsets, memory_order_acquire);
    const double now = monotonic_seconds();
    const double interval = now - *previous_time;
    const double rate = interval > 0.0
                            ? (double)(completed - *previous) /
                                  interval / 1000000.0
                            : 0.0;
    const double percent = queue->target_subsets == 0
                               ? 100.0
                               : 100.0 * (double)completed /
                                     (double)queue->target_subsets;
    fprintf(stderr,
            terminal ? "\r398911_05: n=%d progress subsets=%" PRIu64
                       "/%" PRIu64 " (%5.1f%%), rate=%.2f M/s, %.1f s"
                     : "398911_05: n=%d progress subsets=%" PRIu64
                       "/%" PRIu64 " (%5.1f%%), rate=%.2f M/s, %.1f s\n",
            n, completed, queue->target_subsets, percent, rate,
            now - started);
    if (terminal && final) fputc('\n', stderr);
    fflush(stderr);
    *previous = completed;
    *previous_time = now;
}

static U128 compute_range(int n, unsigned requested_threads,
                          unsigned progress_seconds, unsigned split,
                          uint64_t chunk_lo, uint64_t chunk_hi,
                          bool use_reflection, bool complete,
                          bool report, SearchStats *combined_stats,
                          unsigned *actual_workers)
{
    if (n < MIN_N || n > MAX_N || 3 * n >= 64) {
        die("unsupported dimension in reflected inclusion-exclusion");
    }
    (void)unrestricted_partition_bound(n);
    memset(combined_stats, 0, sizeof(*combined_stats));
    *actual_workers = 1;
    if (n == 0) {
        if (split != 0 || chunk_lo != 0 || chunk_hi != 1) {
            die("split or slice is invalid for n=0");
        }
        combined_stats->chunks = 1;
        combined_stats->subsets = 1;
        return 1;
    }

    const unsigned element_count = 3U * (unsigned)n;
    if (split < 1 || split > element_count) {
        die("internal split is outside the element-mask width");
    }
    const uint64_t all_chunks = UINT64_C(1) << split;
    if (chunk_lo >= chunk_hi || chunk_hi > all_chunks) {
        die("invalid chunk interval");
    }
    const unsigned low_bits = element_count - split;
    const uint64_t inner_steps = UINT64_C(1) << low_bits;
    const uint64_t range_chunks = chunk_hi - chunk_lo;
    if (range_chunks > UINT64_MAX / inner_steps) {
        die("subset-range size overflow");
    }
    const uint64_t target_subsets = range_chunks * inner_steps;

    TaskQueue queue = {
        .n = n,
        .element_count = element_count,
        .maximum_sum = 3U * element_count - 3U,
        .low_bits = low_bits,
        .inner_steps = inner_steps,
        .chunk_hi = chunk_hi,
        .target_subsets = target_subsets,
        .use_reflection = use_reflection,
        .next_chunk = chunk_lo,
        .completed_subsets = 0
    };
    unsigned workers = requested_threads;
    if ((uint64_t)workers > range_chunks) workers = (unsigned)range_chunks;
    Worker *worker = calloc(workers, sizeof(*worker));
    pthread_t *threads = calloc(workers, sizeof(*threads));
    if (worker == NULL || threads == NULL) {
        free(threads);
        free(worker);
        die("cannot allocate inclusion-exclusion workers");
    }

    const double started = monotonic_seconds();
    for (unsigned index = 0; index < workers; ++index) {
        worker[index].queue = &queue;
        if (pthread_create(&threads[index], NULL, worker_main,
                           &worker[index]) != 0) {
            die("pthread_create failed");
        }
    }

    const bool terminal = isatty(fileno(stderr)) != 0;
    bool progress_printed = false;
    double previous_time = started;
    double next_report = started + (double)progress_seconds;
    uint64_t previous = 0;
    while (atomic_load_explicit(&queue.completed_subsets,
                                memory_order_acquire) < target_subsets) {
        const double now = monotonic_seconds();
        if (progress_seconds != 0 && now >= next_report) {
            report_progress(n, &queue, started, &previous,
                            &previous_time, terminal, false);
            progress_printed = true;
            next_report = now + (double)progress_seconds;
        }
        struct timespec pause = {.tv_sec = 0, .tv_nsec = 20000000L};
        while (nanosleep(&pause, &pause) != 0 && errno == EINTR) {
            /* Resume the short progress-monitoring sleep. */
        }
    }

    for (unsigned index = 0; index < workers; ++index) {
        if (pthread_join(threads[index], NULL) != 0) {
            die("pthread_join failed");
        }
    }
    if (progress_printed) {
        report_progress(n, &queue, started, &previous,
                        &previous_time, terminal, true);
    }
    if (atomic_load_explicit(&queue.completed_subsets,
                             memory_order_acquire) != target_subsets) {
        die("internal subset-coverage mismatch");
    }

    U128 residue = 0;
    for (unsigned index = 0; index < workers; ++index) {
        residue = residue_add(residue, worker[index].residue);
        merge_stats(combined_stats, &worker[index].stats);
    }
    free(threads);
    free(worker);
    *actual_workers = workers;

    if (complete) verify_complete_result(n, residue);
    if (report) {
        fprintf(stderr,
                "398911_05: n=%d, reflected Gray-code IE, chunks=%" PRIu64
                ", subsets=%" PRIu64 ", pair-updates=%" PRIu64
                ", e-evaluations=%" PRIu64 ", nz-skips=%" PRIu64
                ", reflection-skips=%" PRIu64 ", threads=%u, %.3f s\n",
                n, combined_stats->chunks, combined_stats->subsets,
                combined_stats->pair_updates,
                combined_stats->polynomial_evaluations,
                combined_stats->nonzero_skips,
                combined_stats->reflection_skips, workers,
                monotonic_seconds() - started);
    }
    return residue;
}

static unsigned default_split_for_n(int n)
{
    const unsigned elements = 3U * (unsigned)n;
    return elements < DEFAULT_SPLIT ? elements : DEFAULT_SPLIT;
}

static U128 compute_complete(int n, unsigned requested_threads,
                             unsigned progress_seconds, unsigned split,
                             bool use_reflection, bool report)
{
    SearchStats stats;
    unsigned workers;
    if (n == 0) {
        return compute_range(n, requested_threads, progress_seconds,
                             0, 0, 1, use_reflection, true,
                             report, &stats, &workers);
    }
    const uint64_t chunks = UINT64_C(1) << split;
    return compute_range(n, requested_threads, progress_seconds,
                         split, 0, chunks, use_reflection, true,
                         report, &stats, &workers);
}

static char *path_beside_executable(const char *argv0,
                                    const char *filename)
{
    char executable[PATH_MAX];
    char resolved[PATH_MAX];
    bool found = false;
#ifdef __APPLE__
    uint32_t size = (uint32_t)sizeof(executable);
    if (_NSGetExecutablePath(executable, &size) == 0) found = true;
#elif defined(__linux__)
    const ssize_t length = readlink("/proc/self/exe", executable,
                                    sizeof(executable) - 1U);
    if (length >= 0) {
        executable[(size_t)length] = '\0';
        found = true;
    }
#endif
    if (!found) {
        const size_t length = strlen(argv0);
        if (length >= sizeof(executable)) die("executable path is too long");
        memcpy(executable, argv0, length + 1U);
    }
    const char *base = realpath(executable, resolved);
    if (base == NULL) base = executable;
    const char *slash = strrchr(base, '/');
    size_t directory_length = slash == NULL ? 1U : (size_t)(slash - base);
    if (slash != NULL && directory_length == 0) directory_length = 1U;
    const size_t filename_length = strlen(filename);
    if (directory_length > SIZE_MAX - filename_length - 2U) {
        die("b-file path length overflow");
    }
    char *path = malloc(directory_length + filename_length + 2U);
    if (path == NULL) die("cannot allocate b-file path");
    if (slash == NULL) path[0] = '.';
    else if (slash == base) path[0] = '/';
    else memcpy(path, base, directory_length);
    path[directory_length] = '/';
    memcpy(path + directory_length + 1U, filename, filename_length + 1U);
    return path;
}

static int lock_bfile(const char *path)
{
    const int fd = open(path, O_RDWR | O_CREAT, 0666);
    if (fd < 0) die("cannot open b-file lock");
    struct flock lock = {.l_type = F_WRLCK, .l_whence = SEEK_SET};
    while (fcntl(fd, F_SETLKW, &lock) != 0) {
        if (errno != EINTR) die("cannot lock b-file");
    }
    return fd;
}

static void unlock_bfile(int fd)
{
    struct flock lock = {.l_type = F_UNLCK, .l_whence = SEEK_SET};
    if (fcntl(fd, F_SETLK, &lock) != 0 || close(fd) != 0) {
        die("cannot unlock b-file");
    }
}

static void sync_parent_directory(const char *path)
{
    const char *slash = strrchr(path, '/');
    if (slash == NULL) die("internal b-file path has no directory");
    const size_t length = slash == path ? 1U : (size_t)(slash - path);
    char *directory = malloc(length + 1U);
    if (directory == NULL) die("cannot allocate parent-directory path");
    memcpy(directory, path, length);
    directory[length] = '\0';
    const int fd = open(directory, O_RDONLY);
    free(directory);
    if (fd < 0) die("cannot open b-file parent directory");
    if (fsync(fd) != 0 || close(fd) != 0) {
        die("cannot sync b-file parent directory");
    }
}

static void produce_bfile(const char *argv0, int maximum_n,
                          unsigned requested_threads,
                          unsigned progress_seconds)
{
    char *final_path = path_beside_executable(argv0, "b398911_5.txt");
    char *part_path = path_beside_executable(argv0, "b398911_5_part.txt");
    char *lock_path = path_beside_executable(argv0, "b398911_5.lock");
    const int lock_fd = lock_bfile(lock_path);
    FILE *output = fopen(part_path, "w");
    if (output == NULL) die("cannot create b-file part file");

    for (int n = MIN_N; n <= maximum_n; ++n) {
        const unsigned split = n == 0 ? 0 : default_split_for_n(n);
        const U128 value = compute_complete(
            n, requested_threads, progress_seconds, split, true, true);
        if (fprintf(output, "%d ", n) < 0 ||
            fprint_u128(output, value) != 0 || fputc('\n', output) == EOF ||
            fflush(output) != 0 || fsync(fileno(output)) != 0) {
            die("cannot write b-file part file");
        }
        print_term(n, value);
    }
    if (fclose(output) != 0) die("cannot close b-file part file");
    if (rename(part_path, final_path) != 0) {
        die("cannot atomically install completed b-file");
    }
    sync_parent_directory(final_path);
    fprintf(stderr, "398911_05: wrote actually computed terms n=0..%d to %s\n",
            maximum_n, final_path);
    unlock_bfile(lock_fd);
    free(lock_path);
    free(part_path);
    free(final_path);
}

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s N [--threads T] [--progress SEC]\n"
            "       %s --term N [--threads T] [--split K] "
            "[--slice LO HI] [--progress SEC]\n"
            "       %s --upto N [--threads T] [--progress SEC]\n"
            "       %s --check [N] [--threads T] [--progress SEC]\n"
            "A positional N is shorthand for --upto N.\n",
            program, program, program, program);
}

int main(int argc, char **argv)
{
    RunMode mode = MODE_TERM;
    bool mode_set = false;
    int target = -1;
    unsigned requested_threads = DEFAULT_THREADS;
    unsigned progress_seconds = DEFAULT_PROGRESS_SECONDS;
    unsigned requested_split = 0;
    bool split_set = false;
    uint64_t chunk_lo = 0, chunk_hi = 0;
    bool slice_set = false;

    for (int argument = 1; argument < argc; ++argument) {
        const char *text = argv[argument];
        if (strcmp(text, "--term") == 0 || strcmp(text, "--upto") == 0) {
            if (mode_set || target >= 0 || argument + 1 == argc) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            mode = strcmp(text, "--term") == 0 ? MODE_TERM : MODE_UPTO;
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
        } else if (strcmp(text, "--threads") == 0) {
            if (argument + 1 == argc) die("--threads needs an argument");
            requested_threads = parse_unsigned(
                argv[++argument], 1, MAX_THREADS, "threads");
        } else if (strcmp(text, "--progress") == 0) {
            if (argument + 1 == argc) die("--progress needs an argument");
            progress_seconds = parse_unsigned(
                argv[++argument], 0, MAX_PROGRESS_SECONDS,
                "progress seconds");
        } else if (strcmp(text, "--split") == 0) {
            if (split_set || argument + 1 == argc) die("invalid --split");
            requested_split = parse_unsigned(
                argv[++argument], 1, MAX_ELEMENTS, "split");
            split_set = true;
        } else if (strcmp(text, "--slice") == 0) {
            if (slice_set || argument + 2 >= argc) die("invalid --slice");
            chunk_lo = parse_u64(argv[++argument], "slice LO");
            chunk_hi = parse_u64(argv[++argument], "slice HI");
            slice_set = true;
        } else if (strcmp(text, "--help") == 0 || strcmp(text, "-h") == 0) {
            usage(argv[0]);
            return EXIT_SUCCESS;
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
    if ((split_set || slice_set) && mode != MODE_TERM) {
        die("--split and --slice are valid only with --term");
    }
    if (slice_set && !split_set) die("--slice requires an explicit --split");
    if (target == 0 && (split_set || slice_set)) {
        die("--split and --slice are invalid for n=0");
    }

    if (mode == MODE_UPTO) {
        produce_bfile(argv[0], target, requested_threads, progress_seconds);
        return EXIT_SUCCESS;
    }
    if (mode == MODE_CHECK) {
        for (int n = MIN_N; n <= target; ++n) {
            const unsigned split = n == 0 ? 0 : default_split_for_n(n);
            const U128 reflected = compute_complete(
                n, requested_threads, 0, split, true, false);
            if (n <= 6) {
                const U128 plain = compute_complete(
                    n, requested_threads, 0, split, false, false);
                if (plain != reflected) {
                    die("reflection self-check mismatch");
                }
            }
        }
        printf("ok: reflected Gray-code inclusion-exclusion agrees with "
               "A398911 for n=0..%d; reflection checked through n=%d\n",
               target, target < 6 ? target : 6);
        return EXIT_SUCCESS;
    }

    if (target == 0) {
        print_term(0, 1);
        return EXIT_SUCCESS;
    }
    const unsigned element_count = 3U * (unsigned)target;
    const unsigned split = split_set
                               ? requested_split
                               : default_split_for_n(target);
    if (split > element_count) {
        fprintf(stderr, "error: split must be in 1..%u for n=%d\n",
                element_count, target);
        return EXIT_FAILURE;
    }
    const uint64_t chunks = UINT64_C(1) << split;
    if (!slice_set) {
        chunk_lo = 0;
        chunk_hi = chunks;
    } else if (chunk_lo >= chunk_hi || chunk_hi > chunks) {
        fprintf(stderr, "error: require 0 <= LO < HI <= %" PRIu64 "\n",
                chunks);
        return EXIT_FAILURE;
    }

    SearchStats stats;
    unsigned workers;
    const bool complete = chunk_lo == 0 && chunk_hi == chunks;
    const U128 value = compute_range(
        target, requested_threads, progress_seconds, split,
        chunk_lo, chunk_hi, true, complete, true, &stats, &workers);
    if (slice_set) {
        printf("PARTIAL n=%d split=%u lo=%" PRIu64 " hi=%" PRIu64
               " value=%016" PRIx64 "%016" PRIx64 "\n",
               target, split, chunk_lo, chunk_hi,
               (uint64_t)(value >> 64), (uint64_t)value);
    } else {
        print_term(target, value);
    }
    return EXIT_SUCCESS;
}
