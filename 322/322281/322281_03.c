/*
 * A322281 -- polynomial-space inclusion-exclusion evaluator.
 *
 * Count permutations sigma of [n] satisfying
 *
 *   |sigma(i+j)-sigma(i)| >= 4,  1 <= j <= 3.
 *
 * This is mathematically independent of 322281_01.c and 322281_02.c.
 * Those programs remember the set already used by a permutation prefix.
 * Here, for each S subseteq [n], W_n(S) counts length-n words over S
 * (repetition allowed) satisfying the same distance-at-most-three rule.
 * Inclusion-exclusion on the letters that occur gives
 *
 *   a(n) = Sum_{S subseteq [n]} (-1)^(n-|S|) W_n(S).              (1)
 *
 * A word surviving (1) uses all n letters.  Since its length is n, every
 * letter occurs exactly once, so it is precisely a required permutation.
 *
 * For fixed S, let D_t(a,b,c) count valid length-t words ending a,b,c.
 * For t>=3,
 *
 *   D_{t+1}(b,c,d) = Sum_{a in S, |a-d|>=4} D_t(a,b,c),           (2)
 *
 * provided d is in S and is compatible with b,c.  First compute
 * T_t(b,c)=Sum_a D_t(a,b,c); (2) is T_t(b,c) minus at most the seven
 * terms a=d-3,...,d+3.  Thus one S costs O(n^4) arithmetic operations
 * and O(n^3) words of memory; all S cost O(2^n n^4) time and O(n^3)
 * memory per worker.  Complementing values preserves W_n(S), so only the
 * smaller mask in each reflection orbit is evaluated.  A set without four
 * pairwise-4-separated values cannot contain a valid length-4 block and is
 * skipped.
 *
 * Exactness bound: W_n(S)<=|S|^n<=n^n.  Each positive or negative half of
 * (1) is at most 2^(n-1)n^n, which is below 2^128 for n<=23.  All additions
 * and subtractions are checked.  The result is therefore exact on the full
 * advertised range; no cancellation is performed in an overflowing type.
 *
 * This method uses very little RAM but is intentionally much slower than
 * the exponential-memory prefix DPs.  --threads partitions the subset sum
 * among independent workers.  Range mode writes b322281_03_part.txt after
 * every completed term and renames it only after the full run succeeds.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic -pthread \
 *     322281_03.c -o 322281_03
 *
 * Usage:
 *   ./322281_03 --term 16 --threads 8
 *   ./322281_03 --upto 16 --threads 8
 *   ./322281_03 --check 15 --threads 8
 *
 * General inclusion-exclusion background:
 *   Kohn, Gottlieb and Kohn, Information Processing Letters 47 (1993),
 *   203-207, doi:10.1016/0020-0190(93)90033-6.
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

#if !defined(__SIZEOF_INT128__)
#error "322281_03 requires compiler support for unsigned __int128"
#endif

__extension__ typedef unsigned __int128 u128;

#define OFFSET 0
#define DEFAULT_MAX_N 15
#define MAX_N 23
#define KNOWN_MAX_N 22
#define DEFAULT_CHECK_N 15
#define DIRECT_CHECK_N 10
#define MAX_THREADS 64

typedef enum { RUN_UPTO, RUN_TERM, RUN_CHECK } RunMode;

typedef struct {
    int n;
    int worker_id;
    int worker_count;
    uint64_t mask_limit;
    u128 positive;
    u128 negative;
    uint64_t evaluated_subsets;
    uint64_t skipped_subsets;
    uint64_t recurrence_cells;
    int failed;
    char error[128];
} Worker;

typedef struct {
    uint64_t evaluated_subsets;
    uint64_t skipped_subsets;
    uint64_t recurrence_cells;
    size_t workspace_bytes;
    double seconds;
} Stats;

static const char *const known[KNOWN_MAX_N + 1] = {
    "1", "1", "0", "0", "0", "0", "0", "0", "0", "0", "0",
    "0", "0", "0", "0", "0", "2", "74", "2424", "93424",
    "4394386", "201355480", "8665643734"
};

static void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static double now_seconds(void)
{
    struct timespec value;
    if (clock_gettime(CLOCK_MONOTONIC, &value) != 0)
        die("clock_gettime failed");
    return (double)value.tv_sec + (double)value.tv_nsec / 1000000000.0;
}

static size_t checked_size_product(size_t a, size_t b)
{
    if (a != 0U && b > SIZE_MAX / a) die("allocation size overflow");
    return a * b;
}

static bool add_checked(u128 *destination, u128 addend)
{
    u128 maximum = ~(u128)0;
    if (*destination > maximum - addend) return false;
    *destination += addend;
    return true;
}

static int print_u128(FILE *stream, u128 value)
{
    char digits[40];
    size_t length = 0;
    do {
        digits[length++] = (char)('0' + (unsigned)(value % 10U));
        value /= 10U;
    } while (value != 0U);
    while (length != 0U)
        if (fputc((unsigned char)digits[--length], stream) == EOF) return -1;
    return 0;
}

static bool parse_u128_decimal(const char *text, u128 *result)
{
    u128 value = 0;
    u128 maximum = ~(u128)0;
    if (*text == '\0') return false;
    for (const unsigned char *p = (const unsigned char *)text; *p; ++p) {
        if (*p < '0' || *p > '9') return false;
        unsigned digit = (unsigned)(*p - '0');
        if (value > (maximum - digit) / 10U) return false;
        value = value * 10U + digit;
    }
    *result = value;
    return true;
}

static int parse_bounded_int(const char *text, const char *label,
                             int minimum, int maximum)
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

static unsigned popcount64(uint64_t value)
{
#if defined(__GNUC__) || defined(__clang__)
    return (unsigned)__builtin_popcountll(value);
#else
    unsigned result = 0;
    while (value != 0U) {
        value &= value - 1U;
        ++result;
    }
    return result;
#endif
}

static uint64_t reverse_mask(uint64_t mask, int n)
{
    uint64_t result = 0;
    for (int i = 0; i < n; ++i)
        if ((mask & (UINT64_C(1) << (unsigned)i)) != 0U)
            result |= UINT64_C(1) << (unsigned)(n - 1 - i);
    return result;
}

static bool has_four_separated(uint64_t mask, int n)
{
    int count = 0;
    int last = -4;
    for (int value = 0; value < n; ++value) {
        if ((mask & (UINT64_C(1) << (unsigned)value)) != 0U &&
            value - last >= 4) {
            last = value;
            if (++count == 4) return true;
        }
    }
    return false;
}

static size_t index3(int a, int b, int c, int n)
{
    return ((size_t)a * (size_t)n + (size_t)b) * (size_t)n + (size_t)c;
}

/* Compute W_n(S) from (2).  Arrays belong exclusively to one worker. */
static bool word_count(int n, uint64_t mask, u128 *dp, u128 *next,
                       u128 *totals, u128 *answer, uint64_t *cells)
{
    if (n < 4) return false; /* compute_value handles these terms directly. */
    size_t n2 = (size_t)n * (size_t)n;
    size_t n3 = n2 * (size_t)n;
    memset(dp, 0, n3 * sizeof(*dp));

    for (int a = 0; a < n; ++a) {
        if ((mask & (UINT64_C(1) << (unsigned)a)) == 0U) continue;
        for (int b = 0; b < n; ++b) {
            if ((mask & (UINT64_C(1) << (unsigned)b)) == 0U ||
                abs(a - b) < 4) continue;
            for (int c = 0; c < n; ++c) {
                if ((mask & (UINT64_C(1) << (unsigned)c)) == 0U ||
                    abs(a - c) < 4 || abs(b - c) < 4) continue;
                dp[index3(a, b, c, n)] = 1U;
            }
        }
    }

    for (int length = 3; length < n; ++length) {
        memset(totals, 0, n2 * sizeof(*totals));
        memset(next, 0, n3 * sizeof(*next));
        for (int b = 0; b < n; ++b) {
            for (int c = 0; c < n; ++c) {
                u128 total = 0;
                for (int a = 0; a < n; ++a)
                    if (!add_checked(&total, dp[index3(a, b, c, n)]))
                        return false;
                totals[(size_t)b * (size_t)n + (size_t)c] = total;
            }
        }
        for (int b = 0; b < n; ++b) {
            for (int c = 0; c < n; ++c) {
                if (abs(b - c) < 4) continue;
                u128 total = totals[(size_t)b * (size_t)n + (size_t)c];
                for (int d = 0; d < n; ++d) {
                    if ((mask & (UINT64_C(1) << (unsigned)d)) == 0U ||
                        abs(b - d) < 4 || abs(c - d) < 4) continue;
                    u128 value = total;
                    int first = d >= 3 ? d - 3 : 0;
                    int last = d + 3 < n ? d + 3 : n - 1;
                    for (int a = first; a <= last; ++a) {
                        u128 term = dp[index3(a, b, c, n)];
                        if (value < term) return false;
                        value -= term;
                    }
                    next[index3(b, c, d, n)] = value;
                    if (*cells == UINT64_MAX) return false;
                    ++*cells;
                }
            }
        }
        u128 *temporary = dp;
        dp = next;
        next = temporary;
    }

    u128 result = 0;
    for (size_t i = 0; i < n3; ++i)
        if (!add_checked(&result, dp[i])) return false;
    *answer = result;
    return true;
}

