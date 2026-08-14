/*
 * A320129 -- naive exact backtracking.
 *
 * Count partitions of {1,...,2*n} into n unordered pairs such that the n
 * pair sums are all different.  At every recursive node, take the smallest
 * unused element x and try pairing it with every other unused element y.
 * A branch is rejected exactly when x+y has already occurred.
 *
 * Taking the smallest unused element makes the order of the pairs canonical;
 * trying each partner once makes the order inside a pair canonical.  Thus
 * every set partition into pairs is visited exactly once.  This deliberately
 * uses no memoization, permanent formula, CRT, symmetry reduction, or other
 * nontrivial optimization: its worst-case search space is (2*n-1)!!.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic \
 *       320129_01.c -o 320129_01
 *
 * Usage:
 *   ./320129_01 --term 6
 *   ./320129_01 --upto 8
 *   ./320129_01 --upto 12 --start 13
 *   ./320129_01 --check 7
 *
 * A positional N is shorthand for --upto N.  --upto writes b320129.txt
 * beside the executable through interruption-safe b320129_part.txt.  With
 * --start S, the verified built-in prefix n<S is copied without recomputing
 * it, then S..N are calculated.  --term never changes the b-file.
 */

#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <inttypes.h>
#include <limits.h>
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
#error "320129_01 requires unsigned __int128"
#endif

__extension__ typedef unsigned __int128 U128;

#define MIN_N 0
#define MAX_N 12
#define KNOWN_MAX_N 12
#define DEFAULT_CHECK_N 6

typedef enum {
    MODE_TERM,
    MODE_UPTO,
    MODE_CHECK
} RunMode;

typedef struct {
    uint64_t nodes;
    uint64_t duplicate_sum_rejections;
} SearchStats;

