/*
 * A398599 -- direct exhaustive reference implementation.
 *
 * Count permutations p of {1,...,n} satisfying
 *
 *        |p(i+1) - p(i)| != s,   1 <= i < n.
 *
 * The default is s=3.  This program follows the definition directly.  It
 * fills the permutation from left to right and tries every unused value.
 * A branch is rejected exactly when the newly appended value differs from
 * the preceding value by s.  It uses no inclusion-exclusion formula, tile
 * partition, generating function, or memoization.
 *
 * For n>s, runtime is O(n!) in the worst case and working memory is O(n).
 * The input guard n<=30 is an implementation limit, not a practical speed
 * claim: s=3, n=30 is far beyond the reach of exhaustive enumeration.
 * GMP and a safely flushed unsigned-long leaf counter make the final count
 * exact without integer overflow.  If n<=s, the condition is vacuous and
 * the program returns n! directly.
 *
 * --upto prints a(0),...,a(MAX_N) as one comma-separated line.  No b-file
 * is created.
 *
 * Build (Homebrew GMP on Apple silicon):
 *
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic \
 *     -I/opt/homebrew/include -L/opt/homebrew/lib \
 *     398599_01.c -lgmp -o 398599_01
 *
 * Usage:
 *
 *   ./398599_01                     # s=3, print a(0)..a(10)
 *   ./398599_01 --upto 10
 *   ./398599_01 --term 10
 *   ./398599_01 --s 4 --upto 10
 *   ./398599_01 --check             # check s=3 through n=10
 *   ./398599_01 --check 11
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

#define DEFAULT_S 3
#define DEFAULT_MAX_N 10
#define DEFAULT_CHECK_N 10
#define MAX_SUPPORTED_N 30
#define MAX_SUPPORTED_S 128
#define KNOWN_MAX_N 12

/* A398599, n=0..12. */
static const char *const known_terms[KNOWN_MAX_N + 1] = {
    "1",
    "1",
    "2",
    "6",
    "12",
    "48",
    "240",
    "1392",
    "9936",
    "82416",
    "771888",
    "8081808",
    "93273936"
};

typedef struct {
    int n;
    int s;
    bool used[MAX_SUPPORTED_N];
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
    return parse_integer(text, label, 0, MAX_SUPPORTED_N);
}

static int parse_s(const char *text)
{
    return parse_integer(text, "S", 1, MAX_SUPPORTED_S);
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
 * Directly fill position 0,1,...,n-1.  Values are represented by
 * 0,...,n-1 instead of 1,...,n; this does not change their differences.
 */
static void enumerate(SearchContext *context, int position, int last_value)
{
    if (position == context->n) {
        record_permutation(context);
        return;
    }

    for (int value = 0; value < context->n; ++value) {
        if (context->used[value]) {
            continue;
        }
        if (position > 0) {
            int difference = value - last_value;
            if (difference == context->s || difference == -context->s) {
                continue;
            }
        }

        context->used[value] = true;
        enumerate(context, position + 1, value);
        context->used[value] = false;
    }
}

static void verify_known_value(const mpz_t value, int n, int s)
{
    if (s != DEFAULT_S || n > KNOWN_MAX_N) {
        return;
    }

    mpz_t expected;
    mpz_init(expected);
    if (mpz_set_str(expected, known_terms[n], 10) != 0) {
        mpz_clear(expected);
        die("invalid built-in decimal value");
    }
    if (mpz_cmp(value, expected) != 0) {
        gmp_fprintf(stderr,
                    "error: A398599 mismatch at n=%d: got %Zd, "
                    "expected %s\n",
                    n, value, known_terms[n]);
        mpz_clear(expected);
        exit(EXIT_FAILURE);
    }
    mpz_clear(expected);
}

static void compute_value(mpz_t result, int n, int s, bool verbose)
{
    double started = monotonic_seconds();

    if (n <= s) {
        mpz_fac_ui(result, (unsigned long)n);
    } else {
        SearchContext context;
        context.n = n;
        context.s = s;
        memset(context.used, 0, sizeof(context.used));
        context.total = result;
        context.pending = 0UL;

        mpz_set_ui(result, 0UL);
        enumerate(&context, 0, 0);
        flush_pending(&context);
    }

    verify_known_value(result, n, s);
    if (verbose) {
        fprintf(stderr,
                "398599_01: s=%d, n=%d, direct exhaustive DFS, %.3f s\n",
                s, n, monotonic_seconds() - started);
    }
}

static void print_value(const mpz_t value)
{
    if (mpz_out_str(stdout, 10, value) == 0U) {
        die("could not write result");
    }
}

static int print_sequence(int maximum_n, int s)
{
    mpz_t value;
    mpz_init(value);
    for (int n = 0; n <= maximum_n; ++n) {
        compute_value(value, n, s, false);
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
        compute_value(value, n, DEFAULT_S, false);
    }
    mpz_clear(value);

    printf("ok: direct enumeration agrees with A398599 for "
           "s=%d, n=0..%d\n",
           DEFAULT_S, maximum_n);
    return EXIT_SUCCESS;
}

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s [MAX_N]\n"
            "       %s --term N\n"
            "       %s --upto MAX_N\n"
            "       %s --s S [MAX_N]\n"
            "       %s --s S --term N\n"
            "       %s --s S --upto MAX_N\n"
            "       %s --check [CHECK_N]\n"
            "\n"
            "S defaults to %d and may be 1..%d.\n"
            "MAX_N defaults to %d and may be 0..%d.\n"
            "--upto prints a(0)..a(MAX_N), comma-separated.\n"
            "No b-file is created.\n",
            program, program, program, program, program, program, program,
            DEFAULT_S, MAX_SUPPORTED_S, DEFAULT_MAX_N, MAX_SUPPORTED_N);
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

    int s = DEFAULT_S;
    int n = -1;
    bool have_s = false;
    OutputMode mode = MODE_UPTO;

    for (int argument = 1; argument < argc; ++argument) {
        const char *text = argv[argument];
        if (strcmp(text, "--s") == 0) {
            if (have_s || argument + 1 >= argc) {
                usage(program);
                return EXIT_FAILURE;
            }
            s = parse_s(argv[++argument]);
            have_s = true;
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
    if (mode == MODE_UPTO) {
        return print_sequence(n, s);
    }

    mpz_t value;
    mpz_init(value);
    compute_value(value, n, s, true);
    printf("%d ", n);
    print_value(value);
    putchar('\n');
    mpz_clear(value);
    return EXIT_SUCCESS;
}
