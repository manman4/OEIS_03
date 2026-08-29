/*
 * A349145 -- ordered n-tuples with integral sum Sum(k/x_k), algorithm 02.
 *
 * Put L=lcm(1,...,n).  For each tuple, integrality is equivalent to
 *
 *   Sum_{k=1}^n k*(L/x_k) == 0 (mod L).                 (1)
 *
 * Unlike 349145_01, which carries one residue distribution through all n
 * positions, this program uses meet in the middle.  Split the positions into
 * a left half and a right half.  Independently compute
 *
 *   A(r) = number of left-half assignments with residue r,
 *   B(r) = number of right-half assignments with residue r.
 *
 * Every full tuple has one unique pair of half assignments, so (1) gives
 *
 *   a(n) = Sum_{r=0}^{L-1} A(r) * B(-r mod L).          (2)
 *
 * Formula (2) is exact and uses no heuristic pruning.  The left distribution
 * is temporarily written to disk while the same two work arrays are reused
 * for the right distribution.  It is read back only for the final scalar
 * product.  At n=19..21 this needs about 3552.1 MiB of work memory and
 * 1776.1 MiB of temporary disk space, instead of keeping three distributions
 * in memory.  The requirements are equal because
 * lcm(1,...,21)=lcm(1,...,19)=232792560.
 * The temporary file is unlinked immediately after creation and is removed
 * automatically when closed or if the process terminates.
 *
 * Each half contains at most ceil(n/2) positions.  For supported n<=21, every
 * half-distribution entry is at most
 *
 *   21^11 = 350277500542221 < 2^49,
 *
 * so uint64_t is sufficient.  The final answer is at most
 *
 *   21^21 = 5842587018385982521381124421 < 2^93,
 *
 * and is accumulated in an explicitly checked unsigned 96-bit type.
 * Allocation sizes, configured memory, available temporary disk space,
 * transition additions, products, counters, I/O, and b-file replacement are
 * checked at runtime.
 *
 * Known OEIS terms through n=13 are verification data only.  --check also
 * compares the meet-in-the-middle result with direct enumeration of all n^n
 * tuples for n<=8.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic -Werror \
 *       349145_02.c -o 349145_02
 *
 * Usage:
 *   ./349145_02
 *   ./349145_02 --upto 21 --memory-mb 4096 --verbose
 *   ./349145_02 --term 21 --memory-mb 4096 --verbose
 *   ./349145_02 --check
 *
 * The default and --upto modes print completed terms and atomically replace
 * b349145_02.txt.  --term and --check do not modify the b-file.  Runs lasting
 * at least 60 seconds report progress to standard error approximately every
 * 60 seconds.  --verbose additionally reports every completed position and
 * final per-term statistics.
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
#include <sys/statvfs.h>
#include <time.h>
#include <unistd.h>

#define MAX_N 21
#define DEFAULT_UPTO 13
#define KNOWN_MAX_N 13
#define DIRECT_CHECK_MAX_N 8
#define DEFAULT_MEMORY_MB 4096
#define MIN_MEMORY_MB 16
#define MAX_MEMORY_MB 65536
#define BFILE_NAME "b349145_02.txt"
#define BFILE_TEMP_TEMPLATE BFILE_NAME ".tmp.XXXXXX"
#define BFILE_LOCK_NAME BFILE_NAME ".lock"
#define SCRATCH_TEMPLATE ".349145_02.mitm.XXXXXX"
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
    uint64_t temporary_bytes;
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

static void multiply_u64(uint64_t left, uint64_t right,
                         uint32_t product[4])
{
    const uint64_t left_low = (uint32_t)left;
    const uint64_t left_high = left >> 32;
    const uint64_t right_low = (uint32_t)right;
    const uint64_t right_high = right >> 32;

    uint64_t temporary = left_low * right_low;
    product[0] = (uint32_t)temporary;
    uint64_t carry = temporary >> 32;

    temporary = left_high * right_low + carry;
    uint32_t middle = (uint32_t)temporary;
    const uint64_t upper = temporary >> 32;

    temporary = left_low * right_high + middle;
    product[1] = (uint32_t)temporary;
    carry = temporary >> 32;

    temporary = left_high * right_high + upper + carry;
    product[2] = (uint32_t)temporary;
    product[3] = (uint32_t)(temporary >> 32);
}

static void count_add_product(Count *total, uint64_t left, uint64_t right)
{
    uint32_t product[4];
    multiply_u64(left, right, product);
    uint64_t carry = 0U;
    for (unsigned i = 0U; i < COUNT_LIMBS; ++i) {
        const uint64_t sum = (uint64_t)total->limb[i] + product[i] + carry;
        total->limb[i] = (uint32_t)sum;
        carry = sum >> 32;
    }
    if ((uint64_t)product[3] + carry != 0U)
        die("final count exceeds the internal 96-bit type");
}

static void count_add_one(Count *value)
{
    uint64_t carry = 1U;
    for (unsigned i = 0U; i < COUNT_LIMBS && carry != 0U; ++i) {
        const uint64_t sum = (uint64_t)value->limb[i] + carry;
        value->limb[i] = (uint32_t)sum;
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
    uint32_t chunks[MAX_DECIMAL_CHUNKS] = { 0U };
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
        fprintf(stderr, "error: size overflow for %s\n", description);
        exit(EXIT_FAILURE);
    }
    return left * right;
}

static void *checked_calloc(size_t count, size_t width,
                            size_t *live_memory, size_t memory_limit,
                            size_t *peak_memory, const char *description)
{
    if (count == 0U || width == 0U)
        die("internal zero-sized allocation request");
    const size_t bytes = checked_product(count, width, description);
    if (*live_memory > memory_limit || bytes > memory_limit - *live_memory) {
        fprintf(stderr,
                "error: memory limit exceeded allocating %s "
                "(%zu bytes requested, %zu bytes live, %zu bytes limit)\n",
                description, bytes, *live_memory, memory_limit);
        exit(EXIT_FAILURE);
    }
    void *result = calloc(count, width);
    if (result == NULL)
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
        if (modulus % (uint64_t)denominator != 0U)
            die("denominator does not divide the LCM");
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

static void add_half_transition(uint64_t *destination, uint64_t value,
                                uint32_t multiplicity)
{
    if (multiplicity != 0U && value > UINT64_MAX / multiplicity)
        die("half-distribution multiplication overflow");
    const uint64_t increment = value * multiplicity;
    if (*destination > UINT64_MAX - increment)
        die("half-distribution addition overflow");
    *destination += increment;
}

static void compute_half(int n, int first_position, int last_position,
                         uint64_t modulus, size_t capacity,
                         uint64_t **current_pointer, uint64_t **next_pointer,
                         size_t *peak_active, uint64_t *transitions,
                         double started, double *next_progress,
                         const char *half_name, bool verbose)
{
    uint64_t *current = *current_pointer;
    uint64_t *next = *next_pointer;
    current[0] = 1U;
    size_t current_active = 1U;

    for (int position = first_position; position <= last_position; ++position) {
        uint64_t shifts[MAX_N];
        uint32_t multiplicities[MAX_N];
        const size_t shift_count = make_unique_shifts(
            n, position, modulus, shifts, multiplicities);
        size_t next_active = 0U;
        size_t visited = 0U;

        for (size_t residue = 0U; residue < capacity; ++residue) {
            if ((residue & (size_t)1048575U) == 0U) {
                const double now = monotonic_seconds();
                if (now >= *next_progress) {
                    fprintf(stderr,
                            "349145_02 progress: n=%d, half=%s, "
                            "position=%d, scan=%zu/%zu, current_active=%zu, "
                            "next_active=%zu, transitions=%" PRIu64
                            ", elapsed=%.1f min\n",
                            n, half_name, position, residue, capacity,
                            current_active, next_active, *transitions,
                            (now - started) / 60.0);
                    *next_progress = now + 60.0;
                }
            }

            const uint64_t state_count = current[residue];
            if (state_count == 0U)
                continue;
            ++visited;
            for (size_t shift_index = 0U;
                 shift_index < shift_count; ++shift_index) {
                checked_increment(transitions, "transition");
                const uint64_t destination = add_mod(
                    (uint64_t)residue, shifts[shift_index], modulus);
                uint64_t *const destination_count =
                    &next[(size_t)destination];
                if (*destination_count == 0U) {
                    if (next_active == capacity)
                        die("half active-state counter overflow");
                    ++next_active;
                }
                add_half_transition(destination_count, state_count,
                                    multiplicities[shift_index]);
            }
        }
        if (visited != current_active)
            die("half active-state count mismatch");
        memset(current, 0, capacity * sizeof(*current));

        uint64_t *const swap = current;
        current = next;
        next = swap;
        current_active = next_active;
        if (current_active > *peak_active)
            *peak_active = current_active;

        if (verbose) {
            fprintf(stderr,
                    "349145_02 position: n=%d, half=%s, position=%d, "
                    "active=%zu, transitions=%" PRIu64
                    ", elapsed=%.1f min\n",
                    n, half_name, position, current_active, *transitions,
                    (monotonic_seconds() - started) / 60.0);
        }
    }

    *current_pointer = current;
    *next_pointer = next;
}

static FILE *open_scratch_file(uint64_t required_bytes)
{
    char path[] = SCRATCH_TEMPLATE;
    const int descriptor = mkstemp(path);
    if (descriptor < 0)
        die("cannot create meet-in-the-middle temporary file");
    if (unlink(path) != 0) {
        const int saved_errno = errno;
        (void)close(descriptor);
        (void)unlink(path);
        errno = saved_errno;
        die("cannot unlink meet-in-the-middle temporary file");
    }

    struct statvfs file_system;
    if (fstatvfs(descriptor, &file_system) != 0) {
        const int saved_errno = errno;
        (void)close(descriptor);
        errno = saved_errno;
        die("cannot inspect temporary-file free space");
    }
    uint64_t available_bytes;
    if (file_system.f_frsize != 0U &&
        file_system.f_bavail > UINT64_MAX / file_system.f_frsize) {
        available_bytes = UINT64_MAX;
    } else {
        available_bytes =
            (uint64_t)file_system.f_bavail * file_system.f_frsize;
    }
    if (required_bytes > available_bytes) {
        (void)close(descriptor);
        fprintf(stderr,
                "error: temporary distribution needs %.1f MiB, but only "
                "%.1f MiB is available\n",
                (double)required_bytes / (1024.0 * 1024.0),
                (double)available_bytes / (1024.0 * 1024.0));
        exit(EXIT_FAILURE);
    }

    FILE *const stream = fdopen(descriptor, "w+b");
    if (stream == NULL) {
        const int saved_errno = errno;
        (void)close(descriptor);
        errno = saved_errno;
        die("cannot open meet-in-the-middle temporary stream");
    }
    return stream;
}

static Count count_ordered_tuples(int n, size_t memory_limit,
                                  Statistics *statistics, bool verbose)
{
    const double started = monotonic_seconds();
    double next_progress = started + 60.0;
    const uint64_t modulus = make_lcm(n);
    if (modulus > SIZE_MAX)
        die("modulus exceeds size_t");
    const size_t capacity = (size_t)modulus;
    if (capacity == 0U)
        die("internal zero modulus");
    const size_t array_bytes = checked_product(
        capacity, sizeof(uint64_t), "half-distribution array");
    if (array_bytes > SIZE_MAX / 2U)
        die("combined work-array size overflow");
    const size_t required_memory = 2U * array_bytes;
    if (required_memory > memory_limit) {
        fprintf(stderr,
                "error: n=%d requires %.1f MiB for the two work arrays, "
                "exceeding the %.1f MiB limit\n",
                n, (double)required_memory / (1024.0 * 1024.0),
                (double)memory_limit / (1024.0 * 1024.0));
        exit(EXIT_FAILURE);
    }

    size_t live_memory = 0U;
    size_t peak_memory = 0U;
    uint64_t *current = checked_calloc(
        capacity, sizeof(*current), &live_memory, memory_limit, &peak_memory,
        "first half-distribution array");
    uint64_t *next = checked_calloc(
        capacity, sizeof(*next), &live_memory, memory_limit, &peak_memory,
        "second half-distribution array");

    if ((uint64_t)capacity > UINT64_MAX / sizeof(uint64_t))
        die("temporary-file size overflow");
    const uint64_t temporary_bytes =
        (uint64_t)capacity * sizeof(uint64_t);
    FILE *const scratch = open_scratch_file(temporary_bytes);

    size_t peak_active = 1U;
    uint64_t transitions = 0U;
    const int left_last = n / 2;
    compute_half(n, 1, left_last, modulus, capacity, &current, &next,
                 &peak_active, &transitions, started, &next_progress,
                 "left", verbose);

    if (fwrite(current, sizeof(*current), capacity, scratch) != capacity ||
        fflush(scratch) != 0 || fsync(fileno(scratch)) != 0)
        die("cannot write the temporary left distribution");
    memset(current, 0, array_bytes);

    compute_half(n, left_last + 1, n, modulus, capacity, &current, &next,
                 &peak_active, &transitions, started, &next_progress,
                 "right", verbose);

    if (fseeko(scratch, 0, SEEK_SET) != 0)
        die("cannot rewind the temporary left distribution");
    if (fread(next, sizeof(*next), capacity, scratch) != capacity)
        die("cannot read the temporary left distribution");
    if (fclose(scratch) != 0)
        die("cannot close the temporary left distribution");

    Count answer;
    memset(&answer, 0, sizeof(answer));
    for (size_t residue = 0U; residue < capacity; ++residue) {
        const size_t complement = residue == 0U ? 0U : capacity - residue;
        if (next[residue] != 0U && current[complement] != 0U)
            count_add_product(&answer, next[residue], current[complement]);
    }

    free(next);
    free(current);
    statistics->modulus = modulus;
    statistics->peak_active = peak_active;
    statistics->peak_memory = peak_memory;
    statistics->temporary_bytes = temporary_bytes;
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
        if (residue == 0U)
            count_add_one(&search->answer);
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
    const Count result = count_ordered_tuples(
        n, memory_limit, statistics, verbose);
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
            "349145_02: n=%d, L=%" PRIu64 ", peak_active=%zu, "
            "transitions=%" PRIu64 ", peak_memory=%.1f MiB, "
            "temporary=%.1f MiB, time=%.3f s\n",
            n, statistics->modulus, statistics->peak_active,
            statistics->transitions,
            (double)statistics->peak_memory / (1024.0 * 1024.0),
            (double)statistics->temporary_bytes / (1024.0 * 1024.0),
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
    Count arithmetic_test;
    memset(&arithmetic_test, 0, sizeof(arithmetic_test));
    count_add_product(&arithmetic_test, UINT64_C(8796093022207),
                      UINT64_C(8796093022207));
    const Count arithmetic_expected = {
        { UINT32_C(1), UINT32_C(4294963200), UINT32_C(4194303) }
    };
    if (!count_equal(&arithmetic_test, &arithmetic_expected))
        die("64-by-64 to 96-bit arithmetic self-test failed");

    for (int n = 0; n <= DIRECT_CHECK_MAX_N; ++n) {
        Statistics statistics;
        const Count middle = compute_term(n, memory_limit, &statistics, false);
        const Count direct = brute_count(n);
        if (!count_equal(&middle, &direct)) {
            fprintf(stderr,
                    "error: meet-in-the-middle and direct enumeration "
                    "disagree at n=%d\n", n);
            exit(EXIT_FAILURE);
        }
        if (verbose)
            report_statistics(n, &statistics);
    }
    fprintf(stderr,
            "check passed: meet-in-the-middle equals direct enumeration for "
            "n=0..%d; known OEIS terms are checked after every computation\n",
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
        const Count result = compute_term(
            requested, memory_limit, &statistics, verbose);
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
