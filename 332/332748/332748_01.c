/*
 * A332748 -- algebraic coefficient extraction.
 *
 * A word has three copies of every label 1,...,n.  If the three copies of k
 * occupy i, i+d_k, i+2*d_k, then d_k=b(k)+1.  The required condition is
 *
 *     d_1 >= d_2 >= ... >= d_n.
 *
 * Put m=3*n and, for 1<=d<=(m-1)/2, define the cubic form
 *
 *     Q_d(x) = sum_(0 <= i < m-2*d) x_i*x_(i+d)*x_(i+2*d).
 *
 * In an unlabelled partition into equally spaced triples, let r_d triples
 * have step d.  The expansion Q_d(x)^r_d contains each set of r_d disjoint
 * step-d triples exactly r_d! times: its ordered factors assign that set to
 * the consecutive labels in the constant d-block.  Hence
 *
 *   A332748(n) = [t^n*x_0*...*x_(m-1)]
 *                product_(d=1)^floor((m-1)/2) 1/(1-t*Q_d(x)).       (1)
 *
 * Boolean Fourier extraction gives
 *
 *   2^(-m) sum_(epsilon_i in {+1,-1}) (product_i epsilon_i)
 *       [t^n] product_d 1/(1-t*Q_d(epsilon)).                       (2)
 *
 * A term of t-degree n in (1) has total x-degree 3*n=m.  The sign sum in
 * (2) retains monomials whose exponent of every x_i is positive and odd.
 * Their total degree is already the minimum possible m, so every exponent
 * must be exactly one.  Thus (2) extracts precisely the squarefree monomial
 * in (1), with no contamination from higher powers.
 *
 * Global sign negation changes every cubic Q_d by -1, and therefore changes
 * its t^n coefficient by (-1)^n.  It also changes product_i epsilon_i by
 * (-1)^m=(-1)^(3*n)=(-1)^n.  The two signs cancel.  We may consequently fix
 * epsilon_0=+1 and divide by 2^(m-1).  Reversal, followed if necessary by
 * global negation to restore epsilon_0=+1, is another exact symmetry and
 * reduces the evaluated representatives by almost two.
 *
 * Each geometric factor is multiplied by the exact ascending recurrence
 * new[j]=old[j]+Q_d*new[j-1], truncated at degree n.  Arithmetic is performed
 * modulo one, two, or three pairwise-coprime odd Mersenne numbers.  CRT lanes
 * are selected automatically until their product M strictly exceeds
 *
 *     B(n)=(3*n)!/6^n,
 *
 * the total number of permutations of the tripled multiset.  Since
 * 0<=A332748(n)<=B(n)<M, ordinary CRT reconstructs one deterministic exact
 * integer; no probabilistic step is used.  The three configured moduli cover
 * n<=17, while B(18) exceeds their product.  Primality is unnecessary:
 * oddness permits exact division by powers of two, and pairwise coprimality
 * is sufficient for CRT.  The --check self-test verifies all wide arithmetic
 * paths and cutoff assumptions.  Because every modulus is 2^s-1,
 * multiplication by the small |Q_d| is reduced exactly by one Mersenne fold,
 * without division or a second wide product in the hot DP loop.  Invalid
 * distances are omitted and their fixed bit masks are prepared per worker.
 * pthread workers use a dynamic atomic schedule.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic -pthread \
 *       332748_01.c -o 332748_01
 *
 * Usage:
 *   ./332748_01 8 --threads 8
 *   ./332748_01 --term 12 --threads 8
 *   ./332748_01 --check --threads 8 --no-bfile
 *
 * Completed terms are atomically recorded in b332748_01.txt by default.
 * The b-file may be sparse and contains only terms actually computed by this
 * program.  Use --output FILE to select another file or --no-bfile to disable
 * recording.
 */

#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#if !defined(__SIZEOF_INT128__)
#error "332748_01.c requires unsigned __int128"
#endif

__extension__ typedef unsigned __int128 U128;
__extension__ typedef __int128 I128;

#define MAX_N 17
#define DEFAULT_N 7
#define CHECK_N 9
#define MAX_THREADS 64
#define MAX_LANES 3
#define CHUNK_BITS 18
#define MAX_POSITIONS (3 * MAX_N)

_Static_assert(MAX_POSITIONS < 64,
               "sign masks require fewer than 64 positions");
_Static_assert(CHUNK_BITS > 0 && CHUNK_BITS <= 57,
               "chunk size must not overflow the 64-bit atomic schedule");

typedef struct {
    uint64_t limb[4];
} U256;

static const uint64_t moduli[MAX_LANES] = {
    (UINT64_C(1) << 61) - 1,
    (UINT64_C(1) << 59) - 1,
    (UINT64_C(1) << 57) - 1
};

static const char *const known[] = {
    "1", "1", "4", "18", "124", "738", "7464", "55890",
    "668778", "7030210", "90713844", "1054221258", "18597735744",
    "242795838520"
};

static const char *output_path = "b332748_01.txt";
static bool write_bfile = true;
static int requested_threads = 4;

static _Noreturn void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static double now_seconds(void)
{
    struct timespec value;
    if (clock_gettime(CLOCK_MONOTONIC, &value) != 0) {
        die("clock_gettime failed");
    }
    return (double)value.tv_sec + (double)value.tv_nsec / 1e9;
}