static const char *const known_terms[KNOWN_MAX_N + 1] = {
    "1",
    "1",
    "2",
    "10",
    "55",
    "412",
    "3736",
    "40518",
    "505486",
    "7145031",
    "112844566",
    "1970286922",
    "37676184205"
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

static bool parse_u128(const char *text, U128 *result)
{
    U128 value = 0;
    const U128 maximum = ~(U128)0;
    if (*text == '\0') {
        return false;
    }
    for (; *text != '\0'; ++text) {
        if (*text < '0' || *text > '9') {
            return false;
        }
        unsigned digit = (unsigned)(*text - '0');
        if (value > (maximum - digit) / 10) {
            return false;
        }
        value = value * 10 + digit;
    }
    *result = value;
    return true;
}

static int fprint_u128(FILE *stream, U128 value)
{
    char digits[40];
    size_t length = 0;
    do {
        digits[length++] = (char)('0' + (unsigned)(value % 10));
        value /= 10;
    } while (value != 0);
    while (length != 0) {
        if (fputc(digits[--length], stream) == EOF) {
            return -1;
        }
    }
    return 0;
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

static void increment_stat(uint64_t *value)
{
    if (*value != UINT64_MAX) {
        ++*value;
    }
}

static U128 search_pairings(uint64_t unused, uint64_t used_sums,
                            SearchStats *stats)
{
    increment_stat(&stats->nodes);
    if (unused == 0) {
        return 1;
    }

    uint64_t first_bit = unused & (UINT64_C(0) - unused);
    unsigned first = (unsigned)__builtin_ctzll(first_bit) + 1U;
    uint64_t partners = unused ^ first_bit;
    U128 total = 0;
    while (partners != 0) {
        uint64_t partner_bit = partners & (UINT64_C(0) - partners);
        unsigned partner =
            (unsigned)__builtin_ctzll(partner_bit) + 1U;
        unsigned sum = first + partner;
        uint64_t sum_bit = UINT64_C(1) << sum;
        if ((used_sums & sum_bit) != 0) {
            increment_stat(&stats->duplicate_sum_rejections);
        } else {
            U128 addend = search_pairings(
                unused ^ first_bit ^ partner_bit,
                used_sums | sum_bit, stats);
            if (total > ~(U128)0 - addend) {
                die("A320129 count overflowed U128");
            }
            total += addend;
        }
        partners ^= partner_bit;
    }
    return total;
}

static U128 compute_term(int n, SearchStats *stats)
{
    if (n < MIN_N || n > MAX_N || 2 * n >= 64 || 4 * n - 1 >= 64) {
        die("unsupported dimension in naive pairing search");
    }
    memset(stats, 0, sizeof(*stats));
    uint64_t full = n == 0
                        ? 0
                        : (UINT64_C(1) << (2 * n)) - UINT64_C(1);
    return search_pairings(full, 0, stats);
}

static void verify_known(U128 value, int n)
{
    if (n < MIN_N || n > KNOWN_MAX_N) {
        return;
    }
    U128 expected = 0;
    if (!parse_u128(known_terms[n], &expected)) {
        die("invalid built-in A320129 term");
    }
    if (value != expected) {
        fprintf(stderr, "error: A320129 mismatch at n=%d: got ", n);
        fprint_u128(stderr, value);
        fprintf(stderr, ", expected %s\n", known_terms[n]);
        exit(EXIT_FAILURE);
    }
}

static U128 compute_checked(int n, bool report)
{
    double start = monotonic_seconds();
    SearchStats stats;
    U128 value = compute_term(n, &stats);
    verify_known(value, n);
    if (report) {
        fprintf(stderr,
                "320129_01: n=%d, naive DFS, nodes=%" PRIu64
                ", duplicate-sum rejects=%" PRIu64 ", %.3f s\n",
                n, stats.nodes, stats.duplicate_sum_rejections,
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

static void write_known_prefix_term(FILE *stream, int n,
                                    const char *path)
{
    if (n < MIN_N || n > KNOWN_MAX_N) {
        die("requested built-in A320129 prefix term is unavailable");
    }
    if (fprintf(stream, "%d %s\n", n, known_terms[n]) < 0) {
        fprintf(stderr, "error: cannot write %s: %s\n",
                path, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

static void produce_b_file(const char *argv0, int maximum_n, int start_n)
{
    if (start_n < MIN_N || start_n > maximum_n + 1 ||
        start_n > KNOWN_MAX_N + 1) {
        fprintf(stderr,
                "error: start N must be in %d..min(UPTO_N+1,%d)\n",
                MIN_N, KNOWN_MAX_N + 1);
        exit(EXIT_FAILURE);
    }

    char *final_path = path_beside_executable(argv0, "b320129.txt");
    char *part_path =
        path_beside_executable(argv0, "b320129_part.txt");
    FILE *output = fopen(part_path, "w");
    if (output == NULL) {
        fprintf(stderr, "error: cannot open %s: %s\n",
                part_path, strerror(errno));
        free(part_path);
        free(final_path);
        exit(EXIT_FAILURE);
    }

    for (int n = MIN_N; n < start_n && n <= maximum_n; ++n) {
        write_known_prefix_term(output, n, part_path);
    }
    flush_b_file(output, part_path);
    if (start_n > MIN_N) {
        fprintf(stderr,
                "320129_01: using built-in verified prefix n=%d..%d\n",
                MIN_N, start_n - 1);
    }

    for (int n = start_n; n <= maximum_n; ++n) {
        U128 value = compute_checked(n, true);
        if (fprintf(output, "%d ", n) < 0 ||
            fprint_u128(output, value) != 0 ||
            fputc('\n', output) == EOF) {
            fprintf(stderr, "error: cannot write %s: %s\n",
                    part_path, strerror(errno));
            fclose(output);
            free(part_path);
            free(final_path);
            exit(EXIT_FAILURE);
        }
        flush_b_file(output, part_path);
        printf("%d ", n);
        if (fprint_u128(stdout, value) != 0 || fputc('\n', stdout) == EOF) {
            fclose(output);
            free(part_path);
            free(final_path);
            die("could not write a result to stdout");
        }
    }

    if (fclose(output) != 0) {
        fprintf(stderr, "error: cannot close %s: %s\n",
                part_path, strerror(errno));
        free(part_path);
        free(final_path);
        exit(EXIT_FAILURE);
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
            "usage: %s N [--start S]\n"
            "       %s --term N\n"
            "       %s --upto N [--start S]\n"
            "       %s --check [N]\n",
            program, program, program, program);
}

int main(int argc, char **argv)
{
    RunMode mode = MODE_TERM;
    bool mode_set = false;
    int target = -1;
    int start_n = MIN_N;
    bool start_set = false;

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
        U128 value = compute_checked(target, true);
        printf("%d ", target);
        if (fprint_u128(stdout, value) != 0 || fputc('\n', stdout) == EOF) {
            die("could not write the term to stdout");
        }
    } else if (mode == MODE_UPTO) {
        produce_b_file(argv[0], target, start_n);
    } else {
        for (int n = MIN_N; n <= target; ++n) {
            (void)compute_checked(n, false);
        }
        printf("ok: naive pairing DFS agrees with A320129 for n=%d..%d\n",
               MIN_N, target);
    }
    return EXIT_SUCCESS;
}
