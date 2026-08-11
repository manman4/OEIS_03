/*
 * A322178 -- algebraic coefficient extraction.
 *
 * For a word containing two copies of each label 1,...,n, let d_k=b(k)+1
 * be the distance between the two copies of k.  The condition is
 *
 *     d_1 >= d_2 >= ... >= d_n.
 *
 * Put m=2*n and, for each possible distance d, define
 *
 *     Q_d(x) = sum_(0 <= i < m-d) x_i*x_(i+d).
 *
 * An unlabelled pairing with r_d pairs of distance d admits r_d! assignments
 * to the consecutive labels having that distance.  In Q_d(x)^r_d every set
 * of r_d disjoint edges occurs exactly r_d! times.  Consequently
 *
 *   A322178(n) = [t^n*x_0*...*x_(m-1)]
 *                product_(d=1)^(m-1) 1/(1-t*Q_d(x)).
 *
 * For example, when n=2 the three unlabelled pairings have distance
 * multisets {1,1}, {2,2}, and {3,1}.  Their weights are respectively
 * 2!, 2!, and 1, giving A322178(2)=2+2+1=5.  The factorial weights are
 * precisely the assignments of equal-distance edges to the labels in the
 * corresponding constant block of d_1>=...>=d_n.
 *
 * Boolean Fourier extraction, followed by fixing epsilon_0=+1 using global
 * sign symmetry, gives
 *
 *   2^(-(m-1)) sum_epsilon (product_i epsilon_i)
 *       [t^n] product_d 1/(1-t*Q_d(epsilon)).
 *
 * The sign sum retains exactly those monomials in which every x_i has odd
 * exponent.  The coefficient of t^n is homogeneous of total x-degree
 * 2*n=m in the m variables x_0,...,x_(m-1).  Since m positive odd exponents
 * have sum at least m, equality forces every exponent to be exactly one.
 * Thus the sign sum extracts x_0*...*x_(m-1), not any unwanted higher-power
 * monomial.  Global negation preserves every Q_d; it also preserves the
 * product of the m signs because m is even, justifying the factor 2^(-(m-1)).
 *
 * The coefficient in t is computed by an ascending in-place geometric-factor
 * DP.  Reversal symmetry halves the sign representatives.  Zero Q_d factors
 * are identities.  The computation is performed modulo one, two, or three
 * pairwise-coprime odd Mersenne numbers, selected automatically so their
 * product strictly exceeds a rigorous upper bound.  The general bound is
 *
 *     (2*n)!/2^n = n!*(2*n-1)!!,
 *
 * the total number of permutations of the doubled multiset.  For n=18,19 a
 * sharper proved bound is used.  If no distance occurs more than 10 times,
 * the weight product_d r_d! is at most 10!*(n-10)!.  Pairings having at least
 * 11 equal-distance edges are bounded by choosing those 11 edges (temporarily
 * ignoring collisions) and pairing all remaining vertices.  Thus, with
 * T=(2*n-1)!! and H=sum_(L=11)^(2*n-1) C(L,11)*(2*(n-11)-1)!!,
 *
 *   A322178(n) <= T*10!*(n-10)! + H*(n!-10!*(n-10)!).
 *
 * This is below the 61/59-bit modulus product for both n=18 and n=19, so the
 * expensive third DP lane is unnecessary.  Ordinary CRT in a checked 256-bit
 * integer reconstructs the unique nonnegative answer.  Primality is not
 * required: oddness permits division by powers of two and pairwise
 * coprimality is sufficient for CRT.  Precomputed 64-bit Shoup constants
 * make multiplication by |Q_d| exact without division in the hot DP loop.
 * Small multipliers use exact modular addition chains.  Work is dynamically
 * distributed among pthread workers.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic -pthread \
 *       322178_01.c -o 322178_01
 *
 * Usage:
 *   ./322178_01 12
 *   ./322178_01 --term 19 --threads 8
 *   ./322178_01 --check --threads 8 --no-bfile
 *
 * Completed terms are atomically recorded in b322178_01.txt by default.
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
#error "322178_01.c requires unsigned __int128"
#endif

__extension__ typedef unsigned __int128 U128;
__extension__ typedef __int128 I128;

#define MAX_N 23
#define DEFAULT_N 9
#define CHECK_N 10
#define MAX_THREADS 64
#define MAX_LANES 3
#define CHUNK_BITS 18
#define MAX_POSITIONS (2 * MAX_N)

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
    "1", "1", "5", "33", "329", "3825", "57293", "977581",
    "19619645", "442155529", "11183272973", "312134648549",
    "9554405887621", "317670072938621", "11411690507968361",
    "440231352579839965", "18158142912306766989",
    "797183010461708801341", "37120529214984142435969"
};

static const char *output_path = "b322178_01.txt";
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
    /* (2*n)!/2^n obeys B(n)=B(n-1)*n*(2*n-1). */
    U256 result = u256_from_u64(1);
    for (unsigned index = 1; index <= (unsigned)n; ++index) {
        if (!u256_multiply_u64(&result, index) ||
            !u256_multiply_u64(&result, 2 * index - 1)) {
            die("multiset-permutation upper bound overflow");
        }
    }
    return result;
}