static int parse_integer(const char *text, const char *name,
                         int minimum, int maximum)
{
    char *end = NULL;
    errno = 0;
    const long value = strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' ||
        value < minimum || value > maximum) {
        fprintf(stderr, "error: %s must be in %d..%d: %s\n",
                name, minimum, maximum, text);
        exit(EXIT_FAILURE);
    }
    return (int)value;
}

static U256 u256_from_u64(uint64_t value)
{
    U256 result = {{value, 0, 0, 0}};
    return result;
}

static bool u256_is_zero(U256 value)
{
    return (value.limb[0] | value.limb[1] |
            value.limb[2] | value.limb[3]) == 0;
}

static int u256_compare(U256 left, U256 right)
{
    for (int index = 3; index >= 0; --index) {
        if (left.limb[index] < right.limb[index]) {
            return -1;
        }
        if (left.limb[index] > right.limb[index]) {
            return 1;
        }
    }
    return 0;
}

static bool u256_equal(U256 left, U256 right)
{
    return u256_compare(left, right) == 0;
}

static bool u256_add(U256 *destination, U256 addend)
{
    uint64_t carry = 0;
    for (unsigned index = 0; index < 4; ++index) {
        const uint64_t old = destination->limb[index];
        uint64_t sum = old + addend.limb[index];
        const uint64_t carry1 = sum < old;
        const uint64_t before = sum;
        sum += carry;
        const uint64_t carry2 = sum < before;
        destination->limb[index] = sum;
        carry = carry1 | carry2;
    }
    return carry == 0;
}

static bool u256_add_u64(U256 *destination, uint64_t addend)
{
    U256 value = u256_from_u64(addend);
    return u256_add(destination, value);
}

static bool u256_multiply_u64(U256 *value, uint64_t factor)
{
    uint64_t carry = 0;
    for (unsigned index = 0; index < 4; ++index) {
        const U128 product = (U128)value->limb[index] * factor + carry;
        value->limb[index] = (uint64_t)product;
        carry = (uint64_t)(product >> 64);
    }
    return carry == 0;
}

static bool u256_subtract_u64(U256 *value, uint64_t subtrahend)
{
    const uint64_t old = value->limb[0];
    value->limb[0] -= subtrahend;
    uint64_t borrow = old < subtrahend;
    for (unsigned index = 1; index < 4 && borrow != 0; ++index) {
        const uint64_t before = value->limb[index];
        --value->limb[index];
        borrow = before == 0;
    }
    return borrow == 0;
}

static uint64_t u256_divide_small(U256 *value, uint64_t divisor)
{
    uint64_t remainder = 0;
    for (int index = 3; index >= 0; --index) {
        const U128 current = ((U128)remainder << 64) | value->limb[index];
        value->limb[index] = (uint64_t)(current / divisor);
        remainder = (uint64_t)(current % divisor);
    }
    return remainder;
}

static uint64_t u256_mod_u64(U256 value, uint64_t modulus)
{
    uint64_t remainder = 0;
    for (int index = 3; index >= 0; --index) {
        const U128 current = ((U128)remainder << 64) | value.limb[index];
        remainder = (uint64_t)(current % modulus);
    }
    return remainder;
}

static int print_u256(FILE *stream, U256 value)
{
    char digits[80];
    size_t length = 0;
    do {
        digits[length++] = (char)('0' + u256_divide_small(&value, 10));
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
        if (*text < '0' || *text > '9' ||
            !u256_multiply_u64(&value, 10) ||
            !u256_add_u64(&value, (unsigned)(*text - '0'))) {
            return false;
        }
    }
    *result = value;
    return true;
}

static U256 multiset_permutation_bound(int n)
{
    /*
     * B(n)=(3*n)!/6^n and
     *
     *   B(j)/B(j-1)=(3*j)*(3*j-1)*(3*j-2)/6
     *                =j*(3*j-1)*(3*j-2)/2.
     *
     * One of the last two factors is even, so division by two is exact.
     * We still check the remainder, every wide multiplication, and the
     * startup thresholds that make MAX_N a rigorous cutoff.
     */
    U256 result = u256_from_u64(1);
    for (unsigned index = 1; index <= (unsigned)n; ++index) {
        if (!u256_multiply_u64(&result, index) ||
            !u256_multiply_u64(&result, 3 * index - 1) ||
            !u256_multiply_u64(&result, 3 * index - 2)) {
            die("multiset-permutation upper bound overflow");
        }
        if (u256_divide_small(&result, 2) != 0) {
            die("multiset-permutation bound division is not exact");
        }
    }
    return result;
}

static U256 reconstruction_bound(int n)
{
    return multiset_permutation_bound(n);
}

