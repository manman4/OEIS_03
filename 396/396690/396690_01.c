/*
 * Direct exhaustive reference implementation for
 *
 *   Number of permutations p of [n+k] such that
 *   |p(i+1) - p(i)| != n for every 1 <= i < n+k.
 *
 * The default is k=4.  This program deliberately follows the definition:
 * it constructs every admissible permutation from left to right and rejects
 * a branch as soon as its newest adjacent difference equals n.  It does not
 * use inclusion-exclusion, the closed formula valid for n>=k, or the path-
 * polynomial algorithm for the exceptional range 0<n<k.
 *
 * If n=0, a zero difference cannot occur between two distinct permutation
 * values.  If k=0, the forbidden difference n equals the permutation size
 * and again cannot occur.  Those cases are returned directly as (n+k)!.
 *
 * Runtime is factorial in the worst case and working memory is O(n+k).  The
 * n+k<=30 guard is an implementation limit, not a practical speed claim.
 * GMP and a safely flushed unsigned-long leaf counter make the count exact
 * without integer overflow.  Stopping the program early produces no output
 * file and cannot corrupt persistent data.
 *
 * --upto prints a(0),...,a(MAX_N) as one comma-separated line.  Here k is
 * fixed, term n uses permutation size n+k, and the forbidden difference is
 * n.  No b-file is created.
 *
 * Build (Homebrew GMP on Apple silicon):
 *
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic \
 *     -I/opt/homebrew/include -L/opt/homebrew/lib \
 *     396690_01.c -lgmp -o 396690_01
 *
 * Usage:
 *
 *   ./396690_01                         # k=4, a(0)..a(6)
 *   ./396690_01 --k 4 --upto 6
 *   ./396690_01 --k 4 --term 6
 *   ./396690_01 --check                 # k=4, n=0..6
 *   ./396690_01 --check 7
 */

#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <gmp.h>

#define DEFAULT_K 4
#define DEFAULT_MAX_N 6
#define DEFAULT_CHECK_N 6
#define MAX_SUPPORTED_TOTAL_SIZE 30
#define KNOWN_MAX_N 8

/* Fixed k=4, new index n=0..8 (permutation size n+4). */
static const char *const known_k4_terms[KNOWN_MAX_N + 1] = {
    "24",
    "14",
    "152",
    "1392",
    "13824",
    "140160",
    "1543680",
    "18385920",
    "235791360"
};

typedef struct {
    int size;
    int forbidden_difference;
    bool used[MAX_SUPPORTED_TOTAL_SIZE];
    mpz_ptr total;
    unsigned long pending;
} SearchContext;

typedef enum {
    MODE_UPTO,
    MODE_TERM
} OutputMode;

static void die(const char *message)
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

