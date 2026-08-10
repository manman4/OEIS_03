/*
 * A322308 -- polynomial-space inclusion-exclusion evaluator.
 *
 * Count permutations sigma of [n] satisfying
 *
 *   |sigma(i+j)-sigma(i)| >= 5,  1 <= j <= 4.
 *
 * This is mathematically independent of 322308_01.c and 322308_02.c.
 * Those programs remember the set already used by a permutation prefix.
 * Here, for each S subseteq [n], W_n(S) counts length-n words over S
 * (repetition allowed) satisfying the same distance-at-most-four rule.
 * Inclusion-exclusion on the letters that occur gives
 *
 *   a(n) = Sum_{S subseteq [n]} (-1)^(n-|S|) W_n(S).              (1)
 *
 * A word surviving (1) uses all n letters.  Since its length is n, every
 * letter occurs exactly once, so it is precisely a required permutation.
 *
 * For fixed S, let D_t(a,b,c,d) count valid words ending a,b,c,d. For t>=4,
 *
 *   D_{t+1}(b,c,d,e) = Sum_{a in S, |a-e|>=5} D_t(a,b,c,d).      (2)
 *
 * provided e is in S and is compatible with b,c,d. First compute
 * T_t(b,c,d)=Sum_a D_t(a,b,c,d); (2) is T_t(b,c,d) minus at most the nine
 * terms a=e-4,...,e+4. Thus one S costs O(n^5) arithmetic operations
 * and O(n^4) words of memory; all S cost O(2^n n^5) time and O(n^4)
 * memory per worker.  Complementing values preserves W_n(S), so only the
 * smaller mask in each reflection orbit is evaluated. A set without five
 * pairwise-5-separated values cannot contain a valid length-5 block and is
 * skipped.
 * Within one S, its actual values are renumbered to 0..|S|-1, so the worker
 * scans |S|^4 rather than n^4 coefficient cells without changing (2).
 *
 * Exactness bound: W_n(S)<=|S|^n<=n^n.  Each positive or negative half of
 * (1) is at most 2^(n-1)n^n, which is below 2^256 for n<=40. All additions
 * and subtractions are checked.  The result is therefore exact on the full
 * advertised range; no cancellation is performed in an overflowing type.
 *
 * This method uses very little RAM but is intentionally much slower than
 * the exponential-memory prefix DPs.  --threads partitions the subset sum
 * among independent workers.  Range mode writes b322308_03_part.txt after
 * every completed term and renames it only after the full run succeeds.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic -pthread \
 *     322308_03.c -o 322308_03
 *
 * Usage:
 *   ./322308_03 --term 25 --threads 8
 *   ./322308_03 --upto 25 --threads 8
 *   ./322308_03 --check 25 --threads 8
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
#error "322308_03 requires compiler support for unsigned __int128"
#endif

__extension__ typedef unsigned __int128 uint128_t;

typedef struct {
    uint64_t limb[4]; /* little-endian, exact unsigned 256-bit integer */
} U256;

#define OFFSET 0
#define DEFAULT_MAX_N 25
#define MAX_N 40
#define KNOWN_MAX_N 28
#define DEFAULT_CHECK_N 20
#define DIRECT_CHECK_N 10
#define MAX_THREADS 64

typedef enum { RUN_UPTO, RUN_TERM, RUN_CHECK } RunMode;

