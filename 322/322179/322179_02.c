/*
 * A322179 -- inclusion-exclusion coefficient extraction.
 *
 * This algorithm is independent of the occupied-position subset DP in
 * 322179_01.c.  For the 2*n positions introduce commuting variables x_i and
 * define, for each label k,
 *
 *   Q_k(x) = sum_{i<j, j-i>=k+1} x_i*x_j.
 *
 * Choosing one term from every Q_k chooses the two positions occupied by k.
 * Hence a(n) is the coefficient of x_0*x_1*...*x_(2*n-1) in
 * P(x)=product_k Q_k(x).  Since P is homogeneous of total degree 2*n,
 * ordinary Boolean-lattice inclusion-exclusion gives
 *
 *   a(n) = sum_{S subseteq [2*n]} (-1)^|S| product_k E_k(S),
 *
 * where E_k(S) is the number of allowed position pairs for k contained in
 * S.  (The usual sign (-1)^(2*n-|S|) equals (-1)^|S|.)
 *
 * Subsets are visited in Gray-code order.  Only one position changes at a
 * time, so every E_k is updated by a popcount against a precomputed mask.
 * Independent Gray-code intervals are evaluated by pthread workers.  The
 * products and alternating sums use checked 256-bit accumulators.
 *
 * Compared with _01 this uses O(n^2 + threads) memory rather than a large
 * subset table.  The 2^(2*n) running time is substantial at n=18, but the
 * calculation remains memory-safe and parallelizable.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic -pthread \
 *       322179_02.c -o 322179_02
 *
 * Usage:
 *   ./322179_02 14
 *   ./322179_02 --term 14 --threads 8
 *   ./322179_02 --check --threads 8
 * Completed terms are atomically recorded in b322179_02.txt by default.
 * Use --output FILE to select another b-file or --no-bfile to disable it.
 */

#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#if !defined(__SIZEOF_INT128__)
#error "322179_02.c requires unsigned __int128"
#endif

__extension__ typedef unsigned __int128 U128;

#define MAX_N 18
#define MAX_POSITIONS (2 * MAX_N)
#define MAX_THREADS 64
#define DEFAULT_N 9

static const char *output_path = "b322179_02.txt";
static bool write_bfile = true;

typedef struct {
    uint64_t limb[4];
} U256;

typedef struct {
    int n;
    unsigned positions;
    uint64_t begin;
    uint64_t end;
    const uint64_t (*far_mask)[MAX_N + 1];
    U256 positive;
    U256 negative;
    int failed;
    char error[96];
} Worker;

static const char *const known[] = {
    "1", "0", "0", "2", "40", "1070", "38936", "1896220",
    "119912476", "9587033840", "946858118960",
    "113306859860824", "16161535729743216",
    "2709775402648307208", "527750051727868912592",
    "118165073636280852175296", "30144089315619479375954448"
};

static void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static double now_seconds(void)
{
    struct timespec time;
    if (clock_gettime(CLOCK_MONOTONIC, &time) != 0) {
        die("clock_gettime failed");
    }
    return (double)time.tv_sec + (double)time.tv_nsec / 1e9;
}

static int parse_int(const char *text, const char *name, int low, int high)
{
    char *end = NULL;
    errno = 0;
    const long value = strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' ||
        value < low || value > high) {
        fprintf(stderr, "error: %s must be in %d..%d: %s\n",
                name, low, high, text);
        exit(EXIT_FAILURE);
    }
    return (int)value;
}

static int default_threads(void)
{
#if defined(_SC_NPROCESSORS_ONLN)
    const long count = sysconf(_SC_NPROCESSORS_ONLN);
    if (count < 1) {
        return 1;
    }
    return count > MAX_THREADS ? MAX_THREADS : (int)count;
#else
    return 1;
#endif
}