static int acquire_bfile_lock(void)
{
    const char suffix[] = ".lock";
    const size_t length = strlen(output_path);
    if (length > SIZE_MAX - sizeof(suffix)) {
        die("b-file lock path is too long");
    }
    char *path = malloc(length + sizeof(suffix));
    if (path == NULL) {
        die("cannot allocate b-file lock path");
    }
    memcpy(path, output_path, length);
    memcpy(path + length, suffix, sizeof(suffix));
    const int fd = open(path, O_RDWR | O_CREAT, 0666);
    free(path);
    if (fd < 0) {
        die("cannot open b-file lock");
    }
    struct flock lock = {
        .l_type = F_WRLCK, .l_whence = SEEK_SET, .l_start = 0, .l_len = 0
    };
    int result;
    do {
        result = fcntl(fd, F_SETLKW, &lock);
    } while (result != 0 && errno == EINTR);
    if (result != 0) {
        close(fd);
        die("cannot lock b-file");
    }
    return fd;
}

static void release_bfile_lock(int fd)
{
    struct flock lock = {
        .l_type = F_UNLCK, .l_whence = SEEK_SET, .l_start = 0, .l_len = 0
    };
    const bool unlock_failed = fcntl(fd, F_SETLK, &lock) != 0;
    const bool close_failed = close(fd) != 0;
    if (unlock_failed || close_failed) {
        die("cannot release b-file lock");
    }
}

static void sync_parent_directory(const char *path)
{
    const char *slash = strrchr(path, '/');
    char *allocated = NULL;
    const char *directory;
    if (slash == NULL) {
        directory = ".";
    } else if (slash == path) {
        directory = "/";
    } else {
        const size_t length = (size_t)(slash - path);
        allocated = malloc(length + 1);
        if (allocated == NULL) {
            die("cannot allocate b-file directory path");
        }
        memcpy(allocated, path, length);
        allocated[length] = '\0';
        directory = allocated;
    }
    const int fd = open(directory, O_RDONLY);
    free(allocated);
    if (fd < 0) {
        die("cannot open b-file parent directory");
    }
    const int result = fsync(fd);
    const int saved_errno = errno;
    const bool close_failed = close(fd) != 0;
    if ((result != 0 && saved_errno != EINVAL && saved_errno != ENOTSUP) ||
        close_failed) {
        die("cannot synchronize b-file parent directory");
    }
}

