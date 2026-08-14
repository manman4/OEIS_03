/*
 * A398911 -- exact subset DP grouped by triple sum.
 *
 * Count partitions of {1,...,3*n} into unordered triples whose sums are all
 * different.  This program uses the coefficient identity
 *
 *   product over sums s of
 *       (1 + sum_{x<y<z, x+y+z=s} X_x X_y X_z).
 *
 * The required answer is the coefficient of X_1*...*X_(3*n), with products
 * containing X_i^2 discarded.  Processing one factor per sum makes it
 * impossible to select two triples having the same sum.
 *
 * dp[mask] is the number of ways to cover exactly mask after all smaller sum
 * classes have been processed.  For one sum class, every worker reads the
 * unchanged dp array and writes to a private delta array.  After all workers
 * finish, disjoint mask ranges merge the deltas into dp.  Consequently there
 * are no concurrent writes to the same object and no hash table or recursive
 * search is used.  When at least two tables fit, private delta tables
 * parallelize a sum class.  When only one table fits (notably n=9 at the
 * default memory limit), a one-table variant processes subset-cardinality
 * layers in descending order.  A source layer is therefore still unchanged
 * when read, so this update also selects at most one triple from each sum.
 *
 * The unrestricted number of triple partitions is checked to fit uint64_t.
 * It bounds every DP entry.  Every DP addition is checked as well.  At n=9
 * the dense uint64_t table is 1024 MiB.  --memory-mib limits the DP arrays and
 * may reduce the actual number of workers below --threads.  A one-table
 * calculation is deliberately single-threaded to avoid concurrent updates;
 * with enough memory the private-delta parallel variant is selected instead.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic -pthread \
 *       398911_03.c -o 398911_03
 *
 * Usage:
 *   ./398911_03 --term 7 --threads 4 --memory-mib 256
 *   ./398911_03 --upto 7 --start 7 --threads 4 --memory-mib 256
 *   ./398911_03 --upto 9 --start 7 --memory-mib 1024
 *   ./398911_03 --check 6 --threads 4 --memory-mib 64
 *
 * A positional N is shorthand for --upto N.  --upto writes b398911_3.txt
 * beside the executable through interruption-safe b398911_3_part.txt.
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

#define MIN_N 0
#define MAX_N 9
#define KNOWN_MAX_N 6
#define DEFAULT_CHECK_N 5
#define DEFAULT_THREADS 4
#define MAX_THREADS 32
#define DEFAULT_MEMORY_MIB 1024
#define MIN_MEMORY_MIB 1
#define MAX_MEMORY_MIB 8192
#define DEFAULT_PROGRESS_SECONDS 30
#define MAX_PROGRESS_SECONDS 3600
#define MAX_TRIPLE_SUM (9 * MAX_N - 3)

typedef enum {
    MODE_TERM,
    MODE_UPTO,
    MODE_CHECK
} RunMode;

typedef struct {
    uint64_t bits;
} Triple;

typedef struct {
    Triple *triples;
    size_t offsets[MAX_TRIPLE_SUM + 2];
    unsigned total_triples;
    unsigned nonempty_groups;
    unsigned maximum_sum;
    unsigned maximum_group_size;
} TripleTable;

typedef struct {
    const uint64_t *dp;
    uint64_t *delta;
    const Triple *triples;
    size_t triple_count;
    unsigned worker_index;
    unsigned worker_count;
    uint64_t full_mask;
    uint64_t nonzero_sources;
} UpdateJob;

typedef struct {
    uint64_t *dp;
    uint64_t **deltas;
    unsigned delta_count;
    size_t begin;
    size_t end;
} ReduceJob;

typedef struct {
    uint64_t subset_probes;
    uint64_t nonzero_sources;
    unsigned sum_groups;
    unsigned triples;
} DpStats;

static const uint64_t known_terms[KNOWN_MAX_N + 1] = {
    UINT64_C(1),
    UINT64_C(1),
    UINT64_C(10),
    UINT64_C(246),
    UINT64_C(12184),
    UINT64_C(995001),
    UINT64_C(122605952)
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

static uint64_t checked_add(uint64_t left, uint64_t right)
{
    if (left > UINT64_MAX - right) {
        die("A398911 DP count overflowed uint64_t");
    }
    return left + right;
}

static void add_stat_saturating(uint64_t *destination, uint64_t addend)
{
    *destination = *destination > UINT64_MAX - addend
                       ? UINT64_MAX
                       : *destination + addend;
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

static TripleTable make_triple_table(unsigned element_count)
{
    TripleTable table;
    memset(&table, 0, sizeof(table));
    table.maximum_sum = 3U * element_count - 3U;

    size_t counts[MAX_TRIPLE_SUM + 1] = {0};
    for (unsigned x = 1; x + 2 <= element_count; ++x) {
        for (unsigned y = x + 1; y + 1 <= element_count; ++y) {
            for (unsigned z = y + 1; z <= element_count; ++z) {
                ++counts[x + y + z];
                ++table.total_triples;
            }
        }
    }

    for (unsigned sum = 0; sum <= table.maximum_sum; ++sum) {
        table.offsets[sum + 1] = table.offsets[sum] + counts[sum];
        if (counts[sum] != 0) {
            ++table.nonempty_groups;
            if (counts[sum] > table.maximum_group_size) {
                table.maximum_group_size = (unsigned)counts[sum];
            }
        }
    }
    table.triples = calloc(table.total_triples, sizeof(*table.triples));
    if (table.triples == NULL) {
        die("cannot allocate triple table");
    }

    size_t cursor[MAX_TRIPLE_SUM + 1];
    memcpy(cursor, table.offsets, sizeof(cursor));
    for (unsigned x = 1; x + 2 <= element_count; ++x) {
        for (unsigned y = x + 1; y + 1 <= element_count; ++y) {
            for (unsigned z = y + 1; z <= element_count; ++z) {
                unsigned sum = x + y + z;
                Triple triple = {
                    .bits = (UINT64_C(1) << (x - 1U)) |
                            (UINT64_C(1) << (y - 1U)) |
                            (UINT64_C(1) << (z - 1U))
                };
                table.triples[cursor[sum]++] = triple;
            }
        }
    }
    if (table.offsets[table.maximum_sum + 1] != table.total_triples) {
        die("internal triple-table size mismatch");
    }
    return table;
}

static void destroy_triple_table(TripleTable *table)
{
    free(table->triples);
    memset(table, 0, sizeof(*table));
}

static void *update_worker_main(void *argument)
{
    UpdateJob *job = argument;
    uint64_t local_nonzero = 0;
    for (size_t index = job->worker_index;
         index < job->triple_count;
         index += job->worker_count) {
        uint64_t triple = job->triples[index].bits;
        uint64_t complement = job->full_mask ^ triple;
        uint64_t subset = complement;
        for (;;) {
            uint64_t value = job->dp[subset];
            if (value != 0) {
                size_t destination = (size_t)(subset | triple);
                job->delta[destination] = checked_add(
                    job->delta[destination], value);
                if (local_nonzero != UINT64_MAX) {
                    ++local_nonzero;
                }
            }
            if (subset == 0) {
                break;
            }
            subset = (subset - 1) & complement;
        }
    }
    job->nonzero_sources = local_nonzero;
    return NULL;
}

static void *reduce_worker_main(void *argument)
{
    ReduceJob *job = argument;
    for (size_t mask = job->begin; mask < job->end; ++mask) {
        uint64_t value = job->dp[mask];
        for (unsigned worker = 0; worker < job->delta_count; ++worker) {
            value = checked_add(value, job->deltas[worker][mask]);
            job->deltas[worker][mask] = 0;
        }
        job->dp[mask] = value;
    }
    return NULL;
}

static uint64_t run_sum_group(const Triple *triples, size_t triple_count,
                              uint64_t full_mask, uint64_t *dp,
                              size_t state_count, uint64_t **deltas,
                              unsigned workers, pthread_t *threads,
                              UpdateJob *update_jobs,
                              ReduceJob *reduce_jobs)
{
    unsigned group_workers = workers;
    if ((size_t)group_workers > triple_count) {
        group_workers = (unsigned)triple_count;
    }

    for (unsigned worker = 0; worker < group_workers; ++worker) {
        update_jobs[worker] = (UpdateJob){
            .dp = dp,
            .delta = deltas[worker],
            .triples = triples,
            .triple_count = triple_count,
            .worker_index = worker,
            .worker_count = group_workers,
            .full_mask = full_mask,
            .nonzero_sources = 0
        };
        if (pthread_create(&threads[worker], NULL, update_worker_main,
                           &update_jobs[worker]) != 0) {
            die("pthread_create failed during DP update");
        }
    }
    for (unsigned worker = 0; worker < group_workers; ++worker) {
        if (pthread_join(threads[worker], NULL) != 0) {
            die("pthread_join failed during DP update");
        }
    }

    uint64_t nonzero_sources = 0;
    for (unsigned worker = 0; worker < group_workers; ++worker) {
        add_stat_saturating(&nonzero_sources,
                            update_jobs[worker].nonzero_sources);
    }

    for (unsigned worker = 0; worker < workers; ++worker) {
        size_t begin = state_count * worker / workers;
        size_t end = state_count * (worker + 1U) / workers;
        reduce_jobs[worker] = (ReduceJob){
            .dp = dp,
            .deltas = deltas,
            .delta_count = group_workers,
            .begin = begin,
            .end = end
        };
        if (pthread_create(&threads[worker], NULL, reduce_worker_main,
                           &reduce_jobs[worker]) != 0) {
            die("pthread_create failed during DP reduction");
        }
    }
    for (unsigned worker = 0; worker < workers; ++worker) {
        if (pthread_join(threads[worker], NULL) != 0) {
            die("pthread_join failed during DP reduction");
        }
    }
    return nonzero_sources;
}

static uint64_t binomial_small(unsigned n, unsigned k)
{
    if (k > n) {
        return 0;
    }
    if (k > n - k) {
        k = n - k;
    }
    uint64_t value = 1;
    for (unsigned index = 1; index <= k; ++index) {
        value = value * (n - k + index) / index;
    }
    return value;
}

static uint64_t update_from_fixed_size_sources(
    uint64_t *dp, uint64_t triple, uint64_t full_mask,
    unsigned source_size)
{
    uint64_t complement = full_mask ^ triple;
    uint64_t position_bits[3 * MAX_N];
    unsigned available = 0;
    uint64_t remaining = complement;
    while (remaining != 0) {
        uint64_t bit = remaining & (UINT64_C(0) - remaining);
        position_bits[available++] = bit;
        remaining ^= bit;
    }
    if (source_size > available) {
        return 0;
    }

    if (source_size == 0) {
        uint64_t value = dp[0];
        if (value != 0) {
            dp[(size_t)triple] = checked_add(
                dp[(size_t)triple], value);
            return 1;
        }
        return 0;
    }

    unsigned indices[3 * MAX_N];
    uint64_t subset = 0;
    for (unsigned index = 0; index < source_size; ++index) {
        indices[index] = index;
        subset |= position_bits[index];
    }

    uint64_t nonzero_sources = 0;
    for (;;) {
        uint64_t value = dp[(size_t)subset];
        if (value != 0) {
            size_t destination = (size_t)(subset | triple);
            dp[destination] = checked_add(dp[destination], value);
            if (nonzero_sources != UINT64_MAX) {
                ++nonzero_sources;
            }
        }

        int change = (int)source_size - 1;
        while (change >= 0 &&
               indices[change] ==
                   available - source_size + (unsigned)change) {
            --change;
        }
        if (change < 0) {
            break;
        }
        for (unsigned index = (unsigned)change;
             index < source_size; ++index) {
            subset &= ~position_bits[indices[index]];
        }
        ++indices[change];
        for (unsigned index = (unsigned)change + 1;
             index < source_size; ++index) {
            indices[index] = indices[index - 1] + 1U;
        }
        for (unsigned index = (unsigned)change;
             index < source_size; ++index) {
            subset |= position_bits[indices[index]];
        }
    }
    return nonzero_sources;
}

static void run_sum_group_in_place(
    const Triple *triples, size_t triple_count,
    unsigned element_count, uint64_t full_mask, uint64_t *dp,
    DpStats *stats)
{
    unsigned available = element_count - 3U;
    uint64_t probes_per_triple = 0;

    /*
     * A destination has three more elements than its source.  Descending
     * source cardinality means no source layer has yet been changed by this
     * sum class, even after earlier triples in the same class were handled.
     */
    for (int source_size = (int)available;
         source_size >= 0; source_size -= 3) {
        unsigned source = (unsigned)source_size;
        add_stat_saturating(
            &probes_per_triple,
            binomial_small(available, source));
        for (size_t index = 0; index < triple_count; ++index) {
            uint64_t nonzero = update_from_fixed_size_sources(
                dp, triples[index].bits, full_mask, source);
            add_stat_saturating(&stats->nonzero_sources, nonzero);
        }
    }
    add_stat_saturating(
        &stats->subset_probes,
        probes_per_triple * (uint64_t)triple_count);
}

