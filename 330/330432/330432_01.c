/*
 * A330432 -- number of permutations sigma of [n] such that
 *
 *                     k * sigma(k) >= n                 (1)
 *
 * for every 1 <= k <= n.
 *
 * Exact product formula
 * ---------------------
 * At position k, (1) says
 *
 *        sigma(k) is in A_k = {ceil(n/k), ..., n}.
 *
 * The sets are nested:
 *
 *        A_1 subseteq A_2 subseteq ... subseteq A_n.
 *
 * Process positions in the order 1,2,...,n.  All k-1 values already used
 * belong to A_k, so precisely |A_k|-(k-1) choices remain at position k.
 * Therefore
 *
 *   a(n) = Product_{k=1..n} (n-k+2-ceil(n/k)),          (2)
 *
 * with a(0)=1.  Every factor is positive.  Formula (2) is an exact count of
 * the permutations, not a heuristic.  It also immediately shows sigma(1)=n;
 * value 1 is allowed only at k=n, so sigma(n)=1.
 *
 * The result can exceed machine integer types.  It is stored in a checked,
 * dynamically growing base-10^9 unsigned integer and multiplied by the
 * factors in (2).  Known OEIS values through n=24 are verification strings
 * only: each requested term is calculated first and then compared.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic -Werror \
 *       330432_01.c -o 330432_01
 *
 * Usage:
 *   ./330432_01
 *   ./330432_01 --upto 100
 *   ./330432_01 --term 10000 --verbose
 *   ./330432_01 --check
 *
 * Default/--upto prints completed terms and atomically replaces
 * b330432_01.txt.  --term and --check do not alter the b-file.  Long
 * individual terms report progress approximately once per minute.
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

#define MAX_N 100000
#define DEFAULT_UPTO 24
#define DIRECT_CHECK_MAX_N 10
#define BASE UINT32_C(1000000000)
#define PROGRESS_GRANULARITY 16384
#define BFILE_NAME "b330432_01.txt"
#define BFILE_TEMP_TEMPLATE BFILE_NAME ".tmp.XXXXXX"
#define BFILE_LOCK_NAME BFILE_NAME ".lock"

static const char *const known[] = {
    "1", "1", "1", "1", "2", "4", "18", "54", "384", "1920",
    "15000", "108000", "1270080", "8890560", "126443520",
    "1440270720", "18811699200", "232243200000", "4303470124800",
    "56336336179200", "1165698293760000", "17784994106880000",
    "349410939842764800", "6977881195084800000",
    "173541988447027200000"
};

#define KNOWN_MAX_N ((int)(sizeof(known) / sizeof(known[0]) - 1U))

typedef struct {
    uint32_t *digit;
    size_t length;
    size_t capacity;
} BigUInt;

typedef struct {
    FILE *stream;
    char temporary[sizeof(BFILE_TEMP_TEMPLATE)];
} BFile;

typedef enum { MODE_UPTO, MODE_TERM, MODE_CHECK } Mode;

static char active_temporary[sizeof(BFILE_TEMP_TEMPLATE)] = "";

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

static void big_init(BigUInt *value)
{
    value->digit = malloc(sizeof(*value->digit));
    if (value->digit == NULL)
        die("cannot allocate big integer");
    value->digit[0] = 1U;
    value->length = 1U;
    value->capacity = 1U;
}

static void big_destroy(BigUInt *value)
{
    free(value->digit);
    value->digit = NULL;
    value->length = 0U;
    value->capacity = 0U;
}

static void big_reserve(BigUInt *value, size_t needed)
{
    if (needed <= value->capacity)
        return;
    size_t capacity = value->capacity;
    while (capacity < needed) {
        if (capacity > SIZE_MAX / 2U)
            die("big-integer capacity overflow");
        capacity *= 2U;
    }
    if (capacity > SIZE_MAX / sizeof(*value->digit))
        die("big-integer allocation size overflow");
    uint32_t *const grown = realloc(value->digit,
                                    capacity * sizeof(*grown));
    if (grown == NULL)
        die("cannot grow big integer");
    value->digit = grown;
    value->capacity = capacity;
}

static void big_multiply_small(BigUInt *value, uint32_t factor)
{
    if (factor == 0U)
        die("internal zero product factor");
    big_reserve(value, value->length + 1U);
    uint64_t carry = 0U;
    for (size_t i = 0U; i < value->length; ++i) {
        const uint64_t product =
            (uint64_t)value->digit[i] * factor + carry;
        value->digit[i] = (uint32_t)(product % BASE);
        carry = product / BASE;
    }
    if (carry != 0U) {
        if (carry >= BASE)
            die("internal multiplication carry is too large");
        value->digit[value->length++] = (uint32_t)carry;
    }
}

static unsigned decimal_digits_u32(uint32_t value)
{
    unsigned digits = 1U;
    while (value >= 10U) {
        value /= 10U;
        ++digits;
    }
    return digits;
}

static size_t big_decimal_digits(const BigUInt *value)
{
    if (value->length - 1U > (SIZE_MAX - 9U) / 9U)
        die("decimal length overflow");
    return (value->length - 1U) * 9U +
        decimal_digits_u32(value->digit[value->length - 1U]);
}

static char *big_to_string(const BigUInt *value)
{
    const size_t digits = big_decimal_digits(value);
    if (digits == SIZE_MAX)
        die("decimal string size overflow");
    char *const text = malloc(digits + 1U);
    if (text == NULL)
        die("cannot allocate decimal string");

    size_t used = 0U;
    int written = snprintf(text, digits + 1U, "%" PRIu32,
                           value->digit[value->length - 1U]);
    if (written < 0 || (size_t)written > digits) {
        free(text);
        die("cannot format big integer");
    }
    used = (size_t)written;
    for (size_t i = value->length - 1U; i > 0U;) {
        --i;
        written = snprintf(text + used, digits + 1U - used,
                           "%09" PRIu32, value->digit[i]);
        if (written != 9) {
            free(text);
            die("cannot format big-integer limb");
        }
        used += 9U;
    }
    if (used != digits) {
        free(text);
        die("internal decimal length mismatch");
    }
    return text;
}

static BigUInt compute_term(int n, bool progress)
{
    BigUInt result;
    big_init(&result);
    if (n == 0)
        return result;

    const double start = now_seconds();
    double next_report = start + 60.0;
    for (int k = 1; k <= n; ++k) {
        const int quotient = n / k;
        const int ceiling = quotient + (n % k != 0 ? 1 : 0);
        const int factor = n - k + 2 - ceiling;
        if (factor < 1 || factor > n)
            die("internal product factor is out of range");
        big_multiply_small(&result, (uint32_t)factor);

        if (progress && (k % PROGRESS_GRANULARITY == 0 || k == n)) {
            const double current = now_seconds();
            if (current >= next_report) {
                fprintf(stderr,
                        "330432_01 progress: n=%d, factors=%d/%d, "
                        "digits=%zu, elapsed=%.1f min\n",
                        n, k, n, big_decimal_digits(&result),
                        (current - start) / 60.0);
                do {
                    next_report += 60.0;
                } while (next_report <= current);
            }
        }
    }
    return result;
}

static void verify_known(int n, const char *text)
{
    if (n <= KNOWN_MAX_N && strcmp(text, known[n]) != 0) {
        fprintf(stderr, "error: computed value disagrees with known "
                "A330432 term at n=%d\n", n);
        exit(EXIT_FAILURE);
    }
}

/* Independent definition-level recursion, used only by --check. */
static void brute_rec(int n, int k, uint64_t used, uint64_t *count)
{
    if (k > n) {
        if (*count == UINT64_MAX)
            die("direct-check count overflow");
        ++*count;
        return;
    }
    for (int value = 1; value <= n; ++value) {
        const uint64_t bit = UINT64_C(1) << (unsigned)value;
        if ((used & bit) == 0U && k * value >= n)
            brute_rec(n, k + 1, used | bit, count);
    }
}