static void *worker_main(void *argument)
{
    Worker *worker = argument;
    int n = worker->n;
    size_t n2 = checked_size_product((size_t)n, (size_t)n);
    size_t n3 = checked_size_product(n2, (size_t)n);
    size_t allocation_n2 = n2 == 0U ? 1U : n2;
    size_t allocation_n3 = n3 == 0U ? 1U : n3;
    u128 *dp = calloc(allocation_n3, sizeof(*dp));
    u128 *next = calloc(allocation_n3, sizeof(*next));
    u128 *totals = calloc(allocation_n2, sizeof(*totals));
    if (dp == NULL || next == NULL || totals == NULL) {
        worker->failed = 1;
        snprintf(worker->error, sizeof(worker->error),
                 "worker workspace allocation failed");
        free(dp); free(next); free(totals);
        return NULL;
    }

    for (uint64_t mask = (uint64_t)worker->worker_id;
         mask < worker->mask_limit;
         mask += (uint64_t)worker->worker_count) {
        uint64_t reflected = reverse_mask(mask, n);
        if (mask > reflected) continue;
        unsigned cardinality = popcount64(mask);
        if (cardinality < 4U || !has_four_separated(mask, n)) {
            ++worker->skipped_subsets;
            continue;
        }
        u128 value;
        if (!word_count(n, mask, dp, next, totals, &value,
                        &worker->recurrence_cells)) {
            worker->failed = 1;
            snprintf(worker->error, sizeof(worker->error),
                     "128-bit intermediate overflow or recurrence failure");
            break;
        }
        unsigned orbit = mask == reflected ? 1U : 2U;
        if (orbit == 2U && !add_checked(&value, value)) {
            worker->failed = 1;
            snprintf(worker->error, sizeof(worker->error),
                     "128-bit reflected-orbit overflow");
            break;
        }
        u128 *destination = ((n - (int)cardinality) & 1) != 0
                          ? &worker->negative : &worker->positive;
        if (!add_checked(destination, value)) {
            worker->failed = 1;
            snprintf(worker->error, sizeof(worker->error),
                     "128-bit inclusion-exclusion half-sum overflow");
            break;
        }
        ++worker->evaluated_subsets;
    }
    free(dp); free(next); free(totals);
    return NULL;
}