static bool add_u256(U256 *destination, U256 addend)
{
    uint64_t carry = 0;
    for (unsigned i = 0; i < 4; ++i) {
        const uint64_t old = destination->limb[i];
        uint64_t value = old + addend.limb[i];
        const uint64_t carry1 = value < old;
        const uint64_t before_carry = value;
        value += carry;
        const uint64_t carry2 = value < before_carry;
        destination->limb[i] = value;
        carry = carry1 | carry2;
    }
    return carry == 0;
}

static bool multiply_u256_small(U256 *value, unsigned factor)
{
    uint64_t carry = 0;
    for (unsigned i = 0; i < 4; ++i) {
        const U128 product = (U128)value->limb[i] * factor + carry;
        value->limb[i] = (uint64_t)product;
        carry = (uint64_t)(product >> 64);
    }
    return carry == 0;
}

static int compare_u256(U256 left, U256 right)
{
    for (int i = 3; i >= 0; --i) {
        if (left.limb[i] < right.limb[i]) {
            return -1;
        }
        if (left.limb[i] > right.limb[i]) {
            return 1;
        }
    }
    return 0;
}

static U256 subtract_u256(U256 left, U256 right)
{
    U256 result = {{0, 0, 0, 0}};
    uint64_t borrow = 0;
    for (unsigned i = 0; i < 4; ++i) {
        const uint64_t after_borrow = left.limb[i] - borrow;
        const uint64_t borrow1 = after_borrow > left.limb[i];
        result.limb[i] = after_borrow - right.limb[i];
        const uint64_t borrow2 = result.limb[i] > after_borrow;
        borrow = borrow1 | borrow2;
    }
    if (borrow != 0) {
        die("internal U256 subtraction underflow");
    }
    return result;
}

static bool u256_is_zero(U256 value)
{
    return (value.limb[0] | value.limb[1] |
            value.limb[2] | value.limb[3]) == 0;
}

static unsigned divide_u256_by_10(U256 *value)
{
    uint64_t remainder = 0;
    for (int i = 3; i >= 0; --i) {
        const U128 current = ((U128)remainder << 64) | value->limb[i];
        value->limb[i] = (uint64_t)(current / 10);
        remainder = (uint64_t)(current % 10);
    }
    return (unsigned)remainder;
}

static int print_u256(FILE *stream, U256 value)
{
    char digits[80];
    size_t length = 0;
    do {
        digits[length++] = (char)('0' + divide_u256_by_10(&value));
    } while (!u256_is_zero(value));
    while (length != 0) {
        if (fputc(digits[--length], stream) == EOF) {
            return -1;
        }
    }
    return 0;
}

static bool parse_u256(const char *text, U256 *result)
{
    U256 value = {{0, 0, 0, 0}};
    if (*text == '\0') {
        return false;
    }
    for (; *text != '\0'; ++text) {
        if (*text < '0' || *text > '9') {
            return false;
        }
        uint64_t carry = (unsigned)(*text - '0');
        for (unsigned i = 0; i < 4; ++i) {
            const U128 current = (U128)value.limb[i] * 10 + carry;
            value.limb[i] = (uint64_t)current;
            carry = (uint64_t)(current >> 64);
        }
        if (carry != 0) {
            return false;
        }
    }
    *result = value;
    return true;
}

