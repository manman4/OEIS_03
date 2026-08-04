/*
 * A189285 and directed a_{d,d} -- deliberately naive enumeration.
 *
 * Count permutations p of {1,...,n} satisfying
 *
 *                 p(i+d) - p(i) != d,   1 <= i <= n-d.
 *
 * The command line accepts 2<=d<=64.  Its default d=6 is A189285.
 *
 * This is a direct reference implementation.  It uses neither the
 * inclusion-exclusion formula nor memoization.  Positions are filled from
 * left to right, and a branch is rejected exactly when its newly assigned
 * value makes p(i)-p(i-d)=d.  Thus all permutations are accounted for once,
 * with only immediate constraint pruning.  GMP stores the count, so
 * arithmetic does not overflow; allocated memory and recursion depth are
 * O(n).  Runtime is O(n!), making large n
 * intentionally impractical even though the input guard permits n<=128.
 *
 * The sequence has OEIS offset 0.  A range run first writes
 * b189285_01_dD_part.txt, flushes each completed term, and renames it to
 * b189285_01_dD.txt only after the requested range finishes.
 *
 * Reference:
 *   https://oeis.org/A189285
 *
 * Build:
 *
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic \
 *     -I/opt/homebrew/include -L/opt/homebrew/lib \
 *     189285_01.c -lgmp -o 189285_01
 *
 * Usage:
 *
 *   ./189285_01                 # write a(0)..a(10)
 *   ./189285_01 11
 *   ./189285_01 --upto 11
 *   ./189285_01 --term 11
 *   ./189285_01 --d 2 --term 11
 *   ./189285_01 --check         # compare a(0)..a(10) with OEIS
 *   ./189285_01 --check 11
 */

#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <gmp.h>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#define A189285_D 6
#define SEQUENCE_OFFSET 0
#define DEFAULT_MAX_N 10
#define DEFAULT_CHECK_N 10
#define MAX_SUPPORTED_N 128
#define MAX_SUPPORTED_D 64
#define KNOWN_MAX_N 24

static const char *const known_terms[KNOWN_MAX_N + 1] = {
    "1",
    "1",
    "2",
    "6",
    "24",
    "120",
    "720",
    "4920",
    "37488",
    "319644",
    "3033264",
    "31784280",
    "364902480",
    "4538652840",
    "61102571376",
    "885045657564",
    "13722397569072",
    "226742901078120",
    "3977354871110160",
    "73816786920489720",
    "1444940702597713008",
    "29750236302549282948",
    "642693417769786746864",
    "14535097378893765189240",
    "343437047642562028552080"
};

typedef struct {
    int n;
    int d;
    bool used[MAX_SUPPORTED_N];
    int permutation[MAX_SUPPORTED_N];
} SearchContext;

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

static int parse_n(const char *text, const char *label)
{
    char *end = NULL;
    errno = 0;
    long value = strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' ||
        value < SEQUENCE_OFFSET || value > MAX_SUPPORTED_N) {
        fprintf(stderr, "error: %s must be in %d..%d: %s\n",
                label, SEQUENCE_OFFSET, MAX_SUPPORTED_N, text);
        exit(EXIT_FAILURE);
    }
    return (int)value;
}

static int parse_d(const char *text)
{
    char *end = NULL;
    errno = 0;
    long value = strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' ||
        value < 2 || value > MAX_SUPPORTED_D) {
        fprintf(stderr, "error: D must be in 2..%d: %s\n",
                MAX_SUPPORTED_D, text);
        exit(EXIT_FAILURE);
    }
    return (int)value;
}

static void enumerate(SearchContext *context, int position, mpz_t total)
{
    if (position == context->n) {
        mpz_add_ui(total, total, 1UL);
        return;
    }
    for (int value = 0; value < context->n; ++value) {
        if (context->used[value]) {
            continue;
        }
        if (position >= context->d &&
            value - context->permutation[position - context->d] ==
                context->d) {
            continue;
        }
        context->used[value] = true;
        context->permutation[position] = value;
        enumerate(context, position + 1, total);
        context->used[value] = false;
    }
}

