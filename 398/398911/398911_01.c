/*
 * A398911 -- naive exact backtracking.
 *
 * Count partitions of {1,...,3*n} into n unordered triples such that the n
 * triple sums are all different.  At every recursive node, take the smallest
 * unused element x and try every unordered pair y<z of other unused elements.
 * A branch is rejected exactly when x+y+z has already occurred.
 *
 * Taking the smallest unused element makes the order of the triples
 * canonical, and requiring y<z makes the order inside each triple canonical.
 * Thus every set partition into triples is visited exactly once.  This
 * deliberately uses no memoization or other nontrivial optimization.  Its
 * worst-case number of leaves is
 *
 *                 (3*n)! / ((3!)^n * n!).
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic \
 *       398911_01.c -o 398911_01
 *
 * Usage:
 *   ./398911_01 --term 5
 *   ./398911_01 --upto 6
 *   ./398911_01 6
 *
 * A positional N is shorthand for --upto N.  --upto writes b398911_1.txt
 * beside the executable through interruption-safe b398911_1_part.txt.
 * --term does not change the b-file.
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
#error "398911_01 requires unsigned __int128"
#endif

__extension__ typedef unsigned __int128 U128;

#define MIN_N 0
/* For n=14, element masks fit uint64_t and all sums fit a U128 bitset. */
#define MAX_N 14

typedef enum {
    MODE_TERM,
    MODE_UPTO
} RunMode;

typedef struct {
    uint64_t nodes;
    uint64_t candidate_triples;
    uint64_t duplicate_sum_rejections;
} SearchStats;

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

static U128 search_partitions(uint64_t unused, U128 used_sums,
                              SearchStats *stats)
{
    increment_stat(&stats->nodes);
    if (unused == 0) {
        return 1;
    }

    uint64_t first_bit = unused & (UINT64_C(0) - unused);
    unsigned first = (unsigned)__builtin_ctzll(first_bit) + 1U;
    uint64_t second_choices = unused ^ first_bit;
    U128 total = 0;

    while (second_choices != 0) {
        uint64_t second_bit =
            second_choices & (UINT64_C(0) - second_choices);
        unsigned second =
            (unsigned)__builtin_ctzll(second_bit) + 1U;
        /* Earlier second choices have been removed, so these are all >y. */
        uint64_t third_choices = second_choices ^ second_bit;

        while (third_choices != 0) {
            uint64_t third_bit =
                third_choices & (UINT64_C(0) - third_choices);
            unsigned third =
                (unsigned)__builtin_ctzll(third_bit) + 1U;
            unsigned sum = first + second + third;
            U128 sum_bit = (U128)1 << sum;
            increment_stat(&stats->candidate_triples);

            if ((used_sums & sum_bit) != 0) {
                increment_stat(&stats->duplicate_sum_rejections);
            } else {
                U128 addend = search_partitions(
                    unused ^ first_bit ^ second_bit ^ third_bit,
                    used_sums | sum_bit, stats);
                if (total > ~(U128)0 - addend) {
                    die("A398911 count overflowed U128");
                }
                total += addend;
            }
            third_choices ^= third_bit;
        }
        second_choices ^= second_bit;
    }
    return total;
}

static U128 partition_upper_bound(int n)
{
    U128 bound = 1;
    const U128 maximum = ~(U128)0;
    for (unsigned triples = 1; triples <= (unsigned)n; ++triples) {
        U128 a = 3U * triples - 1U;
        U128 b = 3U * triples - 2U;
        U128 factor = (a * b) / 2U;
        if (bound > maximum / factor) {
            die("triple-partition bound exceeds U128");
        }
        bound *= factor;
    }
    return bound;
}

static U128 compute_term(int n, SearchStats *stats)
{
    if (n < MIN_N || n > MAX_N || 3 * n >= 64 || 9 * n - 3 >= 128) {
        die("unsupported dimension in naive triple search");
    }
    (void)partition_upper_bound(n);
    memset(stats, 0, sizeof(*stats));
    uint64_t full = n == 0
                        ? 0
                        : (UINT64_C(1) << (3 * n)) - UINT64_C(1);
    return search_partitions(full, 0, stats);
}

static U128 compute_checked(int n)
{
    double start = monotonic_seconds();
    SearchStats stats;
    U128 value = compute_term(n, &stats);
    fprintf(stderr,
            "398911_01: n=%d, naive DFS, nodes=%" PRIu64
            ", candidate-triples=%" PRIu64
            ", duplicate-sum rejects=%" PRIu64 ", %.3f s\n",
            n, stats.nodes, stats.candidate_triples,
            stats.duplicate_sum_rejections,
            monotonic_seconds() - start);
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

static void write_result(FILE *stream, int n, U128 value,
                         const char *description)
{
    if (fprintf(stream, "%d ", n) < 0 ||
        fprint_u128(stream, value) != 0 ||
        fputc('\n', stream) == EOF) {
        fprintf(stderr, "error: cannot write %s\n", description);
        exit(EXIT_FAILURE);
    }
}

static void produce_b_file(const char *argv0, int maximum_n)
{
    char *final_path = path_beside_executable(argv0, "b398911_1.txt");
    char *part_path =
        path_beside_executable(argv0, "b398911_1_part.txt");
    FILE *output = fopen(part_path, "w");
    if (output == NULL) {
        fprintf(stderr, "error: cannot open %s: %s\n",
                part_path, strerror(errno));
        free(part_path);
        free(final_path);
        exit(EXIT_FAILURE);
    }

    for (int n = MIN_N; n <= maximum_n; ++n) {
        U128 value = compute_checked(n);
        write_result(output, n, value, part_path);
        flush_b_file(output, part_path);
        write_result(stdout, n, value, "result to stdout");
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
            "usage: %s N\n"
            "       %s --term N\n"
            "       %s --upto N\n",
            program, program, program);
}

int main(int argc, char **argv)
{
    RunMode mode = MODE_TERM;
    bool mode_set = false;
    int target = -1;

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

    if (mode == MODE_TERM) {
        U128 value = compute_checked(target);
        write_result(stdout, target, value, "result to stdout");
    } else {
        produce_b_file(argv[0], target);
    }
    return EXIT_SUCCESS;
}