static void report_progress(int n, const DpStats *stats,
                            unsigned total_groups,
                            unsigned total_triples, double start,
                            bool terminal, bool final)
{
    double percent = total_triples == 0
                         ? 100.0
                         : 100.0 * (double)stats->triples /
                               (double)total_triples;
    double elapsed = monotonic_seconds() - start;
    fprintf(stderr,
            terminal ? "\r398911_03: n=%d progress sums=%u/%u, "
                       "triples=%u/%u (%5.1f%%), elapsed=%.1f s"
                     : "398911_03: n=%d progress sums=%u/%u, "
                       "triples=%u/%u (%5.1f%%), elapsed=%.1f s\n",
            n, stats->sum_groups, total_groups, stats->triples,
            total_triples, percent, elapsed);
    if (terminal && final) {
        fputc('\n', stderr);
    }
    fflush(stderr);
}

static uint64_t compute_in_place_dp(
    int n, unsigned element_count, const TripleTable *table,
    uint64_t full_mask, uint64_t *dp, unsigned progress_seconds,
    bool show_progress, DpStats *stats)
{
    dp[0] = 1;
    double start = monotonic_seconds();
    double next_report = start + (double)progress_seconds;
    bool terminal = isatty(fileno(stderr)) != 0;
    bool progress_printed = false;

    for (unsigned sum = 0; sum <= table->maximum_sum; ++sum) {
        size_t begin = table->offsets[sum];
        size_t end = table->offsets[sum + 1];
        size_t group_size = end - begin;
        if (group_size == 0) {
            continue;
        }
        run_sum_group_in_place(
            &table->triples[begin], group_size, element_count,
            full_mask, dp, stats);
        ++stats->sum_groups;
        stats->triples += (unsigned)group_size;

        double now = monotonic_seconds();
        if (show_progress && progress_seconds != 0 &&
            now >= next_report) {
            report_progress(n, stats, table->nonempty_groups,
                            table->total_triples, start, terminal, false);
            progress_printed = true;
            next_report = now + (double)progress_seconds;
        }
    }
    if (progress_printed) {
        report_progress(n, stats, table->nonempty_groups,
                        table->total_triples, start, terminal, true);
    }
    return dp[(size_t)full_mask];
}

