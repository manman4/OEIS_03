/*
 * A396832 -- number of permutations p of [n] such that
 *
 *                       k * p(k) >= n - 1
 *
 * for every 1 <= k <= n.
 *
 * Algorithm
 * ---------
 * This implementation deliberately does not use the A396832 product formula,
 * a ceiling formula, or a count derived from nested allowed sets.  It builds
 * the 0-1 matrix
 *
 *             A[k,v] = 1 if and only if k*v >= n-1
 *
 * directly from the definition and evaluates its permanent with Ryser's
 * general inclusion-exclusion identity
 *
 *   per(A) = (-1)^n Sum_{S subseteq [n]} (-1)^|S|
 *              Product_k Sum_{v in S} A[k,v].
 *
 * Subsets are visited in Gray-code order.  Equal matrix rows are grouped;
 * this only avoids repeating identical factors and does not use a formula
 * specific to A396832.
 *
 * Exactness and supported range
 * -----------------------------
 * All arithmetic is integer arithmetic.  For n <= 28, each Ryser term is at
 * most n^n and each of the positive and negative accumulators is at most
 * 2^(n-1)*n^n.  The largest bound is
 *
 *   2^27 * 28^28 =
 *   4448716805632354662386823820928005747801019383808 < 2^162.
 *
 * Thus a checked four-limb unsigned 256-bit integer is exact throughout the
 * declared range.  The final answer is at most n!, hence also fits.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic -Werror \
 *       396832_01.c -o 396832_01
 *
 * Usage:
 *   ./396832_01
 *   ./396832_01 --upto 28
 *   ./396832_01 --term 28 --verbose
 *   ./396832_01 --check
 *
 * Default/--upto prints completed terms and atomically replaces
 * b396832_01.txt.  --term and --check do not alter the b-file.  Long
 * calculations report progress approximately once per minute.
 */

#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define MAX_N 28
#define DEFAULT_UPTO 28
#define DIRECT_CHECK_MAX_N 10
#define U256_LIMBS 4
#define DECIMAL_BUFFER_SIZE 80
#define BFILE_NAME "b396832_01.txt"
#define BFILE_TEMP_TEMPLATE BFILE_NAME ".tmp.XXXXXX"
#define BFILE_LOCK_NAME BFILE_NAME ".lock"

typedef __uint128_t u128;

typedef struct {
    uint64_t limb[U256_LIMBS];
} U256;

typedef struct {
    uint64_t allowed_mask;
    unsigned multiplicity;
    unsigned selected;
} RowGroup;

typedef struct {
    FILE *stream;
    char temporary[sizeof(BFILE_TEMP_TEMPLATE)];
} BFile;

typedef enum { MODE_UPTO, MODE_TERM, MODE_CHECK } Mode;

static char active_temporary[sizeof(BFILE_TEMP_TEMPLATE)] = "";

_Static_assert(MAX_N < 64, "subset masks require n < 64");
_Static_assert(DIRECT_CHECK_MAX_N < 63,
               "direct-check masks require n < 63");