static void compute_value(mpz_t result, int n, int d, bool verbose)
{
    SearchContext context;
    context.n = n;
    context.d = d;
    memset(context.used, 0, sizeof(context.used));

    double started = monotonic_seconds();
    mpz_set_ui(result, 0UL);
    enumerate(&context, 0, result);
    double seconds = monotonic_seconds() - started;

    mpz_t expected;
    mpz_init(expected);
    if (d == A189285_D && n <= KNOWN_MAX_N &&
        (mpz_set_str(expected, known_terms[n], 10) != 0 ||
         mpz_cmp(result, expected) != 0)) {
        gmp_fprintf(stderr,
                    "error: A189285 mismatch at n=%d: got %Zd, "
                    "expected %s\n",
                    n, result, known_terms[n]);
        mpz_clear(expected);
        exit(EXIT_FAILURE);
    }
    mpz_clear(expected);
    if (verbose) {
        fprintf(stderr,
                "189285_01: d=%d, n=%d, exhaustive DFS, %.3f s\n",
                d, n, seconds);
    }
}

static char *xmalloc(size_t size)
{
    char *pointer = malloc(size == 0U ? 1U : size);
    if (pointer == NULL) {
        die("out of memory");
    }
    return pointer;
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
                              sizeof(executable) - 1U);
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
        memcpy(executable, argv0, length + 1U);
    }
    const char *base = realpath(executable, resolved);
    if (base == NULL) {
        base = executable;
    }
    const char *slash = strrchr(base, '/');
    size_t directory_length =
        slash == NULL ? 1U : (size_t)(slash - base);
    if (slash != NULL && directory_length == 0U) {
        directory_length = 1U;
    }
    size_t filename_length = strlen(filename);
    if (directory_length > SIZE_MAX - filename_length - 2U) {
        die("output path is too long");
    }
    char *path = xmalloc(directory_length + filename_length + 2U);
    if (slash == NULL) {
        path[0] = '.';
    } else if (slash == base) {
        path[0] = '/';
    } else {
        memcpy(path, base, directory_length);
    }
    path[directory_length] = '/';
    memcpy(path + directory_length + 1U, filename,
           filename_length + 1U);
    return path;
}

static int check_implementation(int maximum_n)
{
    mpz_t value;
    mpz_init(value);
    for (int n = SEQUENCE_OFFSET; n <= maximum_n; ++n) {
        compute_value(value, n, A189285_D, false);
    }
    mpz_clear(value);
    printf("ok: exhaustive enumeration agrees with A189285 "
           "for n=%d..%d\n",
           SEQUENCE_OFFSET, maximum_n);
    return EXIT_SUCCESS;
}

