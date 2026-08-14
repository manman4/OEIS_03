/*
 * A398911 -- vertex inclusion-exclusion in Gray-code order.
 *
 * For V={1,...,3*n}, let
 *
 *   c_s(S) = #{ {x,y,z} subset S : x<y<z and x+y+z=s }.
 *
 * If e_n denotes the n-th elementary symmetric polynomial, then
 *
 *   a(n) = sum_{S subset V} (-1)^(3*n-|S|) e_n(c_6(S),c_7(S),...).
 *
 * Indeed, e_n chooses n triples having distinct sums.  Inclusion-exclusion
 * removes every choice whose union is not V.  Since n triples have exactly
 * 3*n element occurrences, a choice whose union is V is automatically a
 * partition into disjoint triples.
 *
 * Subsets are visited in binary-reflected Gray-code order.  When one element
 * v is toggled, only triples {v,y,z} with y,z in the remaining subset change.
 * Their counts are aggregated by sum.  If one factor (1+old*t) changes to
 * (1+new*t), the elementary-symmetric coefficients are updated exactly by
 * first dividing by the old linear factor and then multiplying by the new
 * one, both truncated after degree n.
 *
 * Gray-code intervals are distributed among pthread workers.  Each worker
 * owns all mutable state and accumulates positive and negative terms
 * separately as U128 values; the only shared mutable objects are atomic task
 * and progress counters.  Before searching, the rigorous bound
 *
 *        2^(3*n-1) * binomial(binomial(3*n,3), n)
 *
 * is checked to fit U128.  It bounds each sign accumulator.  The final answer
 * is also checked against the unrestricted triple-partition count, which fits
 * uint64_t for n<=9.  All arithmetic that contributes to the answer is
 * checked.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic -pthread \
 *       398911_04.c -o 398911_04
 *
 * Usage:
 *   ./398911_04 --term 9 --threads 4
 *   ./398911_04 --upto 9 --start 9 --threads 4
 *   ./398911_04 --check 6 --threads 4
 *
 * A positional N is shorthand for --upto N.  --upto writes b398911_4.txt
 * beside the executable through interruption-safe b398911_4_part.txt.
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
#error "398911_04 requires unsigned __int128"
#endif

__extension__ typedef unsigned __int128 U128;

#define MIN_N 0
#define MAX_N 9
#define KNOWN_MAX_N 8
#define DEFAULT_CHECK_N 5
#define DEFAULT_THREADS 4
#define MAX_THREADS 32
#define DEFAULT_PROGRESS_SECONDS 30
#define MAX_PROGRESS_SECONDS 3600
#define TASK_LOG2 17
#define TASK_SIZE (UINT64_C(1) << TASK_LOG2)
#define MAX_ELEMENT_COUNT (3 * MAX_N)
#define MAX_TRIPLE_SUM (9 * MAX_N - 3)

typedef enum {
    MODE_TERM,
    MODE_UPTO,
    MODE_CHECK
} RunMode;

typedef struct {
    uint64_t bits;
    unsigned sum;
} Triple;

typedef struct {
    uint64_t subsets;
    uint64_t toggles;
    uint64_t pairs_examined;
    uint64_t changed_sum_factors;
} WorkerStats;

typedef struct {
    int n;
    unsigned element_count;
    unsigned maximum_sum;
    uint64_t total_subsets;
    unsigned triple_count;
    const Triple *triples;
    _Atomic uint64_t next_start;
    _Atomic uint64_t completed_subsets;
} TaskQueue;

typedef struct {
    TaskQueue *queue;
    U128 positive;
    U128 negative;
    WorkerStats stats;
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
    UINT64_C(4843191888898)
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

static void add_u64_saturating(uint64_t *destination, uint64_t addend)
{
    *destination = *destination > UINT64_MAX - addend
                       ? UINT64_MAX
                       : *destination + addend;
}

static U128 checked_add_u128(U128 left, U128 right)
{
    if (left > ~(U128)0 - right) {
        die("U128 accumulator overflow");
    }
    return left + right;
}

static U128 checked_multiply_small(U128 value, unsigned factor)
{
    if (factor != 0 && value > ~(U128)0 / factor) {
        die("U128 polynomial coefficient overflow");
    }
    return value * factor;
}

static U128 choose_u128(unsigned n, unsigned k)
{
    if (k > n) {
        return 0;
    }
    if (k > n - k) {
        k = n - k;
    }
    U128 value = 1;
    for (unsigned index = 1; index <= k; ++index) {
        unsigned factor = n - k + index;
        value = checked_multiply_small(value, factor);
        value /= index;
    }
    return value;
}

static uint64_t partition_upper_bound(int n)
{
    uint64_t bound = 1;
    for (unsigned triples = 1; triples <= (unsigned)n; ++triples) {
        uint64_t a = 3U * triples - 1U;
        uint64_t b = 3U * triples - 2U;
        uint64_t factor = (a * b) / 2U;
        if (bound > UINT64_MAX / factor) {
            die("triple-partition bound exceeds uint64_t");
        }
        bound *= factor;
    }
    return bound;
}

static U128 accumulator_upper_bound(unsigned element_count,
                                    unsigned triple_count, int n)
{
    if (element_count == 0) {
        return 1;
    }
    U128 selections = choose_u128(triple_count, (unsigned)n);
    U128 subsets_per_sign = (U128)1 << (element_count - 1U);
    if (selections > ~(U128)0 / subsets_per_sign) {
        die("inclusion-exclusion accumulator bound exceeds U128");
    }
    return selections * subsets_per_sign;
}

static Triple *make_triples(unsigned element_count,
                            unsigned *triple_count)
{
    *triple_count =
        element_count * (element_count - 1U) * (element_count - 2U) / 6U;
    Triple *triples = calloc(*triple_count, sizeof(*triples));
    if (triples == NULL) {
        die("cannot allocate triple list");
    }
    unsigned index = 0;
    for (unsigned x = 1; x + 2 <= element_count; ++x) {
        for (unsigned y = x + 1; y + 1 <= element_count; ++y) {
            for (unsigned z = y + 1; z <= element_count; ++z) {
                triples[index++] = (Triple){
                    .bits = (UINT64_C(1) << (x - 1U)) |
                            (UINT64_C(1) << (y - 1U)) |
                            (UINT64_C(1) << (z - 1U)),
                    .sum = x + y + z
                };
            }
        }
    }
    if (index != *triple_count) {
        die("internal triple-list size mismatch");
    }
    return triples;
}

static void initialize_counts_and_polynomial(
    const TaskQueue *queue, uint64_t subset,
    unsigned counts[MAX_TRIPLE_SUM + 1],
    U128 elementary[MAX_N + 1])
{
    memset(counts, 0,
           (MAX_TRIPLE_SUM + 1) * sizeof(*counts));
    for (unsigned index = 0; index < queue->triple_count; ++index) {
        const Triple *triple = &queue->triples[index];
        if ((triple->bits & ~subset) == 0) {
            ++counts[triple->sum];
        }
    }

    memset(elementary, 0, (MAX_N + 1) * sizeof(*elementary));
    elementary[0] = 1;
    for (unsigned sum = 0; sum <= queue->maximum_sum; ++sum) {
        unsigned count = counts[sum];
        if (count == 0) {
            continue;
        }
        for (int degree = queue->n; degree >= 1; --degree) {
            U128 product = checked_multiply_small(
                elementary[degree - 1], count);
            elementary[degree] = checked_add_u128(
                elementary[degree], product);
        }
    }
}

static void replace_linear_factor(
    U128 elementary[MAX_N + 1], int n,
    unsigned old_count, unsigned new_count)
{
    U128 quotient[MAX_N + 1];
    quotient[0] = 1;
    for (int degree = 1; degree <= n; ++degree) {
        U128 product = checked_multiply_small(
            quotient[degree - 1], old_count);
        if (elementary[degree] < product) {
            die("internal polynomial division underflow");
        }
        quotient[degree] = elementary[degree] - product;
    }
    elementary[0] = 1;
    for (int degree = 1; degree <= n; ++degree) {
        U128 product = checked_multiply_small(
            quotient[degree - 1], new_count);
        elementary[degree] = checked_add_u128(
            quotient[degree], product);
    }
}

static void toggle_element(
    const TaskQueue *queue, uint64_t *subset, unsigned element,
    unsigned counts[MAX_TRIPLE_SUM + 1],
    U128 elementary[MAX_N + 1], WorkerStats *stats)
{
    uint64_t element_bit = UINT64_C(1) << (element - 1U);
    bool adding = (*subset & element_bit) == 0;
    uint64_t remaining;
    if (adding) {
        remaining = *subset;
    } else {
        *subset ^= element_bit;
        remaining = *subset;
    }

    int changes[MAX_TRIPLE_SUM + 1] = {0};
    unsigned touched[MAX_TRIPLE_SUM + 1];
    unsigned touched_count = 0;
    uint64_t first_choices = remaining;
    while (first_choices != 0) {
        uint64_t first_bit =
            first_choices & (UINT64_C(0) - first_choices);
        unsigned first =
            (unsigned)__builtin_ctzll(first_bit) + 1U;
        uint64_t second_choices = first_choices ^ first_bit;
        while (second_choices != 0) {
            uint64_t second_bit =
                second_choices & (UINT64_C(0) - second_choices);
            unsigned second =
                (unsigned)__builtin_ctzll(second_bit) + 1U;
            unsigned sum = element + first + second;
            if (changes[sum] == 0) {
                touched[touched_count++] = sum;
            }
            changes[sum] += adding ? 1 : -1;
            second_choices ^= second_bit;
        }
        first_choices ^= first_bit;
    }

    unsigned remaining_count = (unsigned)__builtin_popcountll(remaining);
    add_u64_saturating(
        &stats->pairs_examined,
        (uint64_t)remaining_count * (remaining_count - 1U) / 2U);
    add_u64_saturating(&stats->changed_sum_factors, touched_count);

    for (unsigned index = 0; index < touched_count; ++index) {
        unsigned sum = touched[index];
        unsigned old_count = counts[sum];
        int signed_new = (int)old_count + changes[sum];
        if (signed_new < 0) {
            die("internal negative triple count");
        }
        unsigned new_count = (unsigned)signed_new;
        replace_linear_factor(
            elementary, queue->n, old_count, new_count);
        counts[sum] = new_count;
    }

    if (adding) {
        *subset |= element_bit;
    }
}

static void accumulate_term(Worker *worker, U128 value, bool positive)
{
    if (positive) {
        worker->positive = checked_add_u128(worker->positive, value);
    } else {
        worker->negative = checked_add_u128(worker->negative, value);
    }
}

static void process_interval(Worker *worker, uint64_t begin, uint64_t end)
{
    TaskQueue *queue = worker->queue;
    uint64_t subset = begin ^ (begin >> 1);
    unsigned counts[MAX_TRIPLE_SUM + 1];
    U128 elementary[MAX_N + 1];
    initialize_counts_and_polynomial(
        queue, subset, counts, elementary);
    bool positive =
        ((queue->element_count -
          (unsigned)__builtin_popcountll(subset)) & 1U) == 0;
    accumulate_term(worker, elementary[queue->n], positive);

    for (uint64_t index = begin + 1; index < end; ++index) {
        unsigned element = (unsigned)__builtin_ctzll(index) + 1U;
        toggle_element(queue, &subset, element,
                       counts, elementary, &worker->stats);
        positive = !positive;
        accumulate_term(worker, elementary[queue->n], positive);
    }
    uint64_t length = end - begin;
    add_u64_saturating(&worker->stats.subsets, length);
    add_u64_saturating(&worker->stats.toggles, length - 1U);
}

static void *worker_main(void *argument)
{
    Worker *worker = argument;
    TaskQueue *queue = worker->queue;
    for (;;) {
        uint64_t begin = atomic_fetch_add_explicit(
            &queue->next_start, TASK_SIZE, memory_order_relaxed);
        if (begin >= queue->total_subsets) {
            break;
        }
        uint64_t end = begin + TASK_SIZE;
        if (end > queue->total_subsets) {
            end = queue->total_subsets;
        }
        process_interval(worker, begin, end);
        atomic_fetch_add_explicit(
            &queue->completed_subsets, end - begin,
            memory_order_release);
    }
    return NULL;
}

static void report_progress(int n, const TaskQueue *queue, double start,
                            uint64_t *previous_completed,
                            double *previous_time, bool terminal,
                            bool final)
{
    uint64_t completed = atomic_load_explicit(
        &queue->completed_subsets, memory_order_acquire);
    double now = monotonic_seconds();
    double interval = now - *previous_time;
    double rate = interval > 0.0
                      ? (double)(completed - *previous_completed) /
                            interval / 1000000.0
                      : 0.0;
    double percent = 100.0 * (double)completed /
                     (double)queue->total_subsets;
    fprintf(stderr,
            terminal ? "\r398911_04: n=%d progress subsets=%" PRIu64
                       "/%" PRIu64 " (%5.1f%%), rate=%.2f M/s, "
                       "elapsed=%.1f s"
                     : "398911_04: n=%d progress subsets=%" PRIu64
                       "/%" PRIu64 " (%5.1f%%), rate=%.2f M/s, "
                       "elapsed=%.1f s\n",
            n, completed, queue->total_subsets, percent, rate,
            now - start);
    if (terminal && final) {
        fputc('\n', stderr);
    }
    fflush(stderr);
    *previous_completed = completed;
    *previous_time = now;
}

static uint64_t compute_term(int n, unsigned requested_threads,
                             unsigned progress_seconds, WorkerStats *stats,
                             unsigned *actual_workers)
{
    if (n < MIN_N || n > MAX_N || 3 * n >= 64) {
        die("unsupported dimension in Gray-code inclusion-exclusion");
    }
    uint64_t unrestricted_bound = partition_upper_bound(n);
    memset(stats, 0, sizeof(*stats));
    *actual_workers = 1;
    if (n == 0) {
        stats->subsets = 1;
        return 1;
    }

    unsigned element_count = 3U * (unsigned)n;
    unsigned triple_count = 0;
    Triple *triples = make_triples(element_count, &triple_count);
    U128 sign_bound = accumulator_upper_bound(
        element_count, triple_count, n);
    uint64_t total_subsets = UINT64_C(1) << element_count;
    uint64_t task_count =
        (total_subsets + TASK_SIZE - 1U) / TASK_SIZE;
    unsigned workers = requested_threads;
    if ((uint64_t)workers > task_count) {
        workers = (unsigned)task_count;
    }

    TaskQueue queue = {
        .n = n,
        .element_count = element_count,
        .maximum_sum = 3U * element_count - 3U,
        .total_subsets = total_subsets,
        .triple_count = triple_count,
        .triples = triples,
        .next_start = 0,
        .completed_subsets = 0
    };
    Worker *worker = calloc(workers, sizeof(*worker));
    pthread_t *threads = calloc(workers, sizeof(*threads));
    if (worker == NULL || threads == NULL) {
        free(threads);
        free(worker);
        free(triples);
        die("cannot allocate inclusion-exclusion workers");
    }
    for (unsigned index = 0; index < workers; ++index) {
        worker[index].queue = &queue;
        if (pthread_create(&threads[index], NULL, worker_main,
                           &worker[index]) != 0) {
            die("pthread_create failed");
        }
    }

    bool terminal = isatty(fileno(stderr)) != 0;
    bool progress_printed = false;
    double start = monotonic_seconds();
    double previous_time = start;
    double next_report = start + (double)progress_seconds;
    uint64_t previous_completed = 0;
    while (atomic_load_explicit(&queue.completed_subsets,
                                memory_order_acquire) < total_subsets) {
        double now = monotonic_seconds();
        if (progress_seconds != 0 && now >= next_report) {
            report_progress(n, &queue, start, &previous_completed,
                            &previous_time, terminal, false);
            progress_printed = true;
            next_report = now + (double)progress_seconds;
        }
        struct timespec pause = {.tv_sec = 0, .tv_nsec = 20000000L};
        while (nanosleep(&pause, &pause) != 0 && errno == EINTR) {
            /* Resume the short progress-monitoring sleep. */
        }
    }
    if (progress_printed) {
        report_progress(n, &queue, start, &previous_completed,
                        &previous_time, terminal, true);
    }
    for (unsigned index = 0; index < workers; ++index) {
        if (pthread_join(threads[index], NULL) != 0) {
            die("pthread_join failed");
        }
    }

    U128 positive = 0;
    U128 negative = 0;
    for (unsigned index = 0; index < workers; ++index) {
        positive = checked_add_u128(positive, worker[index].positive);
        negative = checked_add_u128(negative, worker[index].negative);
        add_u64_saturating(&stats->subsets,
                           worker[index].stats.subsets);
        add_u64_saturating(&stats->toggles,
                           worker[index].stats.toggles);
        add_u64_saturating(&stats->pairs_examined,
                           worker[index].stats.pairs_examined);
        add_u64_saturating(&stats->changed_sum_factors,
                           worker[index].stats.changed_sum_factors);
    }
    if (positive > sign_bound || negative > sign_bound) {
        die("inclusion-exclusion accumulator exceeded proven bound");
    }
    if (positive < negative) {
        die("inclusion-exclusion result is unexpectedly negative");
    }
    U128 answer = positive - negative;
    if (answer > unrestricted_bound || answer > UINT64_MAX) {
        die("A398911 result exceeded the unrestricted partition bound");
    }

    free(threads);
    free(worker);
    free(triples);
    *actual_workers = workers;
    return (uint64_t)answer;
}

