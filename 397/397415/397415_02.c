/*
 * A397415 -- deliberately naive independent counter.
 *
 * Count partitions of {1,...,5*n} into unordered five-element blocks for
 * which two elements have the same sum as the other three.  Generate all
 * valid five-sets once.  At every recursive state choose the smallest
 * remaining element s and try every still-available valid five-set containing
 * s.  In every completed partition exactly one block contains s, so induction
 * proves that every unordered partition is counted exactly once.
 *
 * This version intentionally has no memoization, subset-sum pruning,
 * minimum-column heuristic, or parallel search.  It is therefore useful as a
 * simple independent check for small n, but it is not intended for n=7.
 * The root congruence shortcut is retained because it follows immediately:
 * the sum 1+...+5*n must be twice the sum of all two-element sides, hence it
 * must be even; equivalently n==0 or 3 (mod 4).
 *
 * The two-element side of a valid five-set is unique.  If two equal-sum pairs
 * intersect, distinctness forces their other elements equal; if disjoint,
 * their common sum already equals the sum of their four elements, leaving a
 * positive fifth element and contradicting equality with the complementary
 * triple.  Thus a five-set contributes one block, not multiple orientations.
 *
 * MAX_N=7 uses only 35 mask bits.  The answer is bounded by the number of all
 * partitions into five-sets, (5*n)!/(120^n*n!), which is below 2^128 at n=7;
 * U128 additions are checked anyway.
 *
 * Build:
 *   clang -O3 -march=native -std=c11 -Wall -Wextra -Wpedantic \
 *       397415_02.c -o 397415_02
 *
 * Usage:
 *   ./397415_02 4
 *   ./397415_02 --term 4
 *   ./397415_02 --check --no-bfile
 *
 * Results are atomically recorded in b397415_02.txt by default.  A --term
 * request is rejected before calculation if earlier b-file terms are missing.
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
#include <time.h>
#include <unistd.h>

#if !defined(__SIZEOF_INT128__)
#error "397415_02.c requires unsigned __int128"
#endif
__extension__ typedef unsigned __int128 U128;

#define MAX_N 7
#define MAX_VALUES (5 * MAX_N)
#define DEFAULT_N 4

_Static_assert(MAX_VALUES < 64, "remaining set must fit in uint64_t");

typedef struct {
    uint64_t *rows;
    uint32_t count;
    uint32_t capacity;
} Adjacency;

static Adjacency adjacency[MAX_VALUES];
static uint64_t calls;
static double started;
static const char *output_path = "b397415_02.txt";
static bool write_bfile = true;

static _Noreturn void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static int parse_n(const char *text)
{
    char *end = NULL;
    errno = 0;
    const long value = strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' ||
        value < 0 || value > MAX_N) {
        fprintf(stderr, "error: N must be in 0..%d: %s\n", MAX_N, text);
        exit(EXIT_FAILURE);
    }
    return (int)value;
}

static double now_seconds(void)
{
    struct timespec value;
    if (clock_gettime(CLOCK_MONOTONIC, &value) != 0) {
        die("clock_gettime failed");
    }
    return (double)value.tv_sec + (double)value.tv_nsec / 1e9;
}

static bool add_u128(U128 *destination, U128 addend)
{
    const U128 old = *destination;
    *destination += addend;
    return *destination >= old;
}

static int print_u128(FILE *stream, U128 value)
{
    char digits[40];
    size_t length = 0;
    do {
        digits[length++] = (char)('0' + (unsigned)(value % 10));
        value /= 10;
    } while (value != 0);
    while (length != 0) {
        if (fputc(digits[--length], stream) == EOF) return -1;
    }
    return 0;
}

static void print_term(int n, U128 value)
{
    if (fprintf(stdout, "%d ", n) < 0 || print_u128(stdout, value) != 0 ||
        fputc('\n', stdout) == EOF || fflush(stdout) != 0) {
        die("cannot write result");
    }
}

static bool parse_u128(const char *text, U128 *result)
{
    const U128 maximum = ~(U128)0;
    U128 value = 0;
    if (*text == '\0') return false;
    while (*text != '\0') {
        if (*text < '0' || *text > '9') return false;
        const unsigned digit = (unsigned)(*text++ - '0');
        if (value > (maximum - digit) / 10U) return false;
        value = 10U * value + digit;
    }
    *result = value;
    return true;
}

static void u128_text(U128 value, char text[40])
{
    char reverse[40];
    size_t length = 0;
    do {
        reverse[length++] = (char)('0' + (unsigned)(value % 10));
        value /= 10;
    } while (value != 0);
    for (size_t i = 0; i < length; ++i) text[i] = reverse[length - 1U - i];
    text[length] = '\0';
}

static int lock_bfile(void)
{
    char *path = malloc(strlen(output_path) + 6U);
    if (path == NULL) die("cannot allocate lock path");
    sprintf(path, "%s.lock", output_path);
    const int fd = open(path, O_RDWR | O_CREAT, 0666);
    free(path);
    if (fd < 0) die("cannot open b-file lock");
    struct flock lock = {.l_type=F_WRLCK, .l_whence=SEEK_SET};
    while (fcntl(fd, F_SETLKW, &lock) != 0) {
        if (errno != EINTR) die("cannot lock b-file");
    }
    return fd;
}

static void unlock_bfile(int fd)
{
    struct flock lock = {.l_type=F_UNLCK, .l_whence=SEEK_SET};
    if (fcntl(fd, F_SETLK, &lock) != 0 || close(fd) != 0) {
        die("cannot unlock b-file");
    }
}

static int read_bfile(char values[MAX_N + 1][40])
{
    FILE *input = fopen(output_path, "r");
    if (input == NULL) {
        if (errno == ENOENT) return 0;
        die("cannot read b-file");
    }
    char line[256];
    int next = 0;
    while (fgets(line, sizeof(line), input) != NULL) {
        char *p = line;
        while (*p == ' ' || *p == '\t') ++p;
        if (*p == '\0' || *p == '\n' || *p == '#') continue;
        int index;
        char number[40], extra;
        if (sscanf(p, "%d %39s %c", &index, number, &extra) != 2 ||
            index != next || index < 0 || index > MAX_N) {
            fclose(input);
            die("b-file is malformed or has a gap");
        }
        U128 parsed;
        if (!parse_u128(number, &parsed)) {
            fclose(input);
            die("b-file contains an invalid value");
        }
        strcpy(values[next++], number);
    }
    if (ferror(input) || fclose(input) != 0) die("cannot read complete b-file");
    return next;
}

static void require_recordable(int n)
{
    if (!write_bfile) return;
    char values[MAX_N + 1][40];
    const int fd = lock_bfile();
    const int prefix = read_bfile(values);
    unlock_bfile(fd);
    if (n > prefix) {
        fprintf(stderr,
                "error: %s ends before n=%d; compute the missing prefix "
                "first, or use --no-bfile\n", output_path, n);
        exit(EXIT_FAILURE);
    }
}

static void record_term(int n, U128 value)
{
    if (!write_bfile) return;
    char values[MAX_N + 1][40], text[40];
    u128_text(value, text);
    const int lock_fd = lock_bfile();
    int prefix = read_bfile(values);
    if (n < prefix) {
        if (strcmp(values[n], text) != 0) die("value disagrees with b-file");
        unlock_bfile(lock_fd);
        return;
    }
    if (n != prefix) die("b-file gap detected while recording");
    strcpy(values[prefix++], text);

    char *temporary = malloc(strlen(output_path) + 32U);
    if (temporary == NULL) die("cannot allocate temporary path");
    sprintf(temporary, "%s.tmp.%ld", output_path, (long)getpid());
    FILE *output = fopen(temporary, "w");
    if (output == NULL) die("cannot create temporary b-file");
    bool failed = false;
    for (int i = 0; i < prefix; ++i) {
        if (fprintf(output, "%d %s\n", i, values[i]) < 0) failed = true;
    }
    if (!failed && fflush(output) != 0) failed = true;
    if (!failed && fsync(fileno(output)) != 0) failed = true;
    if (fclose(output) != 0) failed = true;
    if (failed || rename(temporary, output_path) != 0) {
        unlink(temporary);
        free(temporary);
        die("cannot atomically update b-file");
    }
    free(temporary);
    unlock_bfile(lock_fd);
    fprintf(stderr, "397415_02: recorded computed term n=%d in %s\n",
            n, output_path);
}

static void adjacency_add(Adjacency *list, uint64_t row)
{
    if (list->count == list->capacity) {
        const uint32_t next = list->capacity ? 2U * list->capacity : 256U;
        if (next < list->capacity ||
            (size_t)next > SIZE_MAX / sizeof(*list->rows)) {
            die("adjacency capacity overflow");
        }
        uint64_t *rows = realloc(list->rows, (size_t)next * sizeof(*rows));
        if (rows == NULL) die("cannot allocate adjacency list");
        list->rows = rows;
        list->capacity = next;
    }
    list->rows[list->count++] = row;
}

static void generate_rows(int n)
{
    const int maximum = 5 * n;
    for (int a = 1; a <= maximum; ++a) {
        for (int b = a + 1; b <= maximum; ++b) {
            const int sum = a + b;
            for (int x = 1; x <= maximum; ++x) {
                if (x == a || x == b) continue;
                for (int y = x + 1; y <= maximum; ++y) {
                    if (y == a || y == b) continue;
                    const int z = sum - x - y;
                    if (z <= y || z > maximum || z == a || z == b) continue;
                    const uint64_t row =
                        (UINT64_C(1) << (a - 1)) |
                        (UINT64_C(1) << (b - 1)) |
                        (UINT64_C(1) << (x - 1)) |
                        (UINT64_C(1) << (y - 1)) |
                        (UINT64_C(1) << (z - 1));
                    uint64_t bits = row;
                    while (bits != 0) {
                        const unsigned value = (unsigned)__builtin_ctzll(bits);
                        bits &= bits - 1U;
                        adjacency_add(&adjacency[value], row);
                    }
                }
            }
        }
    }
}

static void free_rows(void)
{
    for (unsigned i = 0; i < MAX_VALUES; ++i) {
        free(adjacency[i].rows);
        adjacency[i] = (Adjacency){0};
    }
}

static U128 count_naive(uint64_t mask)
{
    if (mask == 0) return 1;
    if (calls == UINT64_MAX) die("call counter overflow");
    ++calls;
    const unsigned first = (unsigned)__builtin_ctzll(mask);
    const Adjacency *list = &adjacency[first];
    U128 answer = 0;
    for (uint32_t i = 0; i < list->count; ++i) {
        const uint64_t row = list->rows[i];
        if ((row & mask) != row) continue;
        const U128 addend = count_naive(mask ^ row);
        if (!add_u128(&answer, addend)) die("answer overflowed U128");
    }
    return answer;
}

static U128 sequence_term(int n)
{
    if (n == 0) return 1;
    if (n % 4 != 0 && n % 4 != 3) return 0;
    generate_rows(n);
    calls = 0;
    started = now_seconds();
    const U128 answer = count_naive(
        (UINT64_C(1) << (5U * (unsigned)n)) - 1U);
    fprintf(stderr,
            "397415_02: n=%d, naive smallest-element recursion, "
            "calls=%" PRIu64 ", %.3f s\n",
            n, calls, now_seconds() - started);
    free_rows();
    return answer;
}

static const char *const known[] = {
    "1", "0", "0", "5452", "2085870", "0", "0"
};

static void verify_known(int n, U128 value)
{
    if ((size_t)n >= sizeof(known) / sizeof(known[0])) return;
    U128 expected;
    if (!parse_u128(known[n], &expected)) die("invalid built-in known term");
    if (value != expected) {
        fprintf(stderr, "error: known-term mismatch n=%d: got ", n);
        print_u128(stderr, value);
        fprintf(stderr, ", expected %s\n", known[n]);
        exit(EXIT_FAILURE);
    }
}

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s [N] [--output FILE|--no-bfile]\n"
            "       %s --term N [--output FILE|--no-bfile]\n"
            "       %s --check [--output FILE|--no-bfile]\n",
            program, program, program);
}

int main(int argc, char **argv)
{
    int n = DEFAULT_N;
    bool positional = false, term_only = false, check = false;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--output") == 0) {
            if (++i == argc || argv[i][0] == '\0') die("--output needs a file");
            output_path = argv[i];
            write_bfile = true;
        } else if (strcmp(argv[i], "--no-bfile") == 0) {
            write_bfile = false;
        } else if (strcmp(argv[i], "--term") == 0) {
            if (term_only || positional || check || ++i == argc) {
                die("invalid --term usage");
            }
            term_only = true;
            n = parse_n(argv[i]);
        } else if (strcmp(argv[i], "--check") == 0) {
            if (term_only || positional || check) die("invalid --check usage");
            check = true;
            n = 4;
        } else if (argv[i][0] == '-') {
            usage(argv[0]);
            return EXIT_FAILURE;
        } else {
            if (positional || term_only || check) die("multiple N arguments");
            positional = true;
            n = parse_n(argv[i]);
        }
    }

    if (term_only) {
        require_recordable(n);
        const U128 value = sequence_term(n);
        verify_known(n, value);
        print_term(n, value);
        record_term(n, value);
        return EXIT_SUCCESS;
    }
    for (int k = 0; k <= n; ++k) {
        require_recordable(k);
        const U128 value = sequence_term(k);
        verify_known(k, value);
        print_term(k, value);
        record_term(k, value);
    }
    if (check) fprintf(stderr, "397415_02: self-check passed\n");
    return EXIT_SUCCESS;
}