static int parse_integer(const char *text, const char *label,
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

static int parse_n(const char *text, const char *label)
{
    return parse_integer(text, label, 0, MAX_SUPPORTED_TOTAL_SIZE);
}

static int parse_k(const char *text)
{
    return parse_integer(text, "K", 0, MAX_SUPPORTED_TOTAL_SIZE);
}

static void validate_total_size(int n, int k)
{
    if (n > MAX_SUPPORTED_TOTAL_SIZE - k) {
        fprintf(stderr,
                "error: N+K must be at most %d: n=%d, k=%d\n",
                MAX_SUPPORTED_TOTAL_SIZE, n, k);
        exit(EXIT_FAILURE);
    }
}

static void flush_pending(SearchContext *context)
{
    if (context->pending != 0UL) {
        mpz_add_ui(context->total, context->total, context->pending);
        context->pending = 0UL;
    }
}

static void record_permutation(SearchContext *context)
{
    if (context->pending == ULONG_MAX) {
        flush_pending(context);
    }
    ++context->pending;
}

/*
 * Fill positions 0,1,...,size-1 directly.  Values are represented by
 * 0,...,size-1 instead of 1,...,size; translation preserves differences.
 */
static void enumerate(SearchContext *context, int position, int last_value)
{
    if (position == context->size) {
        record_permutation(context);
        return;
    }

    for (int value = 0; value < context->size; ++value) {
        if (context->used[value]) {
            continue;
        }
        if (position > 0) {
            int difference = value - last_value;
            if (difference == context->forbidden_difference ||
                difference == -context->forbidden_difference) {
                continue;
            }
        }

        context->used[value] = true;
        enumerate(context, position + 1, value);
        context->used[value] = false;
    }
}

static void verify_known_value(const mpz_t value, int n, int k)
{
    if (k != DEFAULT_K || n > KNOWN_MAX_N) {
        return;
    }

    mpz_t expected;
    mpz_init(expected);
    if (mpz_set_str(expected, known_k4_terms[n], 10) != 0) {
        mpz_clear(expected);
        die("invalid built-in decimal value");
    }
    if (mpz_cmp(value, expected) != 0) {
        gmp_fprintf(stderr,
                    "error: fixed-k mismatch at k=%d, n=%d: got %Zd, "
                    "expected %s\n",
                    k, n, value, known_k4_terms[n]);
        mpz_clear(expected);
        exit(EXIT_FAILURE);
    }
    mpz_clear(expected);
}

static void compute_value(mpz_t result, int n, int k, bool verbose)
{
    double started = monotonic_seconds();
    int size = n + k;

    /*
     * If n=0, equal adjacent values would be required.  If k=0, the
     * forbidden difference n equals the size.  Both are impossible.
     */
    if (n == 0 || k == 0) {
        mpz_fac_ui(result, (unsigned long)size);
    } else {
        SearchContext context;
        context.size = size;
        context.forbidden_difference = n;
        memset(context.used, 0, sizeof(context.used));
        context.total = result;
        context.pending = 0UL;

        mpz_set_ui(result, 0UL);
        enumerate(&context, 0, 0);
        flush_pending(&context);
    }

    verify_known_value(result, n, k);
    if (verbose) {
        fprintf(stderr,
                "396690_01: k=%d, n=%d, size=%d, "
                "forbidden difference=%d, "
                "direct exhaustive DFS, %.3f s\n",
                k, n, size, n, monotonic_seconds() - started);
    }
}

static void print_value(const mpz_t value)
{
    if (mpz_out_str(stdout, 10, value) == 0U) {
        die("could not write result");
    }
}

static int print_sequence(int maximum_n, int k)
{
    mpz_t value;
    mpz_init(value);
    for (int n = 0; n <= maximum_n; ++n) {
        compute_value(value, n, k, false);
        if (n != 0) {
            fputs(", ", stdout);
        }
        print_value(value);
    }
    putchar('\n');
    mpz_clear(value);
    return EXIT_SUCCESS;
}

static int check_implementation(int maximum_n)
{
    if (maximum_n > KNOWN_MAX_N) {
        fprintf(stderr, "error: CHECK_N must be in 0..%d: %d\n",
                KNOWN_MAX_N, maximum_n);
        return EXIT_FAILURE;
    }

    mpz_t value;
    mpz_init(value);
    for (int n = 0; n <= maximum_n; ++n) {
        compute_value(value, n, DEFAULT_K, false);
    }
    mpz_clear(value);

    printf("ok: direct enumeration agrees with built-in fixed-k terms "
           "for k=%d, n=0..%d\n",
           DEFAULT_K, maximum_n);
    return EXIT_SUCCESS;
}

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s [MAX_N]\n"
            "       %s --term N\n"
            "       %s --upto MAX_N\n"
            "       %s --k K [MAX_N]\n"
            "       %s --k K --term N\n"
            "       %s --k K --upto MAX_N\n"
            "       %s --check [CHECK_N]\n"
            "\n"
            "K defaults to %d and may be 0..%d.\n"
            "MAX_N defaults to %d and may be 0..%d.\n"
            "N+K must be at most %d.\n"
            "Term n counts permutations of [n+k] avoiding difference n.\n"
            "--upto fixes k and prints a(0)..a(MAX_N), comma-separated.\n"
            "No b-file is created.\n",
            program, program, program, program, program, program, program,
            DEFAULT_K, MAX_SUPPORTED_TOTAL_SIZE,
            DEFAULT_MAX_N, MAX_SUPPORTED_TOTAL_SIZE,
            MAX_SUPPORTED_TOTAL_SIZE);
}

int main(int argc, char **argv)
{
    const char *program = argv[0];
    if (argc == 2 &&
        (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
        usage(program);
        return EXIT_SUCCESS;
    }

    if (argc >= 2 && strcmp(argv[1], "--check") == 0) {
        if (argc > 3) {
            usage(program);
            return EXIT_FAILURE;
        }
        int maximum_n = argc == 3 ?
            parse_integer(argv[2], "CHECK_N", 0, KNOWN_MAX_N) :
            DEFAULT_CHECK_N;
        return check_implementation(maximum_n);
    }

    int k = DEFAULT_K;
    int n = -1;
    bool have_k = false;
    OutputMode mode = MODE_UPTO;

    for (int argument = 1; argument < argc; ++argument) {
        const char *text = argv[argument];
        if (strcmp(text, "--k") == 0) {
            if (have_k || argument + 1 >= argc) {
                usage(program);
                return EXIT_FAILURE;
            }
            k = parse_k(argv[++argument]);
            have_k = true;
        } else if (strcmp(text, "--term") == 0 ||
                   strcmp(text, "--upto") == 0) {
            if (n >= 0 || argument + 1 >= argc) {
                usage(program);
                return EXIT_FAILURE;
            }
            mode = strcmp(text, "--term") == 0 ? MODE_TERM : MODE_UPTO;
            n = parse_n(argv[++argument],
                        mode == MODE_TERM ? "N" : "MAX_N");
        } else {
            if (text[0] == '-' || n >= 0) {
                usage(program);
                return EXIT_FAILURE;
            }
            n = parse_n(text, "MAX_N");
            mode = MODE_UPTO;
        }
    }

    if (n < 0) {
        n = DEFAULT_MAX_N;
    }
    validate_total_size(n, k);
    if (mode == MODE_UPTO) {
        return print_sequence(n, k);
    }

    mpz_t value;
    mpz_init(value);
    compute_value(value, n, k, true);
    printf("%d ", n);
    print_value(value);
    putchar('\n');
    mpz_clear(value);
    return EXIT_SUCCESS;
}