typedef struct {
    int n;
    int worker_id;
    int worker_count;
    uint64_t mask_limit;
    U256 positive;
    U256 negative;
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
    "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0",
    "0", "0", "0", "2", "128", "6320", "344872"
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

static U256 u256_zero(void)
{
    U256 result = {{0, 0, 0, 0}};
    return result;
}

static U256 u256_from_u64(uint64_t value)
{
    U256 result = {{value, 0, 0, 0}};
    return result;
}

static bool u256_is_zero(U256 value)
{
    return (value.limb[0] | value.limb[1] |
            value.limb[2] | value.limb[3]) == 0U;
}

static bool u256_equal(U256 a, U256 b)
{
    return memcmp(&a, &b, sizeof(a)) == 0;
}

static int u256_compare(U256 a, U256 b)
{
    for (int i = 3; i >= 0; --i)
        if (a.limb[i] != b.limb[i])
            return a.limb[i] < b.limb[i] ? -1 : 1;
    return 0;
}

static bool add_checked(U256 *destination, U256 addend)
{
    uint64_t carry = 0;
    for (int i = 0; i < 4; ++i) {
        uint128_t sum = (uint128_t)destination->limb[i] +
                        (uint128_t)addend.limb[i] + carry;
        destination->limb[i] = (uint64_t)sum;
        carry = (uint64_t)(sum >> 64U);
    }
    return carry == 0U;
}

static bool subtract_checked(U256 minuend, U256 subtrahend, U256 *result)
{
    if (u256_compare(minuend, subtrahend) < 0) return false;
    uint64_t borrow = 0;
    for (int i = 0; i < 4; ++i) {
        uint64_t sub = subtrahend.limb[i] + borrow;
        uint64_t carry_into_sub = sub < subtrahend.limb[i] ? 1U : 0U;
        uint64_t next_borrow =
            carry_into_sub || minuend.limb[i] < sub ? 1U : 0U;
        result->limb[i] = minuend.limb[i] - sub;
        borrow = next_borrow;
    }
    return borrow == 0U;
}

static bool multiply_small(U256 *value, uint32_t multiplier)
{
    uint64_t carry = 0;
    for (int i = 0; i < 4; ++i) {
        uint128_t product = (uint128_t)value->limb[i] * multiplier + carry;
        value->limb[i] = (uint64_t)product;
        carry = (uint64_t)(product >> 64U);
    }
    return carry == 0U;
}

static uint32_t divide_small(U256 *value, uint32_t divisor)
{
    uint64_t remainder = 0;
    for (int i = 3; i >= 0; --i) {
        uint128_t current = ((uint128_t)remainder << 64U) | value->limb[i];
        value->limb[i] = (uint64_t)(current / divisor);
        remainder = (uint64_t)(current % divisor);
    }
    return (uint32_t)remainder;
}

static int print_u256(FILE *stream, U256 value)
{
    char digits[78];
    size_t length = 0;
    do {
        digits[length++] = (char)('0' + divide_small(&value, 10U));
    } while (!u256_is_zero(value));
    while (length != 0U)
        if (fputc((unsigned char)digits[--length], stream) == EOF) return -1;
    return 0;
}

static bool parse_u256_decimal(const char *text, U256 *result)
{
    U256 value = u256_zero();
    if (*text == '\0') return false;
    for (const unsigned char *p = (const unsigned char *)text; *p; ++p) {
        if (*p < '0' || *p > '9') return false;
        if (!multiply_small(&value, 10U) ||
            !add_checked(&value, u256_from_u64((uint64_t)(*p - '0'))))
            return false;
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

static bool has_five_separated(uint64_t mask, int n)
{
    int count = 0;
    int last = -5;
    for (int value = 0; value < n; ++value) {
        if ((mask & (UINT64_C(1) << (unsigned)value)) != 0U &&
            value - last >= 5) {
            last = value;
            if (++count == 5) return true;
        }
    }
    return false;
}

static size_t index3(int a, int b, int c, int n)
{
    return ((size_t)a * (size_t)n + (size_t)b) * (size_t)n + (size_t)c;
}

static size_t index4(int a, int b, int c, int d, int n)
{
    return (index3(a, b, c, n) * (size_t)n) + (size_t)d;
}

/* Compute W_n(S) from (2). Arrays belong exclusively to one worker. */
static bool word_count(int n, uint64_t mask, U256 *dp, U256 *next,
                       U256 *totals, U256 *answer, uint64_t *cells)
{
    if (n < 5) return false; /* compute_value handles these terms directly. */
    int values[MAX_N];
    int m = 0;
    for (int value = 0; value < n; ++value)
        if ((mask & (UINT64_C(1) << (unsigned)value)) != 0U)
            values[m++] = value;
    size_t m2 = checked_size_product((size_t)m, (size_t)m);
    size_t m3 = checked_size_product(m2, (size_t)m);
    size_t m4 = checked_size_product(m3, (size_t)m);
    memset(dp, 0, checked_size_product(m4, sizeof(*dp)));

    for (int a = 0; a < m; ++a) {
        for (int b = 0; b < m; ++b) {
            if (abs(values[a] - values[b]) < 5) continue;
            for (int c = 0; c < m; ++c) {
                if (abs(values[a] - values[c]) < 5 ||
                    abs(values[b] - values[c]) < 5) continue;
                for (int d = 0; d < m; ++d) {
                    if (abs(values[a] - values[d]) < 5 ||
                        abs(values[b] - values[d]) < 5 ||
                        abs(values[c] - values[d]) < 5) continue;
                    dp[index4(a, b, c, d, m)] = u256_from_u64(1U);
                }
            }
        }
    }

    for (int length = 4; length < n; ++length) {
        memset(totals, 0, checked_size_product(m3, sizeof(*totals)));
        memset(next, 0, checked_size_product(m4, sizeof(*next)));
        for (int b = 0; b < m; ++b) {
            for (int c = 0; c < m; ++c) {
                for (int d = 0; d < m; ++d) {
                    U256 total = u256_zero();
                    for (int a = 0; a < m; ++a)
                        if (!add_checked(&total, dp[index4(a, b, c, d, m)]))
                            return false;
                    totals[index3(b, c, d, m)] = total;
                }
            }
        }
        for (int b = 0; b < m; ++b) {
            for (int c = 0; c < m; ++c) {
                if (abs(values[b] - values[c]) < 5) continue;
                for (int d = 0; d < m; ++d) {
                    if (abs(values[b] - values[d]) < 5 ||
                        abs(values[c] - values[d]) < 5) continue;
                    U256 total = totals[index3(b, c, d, m)];
                    for (int e = 0; e < m; ++e) {
                        if (abs(values[b] - values[e]) < 5 ||
                            abs(values[c] - values[e]) < 5 ||
                            abs(values[d] - values[e]) < 5) continue;
                        U256 value = total;
                        for (int a = 0; a < m; ++a) {
                            if (abs(values[a] - values[e]) >= 5) continue;
                            U256 reduced;
                            if (!subtract_checked(
                                    value, dp[index4(a, b, c, d, m)],
                                    &reduced))
                                return false;
                            value = reduced;
                        }
                        next[index4(b, c, d, e, m)] = value;
                        if (*cells == UINT64_MAX) return false;
                        ++*cells;
                    }
                }
            }
        }
        U256 *temporary = dp;
        dp = next;
        next = temporary;
    }

    U256 result = u256_zero();
    for (size_t i = 0; i < m4; ++i)
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
    size_t n4 = checked_size_product(n3, (size_t)n);
    size_t allocation_n3 = n3 == 0U ? 1U : n3;
    size_t allocation_n4 = n4 == 0U ? 1U : n4;
    U256 *dp = calloc(allocation_n4, sizeof(*dp));
    U256 *next = calloc(allocation_n4, sizeof(*next));
    U256 *totals = calloc(allocation_n3, sizeof(*totals));
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
        if (cardinality < 5U || !has_five_separated(mask, n)) {
            ++worker->skipped_subsets;
            continue;
        }
        U256 value;
        if (!word_count(n, mask, dp, next, totals, &value,
                        &worker->recurrence_cells)) {
            worker->failed = 1;
            snprintf(worker->error, sizeof(worker->error),
                     "256-bit intermediate overflow or recurrence failure");
            break;
        }
        unsigned orbit = mask == reflected ? 1U : 2U;
        if (orbit == 2U && !add_checked(&value, value)) {
            worker->failed = 1;
            snprintf(worker->error, sizeof(worker->error),
                     "256-bit reflected-orbit overflow");
            break;
        }
        U256 *destination = ((n - (int)cardinality) & 1) != 0
                          ? &worker->negative : &worker->positive;
        if (!add_checked(destination, value)) {
            worker->failed = 1;
            snprintf(worker->error, sizeof(worker->error),
                     "256-bit inclusion-exclusion half-sum overflow");
            break;
        }
        ++worker->evaluated_subsets;
    }
    free(dp); free(next); free(totals);
    return NULL;
}

static U256 direct_count(int n)
{
    if (n == 0) return u256_from_u64(1U);
    int p[DIRECT_CHECK_N];
    for (int i = 0; i < n; ++i) p[i] = i;
    U256 result = u256_zero();
    for (;;) {
        bool valid = true;
        for (int i = 0; valid && i < n; ++i)
            for (int j = 1; j <= 4 && i + j < n; ++j)
                if (abs(p[i + j] - p[i]) < 5) { valid = false; break; }
        if (valid && !add_checked(&result, u256_from_u64(1U)))
            die("direct-count overflow");
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

static U256 compute_value(int n, int thread_count, Stats *stats)
{
    double started = now_seconds();
    memset(stats, 0, sizeof(*stats));
    if (n <= 4) {
        U256 result = direct_count(n);
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
    U256 positive = u256_zero(), negative = u256_zero();
    for (int i = 0; i < thread_count; ++i) {
        int error = pthread_join(threads[i], NULL);
        if (error != 0) {
            fprintf(stderr, "error: pthread_join: %s\n", strerror(error));
            exit(EXIT_FAILURE);
        }
        if (workers[i].failed) die(workers[i].error);
        if (!add_checked(&positive, workers[i].positive) ||
            !add_checked(&negative, workers[i].negative))
            die("256-bit worker-total overflow");
        stats->evaluated_subsets += workers[i].evaluated_subsets;
        stats->skipped_subsets += workers[i].skipped_subsets;
        if (UINT64_MAX - stats->recurrence_cells < workers[i].recurrence_cells)
            die("recurrence-cell counter overflow");
        stats->recurrence_cells += workers[i].recurrence_cells;
    }
    free(workers); free(threads);
    if (u256_compare(positive, negative) < 0)
        die("internal negative inclusion-exclusion sum");
    size_t n2 = checked_size_product((size_t)n, (size_t)n);
    size_t n3 = checked_size_product(n2, (size_t)n);
    size_t n4 = checked_size_product(n3, (size_t)n);
    stats->workspace_bytes = checked_size_product((size_t)thread_count,
        checked_size_product(2U * n4 + n3, sizeof(U256)));
    stats->seconds = now_seconds() - started;
    U256 result;
    if (!subtract_checked(positive, negative, &result))
        die("internal inclusion-exclusion subtraction failure");
    return result;
}

static void verify_known(int n, U256 value)
{
    if (n <= KNOWN_MAX_N) {
        U256 expected;
        if (!parse_u256_decimal(known[n], &expected))
            die("invalid built-in reference term");
        if (!u256_equal(value, expected)) {
            fprintf(stderr, "error: A322308 mismatch at n=%d: got ", n);
            (void)print_u256(stderr, value);
            fprintf(stderr, ", expected %s\n", known[n]);
            exit(EXIT_FAILURE);
        }
    }
}

static U256 evaluated_value(int n, int threads, bool verbose)
{
    Stats stats;
    U256 value = compute_value(n, threads, &stats);
    verify_known(n, value);
    if (verbose) {
        fprintf(stderr,
                "322308_03: n=%d, inclusion-exclusion, threads=%d, "
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
    /* Five mutually compatible letters must repeat with period five, so
     * there are exactly 5! valid length-21 words over this alphabet. */
    {
        const int test_n = 21;
        const size_t m3 = 125U, m4 = 625U;
        U256 *dp = calloc(m4, sizeof(*dp));
        U256 *next = calloc(m4, sizeof(*next));
        U256 *totals = calloc(m3, sizeof(*totals));
        if (dp == NULL || next == NULL || totals == NULL)
            die("word-recurrence self-test allocation failed");
        uint64_t mask = (UINT64_C(1) << 0U) |
                        (UINT64_C(1) << 5U) |
                        (UINT64_C(1) << 10U) |
                        (UINT64_C(1) << 15U) |
                        (UINT64_C(1) << 20U);
        U256 value;
        uint64_t cells = 0;
        if (!word_count(test_n, mask, dp, next, totals, &value, &cells) ||
            !u256_equal(value, u256_from_u64(120U)))
            die("word-recurrence self-test failed");
        free(dp); free(next); free(totals);
    }
    /* Exercise carries beyond 128 bits and the matching subtraction. */
    {
        U256 original = {{UINT64_MAX, UINT64_MAX, 1U, 0U}};
        U256 value = original;
        U256 expected = {{0U, 0U, 2U, 0U}};
        if (!add_checked(&value, u256_from_u64(1U)) ||
            !u256_equal(value, expected))
            die("256-bit carry self-test failed");
        U256 recovered;
        if (!subtract_checked(value, u256_from_u64(1U), &recovered) ||
            !u256_equal(recovered, original))
            die("256-bit subtraction self-test failed");
    }
    for (int n = 0; n <= maximum; ++n) {
        U256 value = evaluated_value(n, threads, false);
        if (n <= DIRECT_CHECK_N && !u256_equal(value, direct_count(n))) {
            fprintf(stderr, "error: direct-permutation mismatch at n=%d\n", n);
            return EXIT_FAILURE;
        }
    }
    printf("ok: independent inclusion-exclusion agrees with A322308 through "
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
    char *part = path_beside_executable(argv0, "b322308_03_part.txt");
    char *final = path_beside_executable(argv0, "b322308_03.txt");
    FILE *stream = fopen(part, "w");
    if (stream == NULL) die("could not open partial b-file");
    for (int n = OFFSET; n <= maximum; ++n) {
        U256 value = evaluated_value(n, threads, true);
        if (fprintf(stream, "%d ", n) < 0 || print_u256(stream, value) < 0 ||
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
        U256 value = evaluated_value(n, threads, true);
        printf("%d ", n);
        if (print_u256(stdout, value) < 0 || fputc('\n', stdout) == EOF)
            die("stdout write failed");
        return EXIT_SUCCESS;
    }
    produce_file(argv[0], n, threads);
    return EXIT_SUCCESS;
}