static u128 direct_count(int n)
{
    if (n == 0) return 1U;
    int p[DIRECT_CHECK_N];
    for (int i = 0; i < n; ++i) p[i] = i;
    u128 result = 0;
    for (;;) {
        bool valid = true;
        for (int i = 0; valid && i < n; ++i)
            for (int j = 1; j <= 3 && i + j < n; ++j)
                if (abs(p[i + j] - p[i]) < 4) { valid = false; break; }
        if (valid) ++result;
        int i = n - 2;
        while (i >= 0 && p[i] >= p[i + 1]) --i;
        if (i < 0) break;
        int j = n - 1;
        while (p[j] <= p[i]) --j;
        int temporary = p[i]; p[i] = p[j]; p[j] = temporary;
        for (int left = i + 1, right = n - 1; left < right;
             ++left, --right) {
            temporary = p[left]; p[left] = p[right]; p[right] = temporary;
        }
    }
    return result;
}

static u128 compute_value(int n, int thread_count, Stats *stats)
{
    double started = now_seconds();
    memset(stats, 0, sizeof(*stats));
    if (n <= 3) {
        u128 result = direct_count(n);
        stats->seconds = now_seconds() - started;
        return result;
    }
    uint64_t limit = UINT64_C(1) << (unsigned)n;
    Worker *workers = calloc((size_t)thread_count, sizeof(*workers));
    pthread_t *threads = calloc((size_t)thread_count, sizeof(*threads));
    if (workers == NULL || threads == NULL) die("worker allocation failed");
    for (int i = 0; i < thread_count; ++i) {
        workers[i].n = n;
        workers[i].worker_id = i;
        workers[i].worker_count = thread_count;
        workers[i].mask_limit = limit;
        int error = pthread_create(&threads[i], NULL, worker_main, &workers[i]);
        if (error != 0) {
            fprintf(stderr, "error: pthread_create: %s\n", strerror(error));
            exit(EXIT_FAILURE);
        }
    }
    u128 positive = 0, negative = 0;
    for (int i = 0; i < thread_count; ++i) {
        int error = pthread_join(threads[i], NULL);
        if (error != 0) {
            fprintf(stderr, "error: pthread_join: %s\n", strerror(error));
            exit(EXIT_FAILURE);
        }
        if (workers[i].failed) die(workers[i].error);
        if (!add_checked(&positive, workers[i].positive) ||
            !add_checked(&negative, workers[i].negative))
            die("128-bit worker-total overflow");
        stats->evaluated_subsets += workers[i].evaluated_subsets;
        stats->skipped_subsets += workers[i].skipped_subsets;
        if (UINT64_MAX - stats->recurrence_cells < workers[i].recurrence_cells)
            die("recurrence-cell counter overflow");
        stats->recurrence_cells += workers[i].recurrence_cells;
    }
    free(workers); free(threads);
    if (positive < negative) die("internal negative inclusion-exclusion sum");
    size_t n2 = checked_size_product((size_t)n, (size_t)n);
    size_t n3 = checked_size_product(n2, (size_t)n);
    stats->workspace_bytes = checked_size_product((size_t)thread_count,
        checked_size_product(2U * n3 + n2, sizeof(u128)));
    stats->seconds = now_seconds() - started;
    return positive - negative;
}