static void store_bfile_term(int n, U256 value)
{
    const int lock_fd = acquire_bfile_lock();
    U256 values[MAX_N + 1] = {{0}};
    bool present[MAX_N + 1] = {false};
    int previous = -1;
    mode_t mode = 0644;
    struct stat metadata;
    if (stat(output_path, &metadata) == 0) {
        mode = metadata.st_mode & 0777;
    } else if (errno != ENOENT) {
        die("cannot inspect b-file");
    }

    FILE *input = fopen(output_path, "r");
    if (input == NULL && errno != ENOENT) {
        die("cannot open existing b-file");
    }
    if (input != NULL) {
        char line[160];
        while (fgets(line, sizeof(line), input) != NULL) {
            int index;
            char number[96];
            char extra;
            if (sscanf(line, "%d %95s %c", &index, number, &extra) != 2 ||
                index < 0 || index > MAX_N || index <= previous ||
                !parse_u256(number, &values[index])) {
                fclose(input);
                die("existing b-file is malformed or not strictly ordered");
            }
            present[index] = true;
            previous = index;
        }
        if (ferror(input) || fclose(input) != 0) {
            die("cannot read existing b-file");
        }
    }
    if (present[n]) {
        if (!u256_equal(values[n], value)) {
            die("computed term disagrees with existing b-file");
        }
        release_bfile_lock(lock_fd);
        return;
    }
    values[n] = value;
    present[n] = true;

    const char suffix[] = ".tmp.XXXXXX";
    const size_t length = strlen(output_path);
    if (length > SIZE_MAX - sizeof(suffix)) {
        die("b-file path is too long");
    }
    char *temporary = malloc(length + sizeof(suffix));
    if (temporary == NULL) {
        die("cannot allocate temporary b-file path");
    }
    memcpy(temporary, output_path, length);
    memcpy(temporary + length, suffix, sizeof(suffix));
    const int fd = mkstemp(temporary);
    if (fd < 0) {
        free(temporary);
        die("cannot create temporary b-file");
    }
    if (fchmod(fd, mode) != 0) {
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
    for (int index = 0; index <= MAX_N; ++index) {
        if (!present[index]) {
            continue;
        }
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
    sync_parent_directory(output_path);
    free(temporary);
    release_bfile_lock(lock_fd);
    fprintf(stderr, "332748_01: recorded computed term n=%d in %s\n",
            n, output_path);
}

static uint64_t gray_code(uint64_t index)
{
    return index ^ (index >> 1);
}

static uint64_t normalized_reflection(uint64_t negative,
                                      unsigned positions)
{
#if defined(__has_builtin)
#if __has_builtin(__builtin_bitreverse64)
    uint64_t reflected =
        __builtin_bitreverse64(negative) >> (64U - positions);
#else
    uint64_t reflected = 0;
    for (unsigned position = 0; position < positions; ++position) {
        reflected = (reflected << 1) | ((negative >> position) & 1U);
    }
#endif
#else
    uint64_t reflected = 0;
    for (unsigned position = 0; position < positions; ++position) {
        reflected = (reflected << 1) | ((negative >> position) & 1U);
    }
#endif
    if ((reflected & 1U) != 0) {
        reflected ^= (UINT64_C(1) << positions) - 1;
    }
    return reflected;
}

static void correlations(unsigned positions, uint64_t negative,
                         const uint64_t masks[MAX_POSITIONS],
                         int edges[MAX_POSITIONS])
{
    const unsigned maximum_distance = (positions - 1U) / 2U;
    for (unsigned distance = 1;
         distance <= maximum_distance; ++distance) {
        /*
         * A term epsilon_i*epsilon_(i+d)*epsilon_(i+2*d) is negative iff
         * the xor of its three sign bits is one.  Popcount therefore gives
         * all negative summands of the cubic correlation Q_d(epsilon).
         */
        const uint64_t differing = negative ^ (negative >> distance) ^
                                   (negative >> (2U * distance));
        const unsigned disagreements =
            (unsigned)__builtin_popcountll(differing & masks[distance]);
        edges[distance] = (int)(positions - 2U * distance) -
                          2 * (int)disagreements;
    }
}

static void verify_one_correlation(unsigned positions, uint64_t negative,
                                   const uint64_t masks[MAX_POSITIONS])
{
    int edges[MAX_POSITIONS];
    correlations(positions, negative, masks, edges);
    for (unsigned distance = 1;
         2U * distance < positions; ++distance) {
        int expected = 0;
        for (unsigned start = 0;
             start + 2U * distance < positions; ++start) {
            const unsigned parity = (unsigned)(
                ((negative >> start) ^
                 (negative >> (start + distance)) ^
                 (negative >> (start + 2U * distance))) & 1U);
            expected += parity != 0 ? -1 : 1;
        }
        const int terms = (int)(positions - 2U * distance);
        if (edges[distance] != expected ||
            edges[distance] < -terms || edges[distance] > terms ||
            ((edges[distance] + terms) & 1) != 0) {
            die("cubic correlation self-test failed");
        }
    }
    const uint64_t reflected = normalized_reflection(negative, positions);
    if ((reflected & 1U) != 0 ||
        normalized_reflection(reflected, positions) != negative) {
        die("normalized reflection self-test failed");
    }
}

static void verify_correlation_kernel(void)
{
    uint64_t masks[MAX_POSITIONS] = {0};
    for (unsigned n = 1; n <= 4; ++n) {
        const unsigned positions = 3U * n;
        for (unsigned distance = 1;
             2U * distance < positions; ++distance) {
            masks[distance] =
                (UINT64_C(1) << (positions - 2U * distance)) - 1;
        }
        const uint64_t assignments = UINT64_C(1) << (positions - 1);
        for (uint64_t index = 0; index < assignments; ++index) {
            verify_one_correlation(positions, index << 1, masks);
        }
    }

    /* Also exercise the largest configured shifts on deterministic samples. */
    const unsigned positions = MAX_POSITIONS;
    for (unsigned distance = 1;
         2U * distance < positions; ++distance) {
        masks[distance] =
            (UINT64_C(1) << (positions - 2U * distance)) - 1;
    }
    const uint64_t position_mask =
        (UINT64_C(1) << positions) - 1;
    uint64_t state = UINT64_C(0x243f6a8885a308d3);
    for (unsigned sample = 0; sample < 1000; ++sample) {
        state = state * UINT64_C(6364136223846793005) + 1;
        verify_one_correlation(positions,
                               state & position_mask & ~UINT64_C(1),
                               masks);
    }
}

static uint64_t mod_add(uint64_t left, uint64_t right, uint64_t modulus)
{
    const uint64_t sum = left + right;
    return sum >= modulus ? sum - modulus : sum;
}

static uint64_t mod_sub(uint64_t left, uint64_t right, uint64_t modulus)
{
    return left >= right ? left - right : modulus - (right - left);
}

static uint64_t gcd_u64(uint64_t left, uint64_t right)
{
    while (right != 0) {
        const uint64_t remainder = left % right;
        left = right;
        right = remainder;
    }
    return left;
}

static unsigned checked_mersenne_exponent(uint64_t modulus)
{
    if (modulus == UINT64_MAX) {
        die("Mersenne modulus exponent is outside implementation limits");
    }
    const uint64_t power = modulus + 1;
    if (power == 0 || (power & (power - 1)) != 0) {
        die("configured modulus is not a Mersenne number");
    }
    const unsigned exponent = (unsigned)__builtin_ctzll(power);
    if (exponent == 0 || exponent >= 63 ||
        MAX_POSITIONS >= modulus ||
        modulus > UINT64_MAX - MAX_POSITIONS) {
        die("Mersenne small-product fold can overflow");
    }
    return exponent;
}

typedef struct {
    uint64_t modulus;
    unsigned mersenne_exponent;
} Modulus;

static uint64_t mod_multiply_small(uint64_t value, unsigned multiplier,
                                   const Modulus *modulus)
{
    if (multiplier == 0) {
        return 0;
    }
    if (multiplier == 1) {
        return value;
    }
    if (multiplier == 2) {
        return mod_add(value, value, modulus->modulus);
    }
    if (multiplier == 3) {
        return mod_add(mod_add(value, value, modulus->modulus),
                       value, modulus->modulus);
    }
    if (multiplier == 4) {
        const uint64_t twice = mod_add(value, value, modulus->modulus);
        return mod_add(twice, twice, modulus->modulus);
    }
    /*
     * For p=2^s-1, 2^s == 1 (mod p).  Thus, for z=value*multiplier,
     *
     *   z mod p = ((z & p) + (z >> s)) mod p.
     *
     * Since value<p and multiplier<=MAX_POSITIONS=51, the folded value is
     * below p+51; it fits in uint64_t and one subtraction is sufficient.
     * U128 makes the single product exact.
     */
    const U128 product = (U128)value * multiplier;
    const uint64_t low = (uint64_t)product;
    const uint64_t high = (uint64_t)(product >> 64);
    const unsigned exponent = modulus->mersenne_exponent;
    const uint64_t upper =
        (low >> exponent) | (high << (64U - exponent));
    const uint64_t folded = (low & modulus->modulus) + upper;
    if (folded >= modulus->modulus) {
        return folded - modulus->modulus;
    }
    return folded;
}

static uint64_t coefficient_one(int n, unsigned positions,
                                const int edges[MAX_POSITIONS],
                                const Modulus *modulus)
{
    uint64_t coefficient[MAX_N + 1] = {0};
    coefficient[0] = 1;
    for (unsigned distance = 1; 2U * distance < positions; ++distance) {
        const int edge = edges[distance];
        if (edge == 0) {
            continue;
        }
        const unsigned magnitude = (unsigned)(edge < 0 ? -edge : edge);
        /* Ascending order multiplies by 1/(1-edge*t), truncated at t^n. */
        for (int degree = 1; degree <= n; ++degree) {
            const uint64_t product = magnitude == 1 ?
                coefficient[degree - 1] : mod_multiply_small(
                    coefficient[degree - 1], magnitude, modulus);
            coefficient[degree] = edge < 0 ?
                mod_sub(coefficient[degree], product, modulus->modulus) :
                mod_add(coefficient[degree], product, modulus->modulus);
        }
    }
    return coefficient[n];
}

static void coefficient_two(int n, unsigned positions,
                            const int edges[MAX_POSITIONS],
                            const Modulus modulus[2], uint64_t result[2])
{
    /* Fixed lanes let the compiler remove the hot runtime lane loop. */
    uint64_t coefficient[2][MAX_N + 1] = {{0}};
    coefficient[0][0] = 1;
    coefficient[1][0] = 1;
    for (unsigned distance = 1; 2U * distance < positions; ++distance) {
        const int edge = edges[distance];
        if (edge == 0) {
            continue;
        }
        const unsigned magnitude = (unsigned)(edge < 0 ? -edge : edge);
        for (int degree = 1; degree <= n; ++degree) {
            const uint64_t product0 = magnitude == 1 ?
                coefficient[0][degree - 1] : mod_multiply_small(
                    coefficient[0][degree - 1], magnitude, &modulus[0]);
            const uint64_t product1 = magnitude == 1 ?
                coefficient[1][degree - 1] : mod_multiply_small(
                    coefficient[1][degree - 1], magnitude, &modulus[1]);
            if (edge < 0) {
                coefficient[0][degree] = mod_sub(
                    coefficient[0][degree], product0, modulus[0].modulus);
                coefficient[1][degree] = mod_sub(
                    coefficient[1][degree], product1, modulus[1].modulus);
            } else {
                coefficient[0][degree] = mod_add(
                    coefficient[0][degree], product0, modulus[0].modulus);
                coefficient[1][degree] = mod_add(
                    coefficient[1][degree], product1, modulus[1].modulus);
            }
        }
    }
    result[0] = coefficient[0][n];
    result[1] = coefficient[1][n];
}

static void coefficient_many(int n, unsigned positions,
                             const int edges[MAX_POSITIONS],
                             unsigned lanes,
                             const Modulus modulus[MAX_LANES],
                             uint64_t result[MAX_LANES])
{
    uint64_t coefficient[MAX_N + 1][MAX_LANES] = {{0}};
    for (unsigned lane = 0; lane < lanes; ++lane) {
        coefficient[0][lane] = 1;
    }
    for (unsigned distance = 1; 2U * distance < positions; ++distance) {
        const int edge = edges[distance];
        if (edge == 0) {
            continue;
        }
        const unsigned magnitude = (unsigned)(edge < 0 ? -edge : edge);
        for (int degree = 1; degree <= n; ++degree) {
            for (unsigned lane = 0; lane < lanes; ++lane) {
                const uint64_t product = magnitude == 1 ?
                    coefficient[degree - 1][lane] : mod_multiply_small(
                        coefficient[degree - 1][lane], magnitude,
                        &modulus[lane]);
                coefficient[degree][lane] = edge < 0 ?
                    mod_sub(coefficient[degree][lane], product,
                            modulus[lane].modulus) :
                    mod_add(coefficient[degree][lane], product,
                            modulus[lane].modulus);
            }
        }
    }
    for (unsigned lane = 0; lane < lanes; ++lane) {
        result[lane] = coefficient[n][lane];
    }
}

typedef struct {
    _Atomic uint64_t next;
    uint64_t end;
    uint64_t chunk;
} Schedule;

typedef struct {
    int n;
    unsigned positions;
    unsigned lanes;
    Modulus modulus[MAX_LANES];
    uint64_t correlation_mask[MAX_POSITIONS];
    Schedule *schedule;
    uint64_t sum[MAX_LANES];
} Worker;

static void *worker_main(void *argument)
{
    Worker *worker = argument;
    if (worker->lanes < 1 || worker->lanes > MAX_LANES) {
        die("worker modulus lane count is outside implementation limits");
    }
    for (;;) {
        const uint64_t begin = atomic_fetch_add_explicit(
            &worker->schedule->next, worker->schedule->chunk,
            memory_order_relaxed);
        if (begin >= worker->schedule->end) {
            break;
        }
        uint64_t end = begin + worker->schedule->chunk;
        if (end > worker->schedule->end) {
            end = worker->schedule->end;
        }
        for (uint64_t index = begin; index < end; ++index) {
            const uint64_t signs = gray_code(index) << 1;
            const uint64_t reflected =
                normalized_reflection(signs, worker->positions);
            if (signs > reflected) {
                continue;
            }
            const unsigned orbit_weight = signs == reflected ? 1U : 2U;
            /* Zero initialization also makes future distance-loop changes safe. */
            int edges[MAX_POSITIONS] = {0};
            correlations(worker->positions, signs,
                         worker->correlation_mask, edges);
            uint64_t term[MAX_LANES] = {0, 0, 0};
            if (worker->lanes == 1) {
                term[0] = coefficient_one(worker->n, worker->positions,
                                          edges, &worker->modulus[0]);
            } else if (worker->lanes == 2) {
                coefficient_two(worker->n, worker->positions, edges,
                                worker->modulus, term);
            } else {
                coefficient_many(worker->n, worker->positions, edges,
                                 worker->lanes, worker->modulus, term);
            }
            for (unsigned lane = 0; lane < worker->lanes; ++lane) {
                const uint64_t modulus = worker->modulus[lane].modulus;
                if (orbit_weight == 2U) {
                    term[lane] = mod_add(term[lane], term[lane], modulus);
                }
                /*
                 * popcount(gray(index)) mod 2 equals index mod 2.  Bit zero
                 * of signs is fixed at zero, so index&1 is precisely the
                 * Fourier sign product_i epsilon_i.
                 */
                worker->sum[lane] = (index & 1) != 0 ?
                    mod_sub(worker->sum[lane], term[lane], modulus) :
                    mod_add(worker->sum[lane], term[lane], modulus);
            }
        }
    }
    return NULL;
}

static unsigned select_lanes(U256 bound, U256 *product)
{
    *product = u256_from_u64(1);
    unsigned lanes = 0;
    while (u256_compare(*product, bound) <= 0 && lanes < MAX_LANES) {
        if (!u256_multiply_u64(product, moduli[lanes])) {
            die("CRT modulus product overflow");
        }
        ++lanes;
    }
    if (u256_compare(*product, bound) <= 0) {
        die("available CRT moduli do not exceed the rigorous upper bound");
    }
    return lanes;
}

static void modular_passes(int n, int threads, unsigned lanes,
                           uint64_t residues[MAX_LANES])
{
    const unsigned positions = 3U * (unsigned)n;
    if (lanes < 1 || lanes > MAX_LANES || positions == 0 ||
        positions > MAX_POSITIONS || positions >= 64) {
        die("modular dimensions are outside implementation limits");
    }
    for (unsigned lane = 0; lane < lanes; ++lane) {
        if (moduli[lane] <= 1 || (moduli[lane] & 1) == 0 ||
            moduli[lane] >= (UINT64_C(1) << 63)) {
            die("modulus violates Mersenne arithmetic preconditions");
        }
        (void)checked_mersenne_exponent(moduli[lane]);
        for (unsigned other = 0; other < lane; ++other) {
            if (gcd_u64(moduli[lane], moduli[other]) != 1) {
                die("CRT moduli are not pairwise coprime");
            }
        }
    }
    const uint64_t evaluations = UINT64_C(1) << (positions - 1);
    if ((uint64_t)threads > evaluations) {
        threads = (int)evaluations;
    }
    Worker *workers = calloc((size_t)threads, sizeof(*workers));
    pthread_t *ids = calloc((size_t)threads, sizeof(*ids));
    if (workers == NULL || ids == NULL) {
        free(workers);
        free(ids);
        die("cannot allocate modular workers");
    }
    Schedule schedule = {
        .next = 0,
        .end = evaluations,
        .chunk = UINT64_C(1) << CHUNK_BITS
    };
    for (int id = 0; id < threads; ++id) {
        workers[id].n = n;
        workers[id].positions = positions;
        workers[id].lanes = lanes;
        workers[id].schedule = &schedule;
        for (unsigned distance = 1;
             2U * distance < positions; ++distance) {
            workers[id].correlation_mask[distance] =
                (UINT64_C(1) << (positions - 2U * distance)) - 1;
        }
        for (unsigned lane = 0; lane < lanes; ++lane) {
            workers[id].modulus[lane].modulus = moduli[lane];
            workers[id].modulus[lane].mersenne_exponent =
                checked_mersenne_exponent(moduli[lane]);
        }
        const int error = pthread_create(&ids[id], NULL,
                                         worker_main, &workers[id]);
        if (error != 0) {
            fprintf(stderr, "error: pthread_create: %s\n", strerror(error));
            exit(EXIT_FAILURE);
        }
    }
    for (unsigned lane = 0; lane < MAX_LANES; ++lane) {
        residues[lane] = 0;
    }
    for (int id = 0; id < threads; ++id) {
        const int error = pthread_join(ids[id], NULL);
        if (error != 0) {
            fprintf(stderr, "error: pthread_join: %s\n", strerror(error));
            exit(EXIT_FAILURE);
        }
        for (unsigned lane = 0; lane < lanes; ++lane) {
            residues[lane] = mod_add(residues[lane], workers[id].sum[lane],
                                     moduli[lane]);
        }
    }
    free(workers);
    free(ids);

    /* Apply the inverse of 2^(3*n-1) in each odd modulus. */
    for (unsigned lane = 0; lane < lanes; ++lane) {
        for (unsigned bit = 0; bit < positions - 1; ++bit) {
            if ((residues[lane] & 1U) != 0) {
                residues[lane] += moduli[lane];
            }
            residues[lane] /= 2;
        }
    }
}

static uint64_t inverse_mod(uint64_t value, uint64_t modulus)
{
    I128 old_r = value, r = modulus;
    I128 old_s = 1, s = 0;
    while (r != 0) {
        const I128 quotient = old_r / r;
        const I128 next_r = old_r - quotient * r;
        const I128 next_s = old_s - quotient * s;
        old_r = r;
        r = next_r;
        old_s = s;
        s = next_s;
    }
    if (old_r != 1) {
        die("CRT moduli are not coprime");
    }
    old_s %= modulus;
    if (old_s < 0) {
        old_s += modulus;
    }
    return (uint64_t)old_s;
}

static U256 reconstruct(const uint64_t residues[MAX_LANES], unsigned lanes)
{
    if (lanes < 1 || lanes > MAX_LANES) {
        die("invalid CRT reconstruction lane count");
    }
    U256 value = u256_from_u64(residues[0]);
    U256 product = u256_from_u64(moduli[0]);
    for (unsigned lane = 1; lane < lanes; ++lane) {
        const uint64_t modulus = moduli[lane];
        const uint64_t difference = mod_sub(
            residues[lane], u256_mod_u64(value, modulus), modulus);
        const uint64_t inverse = inverse_mod(
            u256_mod_u64(product, modulus), modulus);
        const uint64_t multiplier = (uint64_t)(
            ((U128)difference * inverse) % modulus);
        U256 increment = product;
        if (!u256_multiply_u64(&increment, multiplier) ||
            !u256_add(&value, increment) ||
            !u256_multiply_u64(&product, modulus)) {
            die("CRT reconstruction overflow");
        }
    }
    return value;
}

static U256 a332748(int n)
{
    if (n == 0) {
        return u256_from_u64(1);
    }
    const U256 bound = reconstruction_bound(n);
    U256 modulus_product;
    const unsigned lanes = select_lanes(bound, &modulus_product);
    uint64_t residues[MAX_LANES];
    const double started = now_seconds();
    modular_passes(n, requested_threads, lanes, residues);
    const U256 answer = reconstruct(residues, lanes);
    if (u256_compare(answer, bound) > 0) {
        die("CRT result exceeds total tripled-multiset permutations");
    }
    const uint64_t evaluations = UINT64_C(1) << (3U * (unsigned)n - 1);
    int threads = requested_threads;
    if ((uint64_t)threads > evaluations) {
        threads = (int)evaluations;
    }
    fprintf(stderr,
            "332748_01: n=%d, algebraic CRT, evaluations=%" PRIu64
            ", moduli=%u, threads=%d, %.3f s\n",
            n, evaluations, lanes, threads, now_seconds() - started);
    return answer;
}

static void verify_known(int n, U256 value)
{
    const unsigned count = (unsigned)(sizeof(known) / sizeof(known[0]));
    if ((unsigned)n >= count) {
        return;
    }
    U256 expected;
    if (!parse_u256(known[n], &expected) || !u256_equal(value, expected)) {
        fprintf(stderr, "error: A332748 mismatch at n=%d: got ", n);
        print_u256(stderr, value);
        fprintf(stderr, ", expected %s\n", known[n]);
        exit(EXIT_FAILURE);
    }
}

static void verify_constants_and_paths(void)
{
    for (unsigned lane = 0; lane < MAX_LANES; ++lane) {
        if (moduli[lane] <= 1 || (moduli[lane] & 1) == 0 ||
            moduli[lane] >= (UINT64_C(1) << 63)) {
            die("configured modulus violates Mersenne preconditions");
        }
        for (unsigned other = 0; other < lane; ++other) {
            if (gcd_u64(moduli[lane], moduli[other]) != 1) {
                die("configured CRT moduli are not pairwise coprime");
            }
        }
        Modulus test = {
            .modulus = moduli[lane],
            .mersenne_exponent = checked_mersenne_exponent(moduli[lane])
        };
        uint64_t state = UINT64_C(0x9e3779b97f4a7c15) ^ moduli[lane];
        for (unsigned multiplier = 0;
             multiplier <= MAX_POSITIONS; ++multiplier) {
            for (unsigned sample_index = 0;
                 sample_index < 1000; ++sample_index) {
                state = state * UINT64_C(6364136223846793005) + 1;
                const uint64_t sample = state % moduli[lane];
                const uint64_t expected = (uint64_t)(
                    ((U128)sample * multiplier) % moduli[lane]);
                if (mod_multiply_small(sample, multiplier, &test) !=
                    expected) {
                    die("Mersenne multiplication self-test failed");
                }
            }
            const uint64_t endpoint = moduli[lane] - 1;
            const uint64_t endpoint_expected = (uint64_t)(
                ((U128)endpoint * multiplier) % moduli[lane]);
            if (mod_multiply_small(endpoint, multiplier, &test) !=
                endpoint_expected) {
                die("Mersenne endpoint multiplication self-test failed");
            }
        }
    }

    U256 product1 = u256_from_u64(moduli[0]);
    U256 product2 = product1;
    U256 product3;
    if (!u256_multiply_u64(&product2, moduli[1])) {
        die("CRT product self-test overflow");
    }
    product3 = product2;
    if (!u256_multiply_u64(&product3, moduli[2]) ||
        !(u256_compare(multiset_permutation_bound(8), product1) < 0 &&
          u256_compare(multiset_permutation_bound(9), product1) >= 0 &&
          u256_compare(multiset_permutation_bound(12), product2) < 0 &&
          u256_compare(multiset_permutation_bound(13), product2) >= 0 &&
          u256_compare(multiset_permutation_bound(MAX_N), product3) < 0 &&
          u256_compare(multiset_permutation_bound(MAX_N + 1), product3) >= 0)) {
        die("unexpected automatic CRT threshold");
    }

    U256 prefix = u256_from_u64(1);
    for (unsigned lanes = 1; lanes <= MAX_LANES; ++lanes) {
        if (!u256_multiply_u64(&prefix, moduli[lanes - 1])) {
            die("CRT prefix product overflow");
        }
        U256 sample = prefix;
        if (!u256_subtract_u64(&sample, 12345)) {
            die("CRT reconstruction sample underflow");
        }
        uint64_t residues[MAX_LANES] = {0, 0, 0};
        for (unsigned lane = 0; lane < lanes; ++lane) {
            residues[lane] = u256_mod_u64(sample, moduli[lane]);
        }
        if (!u256_equal(reconstruct(residues, lanes), sample)) {
            die("ordinary CRT reconstruction self-test failed");
        }
    }

    /* Exercise all hot DP lane counts on a known small term. */
    const int n = 5;
    for (unsigned lanes = 1; lanes <= MAX_LANES; ++lanes) {
        uint64_t residues[MAX_LANES];
        modular_passes(n, requested_threads, lanes, residues);
        verify_known(n, reconstruct(residues, lanes));
    }
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
    int maximum = -1;
    bool have_mode = false;
    bool have_threads = false;
    bool have_output = false;

    for (int index = 1; index < argc; ++index) {
        if (!strcmp(argv[index], "--help") || !strcmp(argv[index], "-h")) {
            usage(argv[0]);
            return EXIT_SUCCESS;
        } else if (!strcmp(argv[index], "--threads")) {
            if (have_threads || ++index >= argc) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            requested_threads = parse_integer(
                argv[index], "threads", 1, MAX_THREADS);
            have_threads = true;
        } else if (!strcmp(argv[index], "--output")) {
            if (have_output || ++index >= argc) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            output_path = argv[index];
            write_bfile = true;
            have_output = true;
        } else if (!strcmp(argv[index], "--no-bfile")) {
            if (have_output) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            write_bfile = false;
            have_output = true;
        } else if (!strcmp(argv[index], "--term")) {
            if (have_mode || ++index >= argc) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            mode = MODE_TERM;
            maximum = parse_integer(argv[index], "N", 0, MAX_N);
            have_mode = true;
        } else if (!strcmp(argv[index], "--check")) {
            if (have_mode) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            mode = MODE_CHECK;
            have_mode = true;
        } else if (argv[index][0] != '-' && !have_mode) {
            maximum = parse_integer(argv[index], "N", 0, MAX_N);
            mode = MODE_RANGE;
            have_mode = true;
        } else {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    /* Fast deterministic startup validation replaces hot per-state checks. */
    verify_correlation_kernel();

    if (maximum < 0) {
        maximum = DEFAULT_N;
    }
    if (mode == MODE_CHECK) {
        for (int n = 0; n <= CHECK_N; ++n) {
            verify_known(n, a332748(n));
        }
        verify_constants_and_paths();
        printf("ok: A332748 n=0..%d and all CRT paths verified\n", CHECK_N);
        return EXIT_SUCCESS;
    }
    if (mode == MODE_TERM) {
        const U256 value = a332748(maximum);
        verify_known(maximum, value);
        if (write_bfile) {
            store_bfile_term(maximum, value);
        }
        printf("%d ", maximum);
        print_u256(stdout, value);
        putchar('\n');
        return EXIT_SUCCESS;
    }
    for (int n = 0; n <= maximum; ++n) {
        const U256 value = a332748(n);
        verify_known(n, value);
        if (write_bfile) {
            store_bfile_term(n, value);
        }
        printf("%d ", n);
        print_u256(stdout, value);
        putchar('\n');
        fflush(stdout);
    }
    return EXIT_SUCCESS;
}