static _Noreturn void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static void cleanup_temporary(void)
{
    if (active_temporary[0] != '\0')
        (void)unlink(active_temporary);
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

static U256 u256_from_u64(uint64_t value)
{
    U256 result = {{0U, 0U, 0U, 0U}};
    result.limb[0] = value;
    return result;
}

static bool u256_is_zero(const U256 *value)
{
    uint64_t combined = 0U;
    for (size_t i = 0U; i < U256_LIMBS; ++i)
        combined |= value->limb[i];
    return combined == 0U;
}

static int u256_compare(const U256 *left, const U256 *right)
{
    for (size_t i = U256_LIMBS; i > 0U;) {
        --i;
        if (left->limb[i] < right->limb[i])
            return -1;
        if (left->limb[i] > right->limb[i])
            return 1;
    }
    return 0;
}

static void u256_multiply_small(U256 *value, unsigned factor)
{
    u128 carry = 0U;
    for (size_t i = 0U; i < U256_LIMBS; ++i) {
        const u128 product =
            (u128)value->limb[i] * (u128)factor + carry;
        value->limb[i] = (uint64_t)product;
        carry = product >> 64U;
    }
    if (carry != 0U)
        die("256-bit Ryser term overflow");
}

static void u256_add_checked(U256 *accumulator, const U256 *term)
{
    u128 carry = 0U;
    for (size_t i = 0U; i < U256_LIMBS; ++i) {
        const u128 sum =
            (u128)accumulator->limb[i] + (u128)term->limb[i] + carry;
        accumulator->limb[i] = (uint64_t)sum;
        carry = sum >> 64U;
    }
    if (carry != 0U)
        die("256-bit Ryser accumulator overflow");
}

static U256 u256_subtract(const U256 *left, const U256 *right)
{
    if (u256_compare(left, right) < 0)
        die("internal negative 256-bit subtraction");
    U256 result = u256_from_u64(0U);
    u128 borrow = 0U;
    for (size_t i = 0U; i < U256_LIMBS; ++i) {
        const u128 minuend = (u128)left->limb[i];
        const u128 subtrahend = (u128)right->limb[i] + borrow;
        result.limb[i] = (uint64_t)(minuend - subtrahend);
        borrow = minuend < subtrahend ? 1U : 0U;
    }
    if (borrow != 0U)
        die("internal 256-bit subtraction borrow");
    return result;
}

static unsigned u256_divide_by_10(U256 *value)
{
    u128 remainder = 0U;
    for (size_t i = U256_LIMBS; i > 0U;) {
        --i;
        const u128 current = (remainder << 64U) | value->limb[i];
        value->limb[i] = (uint64_t)(current / 10U);
        remainder = current % 10U;
    }
    return (unsigned)remainder;
}

static void u256_to_decimal(const U256 *value,
                            char text[DECIMAL_BUFFER_SIZE])
{
    U256 quotient = *value;
    char reverse[DECIMAL_BUFFER_SIZE];
    size_t length = 0U;
    do {
        if (length + 1U >= sizeof(reverse))
            die("internal decimal buffer overflow");
        reverse[length++] =
            (char)('0' + u256_divide_by_10(&quotient));
    } while (!u256_is_zero(&quotient));
    for (size_t i = 0U; i < length; ++i)
        text[i] = reverse[length - 1U - i];
    text[length] = '\0';
}

static bool u256_equals_u64(const U256 *value, uint64_t expected)
{
    if (value->limb[0] != expected)
        return false;
    for (size_t i = 1U; i < U256_LIMBS; ++i)
        if (value->limb[i] != 0U)
            return false;
    return true;
}

static unsigned least_bit_index(uint64_t value)
{
    if (value == 0U)
        die("internal zero Gray-code change");
    unsigned index = 0U;
    while ((value & UINT64_C(1)) == 0U) {
        value >>= 1U;
        ++index;
    }
    return index;
}

/* Construct and group rows using only the defining inequality. */
static unsigned build_row_groups(int n, RowGroup groups[MAX_N])
{
    unsigned group_count = 0U;
    for (int k = 1; k <= n; ++k) {
        uint64_t mask = 0U;
        for (int value = 1; value <= n; ++value) {
            if ((int64_t)k * (int64_t)value >= (int64_t)n - 1)
                mask |= UINT64_C(1) << (unsigned)(value - 1);
        }
        unsigned group = 0U;
        while (group < group_count && groups[group].allowed_mask != mask)
            ++group;
        if (group == group_count) {
            if (group_count >= (unsigned)n)
                die("internal row-group overflow");
            groups[group_count].allowed_mask = mask;
            groups[group_count].multiplicity = 1U;
            groups[group_count].selected = 0U;
            ++group_count;
        } else {
            ++groups[group].multiplicity;
        }
    }
    unsigned rows = 0U;
    for (unsigned group = 0U; group < group_count; ++group)
        rows += groups[group].multiplicity;
    if (rows != (unsigned)n)
        die("internal row-group size mismatch");
    return group_count;
}

static U256 ryser_term(const RowGroup groups[MAX_N], unsigned group_count,
                       int n)
{
    U256 term = u256_from_u64(1U);
    unsigned factors = 0U;
    for (unsigned group = 0U; group < group_count; ++group) {
        if (groups[group].selected > (unsigned)n)
            die("internal row sum is out of range");
        if (groups[group].selected == 0U)
            return u256_from_u64(0U);
        for (unsigned copy = 0U; copy < groups[group].multiplicity; ++copy) {
            u256_multiply_small(&term, groups[group].selected);
            ++factors;
        }
    }
    if (factors != (unsigned)n)
        die("internal Ryser factor count mismatch");
    return term;
}

static U256 compute_term(int n, bool progress)
{
    if (n == 0)
        return u256_from_u64(1U);

    RowGroup groups[MAX_N];
    memset(groups, 0, sizeof(groups));
    const unsigned group_count = build_row_groups(n, groups);
    const uint64_t subset_count = UINT64_C(1) << (unsigned)n;
    uint64_t previous_gray = 0U;
    bool odd_subset = false;
    U256 even_sum = u256_from_u64(0U);
    U256 odd_sum = u256_from_u64(0U);

    const double start = now_seconds();
    double next_report = start + 60.0;
    for (uint64_t index = 1U; index < subset_count; ++index) {
        const uint64_t gray = index ^ (index >> 1U);
        const uint64_t changed = gray ^ previous_gray;
        const unsigned column = least_bit_index(changed);
        const uint64_t column_mask = UINT64_C(1) << column;
        const bool adding = (gray & column_mask) != 0U;

        for (unsigned group = 0U; group < group_count; ++group) {
            if ((groups[group].allowed_mask & column_mask) == 0U)
                continue;
            if (adding) {
                if (groups[group].selected >= (unsigned)n)
                    die("internal row-sum increment overflow");
                ++groups[group].selected;
            } else {
                if (groups[group].selected == 0U)
                    die("internal row-sum decrement underflow");
                --groups[group].selected;
            }
        }
        odd_subset = !odd_subset;
        const U256 term = ryser_term(groups, group_count, n);
        if (odd_subset)
            u256_add_checked(&odd_sum, &term);
        else
            u256_add_checked(&even_sum, &term);
        previous_gray = gray;

        if (progress) {
            const double current = now_seconds();
            if (current >= next_report) {
                const double elapsed = current - start;
                const double percent = 100.0 * (double)index /
                    (double)(subset_count - 1U);
                const double rate = elapsed > 0.0 ? (double)index / elapsed : 0.0;
                fprintf(stderr,
                        "396832_01 progress: n=%d, subsets=%" PRIu64
                        "/%" PRIu64 ", %.2f%%, groups=%u, "
                        "rate=%.2f M/s, elapsed=%.1f min\n",
                        n, index, subset_count - 1U, percent, group_count,
                        rate / 1000000.0, elapsed / 60.0);
                do {
                    next_report += 60.0;
                } while (next_report <= current);
            }
        }
    }

    if ((n & 1) == 0) {
        if (u256_compare(&even_sum, &odd_sum) < 0)
            die("internal negative even-order permanent");
        return u256_subtract(&even_sum, &odd_sum);
    }
    if (u256_compare(&odd_sum, &even_sum) < 0)
        die("internal negative odd-order permanent");
    return u256_subtract(&odd_sum, &even_sum);
}

/* Independent definition-level permutation enumeration for --check. */
static void brute_rec(int n, int k, uint64_t used, uint64_t *count)
{
    if (k > n) {
        if (*count == UINT64_MAX)
            die("direct-check count overflow");
        ++*count;
        return;
    }
    for (int value = 1; value <= n; ++value) {
        const uint64_t bit = UINT64_C(1) << (unsigned)(value - 1);
        if ((used & bit) == 0U &&
            (int64_t)k * (int64_t)value >= (int64_t)n - 1)
            brute_rec(n, k + 1, used | bit, count);
    }
}

static void run_check(void)
{
    for (int n = 0; n <= DIRECT_CHECK_MAX_N; ++n) {
        const U256 ryser = compute_term(n, false);
        uint64_t direct = 0U;
        if (n == 0)
            direct = 1U;
        else
            brute_rec(n, 1, 0U, &direct);
        if (!u256_equals_u64(&ryser, direct)) {
            fprintf(stderr,
                    "error: Ryser permanent and direct enumeration "
                    "disagree at n=%d\n", n);
            exit(EXIT_FAILURE);
        }
    }
    fprintf(stderr,
            "check passed: definition-level Ryser permanent equals "
            "independent permutation enumeration for n=0..%d\n",
            DIRECT_CHECK_MAX_N);
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

static BFile bfile_begin(void)
{
    BFile file;
    file.stream = NULL;
    memcpy(file.temporary, BFILE_TEMP_TEMPLATE,
           sizeof(BFILE_TEMP_TEMPLATE));
    const int descriptor = mkstemp(file.temporary);
    if (descriptor < 0)
        die("cannot create temporary b-file");
    memcpy(active_temporary, file.temporary, sizeof(active_temporary));

    const mode_t old_mask = umask(0);
    (void)umask(old_mask);
    if (fchmod(descriptor, (mode_t)(0666 & ~old_mask)) != 0) {
        (void)close(descriptor);
        die("cannot set temporary b-file permissions");
    }
    file.stream = fdopen(descriptor, "w");
    if (file.stream == NULL) {
        (void)close(descriptor);
        die("cannot open temporary b-file stream");
    }
    return file;
}

static void bfile_write(BFile *file, int n, const char *value)
{
    if (fprintf(file->stream, "%d %s\n", n, value) < 0)
        die("cannot write temporary b-file");
}

static void bfile_finish(BFile *file)
{
    const int descriptor = fileno(file->stream);
    if (descriptor < 0)
        die("cannot get temporary b-file descriptor");
    bool failed = fflush(file->stream) != 0;
    if (!failed && fsync(descriptor) != 0)
        failed = true;
    if (fclose(file->stream) != 0)
        failed = true;
    file->stream = NULL;
    if (failed)
        die("cannot finish temporary b-file");

    const int lock_descriptor = acquire_bfile_lock();
    if (rename(file->temporary, BFILE_NAME) != 0) {
        const int saved = errno;
        (void)close(lock_descriptor);
        errno = saved;
        die("cannot replace b-file");
    }
    active_temporary[0] = '\0';
    if (close(lock_descriptor) != 0)
        die("cannot close b-file lock");
}

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s [--upto N | --term N | --check] [--verbose]\n"
            "       N must be in 0..%d; default --upto %d\n",
            program, MAX_N, DEFAULT_UPTO);
}