static void verify_known(int n, u128 value)
{
    if (n <= KNOWN_MAX_N) {
        u128 expected;
        if (!parse_u128_decimal(known[n], &expected))
            die("invalid built-in reference term");
        if (value != expected) {
            fprintf(stderr, "error: A322281 mismatch at n=%d: got ", n);
            (void)print_u128(stderr, value);
            fprintf(stderr, ", expected %s\n", known[n]);
            exit(EXIT_FAILURE);
        }
    }
}

static u128 evaluated_value(int n, int threads, bool verbose)
{
    Stats stats;
    u128 value = compute_value(n, threads, &stats);
    verify_known(n, value);
    if (verbose) {
        fprintf(stderr,
                "322281_03: n=%d, inclusion-exclusion, threads=%d, "
                "evaluated/skipped reflection representatives=%" PRIu64
                "/%" PRIu64 ", recurrence cells=%" PRIu64
                ", workspace=%.2f MiB, %.3f s\n",
                n, threads, stats.evaluated_subsets, stats.skipped_subsets,
                stats.recurrence_cells,
                (double)stats.workspace_bytes / 1048576.0, stats.seconds);
    }
    return value;
}

static int run_check(int maximum, int threads)
{
    for (int n = 0; n <= maximum; ++n) {
        u128 value = evaluated_value(n, threads, false);
        if (n <= DIRECT_CHECK_N && value != direct_count(n)) {
            fprintf(stderr, "error: direct-permutation mismatch at n=%d\n", n);
            return EXIT_FAILURE;
        }
    }
    printf("ok: independent inclusion-exclusion agrees with A322281 through "
           "n=%d and direct permutation enumeration through n=%d\n",
           maximum, maximum < DIRECT_CHECK_N ? maximum : DIRECT_CHECK_N);
    return EXIT_SUCCESS;
}