static void store_bfile_term(int n, U256 value)
{
    U256 values[MAX_N + 1];
    int count = 0;
    mode_t output_mode = 0644;
    struct stat metadata;
    if (stat(output_path, &metadata) == 0) {
        output_mode = metadata.st_mode & 0777;
    } else if (errno != ENOENT) {
        die("cannot inspect b-file");
    }

    FILE *input = fopen(output_path, "r");
    if (input == NULL && errno != ENOENT) {
        die("cannot open existing b-file");
    }
    if (input != NULL) {
        char line[128];
        while (fgets(line, sizeof(line), input) != NULL) {
            int index;
            char number[80];
            char extra;
            if (count > MAX_N ||
                sscanf(line, "%d %79s %c", &index, number, &extra) != 2 ||
                index != count || !parse_u256(number, &values[count])) {
                fclose(input);
                die("existing b-file is malformed or nonconsecutive");
            }
            ++count;
        }
        if (ferror(input) || fclose(input) != 0) {
            die("cannot read existing b-file");
        }
    }

    if (n < count) {
        if (compare_u256(values[n], value) != 0) {
            die("computed term disagrees with existing b-file");
        }
        return;
    }
    if (n != count) {
        die("b-file has a gap; compute the missing earlier terms first");
    }
    values[count++] = value;

    const char suffix[] = ".tmp.XXXXXX";
    const size_t path_length = strlen(output_path);
    if (path_length > SIZE_MAX - sizeof(suffix)) {
        die("b-file path is too long");
    }
    char *temporary = malloc(path_length + sizeof(suffix));
    if (temporary == NULL) {
        die("cannot allocate b-file temporary path");
    }
    memcpy(temporary, output_path, path_length);
    memcpy(temporary + path_length, suffix, sizeof(suffix));

    const int fd = mkstemp(temporary);
    if (fd < 0) {
        free(temporary);
        die("cannot create temporary b-file");
    }
    if (fchmod(fd, output_mode) != 0) {
        close(fd);
        unlink(temporary);
        free(temporary);
        die("cannot set temporary b-file permissions");
    }
    FILE *output = fdopen(fd, "w");
    if (output == NULL) {
        close(fd);
        unlink(temporary);
        free(temporary);
        die("cannot open temporary b-file stream");
    }
    bool failed = false;
    for (int index = 0; index < count; ++index) {
        if (fprintf(output, "%d ", index) < 0 ||
            print_u256(output, values[index]) != 0 ||
            fputc('\n', output) == EOF) {
            failed = true;
            break;
        }
    }
    if (!failed && fflush(output) != 0) {
        failed = true;
    }
    if (!failed && fsync(fd) != 0) {
        failed = true;
    }
    if (fclose(output) != 0) {
        failed = true;
    }
    if (failed) {
        unlink(temporary);
        free(temporary);
        die("cannot write temporary b-file");
    }
    if (rename(temporary, output_path) != 0) {
        unlink(temporary);
        free(temporary);
        die("cannot atomically replace b-file");
    }
    free(temporary);
    fprintf(stderr, "322179_02: updated %s through n=%d\n",
            output_path, n);
}

static uint64_t gray_code(uint64_t index)
{
    return index ^ (index >> 1);
}

static void compute_edges(const Worker *worker, uint64_t subset,
                          uint16_t edges[MAX_N + 1])
{
    memset(edges, 0, (MAX_N + 1) * sizeof(*edges));
    for (int k = 1; k <= worker->n; ++k) {
        unsigned count = 0;
        for (unsigned distance = (unsigned)k + 1;
             distance < worker->positions; ++distance) {
            count += (unsigned)__builtin_popcountll(
                subset & (subset >> distance));
        }
        edges[k] = (uint16_t)count;
    }
}

static U256 edge_product(const Worker *worker,
                         const uint16_t edges[MAX_N + 1], bool *ok)
{
    U256 product = {{1, 0, 0, 0}};
    for (int k = 1; k <= worker->n; ++k) {
        if (edges[k] == 0) {
            U256 zero = {{0, 0, 0, 0}};
            return zero;
        }
        if (!multiply_u256_small(&product, edges[k])) {
            *ok = false;
            U256 zero = {{0, 0, 0, 0}};
            return zero;
        }
    }
    return product;
}

static void update_edges(const Worker *worker, uint16_t edges[MAX_N + 1],
                         uint64_t other, unsigned position, bool adding)
{
    for (int k = 1; k <= worker->n; ++k) {
        const unsigned delta = (unsigned)__builtin_popcountll(
            other & worker->far_mask[position][k]);
        if (adding) {
            edges[k] = (uint16_t)(edges[k] + delta);
        } else {
            if (edges[k] < delta) {
                die("internal edge-count underflow");
            }
            edges[k] = (uint16_t)(edges[k] - delta);
        }
    }
}

