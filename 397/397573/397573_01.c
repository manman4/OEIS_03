/*
 * A397573 -- deliberately naive independent counter.
 *
 * Count partitions of {1,...,4*n} into unordered four-element blocks that
 * can be split into two pairs having the same sum.  If a block is written
 * a < b < c < d, the first two possible pairings are strictly unequal:
 *
 *     a+b < c+d,             a+c < b+d.
 *
 * Consequently the block is valid exactly when a+d = b+c, and that split is
 * unique.  generate_rows() lists every such four-set once.
 *
 * At each recursive state, choose the smallest unused element s and try
 * every still-available valid row containing s.  Every completed partition
 * has exactly one block containing s.  Induction on the number of remaining
 * blocks therefore proves that count_naive() counts each unordered partition
 * exactly once and neither omits nor duplicates any partition.
 *
 * This file intentionally uses no memoization, minimum-column heuristic,
 * feasibility pruning, or parallel search.  It is meant as a transparent,
 * independent check for small n rather than as the fastest A397573 program.
 *
 * MAX_N=12 needs only 48 mask bits.  Every answer is at most the number of
 * unrestricted partitions into four-sets,
 *
 *                 (4*n)! / (24^n n!).
 *
 * At n=12 this upper bound has 120 bits, so unsigned __int128 is sufficient
 * throughout the supported range.  Additions are checked as a second guard.
 *
 * Build:
 *   clang -O3 -march=native -std=c11 -Wall -Wextra -Wpedantic \
 *       397573_01.c -o 397573_01
 *
 * Usage:
 *   ./397573_01 6
 *   ./397573_01 --term 6
 *   ./397573_01 --check --no-bfile
 *
 * A positional N computes a(0),...,a(N).  By default, completed terms are
 * atomically recorded in b397573_01.txt.  --term N is rejected before the
 * search if earlier b-file terms are missing.  An interrupted search never
 * records the unfinished term.
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
#error "397573_01.c requires unsigned __int128"
#endif
__extension__ typedef unsigned __int128 U128;

#define MAX_N 12
#define MAX_VALUES (4 * MAX_N)
#define DEFAULT_N 6

_Static_assert(MAX_VALUES < 64, "unused-element set must fit in uint64_t");

typedef struct {
    uint64_t *rows;
    uint32_t count;
    uint32_t capacity;
} Adjacency;

static Adjacency adjacency[MAX_VALUES];
static uint64_t calls;
static double started;
static double next_heartbeat;
static const char *output_path = "b397573_01.txt";
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
    fprintf(stderr, "397573_01: recorded computed term n=%d in %s\n",
            n, output_path);
}

static void adjacency_add(Adjacency *list, uint64_t row)
{
    if (list->count == list->capacity) {
        const uint32_t next = list->capacity ? 2U * list->capacity : 128U;
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

static uint64_t generate_rows(int n)
{
    const int maximum = 4 * n;
    uint64_t row_count = 0;
    for (int a = 1; a <= maximum; ++a) {
        for (int b = a + 1; b <= maximum; ++b) {
            for (int c = b + 1; c <= maximum; ++c) {
                const int d = b + c - a;
                if (d > maximum) break;
                const uint64_t row =
                    (UINT64_C(1) << (a - 1)) |
                    (UINT64_C(1) << (b - 1)) |
                    (UINT64_C(1) << (c - 1)) |
                    (UINT64_C(1) << (d - 1));
                adjacency_add(&adjacency[a - 1], row);
                adjacency_add(&adjacency[b - 1], row);
                adjacency_add(&adjacency[c - 1], row);
                adjacency_add(&adjacency[d - 1], row);
                if (row_count == UINT64_MAX) die("row counter overflow");
                ++row_count;
            }
        }
    }
    return row_count;
}

static void free_rows(void)
{
    for (unsigned i = 0; i < MAX_VALUES; ++i) {
        free(adjacency[i].rows);
        adjacency[i] = (Adjacency){0};
    }
}

static void heartbeat(void)
{
    if ((calls & UINT64_C(0xfffff)) != 0) return;
    const double elapsed = now_seconds() - started;
    if (elapsed < next_heartbeat) return;
    fprintf(stderr, "397573_01: heartbeat calls=%" PRIu64 ", %.1f s\n",
            calls, elapsed);
    next_heartbeat = elapsed + 10.0;
}

static U128 count_naive(uint64_t mask)
{
    if (mask == 0) return 1;
    if (calls == UINT64_MAX) die("call counter overflow");
    ++calls;
    heartbeat();
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
    const uint64_t rows = generate_rows(n);
    calls = 0;
    started = now_seconds();
    next_heartbeat = 10.0;
    const U128 answer = count_naive(
        (UINT64_C(1) << (4U * (unsigned)n)) - 1U);
    fprintf(stderr,
            "397573_01: n=%d, naive smallest-element recursion, "
            "rows=%" PRIu64 ", calls=%" PRIu64 ", %.3f s\n",
            n, rows, calls, now_seconds() - started);
    free_rows();
    return answer;
}

static const char *const known[] = {
    "1", "1", "6", "86", "1990", "74323", "4226026"
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
            n = 6;
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
    if (check) fprintf(stderr, "397573_01: self-check passed\n");
    return EXIT_SUCCESS;
}