static void produce_file(const char *argv0, int maximum_n, int d)
{
    char final_name[64];
    char part_name[64];
    int final_length = snprintf(final_name, sizeof(final_name),
                                "b189285_01_d%d.txt", d);
    int part_length = snprintf(part_name, sizeof(part_name),
                               "b189285_01_d%d_part.txt", d);
    if (final_length < 0 || (size_t)final_length >= sizeof(final_name) ||
        part_length < 0 || (size_t)part_length >= sizeof(part_name)) {
        die("output filename is too long");
    }
    char *final_path = path_beside_executable(argv0, final_name);
    char *part_path = path_beside_executable(argv0, part_name);
    FILE *stream = fopen(part_path, "w");
    if (stream == NULL) {
        fprintf(stderr, "error: cannot open %s: %s\n",
                part_path, strerror(errno));
        free(final_path);
        free(part_path);
        exit(EXIT_FAILURE);
    }

    mpz_t value;
    mpz_init(value);
    for (int n = SEQUENCE_OFFSET; n <= maximum_n; ++n) {
        compute_value(value, n, d, true);
        if (gmp_fprintf(stream, "%d %Zd\n", n, value) < 0 ||
            fflush(stream) != 0) {
            mpz_clear(value);
            fclose(stream);
            free(final_path);
            free(part_path);
            die("could not write the A189285 output file");
        }
    }
    mpz_clear(value);
    if (fclose(stream) != 0) {
        free(final_path);
        free(part_path);
        die("could not close the A189285 output file");
    }
    if (rename(part_path, final_path) != 0) {
        fprintf(stderr, "error: cannot rename %s to %s: %s\n",
                part_path, final_path, strerror(errno));
        free(final_path);
        free(part_path);
        exit(EXIT_FAILURE);
    }
    printf("wrote %s (d=%d, n=%d..%d)\n",
           final_path, d, SEQUENCE_OFFSET, maximum_n);
    free(final_path);
    free(part_path);
}

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s [MAX_N]\n"
            "       %s --term N\n"
            "       %s --upto MAX_N\n"
            "       %s --d D [MAX_N]\n"
            "       %s --d D --term N\n"
            "       %s --d D --upto MAX_N\n"
            "       %s --check [MAX_N]\n"
            "\n"
            "D defaults to %d (A189285) and may be 2..%d.\n"
            "MAX_N defaults to %d and may be at most %d.\n"
            "A range run writes b189285_01_dD.txt.\n"
            "Warning: this program explicitly visits all n! permutations.\n",
            program, program, program, program, program, program, program,
            A189285_D, MAX_SUPPORTED_D, DEFAULT_MAX_N, MAX_SUPPORTED_N);
}

int main(int argc, char **argv)
{
    if (argc == 2 &&
        (strcmp(argv[1], "--help") == 0 ||
         strcmp(argv[1], "-h") == 0)) {
        usage(argv[0]);
        return EXIT_SUCCESS;
    }
    if (argc >= 2 && strcmp(argv[1], "--check") == 0) {
        if (argc > 3) {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
        int maximum_n = argc == 3
                            ? parse_n(argv[2], "MAX_N")
                            : DEFAULT_CHECK_N;
        if (maximum_n > KNOWN_MAX_N) {
            fprintf(stderr,
                    "error: --check MAX_N may be at most %d because the "
                    "built-in OEIS data ends there\n",
                    KNOWN_MAX_N);
            return EXIT_FAILURE;
        }
        return check_implementation(maximum_n);
    }

    typedef enum {
        RUN_UPTO,
        RUN_TERM
    } RunMode;
    RunMode mode = RUN_UPTO;
    int d = A189285_D;
    int n = -1;
    bool have_d = false;
    bool have_n = false;

    for (int argument = 1; argument < argc; ++argument) {
        const char *text = argv[argument];
        if (strcmp(text, "--d") == 0) {
            if (have_d || argument + 1 >= argc) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            d = parse_d(argv[++argument]);
            have_d = true;
        } else if (strcmp(text, "--term") == 0 ||
            strcmp(text, "--upto") == 0) {
            if (have_n || argument + 1 >= argc) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            mode = strcmp(text, "--term") == 0 ? RUN_TERM : RUN_UPTO;
            n = parse_n(argv[++argument],
                        mode == RUN_TERM ? "N" : "MAX_N");
            have_n = true;
        } else if (text[0] == '-' || have_n) {
            usage(argv[0]);
            return EXIT_FAILURE;
        } else {
            n = parse_n(text, "MAX_N");
            mode = RUN_UPTO;
            have_n = true;
        }
    }
    if (!have_n) {
        n = DEFAULT_MAX_N;
    }

    if (mode == RUN_TERM) {
        mpz_t value;
        mpz_init(value);
        compute_value(value, n, d, true);
        gmp_printf("%d %Zd\n", n, value);
        mpz_clear(value);
        return EXIT_SUCCESS;
    }
    produce_file(argv[0], n, d);
    return EXIT_SUCCESS;
}