static void *worker_main(void *argument)
{
    Worker *worker = argument;
    uint64_t subset = gray_code(worker->begin);
    uint16_t edges[MAX_N + 1];
    compute_edges(worker, subset, edges);

    for (uint64_t index = worker->begin; index < worker->end; ++index) {
        bool ok = true;
        const U256 term = edge_product(worker, edges, &ok);
        if (!ok) {
            worker->failed = 1;
            snprintf(worker->error, sizeof(worker->error),
                     "individual product overflow");
            return NULL;
        }
        U256 *sum = (index & 1) ? &worker->negative : &worker->positive;
        if (!add_u256(sum, term)) {
            worker->failed = 1;
            snprintf(worker->error, sizeof(worker->error),
                     "inclusion-exclusion sum overflow");
            return NULL;
        }

        if (index + 1 == worker->end) {
            break;
        }
        const unsigned position = (unsigned)__builtin_ctzll(index + 1);
        const uint64_t bit = UINT64_C(1) << position;
        if ((subset & bit) == 0) {
            update_edges(worker, edges, subset, position, true);
            subset |= bit;
        } else {
            subset ^= bit;
            update_edges(worker, edges, subset, position, false);
        }
    }
    return NULL;
}

static U256 a322179(int n, int requested_threads)
{
    if (n == 0) {
        U256 one = {{1, 0, 0, 0}};
        return one;
    }

    const unsigned positions = 2U * (unsigned)n;
    const uint64_t subset_count = UINT64_C(1) << positions;
    int thread_count = requested_threads;
    if ((uint64_t)thread_count > subset_count) {
        thread_count = (int)subset_count;
    }

    uint64_t far_mask[MAX_POSITIONS][MAX_N + 1];
    memset(far_mask, 0, sizeof(far_mask));
    for (unsigned position = 0; position < positions; ++position) {
        for (int k = 1; k <= n; ++k) {
            uint64_t mask = 0;
            for (unsigned other = 0; other < positions; ++other) {
                const unsigned distance = position > other ?
                    position - other : other - position;
                if (distance >= (unsigned)k + 1) {
                    mask |= UINT64_C(1) << other;
                }
            }
            far_mask[position][k] = mask;
        }
    }

    Worker *workers = calloc((size_t)thread_count, sizeof(*workers));
    pthread_t *threads = calloc((size_t)thread_count, sizeof(*threads));
    if (workers == NULL || threads == NULL) {
        free(workers);
        free(threads);
        die("cannot allocate workers");
    }

    for (int id = 0; id < thread_count; ++id) {
        Worker *worker = &workers[id];
        worker->n = n;
        worker->positions = positions;
        worker->begin = subset_count * (uint64_t)id /
                        (uint64_t)thread_count;
        worker->end = subset_count * (uint64_t)(id + 1) /
                      (uint64_t)thread_count;
        worker->far_mask = far_mask;
        const int error = pthread_create(&threads[id], NULL,
                                         worker_main, worker);
        if (error != 0) {
            fprintf(stderr, "error: pthread_create: %s\n", strerror(error));
            exit(EXIT_FAILURE);
        }
    }

    U256 positive = {{0, 0, 0, 0}};
    U256 negative = {{0, 0, 0, 0}};
    for (int id = 0; id < thread_count; ++id) {
        const int error = pthread_join(threads[id], NULL);
        if (error != 0) {
            fprintf(stderr, "error: pthread_join: %s\n", strerror(error));
            exit(EXIT_FAILURE);
        }
        if (workers[id].failed) {
            die(workers[id].error);
        }
        if (!add_u256(&positive, workers[id].positive) ||
            !add_u256(&negative, workers[id].negative)) {
            die("merged inclusion-exclusion sum overflow");
        }
    }
    free(workers);
    free(threads);

    if (compare_u256(positive, negative) < 0) {
        die("negative final inclusion-exclusion result");
    }
    return subtract_u256(positive, negative);
}