static uint64_t compute_term(int n, unsigned requested_threads,
                             unsigned memory_mib,
                             unsigned progress_seconds, bool show_progress,
                             DpStats *stats, unsigned *actual_workers,
                             size_t *actual_memory_bytes,
                             bool *used_in_place)
{
    if (n < MIN_N || n > MAX_N || 3 * n >= 64) {
        die("unsupported dimension in subset DP");
    }
    (void)partition_upper_bound(n);
    memset(stats, 0, sizeof(*stats));
    *actual_workers = 1;
    *actual_memory_bytes = 0;
    *used_in_place = false;
    if (n == 0) {
        return 1;
    }

    unsigned element_count = 3U * (unsigned)n;
    size_t state_count = (size_t)1 << element_count;
    if (state_count > SIZE_MAX / sizeof(uint64_t)) {
        die("DP table size overflow");
    }
    size_t array_bytes = state_count * sizeof(uint64_t);
    uint64_t memory_bytes = (uint64_t)memory_mib *
                            UINT64_C(1024) * UINT64_C(1024);
    if ((uint64_t)array_bytes > memory_bytes) {
        uint64_t required_mib =
            ((uint64_t)array_bytes + UINT64_C(1048575)) /
            UINT64_C(1048576);
        fprintf(stderr,
                "error: n=%d needs at least %" PRIu64
                " MiB for subset DP; increase --memory-mib\n",
                n, required_mib);
        exit(EXIT_FAILURE);
    }

    TripleTable table = make_triple_table(element_count);
    uint64_t arrays_allowed = memory_bytes / (uint64_t)array_bytes;
    uint64_t full_mask =
        (UINT64_C(1) << element_count) - UINT64_C(1);
    bool use_in_place = arrays_allowed < 2U;
    if (use_in_place) {
        uint64_t *dp = calloc(state_count, sizeof(*dp));
        if (dp == NULL) {
            destroy_triple_table(&table);
            die("cannot allocate the in-place subset-DP table");
        }
        uint64_t answer = compute_in_place_dp(
            n, element_count, &table, full_mask, dp,
            progress_seconds, show_progress, stats);
        free(dp);
        destroy_triple_table(&table);
        *actual_workers = 1;
        *actual_memory_bytes = array_bytes;
        *used_in_place = true;
        return answer;
    }

    uint64_t workers_allowed = arrays_allowed - 1U;
    unsigned workers = requested_threads;
    if ((uint64_t)workers > workers_allowed) {
        workers = workers_allowed > MAX_THREADS
                      ? MAX_THREADS
                      : (unsigned)workers_allowed;
    }
    if (workers > table.maximum_group_size) {
        workers = table.maximum_group_size;
    }
    if (workers == 0) {
        destroy_triple_table(&table);
        die("memory budget permits no DP worker");
    }

    uint64_t *dp = calloc(state_count, sizeof(*dp));
    uint64_t **deltas = calloc(workers, sizeof(*deltas));
    pthread_t *threads = calloc(workers, sizeof(*threads));
    UpdateJob *update_jobs = calloc(workers, sizeof(*update_jobs));
    ReduceJob *reduce_jobs = calloc(workers, sizeof(*reduce_jobs));
    if (dp == NULL || deltas == NULL || threads == NULL ||
        update_jobs == NULL || reduce_jobs == NULL) {
        die("cannot allocate subset-DP control data");
    }
    for (unsigned worker = 0; worker < workers; ++worker) {
        deltas[worker] = calloc(state_count, sizeof(*deltas[worker]));
        if (deltas[worker] == NULL) {
            die("cannot allocate a worker DP delta table");
        }
    }

    dp[0] = 1;
    double start = monotonic_seconds();
    double next_report = start + (double)progress_seconds;
    bool terminal = isatty(fileno(stderr)) != 0;
    bool progress_printed = false;
    uint64_t probes_per_triple = UINT64_C(1) << (element_count - 3U);

    for (unsigned sum = 0; sum <= table.maximum_sum; ++sum) {
        size_t begin = table.offsets[sum];
        size_t end = table.offsets[sum + 1];
        size_t group_size = end - begin;
        if (group_size == 0) {
            continue;
        }
        uint64_t nonzero = run_sum_group(
            &table.triples[begin], group_size, full_mask,
            dp, state_count, deltas, workers,
            threads, update_jobs, reduce_jobs);
        add_stat_saturating(&stats->nonzero_sources, nonzero);
        add_stat_saturating(
            &stats->subset_probes,
            probes_per_triple * (uint64_t)group_size);
        ++stats->sum_groups;
        stats->triples += (unsigned)group_size;

        double now = monotonic_seconds();
        if (show_progress && progress_seconds != 0 &&
            now >= next_report) {
            report_progress(n, stats, table.nonempty_groups,
                            table.total_triples, start, terminal, false);
            progress_printed = true;
            next_report = now + (double)progress_seconds;
        }
    }
    if (progress_printed) {
        report_progress(n, stats, table.nonempty_groups,
                        table.total_triples, start, terminal, true);
    }

    uint64_t answer = dp[(size_t)full_mask];
    for (unsigned worker = 0; worker < workers; ++worker) {
        free(deltas[worker]);
    }
    free(reduce_jobs);
    free(update_jobs);
    free(threads);
    free(deltas);
    free(dp);
    destroy_triple_table(&table);
    *actual_workers = workers;
    *actual_memory_bytes = (size_t)(workers + 1U) * array_bytes;
    return answer;
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
                                unsigned memory_mib,
                                unsigned progress_seconds, bool report)
{
    double start = monotonic_seconds();
    DpStats stats;
    unsigned workers = 1;
    size_t memory_bytes = 0;
    bool used_in_place = false;
    uint64_t value = compute_term(
        n, requested_threads, memory_mib, progress_seconds, report,
        &stats, &workers, &memory_bytes, &used_in_place);
    verify_known(value, n);
    if (report) {
        fprintf(stderr,
                "398911_03: n=%d, %s sum-group subset DP, "
                "subset-probes=%" PRIu64
                ", nonzero-sources=%" PRIu64 ", %u worker%s, "
                "memory=%.1f MiB, %.3f s\n",
                n, used_in_place ? "in-place" : "parallel",
                stats.subset_probes, stats.nonzero_sources,
                workers, workers == 1 ? "" : "s",
                (double)memory_bytes / (1024.0 * 1024.0),
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
                           unsigned memory_mib,
                           unsigned progress_seconds)
{
    if (start_n < MIN_N || start_n > maximum_n + 1 ||
        start_n > KNOWN_MAX_N + 1) {
        fprintf(stderr,
                "error: start N must be in %d..min(UPTO_N+1,%d)\n",
                MIN_N, KNOWN_MAX_N + 1);
        exit(EXIT_FAILURE);
    }
    char *final_path = path_beside_executable(argv0, "b398911_3.txt");
    char *part_path =
        path_beside_executable(argv0, "b398911_3_part.txt");
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
                "398911_03: using built-in verified prefix n=%d..%d\n",
                MIN_N, start_n - 1);
    }

    for (int n = start_n; n <= maximum_n; ++n) {
        uint64_t value = compute_checked(
            n, requested_threads, memory_mib,
            progress_seconds, true);
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
            "usage: %s N [--start S] [--threads T] [--memory-mib M] "
            "[--progress SEC]\n"
            "       %s --term N [--threads T] [--memory-mib M] "
            "[--progress SEC]\n"
            "       %s --upto N [--start S] [--threads T] "
            "[--memory-mib M] [--progress SEC]\n"
            "       %s --check [N] [--threads T] [--memory-mib M] "
            "[--progress SEC]\n",
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
    unsigned memory_mib = DEFAULT_MEMORY_MIB;
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
        } else if (strcmp(text, "--memory-mib") == 0) {
            if (argument + 1 == argc) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            memory_mib = parse_unsigned(
                argv[++argument], MIN_MEMORY_MIB, MAX_MEMORY_MIB,
                "memory MiB");
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
            target, requested_threads, memory_mib,
            progress_seconds, true);
        printf("%d %" PRIu64 "\n", target, value);
    } else if (mode == MODE_UPTO) {
        produce_b_file(argv[0], target, start_n,
                       requested_threads, memory_mib,
                       progress_seconds);
    } else {
        for (int n = MIN_N; n <= target; ++n) {
            (void)compute_checked(
                n, requested_threads, memory_mib,
                progress_seconds, false);
        }
        printf("ok: sum-group subset DP agrees with A398911 for "
               "n=%d..%d\n", MIN_N, target);
    }
    return EXIT_SUCCESS;
}