static uint64_t factorial_u64(unsigned n)
{
    uint64_t result = 1;
    for (unsigned value = 2; value <= n; ++value) {
        if (result > UINT64_MAX / value) {
            die("small factorial overflow");
        }
        result *= value;
    }
    return result;
}

static uint64_t binomial_u64(unsigned n, unsigned k)
{
    if (k > n) {
        return 0;
    }
    if (k > n - k) {
        k = n - k;
    }
    U128 result = 1;
    for (unsigned index = 1; index <= k; ++index) {
        result = result * (n - k + index) / index;
        if (result > UINT64_MAX) {
            die("small binomial coefficient overflow");
        }
    }
    return (uint64_t)result;
}

static U256 total_pairings_bound(int n)
{
    U256 result = u256_from_u64(1);
    for (unsigned odd = 1; odd < 2U * (unsigned)n; odd += 2) {
        if (!u256_multiply_u64(&result, odd)) {
            die("pairing upper bound overflow");
        }
    }
    return result;
}

static U256 repeated_distance_bound(int n)
{
    /*
     * This sharper bound is used only for n=18,19.  For a perfect matching
     * let r_d be the number of edges of distance d, so its A322178 weight is
     * product_d r_d!.  If every r_d<=10, merging factorial blocks shows that
     * this weight is at most W=10!*(n-10)!.
     *
     * Let H bound the number of matchings with some r_d>=11.  A distance with
     * L available edges has C(L,11) choices for 11 of them.  Ignoring edge
     * collisions can only increase the count, and the remaining 2*(n-11)
     * vertices have at most (2*(n-11)-1)!! pairings.  A union bound over all
     * distances therefore gives the H below.  Every matching has weight at
     * most n!, so T*W + H*(n!-W) is a rigorous upper bound.
     */
    if (n != 18 && n != 19) {
        die("sharper repeated-distance bound requested outside n=18,19");
    }
    const unsigned selected = 11;
    uint64_t remaining_pairings = 1;
    for (unsigned odd = 1;
         odd < 2U * ((unsigned)n - selected); odd += 2) {
        if (remaining_pairings > UINT64_MAX / odd) {
            die("remaining-pairing bound overflow");
        }
        remaining_pairings *= odd;
    }
    U128 high_count = 0;
    for (unsigned available = selected;
         available < 2U * (unsigned)n; ++available) {
        high_count += (U128)binomial_u64(available, selected) *
                      remaining_pairings;
    }
    if (high_count > UINT64_MAX) {
        die("high-multiplicity pairing bound overflow");
    }
    const uint64_t factorial_n = factorial_u64((unsigned)n);
    const uint64_t low_weight =
        factorial_u64(10) * factorial_u64((unsigned)n - 10);
    U256 result = total_pairings_bound(n);
    if (!u256_multiply_u64(&result, low_weight)) {
        die("low-multiplicity contribution bound overflow");
    }
    U256 correction = u256_from_u64((uint64_t)high_count);
    if (!u256_multiply_u64(&correction, factorial_n - low_weight) ||
        !u256_add(&result, correction)) {
        die("high-multiplicity contribution bound overflow");
    }
    return result;
}

