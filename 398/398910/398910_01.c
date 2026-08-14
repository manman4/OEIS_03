/*
 * A398910 -- deliberately naive exact counter for partitions of
 * {1,...,3*n} into
 * unordered three-element subsets {x,y,z}, where z is the largest element,
 * satisfying x+y<z.
 *
 * generate_rows() writes every set canonically as a<b<c, so c is its largest
 * element, and accepts it exactly when a+b<c.  Each valid unordered
 * three-set is generated once.
 *
 * At every recursive state, count_naive() chooses the smallest unused
 * element s and tries every still-available valid row containing s.  Every
 * completed partition has exactly one row containing s.  Induction on the
 * number of remaining rows proves that this counts every unordered
 * partition exactly once, without counting either the order of the rows or
 * the order of elements within a row.
 *
 * This _01 implementation intentionally has no memoization, minimum-column
 * heuristic, feasibility pruning, or parallel search.  It is intended as a
 * transparent independent counter for small n.
 *
 * MAX_N=16 needs 48 mask bits.  Every answer is at most the unrestricted
 * number of partitions into triples,
 *
 *                    (3*n)! / (6^n*n!).
 *
 * At n=16 this is
 * 210314486592266380347977873920000000 < 2^118, so unsigned __int128 is
 * sufficient throughout the supported range.  Every recursive addition is
 * checked as an independent overflow guard.  The naive running time becomes
 * impractical much earlier than MAX_N; the limit is an arithmetic and mask
 * safety limit, not a performance promise.
 *
 * Build:
 *   cc -O3 -mcpu=native -std=c11 -Wall -Wextra -Wpedantic \
 *       398910_01.c -o 398910_01
 *
 * Usage:
 *   ./398910_01 5
 *   ./398910_01 --term 5
 *   ./398910_01 --check --no-bfile
 *
 * A positional N computes a(0),...,a(N).  Completed terms are atomically
 * recorded in b398910_01.txt by default.  --term N requires a complete
 * earlier b-file prefix.  An interrupted calculation never records its
 * unfinished term.  Use --no-bfile to calculate without reading or writing
 * the b-file, or --output FILE to select another output path.
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
#error "398910_01.c requires unsigned __int128"
#endif
__extension__ typedef unsigned __int128 U128;

#define MAX_N 16
#define MAX_VALUES (3 * MAX_N)
#define DEFAULT_N 5

_Static_assert(MAX_VALUES < 64, "unused-element set must fit in uint64_t");

typedef struct {
    uint64_t *rows;
    uint32_t count;
    uint32_t capacity;
} Adjacency;

static Adjacency adjacency[MAX_VALUES];
static uint64_t full_mask;
static uint32_t valid_row_count;
static U128 calls;
static uint32_t calls_until_clock;
static int active_n;
static double started;
static double next_heartbeat;

static const char *output_path = "b398910_01.txt";
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

static void u128_text(U128 value, char text[40])
{
    char reverse[40];
    size_t length = 0;
    do {
        reverse[length++] = (char)('0' + (unsigned)(value % 10));
        value /= 10;
    } while (value != 0);
    for (size_t i = 0; i < length; ++i) {
        text[i] = reverse[length - 1U - i];
    }
    text[length] = '\0';
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

static void print_term(int n, U128 value)
{
    if (fprintf(stdout, "%d ", n) < 0 || print_u128(stdout, value) != 0 ||
        fputc('\n', stdout) == EOF || fflush(stdout) != 0) {
        die("cannot write result");
    }
}

static void print_calls(FILE *stream)
{
    if (print_u128(stream, calls) != 0) die("cannot write diagnostics");
}

static char *path_with_suffix(const char *suffix)
{
    const size_t path_length = strlen(output_path);
    const size_t suffix_length = strlen(suffix);
    if (path_length > SIZE_MAX - suffix_length - 1U) {
        die("output path is too long");
    }
    char *path = malloc(path_length + suffix_length + 1U);
    if (path == NULL) die("cannot allocate derived output path");
    memcpy(path, output_path, path_length);
    memcpy(path + path_length, suffix, suffix_length + 1U);
    return path;
}

static int lock_bfile(void)
{
    char *path = path_with_suffix(".lock");
    const int fd = open(path, O_RDWR | O_CREAT, 0666);
    free(path);
    if (fd < 0) die("cannot open b-file lock");

    struct flock lock = {.l_type = F_WRLCK, .l_whence = SEEK_SET};
    while (fcntl(fd, F_SETLKW, &lock) != 0) {
        if (errno != EINTR) die("cannot lock b-file");
    }
    return fd;
}

static void unlock_bfile(int fd)
{
    struct flock lock = {.l_type = F_UNLCK, .l_whence = SEEK_SET};
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
    if (ferror(input) || fclose(input) != 0) {
        die("cannot read complete b-file");
    }
    return next;
}

static void require_recordable(int n)
{
    if (!write_bfile) return;
    char values[MAX_N + 1][40];
    const int lock_fd = lock_bfile();
    const int prefix = read_bfile(values);
    unlock_bfile(lock_fd);
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
        if (strcmp(values[n], text) != 0) {
            die("computed value disagrees with b-file");
        }
        unlock_bfile(lock_fd);
        return;
    }
    if (n != prefix) die("b-file gap detected while recording");
    strcpy(values[prefix++], text);

    char *temporary = path_with_suffix(".tmp.XXXXXX");
    const int temporary_fd = mkstemp(temporary);
    if (temporary_fd < 0) die("cannot create temporary b-file");
    FILE *output = fdopen(temporary_fd, "w");
    if (output == NULL) {
        close(temporary_fd);
        unlink(temporary);
        free(temporary);
        die("cannot open temporary b-file stream");
    }
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

    fprintf(stderr, "398910_01: recorded computed term n=%d in %s\n",
            n, output_path);
}

static void free_rows(void)
{
    for (int i = 0; i < MAX_VALUES; ++i) {
        free(adjacency[i].rows);
        adjacency[i].rows = NULL;
        adjacency[i].count = 0;
        adjacency[i].capacity = 0;
    }
}

static void add_adjacent_row(int value, uint64_t row)
{
    Adjacency *list = &adjacency[value];
    if (list->count == list->capacity) {
        const uint32_t new_capacity = list->capacity == 0
                                      ? 16U : 2U * list->capacity;
        if (new_capacity < list->capacity ||
            (size_t)new_capacity > SIZE_MAX / sizeof(*list->rows)) {
            die("adjacency capacity overflow");
        }
        uint64_t *replacement = realloc(
            list->rows, (size_t)new_capacity * sizeof(*list->rows));
        if (replacement == NULL) die("cannot allocate adjacency rows");
        list->rows = replacement;
        list->capacity = new_capacity;
    }
    list->rows[list->count++] = row;
}

static void generate_rows(int n)
{
    free_rows();
    valid_row_count = 0;
    const int values = 3 * n;
    full_mask = values == 0 ? UINT64_C(0)
                            : (UINT64_C(1) << values) - UINT64_C(1);

    for (int a = 1; a <= values; ++a) {
        for (int b = a + 1; b <= values; ++b) {
            for (int c = b + 1; c <= values; ++c) {
                if (a + b >= c) continue;
                const uint64_t row = (UINT64_C(1) << (a - 1)) |
                                     (UINT64_C(1) << (b - 1)) |
                                     (UINT64_C(1) << (c - 1));
                add_adjacent_row(a - 1, row);
                add_adjacent_row(b - 1, row);
                add_adjacent_row(c - 1, row);
                if (valid_row_count == UINT32_MAX) {
                    die("row count overflow");
                }
                ++valid_row_count;
            }
        }
    }
}

static void heartbeat_if_due(void)
{
    if (++calls_until_clock != UINT32_C(1048576)) return;
    calls_until_clock = 0;
    const double now = now_seconds();
    if (now < next_heartbeat) return;
    fprintf(stderr, "398910_01: heartbeat n=%d, calls=", active_n);
    print_calls(stderr);
    fprintf(stderr, ", %.1f s\n", now - started);
    next_heartbeat = now + 10.0;
}

static U128 count_naive(uint64_t used)
{
    if (calls == ~(U128)0) die("diagnostic call counter overflow");
    ++calls;
    heartbeat_if_due();
    if (used == full_mask) return 1;

    const uint64_t unused = full_mask ^ used;
    const unsigned selected = (unsigned)__builtin_ctzll(unused);
    const Adjacency *list = &adjacency[selected];
    U128 total = 0;
    for (uint32_t i = 0; i < list->count; ++i) {
        const uint64_t row = list->rows[i];
        if ((row & used) != 0) continue;
        const U128 subtotal = count_naive(used | row);
        if (!add_u128(&total, subtotal)) die("answer exceeds U128");
    }
    return total;
}

static U128 compute_term(int n)
{
    generate_rows(n);
    calls = 0;
    calls_until_clock = 0;
    active_n = n;
    started = now_seconds();
    next_heartbeat = started + 10.0;
    const U128 answer = count_naive(0);
    const double elapsed = now_seconds() - started;

    fprintf(stderr,
            "398910_01: n=%d, naive backtracking, rows=%" PRIu32
            ", calls=", n, valid_row_count);
    print_calls(stderr);
    fprintf(stderr, ", %.3f s\n", elapsed);
    return answer;
}

static void run_check(void)
{
    static const uint64_t expected[] = {1, 0, 0, 2, 46, 1413};
    for (int n = 0; n <= 5; ++n) {
        const U128 answer = compute_term(n);
        if (answer != (U128)expected[n]) {
            fprintf(stderr, "error: self-check failed at n=%d\n", n);
            exit(EXIT_FAILURE);
        }
    }
    fprintf(stderr, "398910_01: self-check passed for n=0..5\n");
}

static void usage(FILE *stream, const char *program)
{
    fprintf(stream,
            "usage: %s [N | --term N] [--no-bfile] [--output FILE]\n"
            "       %s --check\n"
            "  N             compute a(0),...,a(N); default %d\n"
            "  --term N      compute only a(N)\n"
            "  --no-bfile    do not read or write a b-file\n"
            "  --output FILE use FILE instead of b398910_01.txt\n"
            "  --check       verify the independently known terms n=0..5\n",
            program, program, DEFAULT_N);
}

int main(int argc, char **argv)
{
    int target = DEFAULT_N;
    bool target_set = false;
    bool term_mode = false;
    bool check_mode = false;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--term") == 0) {
            if (++i == argc || target_set || check_mode) {
                die("--term requires one N and cannot be combined with N");
            }
            target = parse_n(argv[i]);
            target_set = true;
            term_mode = true;
        } else if (strcmp(argv[i], "--no-bfile") == 0) {
            write_bfile = false;
        } else if (strcmp(argv[i], "--output") == 0) {
            if (++i == argc || argv[i][0] == '\0') {
                die("--output requires a nonempty path");
            }
            output_path = argv[i];
        } else if (strcmp(argv[i], "--check") == 0) {
            if (target_set || check_mode) {
                die("--check cannot be combined with N or --term");
            }
            check_mode = true;
            write_bfile = false;
        } else if (strcmp(argv[i], "--help") == 0 ||
                   strcmp(argv[i], "-h") == 0) {
            usage(stdout, argv[0]);
            return EXIT_SUCCESS;
        } else if (argv[i][0] == '-') {
            fprintf(stderr, "error: unknown option: %s\n", argv[i]);
            usage(stderr, argv[0]);
            return EXIT_FAILURE;
        } else {
            if (target_set || check_mode) {
                die("only one positional N is allowed");
            }
            target = parse_n(argv[i]);
            target_set = true;
        }
    }

    if (check_mode) {
        run_check();
        free_rows();
        return EXIT_SUCCESS;
    }

    if (term_mode) require_recordable(target);
    const int first = term_mode ? target : 0;
    for (int n = first; n <= target; ++n) {
        const U128 answer = compute_term(n);
        record_term(n, answer);
        print_term(n, answer);
    }
    free_rows();
    return EXIT_SUCCESS;
}