static char *path_beside_executable(const char *argv0, const char *filename)
{
    char executable[PATH_MAX];
    char resolved[PATH_MAX];
    bool found = false;
#ifdef __APPLE__
    uint32_t size = (uint32_t)sizeof(executable);
    if (_NSGetExecutablePath(executable, &size) == 0) found = true;
#elif defined(__linux__)
    ssize_t length = readlink("/proc/self/exe", executable,
                              sizeof(executable) - 1U);
    if (length >= 0) { executable[length] = '\0'; found = true; }
#endif
    if (!found) {
        if (strlen(argv0) >= sizeof(executable)) die("executable path too long");
        strcpy(executable, argv0);
    }
    if (realpath(executable, resolved) == NULL) {
        if (strlen(executable) >= sizeof(resolved)) die("path too long");
        strcpy(resolved, executable);
    }
    char *slash = strrchr(resolved, '/');
    const char *directory = ".";
    size_t directory_length = 1U;
    if (slash != NULL) {
        *slash = '\0'; directory = resolved; directory_length = strlen(directory);
    }
    size_t needed = directory_length + 1U + strlen(filename) + 1U;
    char *result = malloc(needed);
    if (result == NULL) die("output path allocation failed");
    snprintf(result, needed, "%s/%s", directory, filename);
    return result;
}

static void produce_file(const char *argv0, int maximum, int threads)
{
    char *part = path_beside_executable(argv0, "b322281_03_part.txt");
    char *final = path_beside_executable(argv0, "b322281_03.txt");
    FILE *stream = fopen(part, "w");
    if (stream == NULL) die("could not open partial b-file");
    for (int n = OFFSET; n <= maximum; ++n) {
        u128 value = evaluated_value(n, threads, true);
        if (fprintf(stream, "%d ", n) < 0 || print_u128(stream, value) < 0 ||
            fputc('\n', stream) == EOF || fflush(stream) != 0)
            die("could not write partial b-file");
    }
    if (fclose(stream) != 0) die("could not close partial b-file");
    if (rename(part, final) != 0) die("could not rename completed b-file");
    fprintf(stderr, "wrote %s (n=%d..%d)\n", final, OFFSET, maximum);
    free(part); free(final);
}

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s [MAX_N] [--threads T]\n"
            "       %s --upto MAX_N [--threads T]\n"
            "       %s --term N [--threads T]\n"
            "       %s --check [CHECK_N] [--threads T]\n"
            "N must be in %d..%d (default %d); T must be in 1..%d.\n",
            program, program, program, program,
            OFFSET, MAX_N, DEFAULT_MAX_N, MAX_THREADS);
}

int main(int argc, char **argv)
{
    RunMode mode = RUN_UPTO;
    int n = -1;
    int threads = 1;
    bool have_mode = false, have_threads = false;
    for (int argument = 1; argument < argc; ++argument) {
        const char *text = argv[argument];
        if (strcmp(text, "--threads") == 0) {
            if (have_threads || argument + 1 >= argc) { usage(argv[0]); return 1; }
            threads = parse_bounded_int(argv[++argument], "THREADS", 1, MAX_THREADS);
            have_threads = true;
        } else if (strcmp(text, "--term") == 0 || strcmp(text, "--upto") == 0) {
            if (have_mode || argument + 1 >= argc) { usage(argv[0]); return 1; }
            mode = strcmp(text, "--term") == 0 ? RUN_TERM : RUN_UPTO;
            n = parse_bounded_int(argv[++argument], "N", OFFSET, MAX_N);
            have_mode = true;
        } else if (strcmp(text, "--check") == 0) {
            if (have_mode) { usage(argv[0]); return 1; }
            mode = RUN_CHECK; have_mode = true;
            if (argument + 1 < argc && argv[argument + 1][0] != '-')
                n = parse_bounded_int(argv[++argument], "CHECK_N", OFFSET, MAX_N);
        } else if (text[0] != '-' && !have_mode) {
            mode = RUN_UPTO;
            n = parse_bounded_int(text, "MAX_N", OFFSET, MAX_N);
            have_mode = true;
        } else {
            usage(argv[0]); return 1;
        }
    }
    if (n < 0) n = mode == RUN_CHECK ? DEFAULT_CHECK_N : DEFAULT_MAX_N;
    if (mode == RUN_CHECK) return run_check(n, threads);
    if (mode == RUN_TERM) {
        u128 value = evaluated_value(n, threads, true);
        printf("%d ", n);
        if (print_u128(stdout, value) < 0 || fputc('\n', stdout) == EOF)
            die("stdout write failed");
        return EXIT_SUCCESS;
    }
    produce_file(argv[0], n, threads);
    return EXIT_SUCCESS;
}