static U256 reconstruction_bound(int n)
{
    return n == 18 || n == 19 ? repeated_distance_bound(n) :
                                multiset_permutation_bound(n);
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
    fprintf(stderr, "322178_01: recorded computed term n=%d in %s\n",
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
                         int edges[MAX_POSITIONS])
{
    for (unsigned distance = 1; distance < positions; ++distance) {
        const uint64_t differing = negative ^ (negative >> distance);
        const uint64_t mask =
            (UINT64_C(1) << (positions - distance)) - 1;
        const unsigned disagreements =
            (unsigned)__builtin_popcountll(differing & mask);
        edges[distance] = (int)(positions - distance) -
                          2 * (int)disagreements;
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

typedef struct {
    uint64_t modulus;
    uint64_t shoup[MAX_POSITIONS + 1];
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
     * q=floor(value*floor(multiplier*2^64/modulus)/2^64) is at most one
     * below the exact quotient.  The exact remainder is therefore below
     * 2*modulus and fits in uint64_t because modulus<2^63.  The two low
     * unsigned products may wrap, but the wrap cancels in their subtraction.
     */
    const uint64_t quotient = (uint64_t)(
        ((U128)value * modulus->shoup[multiplier]) >> 64);
    uint64_t remainder =
        value * multiplier - quotient * modulus->modulus;
    if (remainder >= modulus->modulus) {
        remainder -= modulus->modulus;
    }
    return remainder;
}

static uint64_t coefficient_one(int n, unsigned positions,
                                const int edges[MAX_POSITIONS],
                                const Modulus *modulus)
{
    uint64_t coefficient[MAX_N + 1] = {0};
    coefficient[0] = 1;
    for (unsigned distance = 1; distance < positions; ++distance) {
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
    for (unsigned distance = 1; distance < positions; ++distance) {
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
    for (unsigned distance = 1; distance < positions; ++distance) {
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
            int edges[MAX_POSITIONS];
            correlations(worker->positions, signs, edges);
            for (unsigned distance = 1;
                 distance < worker->positions; ++distance) {
                const int terms = (int)(worker->positions - distance);
                if (edges[distance] < -terms || edges[distance] > terms ||
                    ((edges[distance] + terms) & 1) != 0) {
                    die("correlation invariant failed");
                }
            }
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
    const unsigned positions = 2U * (unsigned)n;
    if (lanes < 1 || lanes > MAX_LANES || positions == 0 ||
        positions > MAX_POSITIONS || positions >= 64) {
        die("modular dimensions are outside implementation limits");
    }
    for (unsigned lane = 0; lane < lanes; ++lane) {
        if (moduli[lane] <= 1 || (moduli[lane] & 1) == 0 ||
            moduli[lane] >= (UINT64_C(1) << 63)) {
            die("modulus violates Shoup arithmetic preconditions");
        }
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
        for (unsigned lane = 0; lane < lanes; ++lane) {
            workers[id].modulus[lane].modulus = moduli[lane];
            for (unsigned multiplier = 0;
                 multiplier <= MAX_POSITIONS; ++multiplier) {
                workers[id].modulus[lane].shoup[multiplier] = (uint64_t)(
                    ((U128)multiplier << 64) / moduli[lane]);
            }
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

    /* Apply the inverse of 2^(2*n-1) in each odd modulus. */
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

static U256 a322178(int n)
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
        die("CRT result exceeds total doubled-multiset permutations");
    }
    const uint64_t evaluations = UINT64_C(1) << (2U * (unsigned)n - 1);
    int threads = requested_threads;
    if ((uint64_t)threads > evaluations) {
        threads = (int)evaluations;
    }
    fprintf(stderr,
            "322178_01: n=%d, algebraic CRT, evaluations=%" PRIu64
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
        fprintf(stderr, "error: A322178 mismatch at n=%d: got ", n);
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
            die("configured modulus violates Shoup preconditions");
        }
        for (unsigned other = 0; other < lane; ++other) {
            if (gcd_u64(moduli[lane], moduli[other]) != 1) {
                die("configured CRT moduli are not pairwise coprime");
            }
        }
        Modulus test = {.modulus = moduli[lane]};
        for (unsigned multiplier = 0;
             multiplier <= MAX_POSITIONS; ++multiplier) {
            test.shoup[multiplier] = (uint64_t)(
                ((U128)multiplier << 64) / moduli[lane]);
        }
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
                    die("Shoup multiplication self-test failed");
                }
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
    U256 expected18, expected19;
    if (!parse_u256("33512221839068180452468224000000", &expected18) ||
        !parse_u256("11466557255140264747179943833600000", &expected19) ||
        !u256_equal(repeated_distance_bound(18), expected18) ||
        !u256_equal(repeated_distance_bound(19), expected19)) {
        die("sharper repeated-distance bound self-test failed");
    }
    if (!u256_multiply_u64(&product3, moduli[2]) ||
        !(u256_compare(multiset_permutation_bound(11), product1) < 0 &&
          u256_compare(multiset_permutation_bound(12), product1) >= 0 &&
          u256_compare(multiset_permutation_bound(17), product2) < 0 &&
          u256_compare(multiset_permutation_bound(18), product2) >= 0 &&
          u256_compare(reconstruction_bound(18), product1) >= 0 &&
          u256_compare(reconstruction_bound(18), product2) < 0 &&
          u256_compare(reconstruction_bound(19), product1) >= 0 &&
          u256_compare(reconstruction_bound(19), product2) < 0 &&
          u256_compare(reconstruction_bound(20), product2) >= 0 &&
          u256_compare(multiset_permutation_bound(MAX_N), product3) < 0)) {
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
    const int n = 7;
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

    if (maximum < 0) {
        maximum = DEFAULT_N;
    }
    if (mode == MODE_CHECK) {
        for (int n = 0; n <= CHECK_N; ++n) {
            verify_known(n, a322178(n));
        }
        verify_constants_and_paths();
        printf("ok: A322178 n=0..%d and all CRT paths verified\n", CHECK_N);
        return EXIT_SUCCESS;
    }
    if (mode == MODE_TERM) {
        const U256 value = a322178(maximum);
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
        const U256 value = a322178(n);
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