static void run_check(void)
{
    for (int n = 0; n <= DIRECT_CHECK_MAX_N; ++n) {
        BigUInt formula = compute_term(n, false);
        char *const text = big_to_string(&formula);
        uint64_t direct = 0U;
        if (n == 0)
            direct = 1U;
        else
            brute_rec(n, 1, 0U, &direct);
        char direct_text[32];
        const int written = snprintf(direct_text, sizeof(direct_text),
                                     "%" PRIu64, direct);
        if (written < 0 || (size_t)written >= sizeof(direct_text))
            die("cannot format direct-check result");
        if (strcmp(text, direct_text) != 0) {
            fprintf(stderr, "error: product formula and direct enumeration "
                    "disagree at n=%d\n", n);
            exit(EXIT_FAILURE);
        }
        verify_known(n, text);
        free(text);
        big_destroy(&formula);
    }
    fprintf(stderr, "check passed: product formula equals independent "
            "permutation enumeration for n=0..%d; known terms are also "
            "verified\n", DIRECT_CHECK_MAX_N);
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
        BigUInt result = compute_term(requested, true);
        char *const text = big_to_string(&result);
        verify_known(requested, text);
        if (printf("%s\n", text) < 0 || fflush(stdout) != 0)
            die("cannot write standard output");
        if (verbose) {
            fprintf(stderr,
                    "330432_01 completed: n=%d, digits=%zu, limbs=%zu, "
                    "memory=%.3f MiB, elapsed=%.6f s\n",
                    requested, strlen(text), result.length,
                    (double)result.capacity * sizeof(*result.digit) /
                        (1024.0 * 1024.0),
                    now_seconds() - start);
        }
        free(text);
        big_destroy(&result);
        return EXIT_SUCCESS;
    }

    BFile bfile = bfile_begin();
    for (int n = 0; n <= requested; ++n) {
        const double start = now_seconds();
        BigUInt result = compute_term(n, true);
        char *const text = big_to_string(&result);
        verify_known(n, text);
        if (printf("%d %s\n", n, text) < 0 || fflush(stdout) != 0)
            die("cannot write standard output");
        bfile_write(&bfile, n, text);
        if (verbose) {
            fprintf(stderr,
                    "330432_01 completed: n=%d, digits=%zu, limbs=%zu, "
                    "memory=%.3f MiB, elapsed=%.6f s\n",
                    n, strlen(text), result.length,
                    (double)result.capacity * sizeof(*result.digit) /
                        (1024.0 * 1024.0),
                    now_seconds() - start);
        }
        free(text);
        big_destroy(&result);
    }
    bfile_finish(&bfile);
    return EXIT_SUCCESS;
}