static void verify_known(uint64_t value, int n)
{
    if (n >= MIN_N && n <= KNOWN_MAX_N && value != known_terms[n]) {
        fprintf(stderr,
                "error: A398911 mismatch at n=%d: got %" PRIu64
                ", expected %" PRIu64 "\n",
                n, value, known_terms[n]);
        exit(EXIT_FAILURE);
    }
}

static uint64_t compute_checked(int n, unsigned requested_threads,
                                unsigned progress_seconds, bool report)
{
    double start = monotonic_seconds();
    WorkerStats stats;
    unsigned workers = 1;
    uint64_t value = compute_term(
        n, requested_threads, report ? progress_seconds : 0,
        &stats, &workers);
    verify_known(value, n);
    if (report) {
        fprintf(stderr,
                "398911_04: n=%d, Gray-code inclusion-exclusion, "
                "subsets=%" PRIu64 ", pairs=%" PRIu64
                ", factor-updates=%" PRIu64 ", %u worker%s, %.3f s\n",
                n, stats.subsets, stats.pairs_examined,
                stats.changed_sum_factors, workers,
                workers == 1 ? "" : "s",
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

static void produce_b_file(const char *argv0, int maximum_n, int start_n,
                           unsigned requested_threads,
                           unsigned progress_seconds)
{
    if (start_n < MIN_N || start_n > maximum_n + 1 ||
        start_n > KNOWN_MAX_N + 1) {
        fprintf(stderr,
                "error: start N must be in %d..min(UPTO_N+1,%d)\n",
                MIN_N, KNOWN_MAX_N + 1);
        exit(EXIT_FAILURE);
    }
    char *final_path = path_beside_executable(argv0, "b398911_4.txt");
    char *part_path =
        path_beside_executable(argv0, "b398911_4_part.txt");
    FILE *output = fopen(part_path, "w");
    if (output == NULL) {
        fprintf(stderr, "error: cannot open %s: %s\n",
                part_path, strerror(errno));
        free(part_path);
        free(final_path);
        exit(EXIT_FAILURE);
    }

    for (int n = MIN_N; n < start_n && n <= maximum_n; ++n) {
        if (fprintf(output, "%d %" PRIu64 "\n",
                    n, known_terms[n]) < 0) {
            die("could not write the built-in A398911 prefix");
        }
    }
    flush_b_file(output, part_path);
    if (start_n > MIN_N) {
        fprintf(stderr,
                "398911_04: using built-in verified prefix n=%d..%d\n",
                MIN_N, start_n - 1);
    }

    for (int n = start_n; n <= maximum_n; ++n) {
        uint64_t value = compute_checked(
            n, requested_threads, progress_seconds, true);
        if (fprintf(output, "%d %" PRIu64 "\n", n, value) < 0) {
            die("could not write the A398911 b-file");
        }
        flush_b_file(output, part_path);
        printf("%d %" PRIu64 "\n", n, value);
    }

    if (fclose(output) != 0) {
        die("could not close the A398911 b-file");
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
            "usage: %s N [--start S] [--threads T] [--progress SEC]\n"
            "       %s --term N [--threads T] [--progress SEC]\n"
            "       %s --upto N [--start S] [--threads T] "
            "[--progress SEC]\n"
            "       %s --check [N] [--threads T] [--progress SEC]\n",
            program, program, program, program);
}

int main(int argc, char **argv)
{
    RunMode mode = MODE_TERM;
    bool mode_set = false;
    int target = -1;
    int start_n = MIN_N;
    bool start_set = false;
    unsigned requested_threads = DEFAULT_THREADS;
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
            requested_threads = parse_unsigned(
                argv[++argument], 1, MAX_THREADS, "threads");
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
        uint64_t value = compute_checked(
            target, requested_threads, progress_seconds, true);
        printf("%d %" PRIu64 "\n", target, value);
    } else if (mode == MODE_UPTO) {
        produce_b_file(argv[0], target, start_n,
                       requested_threads, progress_seconds);
    } else {
        for (int n = MIN_N; n <= target; ++n) {
            (void)compute_checked(
                n, requested_threads, progress_seconds, false);
        }
        printf("ok: Gray-code inclusion-exclusion agrees with A398911 for "
               "n=%d..%d\n", MIN_N, target);
    }
    return EXIT_SUCCESS;
}