int main(int argc, char **argv)
{
    if (atexit(cleanup_temporary) != 0)
        die("cannot register temporary-file cleanup");

    Mode mode = MODE_UPTO;
    int requested = DEFAULT_UPTO;
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
        run_check();
        return EXIT_SUCCESS;
    }
    if (mode == MODE_TERM) {
        const double start = now_seconds();
        const U256 value = compute_term(requested, true);
        char text[DECIMAL_BUFFER_SIZE];
        u256_to_decimal(&value, text);
        if (printf("%s\n", text) < 0 || fflush(stdout) != 0)
            die("cannot write standard output");
        if (verbose) {
            fprintf(stderr,
                    "396832_01 completed: n=%d, digits=%zu, "
                    "elapsed=%.6f s\n",
                    requested, strlen(text), now_seconds() - start);
        }
        return EXIT_SUCCESS;
    }

    BFile bfile = bfile_begin();
    for (int n = 0; n <= requested; ++n) {
        const double start = now_seconds();
        const U256 value = compute_term(n, true);
        char text[DECIMAL_BUFFER_SIZE];
        u256_to_decimal(&value, text);
        if (printf("%d %s\n", n, text) < 0 || fflush(stdout) != 0)
            die("cannot write standard output");
        bfile_write(&bfile, n, text);
        if (verbose) {
            fprintf(stderr,
                    "396832_01 completed: n=%d, digits=%zu, "
                    "elapsed=%.6f s\n",
                    n, strlen(text), now_seconds() - start);
        }
    }
    bfile_finish(&bfile);
    return EXIT_SUCCESS;
}
