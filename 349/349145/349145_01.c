/*
 * A349145 -- ordered n-tuples with integral sum Sum(k/x_k).
 *
 * For a fixed n, put L=lcm(1,...,n).  Since every x_k divides L,
 *
 *   Sum_{k=1}^n k/x_k is an integer
 *
 * if and only if
 *
 *   Sum_{k=1}^n k*(L/x_k) == 0 (mod L).                 (1)
 *
 * The program applies (1) directly.  After positions 1,...,k have been
 * assigned, dp[r] is the number of assignments whose numerator in (1) is
 * congruent to r modulo L.  The transition tries every x_k in [1,n].
 * Residues with equal value are merged by addition; no heuristic pruning is
 * used.  Consequently the computation is an exact reformulation of the
 * definition, not a use of known sequence values.
 *
 * Two dense count arrays give constant-time updates.  At every supported n,
 * the current array is scanned sequentially and zero entries are skipped.
 * Thus all n use exactly the same state representation and traversal, without
 * auxiliary active-residue lists.  Equal shifts at one position are combined
 * exactly before the transition.
 * Counts use an unsigned 96-bit type implemented as three 32-bit limbs.  For
 * every supported n<=20, every partial count is at most
 *
 *   n^n <= 20^20 = 104857600000000000000000000 < 2^87,
 *
 * so this is amply sufficient.  Arithmetic, allocation sizes, memory limits,
 * LCM construction, transition counters, output, and b-file replacement are
 * checked at runtime.
 *
 * The LCM jumps from 12252240 at n=18 to 232792560 at n=19.  At n=19 and 20,
 * the two 96-bit count arrays require about 5328.2 MiB.  Thus --memory-mb 5376
 * is sufficient for the arrays; 6144 is the conservative recommended setting.
 * The allocation is rejected before use if the configured limit is too small.
 *
 * Known OEIS terms through n=13 are verification data only.  They are checked
 * after computation and are never returned as answers.  --check additionally
 * compares the DP with direct enumeration of all n^n tuples for n<=8.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic -Werror \
 *       349145_01.c -o 349145_01
 *
 * Usage:
 *   ./349145_01
 *   ./349145_01 --upto 20 --memory-mb 6144 --verbose
 *   ./349145_01 --term 20 --memory-mb 6144 --verbose
 *   ./349145_01 --check
 *
 * The default and --upto modes print completed terms and atomically replace
 * b349145_01.txt.  --term and --check do not modify the b-file.  A computation
 * lasting at least 60 seconds reports its progress to standard error every
 * approximately 60 seconds.  --verbose additionally reports every completed
 * position and final per-term statistics.
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

#define MAX_N 20
#define DEFAULT_UPTO 13
#define KNOWN_MAX_N 13
#define DIRECT_CHECK_MAX_N 8
#define DEFAULT_MEMORY_MB 6144
#define MIN_MEMORY_MB 16
#define MAX_MEMORY_MB 65536
#define BFILE_NAME "b349145_01.txt"
#define BFILE_TEMP_TEMPLATE BFILE_NAME ".tmp.XXXXXX"
#define BFILE_LOCK_NAME BFILE_NAME ".lock"
#define COUNT_LIMBS 3
#define DECIMAL_BASE UINT32_C(1000000000)
#define MAX_DECIMAL_CHUNKS 5

static const uint64_t known[KNOWN_MAX_N + 1] = {
    UINT64_C(1), UINT64_C(1), UINT64_C(2), UINT64_C(8),
    UINT64_C(43), UINT64_C(207), UINT64_C(2391), UINT64_C(15539),
    UINT64_C(182078), UINT64_C(2070189), UINT64_C(35850460),
    UINT64_C(338695058), UINT64_C(10609401552),
    UINT64_C(115445915555)
};

typedef struct {
    uint32_t limb[COUNT_LIMBS];
} Count;

_Static_assert(sizeof(Count) == 12U, "Count must remain a packed 96-bit type");

typedef struct {
    uint64_t modulus;
    size_t peak_active;
    size_t peak_memory;
    uint64_t transitions;
    double seconds;
} Statistics;

typedef enum {
    MODE_UPTO,
    MODE_TERM,
    MODE_CHECK
} OutputMode;

static _Noreturn void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static double monotonic_seconds(void)
{
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) != 0)
        die("clock_gettime failed");
    return (double)now.tv_sec + (double)now.tv_nsec / 1000000000.0;
}

static int parse_integer(const char *text, const char *label,
                         int minimum, int maximum)
{
    errno = 0;
    char *end = NULL;
    const intmax_t value = strtoimax(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' ||
        value < minimum || value > maximum) {
        fprintf(stderr, "error: %s must be in %d..%d: %s\n",
                label, minimum, maximum, text);
        exit(EXIT_FAILURE);
    }
    return (int)value;
}

static Count count_from_u64(uint64_t value)
{
    Count result;
    memset(&result, 0, sizeof(result));
    result.limb[0] = (uint32_t)value;
    result.limb[1] = (uint32_t)(value >> 32);
    return result;
}

static bool count_is_zero(const Count *value)
{
    for (unsigned i = 0U; i < COUNT_LIMBS; ++i)
        if (value->limb[i] != 0U)
            return false;
    return true;
}

static bool count_equal(const Count *left, const Count *right)
{
    return memcmp(left, right, sizeof(*left)) == 0;
}

static void count_add_multiple(Count *left, const Count *right,
                               uint32_t multiplier)
{
    uint64_t carry = 0U;
    for (unsigned i = 0U; i < COUNT_LIMBS; ++i) {
        const uint64_t sum = (uint64_t)left->limb[i] +
            (uint64_t)right->limb[i] * multiplier + carry;
        left->limb[i] = (uint32_t)sum;
        carry = sum >> 32;
    }
    if (carry != 0U)
        die("count exceeds the internal 96-bit type");
}

static uint32_t count_divide_decimal_base(Count *value)
{
    uint64_t remainder = 0U;
    for (unsigned i = COUNT_LIMBS; i-- > 0U;) {
        const uint64_t part = (remainder << 32) | value->limb[i];
        value->limb[i] = (uint32_t)(part / DECIMAL_BASE);
        remainder = part % DECIMAL_BASE;
    }
    return (uint32_t)remainder;
}

static int print_count(FILE *stream, const Count *value)
{
    if (count_is_zero(value))
        return fputc('0', stream) == EOF ? -1 : 0;

    Count copy = *value;
    uint32_t chunks[MAX_DECIMAL_CHUNKS];
    size_t used = 0U;
    while (!count_is_zero(&copy)) {
        if (used == MAX_DECIMAL_CHUNKS)
            die("internal decimal buffer is too small");
        chunks[used++] = count_divide_decimal_base(&copy);
    }

    if (fprintf(stream, "%" PRIu32, chunks[used - 1U]) < 0)
        return -1;
    while (--used > 0U)
        if (fprintf(stream, "%09" PRIu32, chunks[used - 1U]) < 0)
            return -1;
    return 0;
}

static void checked_increment(uint64_t *value, const char *label)
{
    if (*value == UINT64_MAX) {
        fprintf(stderr, "error: %s counter overflow\n", label);
        exit(EXIT_FAILURE);
    }
    ++*value;
}

static uint64_t gcd_u64(uint64_t left, uint64_t right)
{
    while (right != 0U) {
        const uint64_t remainder = left % right;
        left = right;
        right = remainder;
    }
    return left;
}

static uint64_t make_lcm(int n)
{
    uint64_t result = 1U;
    for (int value = 2; value <= n; ++value) {
        const uint64_t factor = (uint64_t)value /
            gcd_u64(result, (uint64_t)value);
        if (factor != 0U && result > UINT64_MAX / factor)
            die("lcm(1,...,n) exceeds uint64_t");
        result *= factor;
    }
    return result;
}

static uint64_t add_mod(uint64_t left, uint64_t right, uint64_t modulus)
{
    if (left >= modulus || right >= modulus || modulus == 0U)
        die("internal modular-addition precondition failed");
    return left >= modulus - right ? left - (modulus - right) : left + right;
}

static size_t checked_product(size_t left, size_t right,
                              const char *description)
{
    if (right != 0U && left > SIZE_MAX / right) {
        fprintf(stderr, "error: allocation size overflow for %s\n",
                description);
        exit(EXIT_FAILURE);
    }
    return left * right;
}

static void *checked_calloc(size_t count, size_t width,
                            size_t *live_memory, size_t memory_limit,
                            size_t *peak_memory, const char *description)
{
    const size_t bytes = checked_product(count, width, description);
    if (bytes > memory_limit - *live_memory) {
        fprintf(stderr,
                "error: memory limit exceeded allocating %s "
                "(%zu bytes requested, %zu bytes live, %zu bytes limit)\n",
                description, bytes, *live_memory, memory_limit);
        exit(EXIT_FAILURE);
    }
    void *result = calloc(count, width);
    if (result == NULL && bytes != 0U)
        die("memory allocation failed");
    *live_memory += bytes;
    if (*live_memory > *peak_memory)
        *peak_memory = *live_memory;
    return result;
}

static size_t make_unique_shifts(int n, int position, uint64_t modulus,
                                 uint64_t shifts[MAX_N],
                                 uint32_t multiplicities[MAX_N])
{
    size_t used = 0U;
    for (int denominator = 1; denominator <= n; ++denominator) {
        const uint64_t weight = modulus / (uint64_t)denominator;
        if ((uint64_t)position > UINT64_MAX / weight)
            die("transition weight exceeds uint64_t");
        const uint64_t shift = ((uint64_t)position * weight) % modulus;
        size_t index = 0U;
        while (index < used && shifts[index] != shift)
            ++index;
        if (index == used) {
            if (used == MAX_N)
                die("internal shift table overflow");
            shifts[used] = shift;
            multiplicities[used] = 1U;
            ++used;
        } else {
            if (multiplicities[index] == UINT32_MAX)
                die("shift multiplicity overflow");
            ++multiplicities[index];
        }
    }
    return used;
}

static Count count_ordered_tuples(int n, size_t memory_limit,
                                  Statistics *statistics, bool verbose)
{
    const double started = monotonic_seconds();
    const uint64_t modulus = make_lcm(n);
    if (modulus > UINT32_MAX)
        die("residue index exceeds the internal uint32_t type");
    if (modulus > SIZE_MAX)
        die("modulus exceeds size_t");
    const size_t capacity = (size_t)modulus;

    const size_t count_array_bytes = checked_product(
        capacity, sizeof(Count), "residue-count array");
    if (count_array_bytes > SIZE_MAX / 2U)
        die("combined main-array size overflow");
    const size_t required_bytes = 2U * count_array_bytes;
    if (required_bytes > memory_limit) {
        fprintf(stderr,
                "error: n=%d requires %.1f MiB for the main DP arrays, "
                "exceeding the %.1f MiB limit\n",
                n, (double)required_bytes / (1024.0 * 1024.0),
                (double)memory_limit / (1024.0 * 1024.0));
        exit(EXIT_FAILURE);
    }

    size_t live_memory = 0U;
    size_t peak_memory = 0U;
    Count *current = checked_calloc(capacity, sizeof(*current),
                                    &live_memory, memory_limit, &peak_memory,
                                    "first residue-count array");
    Count *next = checked_calloc(capacity, sizeof(*next),
                                 &live_memory, memory_limit, &peak_memory,
                                 "second residue-count array");

    current[0] = count_from_u64(1U);
    size_t current_size = 1U;
    size_t peak_active = 1U;
    uint64_t transitions = 0U;
    double next_progress = started + 60.0;

    for (int position = 1; position <= n; ++position) {
        uint64_t shifts[MAX_N];
        uint32_t multiplicities[MAX_N];
        const size_t shift_count = make_unique_shifts(
            n, position, modulus, shifts, multiplicities);
        size_t next_size = 0U;

        size_t visited = 0U;
        for (size_t residue_index = 0U;
             residue_index < capacity; ++residue_index) {
            if ((residue_index & (size_t)1048575U) == 0U) {
                const double now = monotonic_seconds();
                if (now >= next_progress) {
                    fprintf(stderr,
                            "349145_01 progress: n=%d, position=%d/%d, "
                            "scan=%zu/%zu, current_active=%zu, "
                            "next_active=%zu, transitions=%" PRIu64
                            ", elapsed=%.1f min\n",
                            n, position, n, residue_index, capacity,
                            current_size, next_size, transitions,
                            (now - started) / 60.0);
                    next_progress = now + 60.0;
                }
            }
            const Count state_count = current[residue_index];
            if (count_is_zero(&state_count))
                continue;
            ++visited;
            for (size_t shift_index = 0U;
                 shift_index < shift_count; ++shift_index) {
                checked_increment(&transitions, "transition");
                const uint64_t destination = add_mod(
                    (uint64_t)residue_index, shifts[shift_index], modulus);
                Count *const destination_count = &next[(size_t)destination];
                if (count_is_zero(destination_count)) {
                    if (next_size == capacity)
                        die("active-state counter overflow");
                    ++next_size;
                }
                count_add_multiple(destination_count, &state_count,
                                   multiplicities[shift_index]);
            }
        }
        if (visited != current_size)
            die("dense-scan active-state count mismatch");
        memset(current, 0, count_array_bytes);

        Count *const count_swap = current;
        current = next;
        next = count_swap;
        current_size = next_size;
        if (current_size > peak_active)
            peak_active = current_size;
        if (verbose) {
            fprintf(stderr,
                    "349145_01 position: n=%d, completed=%d/%d, "
                    "active=%zu, transitions=%" PRIu64 ", elapsed=%.1f min\n",
                    n, position, n, current_size, transitions,
                    (monotonic_seconds() - started) / 60.0);
        }
    }

    const Count answer = current[0];
    free(next);
    free(current);

    statistics->modulus = modulus;
    statistics->peak_active = peak_active;
    statistics->peak_memory = peak_memory;
    statistics->transitions = transitions;
    statistics->seconds = monotonic_seconds() - started;
    return answer;
}

typedef struct {
    int n;
    uint64_t modulus;
    uint64_t shifts[MAX_N + 1][MAX_N + 1];
    Count answer;
} BruteSearch;

static void brute_visit(BruteSearch *search, int position, uint64_t residue)
{
    if (position > search->n) {
        if (residue == 0U) {
            const Count one = count_from_u64(1U);
            count_add_multiple(&search->answer, &one, 1U);
        }
        return;
    }

    for (int denominator = 1; denominator <= search->n; ++denominator) {
        const uint64_t destination = add_mod(
            residue, search->shifts[position][denominator], search->modulus);
        brute_visit(search, position + 1, destination);
    }
}

static Count brute_count(int n)
{
    BruteSearch search;
    memset(&search, 0, sizeof(search));
    search.n = n;
    search.modulus = make_lcm(n);
    for (int position = 1; position <= n; ++position) {
        for (int denominator = 1; denominator <= n; ++denominator) {
            const uint64_t weight = search.modulus / (uint64_t)denominator;
            if ((uint64_t)position > UINT64_MAX / weight)
                die("brute-force transition weight exceeds uint64_t");
            search.shifts[position][denominator] =
                ((uint64_t)position * weight) % search.modulus;
        }
    }
    brute_visit(&search, 1, 0U);
    return search.answer;
}

static Count compute_term(int n, size_t memory_limit, Statistics *statistics,
                          bool verbose)
{
    Count result = count_ordered_tuples(n, memory_limit, statistics, verbose);
    if (n <= KNOWN_MAX_N) {
        const Count expected = count_from_u64(known[n]);
        if (!count_equal(&result, &expected)) {
            fprintf(stderr, "error: computed a(%d) disagrees with OEIS data\n",
                    n);
            exit(EXIT_FAILURE);
        }
    }
    return result;
}

static void report_statistics(int n, const Statistics *statistics)
{
    fprintf(stderr,
            "349145_01: n=%d, L=%" PRIu64 ", peak_active=%zu, "
            "transitions=%" PRIu64 ", peak_memory=%.1f MiB, time=%.3f s\n",
            n, statistics->modulus, statistics->peak_active,
            statistics->transitions,
            (double)statistics->peak_memory / (1024.0 * 1024.0),
            statistics->seconds);
}

static int acquire_bfile_lock(void)
{
    const int descriptor = open(BFILE_LOCK_NAME, O_WRONLY | O_CREAT, 0666);
    if (descriptor < 0)
        die("cannot open b-file lock");

    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    if (fcntl(descriptor, F_SETLKW, &lock) < 0) {
        const int saved_errno = errno;
        (void)close(descriptor);
        errno = saved_errno;
        die("cannot acquire b-file lock");
    }
    return descriptor;
}

static void write_bfile(const Count *terms, int upto)
{
    const int lock_descriptor = acquire_bfile_lock();
    char temporary[] = BFILE_TEMP_TEMPLATE;
    const int descriptor = mkstemp(temporary);
    if (descriptor < 0) {
        (void)close(lock_descriptor);
        die("cannot create temporary b-file");
    }

    const mode_t old_mask = umask(0);
    (void)umask(old_mask);
    if (fchmod(descriptor, (mode_t)(0666 & ~old_mask)) != 0) {
        const int saved_errno = errno;
        (void)close(descriptor);
        (void)unlink(temporary);
        (void)close(lock_descriptor);
        errno = saved_errno;
        die("cannot set temporary b-file permissions");
    }

    FILE *const stream = fdopen(descriptor, "w");
    if (stream == NULL) {
        const int saved_errno = errno;
        (void)close(descriptor);
        (void)unlink(temporary);
        (void)close(lock_descriptor);
        errno = saved_errno;
        die("cannot open temporary b-file stream");
    }

    bool failed = false;
    for (int n = 0; n <= upto && !failed; ++n) {
        if (fprintf(stream, "%d ", n) < 0 ||
            print_count(stream, &terms[n]) != 0 ||
            fputc('\n', stream) == EOF)
            failed = true;
    }
    if (!failed && fflush(stream) != 0)
        failed = true;
    if (!failed && fsync(descriptor) != 0)
        failed = true;
    if (fclose(stream) != 0)
        failed = true;

    if (failed) {
        const int saved_errno = errno;
        (void)unlink(temporary);
        (void)close(lock_descriptor);
        errno = saved_errno;
        die("cannot write temporary b-file");
    }
    if (rename(temporary, BFILE_NAME) != 0) {
        const int saved_errno = errno;
        (void)unlink(temporary);
        (void)close(lock_descriptor);
        errno = saved_errno;
        die("cannot replace b-file");
    }
    if (close(lock_descriptor) != 0)
        die("cannot close b-file lock");
}

static void run_check(size_t memory_limit, bool verbose)
{
    for (int n = 0; n <= DIRECT_CHECK_MAX_N; ++n) {
        Statistics statistics;
        const Count dynamic = compute_term(n, memory_limit, &statistics, false);
        const Count direct = brute_count(n);
        if (!count_equal(&dynamic, &direct)) {
            fprintf(stderr,
                    "error: DP and direct enumeration disagree at n=%d\n", n);
            exit(EXIT_FAILURE);
        }
        if (verbose)
            report_statistics(n, &statistics);
    }
    fprintf(stderr,
            "check passed: DP equals direct enumeration for n=0..%d; "
            "known OEIS terms are checked after every computation\n",
            DIRECT_CHECK_MAX_N);
}

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s [--upto N | --term N | --check] "
            "[--memory-mb M] [--verbose]\n"
            "       N must be in 0..%d; default --upto %d\n",
            program, MAX_N, DEFAULT_UPTO);
}

int main(int argc, char **argv)
{
    OutputMode mode = MODE_UPTO;
    int requested = DEFAULT_UPTO;
    int memory_mb = DEFAULT_MEMORY_MB;
    bool verbose = false;
    bool mode_seen = false;

    for (int argument = 1; argument < argc; ++argument) {
        if (strcmp(argv[argument], "--upto") == 0 ||
            strcmp(argv[argument], "--term") == 0) {
            if (mode_seen || argument + 1 >= argc) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            mode = strcmp(argv[argument], "--upto") == 0 ?
                MODE_UPTO : MODE_TERM;
            requested = parse_integer(argv[++argument], "n", 0, MAX_N);
            mode_seen = true;
        } else if (strcmp(argv[argument], "--check") == 0) {
            if (mode_seen) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            mode = MODE_CHECK;
            mode_seen = true;
        } else if (strcmp(argv[argument], "--memory-mb") == 0) {
            if (argument + 1 >= argc) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            memory_mb = parse_integer(argv[++argument], "memory-mb",
                                      MIN_MEMORY_MB, MAX_MEMORY_MB);
        } else if (strcmp(argv[argument], "--verbose") == 0) {
            verbose = true;
        } else if (strcmp(argv[argument], "--help") == 0) {
            usage(argv[0]);
            return EXIT_SUCCESS;
        } else {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    if ((size_t)memory_mb > SIZE_MAX / (1024U * 1024U))
        die("memory limit does not fit size_t");
    const size_t memory_limit = (size_t)memory_mb * 1024U * 1024U;

    if (mode == MODE_CHECK) {
        run_check(memory_limit, verbose);
        return EXIT_SUCCESS;
    }

    if (mode == MODE_TERM) {
        Statistics statistics;
        const Count result = compute_term(requested, memory_limit, &statistics,
                                          verbose);
        if (print_count(stdout, &result) != 0 || fputc('\n', stdout) == EOF ||
            fflush(stdout) != 0)
            die("cannot write standard output");
        if (verbose)
            report_statistics(requested, &statistics);
        return EXIT_SUCCESS;
    }

    const size_t term_count = (size_t)requested + 1U;
    if (term_count > SIZE_MAX / sizeof(Count))
        die("term-array size overflow");
    Count *const terms = calloc(term_count, sizeof(*terms));
    if (terms == NULL)
        die("cannot allocate term array");

    for (int n = 0; n <= requested; ++n) {
        Statistics statistics;
        terms[n] = compute_term(n, memory_limit, &statistics, verbose);
        if (fprintf(stdout, "%d ", n) < 0 ||
            print_count(stdout, &terms[n]) != 0 ||
            fputc('\n', stdout) == EOF || fflush(stdout) != 0) {
            free(terms);
            die("cannot write standard output");
        }
        if (verbose)
            report_statistics(n, &statistics);
    }

    write_bfile(terms, requested);
    free(terms);
    return EXIT_SUCCESS;
}
