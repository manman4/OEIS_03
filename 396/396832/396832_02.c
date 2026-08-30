/*
 * A396832 -- number of permutations p of [n] such that
 *
 *                       k * p(k) >= n - 1
 *
 * for every 1 <= k <= n.
 *
 * Exact product formula
 * ---------------------
 * For n >= 2, the allowed values at position k form the nested suffix
 *
 *        A_k = {ceil((n-1)/k), ..., n}.
 *
 * When positions are filled in the order 1,2,...,n, all k-1 values already
 * used belong to A_k.  Consequently exactly
 *
 *        |A_k|-(k-1) = n-k+2-ceil((n-1)/k)
 *
 * values remain, and
 *
 *   a(n) = Product_{k=1..n} (n-k+2-ceil((n-1)/k))       (n >= 2).
 *
 * The exceptional values are a(0)=a(1)=1.  This formula is proved in
 * 396832_product_formula_proof.md.
 *
 * Exactness and supported range
 * -----------------------------
 * ceil((n-1)/k) is evaluated by integer quotient and remainder; no floating
 * point arithmetic is used.  The product is held in a dynamically growing
 * unsigned integer with base 10^9 limbs.  For n <= 500 every multiplier is
 * in 1..500, so one-limb multiplication fits uint64_t:
 *
 *   (10^9-1)*500 + 499 < 2^64.
 *
 * The result is at most n!, so a(500) has at most 1135 decimal digits.
 * Allocation sizes and arithmetic invariants are checked at runtime.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic -Werror \
 *       396832_02.c -o 396832_02
 *
 * Usage:
 *   ./396832_02
 *   ./396832_02 --upto 500
 *   ./396832_02 --term 500 --verbose
 *   ./396832_02 --check
 *
 * Default/--upto prints completed terms and atomically replaces
 * b396832_02.txt.  --term and --check do not alter the b-file.
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

#define MAX_N 500
#define DEFAULT_UPTO 500
#define DIRECT_CHECK_MAX_N 10
#define BASE UINT32_C(1000000000)
#define BFILE_NAME "b396832_02.txt"
#define BFILE_TEMP_TEMPLATE BFILE_NAME ".tmp.XXXXXX"
#define BFILE_LOCK_NAME BFILE_NAME ".lock"

static const char *const known[] = {
    "1",
    "1",
    "2",
    "4",
    "8",
    "24",
    "72",
    "384",
    "1536",
    "12000",
    "72000",
    "622080",
    "5080320",
    "63221760",
    "505774080",
    "7524679680",
    "92897280000",
    "1293304320000",
    "17213880499200",
    "331085905920000",
    "4662793175040000",
    "99831697097932800",
    "1620935957613772800",
    "33217021226188800000",
    "694167953788108800000",
    "17789060787852017664000",
    "320203094181336317952000",
    "8460040478736327376896000",
    "211941740720760653611008000"
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

_Static_assert(MAX_N < (int)BASE,
               "each small multiplier must be less than BASE");
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

    uint32_t *const grown =
        realloc(value->digit, capacity * sizeof(*value->digit));
    if (grown == NULL)
        die("cannot grow big integer");
    value->digit = grown;
    value->capacity = capacity;
}

static void big_multiply_small(BigUInt *value, uint32_t factor)
{
    if (factor == 0U || factor >= BASE)
        die("internal product factor is out of range");

    big_reserve(value, value->length + 1U);
    uint64_t carry = 0U;
    for (size_t i = 0U; i < value->length; ++i) {
        const uint64_t product =
            (uint64_t)value->digit[i] * (uint64_t)factor + carry;
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
    if (value->length == 0U)
        die("internal empty big integer");
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

    int written = snprintf(text, digits + 1U, "%" PRIu32,
                           value->digit[value->length - 1U]);
    if (written < 0 || (size_t)written > digits) {
        free(text);
        die("cannot format big integer");
    }
    size_t used = (size_t)written;

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

static BigUInt compute_term(int n)
{
    BigUInt result;
    big_init(&result);

    if (n == 0 || n == 1)
        return result;

    const int numerator = n - 1;
    for (int k = 1; k <= n; ++k) {
        const int quotient = numerator / k;
        const int ceiling =
            quotient + (numerator % k != 0 ? 1 : 0);
        const int factor = n - k + 2 - ceiling;

        if (factor < 1 || factor > n)
            die("internal formula factor is out of range");
        big_multiply_small(&result, (uint32_t)factor);
    }
    return result;
}

static void verify_known(int n, const char *text)
{
    if (n <= KNOWN_MAX_N && strcmp(text, known[n]) != 0) {
        fprintf(stderr,
                "error: computed value disagrees with known A396832 "
                "term at n=%d\n", n);
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
        const uint64_t bit = UINT64_C(1) << (unsigned)(value - 1);
        if ((used & bit) == 0U && k * value >= n - 1)
            brute_rec(n, k + 1, used | bit, count);
    }
}

static void run_check(void)
{
    for (int n = 0; n <= KNOWN_MAX_N; ++n) {
        BigUInt formula = compute_term(n);
        char *const text = big_to_string(&formula);
        verify_known(n, text);
        free(text);
        big_destroy(&formula);
    }

    for (int n = 0; n <= DIRECT_CHECK_MAX_N; ++n) {
        BigUInt formula = compute_term(n);
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
            fprintf(stderr,
                    "error: product formula and direct enumeration "
                    "disagree at n=%d\n", n);
            exit(EXIT_FAILURE);
        }
        free(text);
        big_destroy(&formula);
    }

    fprintf(stderr,
            "check passed: known terms n=0..%d and independent direct "
            "enumeration n=0..%d agree\n",
            KNOWN_MAX_N, DIRECT_CHECK_MAX_N);
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
        BigUInt result = compute_term(requested);
        char *const text = big_to_string(&result);
        verify_known(requested, text);
        if (printf("%s\n", text) < 0 || fflush(stdout) != 0)
            die("cannot write standard output");
        if (verbose) {
            fprintf(stderr,
                    "396832_02 completed: n=%d, digits=%zu, limbs=%zu, "
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
        BigUInt result = compute_term(n);
        char *const text = big_to_string(&result);
        verify_known(n, text);
        if (printf("%d %s\n", n, text) < 0 || fflush(stdout) != 0)
            die("cannot write standard output");
        bfile_write(&bfile, n, text);
        if (verbose) {
            fprintf(stderr,
                    "396832_02 completed: n=%d, digits=%zu, limbs=%zu, "
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