static void verify_known(int n, U256 value)
{
    const int known_count = (int)(sizeof(known) / sizeof(known[0]));
    if (n >= known_count) {
        return;
    }
    U256 expected;
    if (!parse_u256(known[n], &expected)) {
        die("invalid built-in known term");
    }
    if (compare_u256(value, expected) != 0) {
        fprintf(stderr, "error: A322179 mismatch at n=%d: got ", n);
        print_u256(stderr, value);
        fprintf(stderr, ", expected %s\n", known[n]);
        exit(EXIT_FAILURE);
    }
}

static U256 evaluated(int n, int threads, bool verbose)
{
    const double started = now_seconds();
    const U256 value = a322179(n, threads);
    verify_known(n, value);
    if (verbose) {
        fprintf(stderr,
                "322179_02: n=%d, Boolean IE, threads=%d, %.3f s\n",
                n, threads, now_seconds() - started);
    }
    return value;
}

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s [MAX_N] [--threads T] [--output FILE]\n"
            "       %s --term N [--threads T] [--output FILE]\n"
            "       %s --check [--threads T] [--no-bfile]\n"
            "N must be in 0..%d; T must be in 1..%d.\n",
            program, program, program, MAX_N, MAX_THREADS);
}

int main(int argc, char **argv)
{
    enum { MODE_RANGE, MODE_TERM, MODE_CHECK } mode = MODE_RANGE;
    int n = -1;
    int threads = default_threads();
    bool have_mode = false;
    bool have_threads = false;
    bool have_output_option = false;

    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")) {
            usage(argv[0]);
            return EXIT_SUCCESS;
        }
        if (!strcmp(argv[i], "--threads")) {
            if (have_threads || ++i >= argc) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            threads = parse_int(argv[i], "threads", 1, MAX_THREADS);
            have_threads = true;
        } else if (!strcmp(argv[i], "--output")) {
            if (have_output_option || ++i >= argc) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            output_path = argv[i];
            write_bfile = true;
            have_output_option = true;
        } else if (!strcmp(argv[i], "--no-bfile")) {
            if (have_output_option) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            write_bfile = false;
            have_output_option = true;
        } else if (!strcmp(argv[i], "--term")) {
            if (have_mode || ++i >= argc) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            mode = MODE_TERM;
            n = parse_int(argv[i], "N", 0, MAX_N);
            have_mode = true;
        } else if (!strcmp(argv[i], "--check")) {
            if (have_mode) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            mode = MODE_CHECK;
            have_mode = true;
        } else if (argv[i][0] != '-' && !have_mode) {
            mode = MODE_RANGE;
            n = parse_int(argv[i], "N", 0, MAX_N);
            have_mode = true;
        } else {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (n < 0) {
        n = DEFAULT_N;
    }
    if (mode == MODE_CHECK) {
        const int known_count = (int)(sizeof(known) / sizeof(known[0]));
        for (int k = 0; k < known_count; ++k) {
            (void)evaluated(k, threads, false);
        }
        printf("ok: A322179 terms n=0..%d verified\n", known_count - 1);
        return EXIT_SUCCESS;
    }
    if (mode == MODE_TERM) {
        const U256 value = evaluated(n, threads, true);
        if (write_bfile) {
            store_bfile_term(n, value);
        }
        printf("%d ", n);
        print_u256(stdout, value);
        putchar('\n');
        return EXIT_SUCCESS;
    }

    for (int k = 0; k <= n; ++k) {
        const U256 value = evaluated(k, threads, true);
        if (write_bfile) {
            store_bfile_term(k, value);
        }
        printf("%d ", k);
        print_u256(stdout, value);
        putchar('\n');
    }
    return EXIT_SUCCESS;
}
