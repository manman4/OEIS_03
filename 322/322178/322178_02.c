/*
 * A322178 -- ordinary-prime algebraic CRT implementation.
 *
 * For a doubled word on labels 1,...,n put d_k=b(k)+1.  The condition is
 * d_1>=...>=d_n.  With m=2*n and
 *
 *     Q_d(x)=sum_(0<=i<m-d) x_i*x_(i+d),
 *
 * one has
 *
 *   A322178(n)=[t^n*x_0*...*x_(m-1)]
 *               product_(d=1)^(m-1) 1/(1-t*Q_d(x)).
 *
 * If a matching has r_d edges of distance d, Q_d^r_d contains those
 * distinct edges r_d! times.  This is exactly the number of assignments of
 * an equal-distance block to its consecutive labels.  For example, at n=2
 * the distance multisets {1,1}, {2,2}, {3,1} have weights 2!,2!,1 and give
 * 5.
 *
 * Boolean Fourier extraction gives
 *
 *   2^(-(m-1)) sum_(epsilon_0=1) (product_i epsilon_i)
 *       [t^n] product_d 1/(1-t*Q_d(epsilon)).
 *
 * At t-degree n every monomial has total x-degree 2*n=m.  The sign sum keeps
 * monomials in which all m exponents are positive and odd, so total degree m
 * forces every exponent to be one.  Global sign negation preserves all Q_d
 * and the sign product because m is even.  Reversal supplies another exact
 * two-element symmetry.
 *
 * There is a second exact involution T(epsilon)_i=(-1)^i*epsilon_i.  It
 * gives Q_d(T epsilon)=(-1)^d Q_d(epsilon), while the sign product changes
 * by (-1)^n.  Therefore the sum of the epsilon and T(epsilon) evaluations
 * is twice the part in which the total multiplicity of odd distances has
 * parity n.  More explicitly, put P(epsilon)=product_i epsilon_i and
 *
 *   E(t)=product_(d even) 1/(1-t*Q_d(epsilon)),
 *   O(t)=product_(d odd)  1/(1-t*Q_d(epsilon)).
 *
 * If E_a=[t^a]E and O_b=[t^b]O, the paired contribution is
 *
 *   P(epsilon)*(H_n(epsilon)+(-1)^n*H_n(T epsilon))
 *     = 2*P(epsilon)*sum_(a+b=n, b == n mod 2) E_a*O_b.       (1)
 *
 * The implementation obtains the sum on the right directly: it builds E
 * and O separately and convolves only degrees of the required parity.
 *
 * Let R be reversal followed, when necessary, by global sign negation so
 * epsilon_0 remains +1.  For even m, R and T commute, T has no fixed point,
 * and their orbits have size 2 or 4.  Reversal leaves P, E, and O unchanged.
 * Equation (1) therefore says that an orbit contributes
 *
 *   orbit_size * P(epsilon) * sum_(a+b=n, b == n mod 2) E_a*O_b.
 *
 * This explains the implemented orbit weights 2 and 4.  They reconstruct
 * the original full sign sum, so its normalization remains 2^(-(m-1)); it
 * is not divided by an additional symmetry factor.  Montgomery form makes
 * the final convolution exact and avoids division in that hot loop.
 *
 * Unlike 322178_01.c, this file uses two preselected ordinary primes near
 * 2^59 and generic incremental CRT.  It does not use Mersenne moduli or a
 * specialized Mersenne CRT.  The primes are checked deterministically at
 * startup, as are coprimality, Shoup and Montgomery multiplication, both
 * symmetry involutions, parity extraction, CRT reconstruction, and the
 * strict upper-bound coverage.
 *
 * The general upper bound is (2*n)!/2^n.  For n=18,19 the sharper bound from
 * 322178_01.c is used.  If every distance multiplicity is at most 10, the
 * weight product_d r_d! is at most W=10!*(n-10)!.  Matchings with at least
 * 11 equal-distance edges are at most
 *
 *   H=sum_(L=11)^(2*n-1) C(L,11)*(2*(n-11)-1)!!.
 *
 * Thus A322178(n)<=T*W+H*(n!-W), where T=(2*n-1)!!.  This proved bound is
 * below the product of the two ordinary primes through n=19, so CRT returns
 * a deterministic exact integer, not a probabilistic answer.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic -pthread \
 *       322178_02.c -o 322178_02
 *
 * Usage:
 *   ./322178_02 15 --threads 8
 *   ./322178_02 --term 19 --threads 8
 *   ./322178_02 --check --threads 8 --no-bfile
 *
 * Completed terms are atomically recorded in b322178_02.txt.  The b-file may
 * be sparse and contains only terms actually computed by this executable.
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
#error "322178_02.c requires unsigned __int128"
#endif

__extension__ typedef unsigned __int128 U128;
__extension__ typedef __int128 I128;

#define MAX_N 19
#define DEFAULT_N 9
#define CHECK_N 10
#define MAX_THREADS 64
#define MAX_LANES 2
#define MAX_POSITIONS (2 * MAX_N)
#define CHUNK_BITS 18

_Static_assert(MAX_POSITIONS < 64,
               "sign masks require fewer than 64 positions");
_Static_assert(CHUNK_BITS > 0 && CHUNK_BITS <= 57,
               "chunk size must not overflow the 64-bit atomic schedule");

/* Distinct ordinary primes below 2^59; neither is a Mersenne number. */
static const uint64_t prime_values[MAX_LANES] = {
    UINT64_C(576460752303423433),
    UINT64_C(576460752303423389)
};

static const char *const known[] = {
    "1", "1", "5", "33", "329", "3825", "57293", "977581",
    "19619645", "442155529", "11183272973", "312134648549",
    "9554405887621", "317670072938621", "11411690507968361",
    "440231352579839965", "18158142912306766989",
    "797183010461708801341", "37120529214984142435969"
};

static const char *output_path = "b322178_02.txt";
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

static int print_u128(FILE *stream, U128 value)
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

static bool parse_u128(const char *text, U128 *result)
{
    const U128 maximum = ~(U128)0;
    U128 value = 0;
    if (*text == '\0') {
        return false;
    }
    for (; *text != '\0'; ++text) {
        if (*text < '0' || *text > '9') {
            return false;
        }
        const unsigned digit = (unsigned)(*text - '0');
        if (value > (maximum - digit) / 10) {
            return false;
        }
        value = value * 10 + digit;
    }
    *result = value;
    return true;
}

static bool multiply_u128(U128 *value, uint64_t factor)
{
    const U128 maximum = ~(U128)0;
    if (factor != 0 && *value > maximum / factor) {
        return false;
    }
    *value *= factor;
    return true;
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

static U128 multiset_permutation_bound(int n)
{
    U128 result = 1;
    for (unsigned index = 1; index <= (unsigned)n; ++index) {
        if (!multiply_u128(&result, index) ||
            !multiply_u128(&result, 2 * index - 1)) {
            die("multiset-permutation upper bound overflow");
        }
    }
    return result;
}

static U128 total_pairings_bound(int n)
{
    U128 result = 1;
    for (unsigned odd = 1; odd < 2U * (unsigned)n; odd += 2) {
        if (!multiply_u128(&result, odd)) {
            die("pairing upper bound overflow");
        }
    }
    return result;
}

static U128 repeated_distance_bound(int n)
{
    /*
     * Write r_d for the number of matching edges of distance d.  Its
     * contribution has weight product_d r_d!, and sum_d r_d=n.
     *
     * If every r_d<=10 (and n is 18 or 19), convexity of factorials, or
     * repeatedly moving one unit from a smaller nonzero block to a larger
     * block, gives
     *
     *       product_d r_d! <= W=10!*(n-10)!.
     *
     * If some r_L>=11, choose 11 of the L possible distance-L edges.
     * There are at most C(L,11) choices; discarding intersecting choices
     * can only lower this number.  Each valid choice leaves 2*(n-11)
     * vertices, with (2*(n-11)-1)!! possible pairings.  Summing over L is
     * a union bound H (a matching may be counted more than once).  Every
     * matching has weight at most n!, since n!/product_d r_d! is an
     * integer.  With T=(2*n-1)!! total matchings, splitting into low and
     * high multiplicity cases therefore proves
     *
     *       A(n) <= T*W + H*(n!-W).
     *
     * The loops below evaluate exactly these T, W, and H.
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
    U128 result = total_pairings_bound(n);
    if (!multiply_u128(&result, low_weight)) {
        die("low-multiplicity contribution bound overflow");
    }
    U128 correction = high_count;
    if (!multiply_u128(&correction, factorial_n - low_weight) ||
        result > (~(U128)0) - correction) {
        die("high-multiplicity contribution bound overflow");
    }
    return result + correction;
}

static U128 reconstruction_bound(int n)
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

static void store_bfile_term(int n, U128 value)
{
    const int lock_fd = acquire_bfile_lock();
    U128 values[MAX_N + 1] = {0};
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
        char line[128];
        while (fgets(line, sizeof(line), input) != NULL) {
            int index;
            char number[64];
            char extra;
            if (sscanf(line, "%d %63s %c", &index, number, &extra) != 2 ||
                index < 0 || index > MAX_N || index <= previous ||
                !parse_u128(number, &values[index])) {
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
        if (values[n] != value) {
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
            print_u128(output, values[index]) != 0 ||
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
    fprintf(stderr, "322178_02: recorded computed term n=%d in %s\n",
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

static uint64_t alternated_signs(uint64_t negative, unsigned positions)
{
    const uint64_t position_mask =
        (UINT64_C(1) << positions) - 1;
    return negative ^
        (UINT64_C(0xaaaaaaaaaaaaaaaa) & position_mask);
}

static bool symmetry_representative(uint64_t negative, unsigned positions,
                                    unsigned *orbit_size)
{
    const uint64_t reflected =
        normalized_reflection(negative, positions);
    const uint64_t alternated = alternated_signs(negative, positions);
    const uint64_t reflected_alternated =
        normalized_reflection(alternated, positions);
    if (negative > reflected || negative > alternated ||
        negative > reflected_alternated) {
        return false;
    }
    *orbit_size = reflected == negative || reflected == alternated ? 2U : 4U;
    return true;
}

static uint64_t symmetry_orbit_count(int n)
{
    /*
     * Burnside check for the group {1,R,T,RT} on epsilon_0=+1.  For n>=2
     * the domain has 2^(2*n-1) points.  T and RT have no fixed points,
     * while normalized reversal R has 2^n fixed points.  Hence the number
     * of orbits is
     *
     *   (2^(2*n-1)+2^n)/4 = 2^(2*n-3)+2^(n-2).
     *
     * At n=1 normalized reversal is the identity, so there is one orbit.
     * This count is diagnostic; symmetry_representative performs the
     * actual exact orbit selection and supplies its size 2 or 4.
     */
    if (n == 1) {
        return 1;
    }
    return (UINT64_C(1) << (2U * (unsigned)n - 3)) +
           (UINT64_C(1) << ((unsigned)n - 2));
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
    uint64_t montgomery_negative_inverse;
    uint64_t montgomery_one;
} Prime;

static uint64_t mod_multiply_small(uint64_t value, unsigned multiplier,
                                   const Prime *prime)
{
    if (multiplier == 0) {
        return 0;
    }
    if (multiplier == 1) {
        return value;
    }
    if (multiplier == 2) {
        return mod_add(value, value, prime->modulus);
    }
    if (multiplier == 3) {
        return mod_add(mod_add(value, value, prime->modulus),
                       value, prime->modulus);
    }
    if (multiplier == 4) {
        const uint64_t twice = mod_add(value, value, prime->modulus);
        return mod_add(twice, twice, prime->modulus);
    }
    /* Exact 64-bit Shoup multiplication; unsigned low-product wraps cancel. */
    const uint64_t quotient = (uint64_t)(
        ((U128)value * prime->shoup[multiplier]) >> 64);
    uint64_t remainder = value * multiplier - quotient * prime->modulus;
    if (remainder >= prime->modulus) {
        remainder -= prime->modulus;
    }
    return remainder;
}

static uint64_t montgomery_multiply(uint64_t left, uint64_t right,
                                    const Prime *prime)
{
    /*
     * Let R=2^64 and n'=-p^(-1) (mod R).  For t=left*right, setting
     * a=(t mod R)*n' (mod R) makes t+a*p divisible by R, so this returns
     * t*R^(-1) (mod p).  Inputs are reduced below p<R; consequently the
     * quotient is below 2*p and one subtraction suffices.  Here p<2^59,
     * so t<2^118 and a*p<2^123: their U128 sum cannot overflow.
     */
    const U128 product = (U128)left * right;
    const uint64_t adjustment =
        (uint64_t)product * prime->montgomery_negative_inverse;
    const U128 reduced =
        (product + (U128)adjustment * prime->modulus) >> 64;
    const uint64_t value = (uint64_t)reduced;
    return value >= prime->modulus ? value - prime->modulus : value;
}

static uint64_t montgomery_negative_inverse(uint64_t modulus)
{
    uint64_t inverse = 1;
    /* Six Newton steps give all 64 bits from the correct low bit. */
    for (unsigned step = 0; step < 6; ++step) {
        inverse *= 2 - modulus * inverse;
    }
    return 0 - inverse;
}

static void initialize_prime(Prime *prime, uint64_t modulus)
{
    prime->modulus = modulus;
    prime->montgomery_negative_inverse =
        montgomery_negative_inverse(modulus);
    prime->montgomery_one =
        (uint64_t)(((U128)1 << 64) % modulus);
    for (unsigned multiplier = 0;
         multiplier <= MAX_POSITIONS; ++multiplier) {
        prime->shoup[multiplier] = (uint64_t)(
            ((U128)multiplier << 64) / modulus);
    }
}

/*
 * Multiplication by one factor (1-q*t)^(-1) uses the ascending recurrence
 *
 *     new[c] = old[c] + q*new[c-1].
 *
 * The already-updated new[c-1] supplies q^2, q^3, ... and therefore the
 * complete geometric factor.  Terms above degree n can never contribute
 * back to [t^n], so arrays truncated at n are exact, not a heuristic
 * cutoff.  coefficient_one/two and the parity variants all use this same
 * recurrence; the latter merely separate even and odd distances.
 */
static uint64_t coefficient_one(int n, unsigned positions,
                                const int edges[MAX_POSITIONS],
                                const Prime *prime)
{
    uint64_t coefficient[MAX_N + 1] = {0};
    coefficient[0] = 1;
    for (unsigned distance = 1; distance < positions; ++distance) {
        const int edge = edges[distance];
        if (edge == 0) {
            continue;
        }
        const unsigned magnitude = (unsigned)(edge < 0 ? -edge : edge);
        for (int degree = 1; degree <= n; ++degree) {
            const uint64_t product = magnitude == 1 ?
                coefficient[degree - 1] : mod_multiply_small(
                    coefficient[degree - 1], magnitude, prime);
            coefficient[degree] = edge < 0 ?
                mod_sub(coefficient[degree], product, prime->modulus) :
                mod_add(coefficient[degree], product, prime->modulus);
        }
    }
    return coefficient[n];
}

static void coefficient_two(int n, unsigned positions,
                            const int edges[MAX_POSITIONS],
                            const Prime prime[2], uint64_t result[2])
{
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
                    coefficient[0][degree - 1], magnitude, &prime[0]);
            const uint64_t product1 = magnitude == 1 ?
                coefficient[1][degree - 1] : mod_multiply_small(
                    coefficient[1][degree - 1], magnitude, &prime[1]);
            if (edge < 0) {
                coefficient[0][degree] = mod_sub(
                    coefficient[0][degree], product0, prime[0].modulus);
                coefficient[1][degree] = mod_sub(
                    coefficient[1][degree], product1, prime[1].modulus);
            } else {
                coefficient[0][degree] = mod_add(
                    coefficient[0][degree], product0, prime[0].modulus);
                coefficient[1][degree] = mod_add(
                    coefficient[1][degree], product1, prime[1].modulus);
            }
        }
    }
    result[0] = coefficient[0][n];
    result[1] = coefficient[1][n];
}

/*
 * T flips signs at odd positions, hence Q_d(T epsilon)=(-1)^d Q_d.
 * Pairing epsilon with T epsilon keeps exactly the terms in which the total
 * multiplicity of odd distances has parity n.  Build the even- and
 * odd-distance products separately and convolve only that parity.  Values
 * stay in Montgomery form so the final variable-by-variable products use no
 * division.  Small signed Q_d multipliers preserve Montgomery form.
 */
static uint64_t coefficient_parity_one(int n, unsigned positions,
                                       const int edges[MAX_POSITIONS],
                                       const Prime *prime)
{
    uint64_t coefficient[2][MAX_N + 1] = {{0}};
    coefficient[0][0] = prime->montgomery_one;
    coefficient[1][0] = prime->montgomery_one;
    for (unsigned distance = 1; distance < positions; ++distance) {
        const int edge = edges[distance];
        if (edge == 0) {
            continue;
        }
        uint64_t *const selected = coefficient[distance & 1U];
        const unsigned magnitude = (unsigned)(edge < 0 ? -edge : edge);
        for (int degree = 1; degree <= n; ++degree) {
            const uint64_t product = magnitude == 1 ?
                selected[degree - 1] : mod_multiply_small(
                    selected[degree - 1], magnitude, prime);
            selected[degree] = edge < 0 ?
                mod_sub(selected[degree], product, prime->modulus) :
                mod_add(selected[degree], product, prime->modulus);
        }
    }
    uint64_t sum = 0;
    for (int odd_degree = n & 1; odd_degree <= n; odd_degree += 2) {
        const uint64_t product = montgomery_multiply(
            coefficient[0][n - odd_degree],
            coefficient[1][odd_degree], prime);
        sum = mod_add(sum, product, prime->modulus);
    }
    return montgomery_multiply(sum, 1, prime);
}

static void coefficient_parity_two(int n, unsigned positions,
                                   const int edges[MAX_POSITIONS],
                                   const Prime prime[2], uint64_t result[2])
{
    uint64_t coefficient[2][2][MAX_N + 1] = {{{0}}};
    for (unsigned parity = 0; parity < 2; ++parity) {
        coefficient[parity][0][0] = prime[0].montgomery_one;
        coefficient[parity][1][0] = prime[1].montgomery_one;
    }
    for (unsigned distance = 1; distance < positions; ++distance) {
        const int edge = edges[distance];
        if (edge == 0) {
            continue;
        }
        uint64_t (*const selected)[MAX_N + 1] =
            coefficient[distance & 1U];
        const unsigned magnitude = (unsigned)(edge < 0 ? -edge : edge);
        for (int degree = 1; degree <= n; ++degree) {
            const uint64_t product0 = magnitude == 1 ?
                selected[0][degree - 1] : mod_multiply_small(
                    selected[0][degree - 1], magnitude, &prime[0]);
            const uint64_t product1 = magnitude == 1 ?
                selected[1][degree - 1] : mod_multiply_small(
                    selected[1][degree - 1], magnitude, &prime[1]);
            if (edge < 0) {
                selected[0][degree] = mod_sub(
                    selected[0][degree], product0, prime[0].modulus);
                selected[1][degree] = mod_sub(
                    selected[1][degree], product1, prime[1].modulus);
            } else {
                selected[0][degree] = mod_add(
                    selected[0][degree], product0, prime[0].modulus);
                selected[1][degree] = mod_add(
                    selected[1][degree], product1, prime[1].modulus);
            }
        }
    }
    uint64_t sum[2] = {0, 0};
    for (int odd_degree = n & 1; odd_degree <= n; odd_degree += 2) {
        for (unsigned lane = 0; lane < 2; ++lane) {
            const uint64_t product = montgomery_multiply(
                coefficient[0][lane][n - odd_degree],
                coefficient[1][lane][odd_degree], &prime[lane]);
            sum[lane] = mod_add(sum[lane], product, prime[lane].modulus);
        }
    }
    result[0] = montgomery_multiply(sum[0], 1, &prime[0]);
    result[1] = montgomery_multiply(sum[1], 1, &prime[1]);
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
    Prime prime[MAX_LANES];
    Schedule *schedule;
    uint64_t sum[MAX_LANES];
} Worker;

static void *worker_main(void *argument)
{
    Worker *worker = argument;
    if (worker->lanes < 1 || worker->lanes > MAX_LANES) {
        die("worker prime lane count is outside implementation limits");
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
            unsigned orbit_weight;
            if (!symmetry_representative(
                    signs, worker->positions, &orbit_weight)) {
                continue;
            }
            int edges[MAX_POSITIONS];
            correlations(worker->positions, signs, edges);
            uint64_t term[MAX_LANES] = {0, 0};
            if (worker->lanes == 1) {
                term[0] = coefficient_parity_one(
                    worker->n, worker->positions, edges, &worker->prime[0]);
            } else {
                coefficient_parity_two(worker->n, worker->positions, edges,
                                       worker->prime, term);
            }
            for (unsigned lane = 0; lane < worker->lanes; ++lane) {
                const uint64_t modulus = worker->prime[lane].modulus;
                term[lane] = mod_add(term[lane], term[lane], modulus);
                if (orbit_weight == 4U) {
                    term[lane] = mod_add(term[lane], term[lane], modulus);
                }
                /*
                 * popcount(gray(index)) mod 2 = index mod 2 (the xor
                 * telescopes to the low bit).  Since epsilon_0 is fixed
                 * positive, index&1 is exactly the Fourier sign P(epsilon).
                 */
                worker->sum[lane] = (index & 1) != 0 ?
                    mod_sub(worker->sum[lane], term[lane], modulus) :
                    mod_add(worker->sum[lane], term[lane], modulus);
            }
        }
    }
    return NULL;
}

static unsigned select_primes(U128 bound, U128 *product)
{
    /*
     * A(n) is a nonnegative integer no larger than bound.  Pairwise
     * coprime moduli with product M>bound identify it uniquely in [0,M).
     * The comparison is deliberately strict: M==bound would not separate
     * the two admissible integers 0 and M.
     */
    *product = 1;
    unsigned lanes = 0;
    while (*product <= bound && lanes < MAX_LANES) {
        if (!multiply_u128(product, prime_values[lanes])) {
            die("ordinary-prime product overflow");
        }
        ++lanes;
    }
    if (*product <= bound) {
        die("ordinary-prime product does not exceed rigorous upper bound");
    }
    return lanes;
}

static void modular_passes(int n, int threads, unsigned lanes,
                           uint64_t residues[MAX_LANES])
{
    const unsigned positions = 2U * (unsigned)n;
    if (lanes < 1 || lanes > MAX_LANES || positions == 0 ||
        positions > MAX_POSITIONS || positions >= 64) {
        die("ordinary-prime dimensions are outside implementation limits");
    }
    for (unsigned lane = 0; lane < lanes; ++lane) {
        if (prime_values[lane] <= 1 ||
            (prime_values[lane] & 1) == 0 ||
            prime_values[lane] >= (UINT64_C(1) << 63)) {
            die("ordinary prime violates Shoup preconditions");
        }
        for (unsigned other = 0; other < lane; ++other) {
            if (gcd_u64(prime_values[lane], prime_values[other]) != 1) {
                die("ordinary CRT moduli are not coprime");
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
        die("cannot allocate ordinary-prime workers");
    }
    Schedule schedule = {
        .next = 0, .end = evaluations,
        .chunk = UINT64_C(1) << CHUNK_BITS
    };
    for (int id = 0; id < threads; ++id) {
        workers[id].n = n;
        workers[id].positions = positions;
        workers[id].lanes = lanes;
        workers[id].schedule = &schedule;
        for (unsigned lane = 0; lane < lanes; ++lane) {
            initialize_prime(&workers[id].prime[lane], prime_values[lane]);
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
                                     prime_values[lane]);
        }
    }
    free(workers);
    free(ids);
    for (unsigned lane = 0; lane < lanes; ++lane) {
        for (unsigned bit = 0; bit < positions - 1; ++bit) {
            if ((residues[lane] & 1U) != 0) {
                residues[lane] += prime_values[lane];
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
        die("ordinary CRT moduli are not coprime");
    }
    old_s %= modulus;
    if (old_s < 0) {
        old_s += modulus;
    }
    return (uint64_t)old_s;
}

static U128 reconstruct(const uint64_t residues[MAX_LANES], unsigned lanes)
{
    /*
     * Incremental ordinary CRT maintains 0<=value<product.  The next
     * multiplier is below its modulus, hence the updated value is below
     * product*modulus.  MAX_LANES is two and their full product is below
     * 2^118, leaving ample U128 headroom for both additions/products.
     */
    if (lanes < 1 || lanes > MAX_LANES) {
        die("invalid ordinary CRT lane count");
    }
    U128 value = residues[0];
    U128 product = prime_values[0];
    for (unsigned lane = 1; lane < lanes; ++lane) {
        const uint64_t modulus = prime_values[lane];
        const uint64_t difference = mod_sub(
            residues[lane], (uint64_t)(value % modulus), modulus);
        const uint64_t inverse = inverse_mod(
            (uint64_t)(product % modulus), modulus);
        const uint64_t multiplier = (uint64_t)(
            ((U128)difference * inverse) % modulus);
        value += product * multiplier;
        product *= modulus;
    }
    return value;
}

static U128 a322178(int n)
{
    if (n == 0) {
        return 1;
    }
    const U128 bound = reconstruction_bound(n);
    U128 prime_product;
    const unsigned lanes = select_primes(bound, &prime_product);
    uint64_t residues[MAX_LANES];
    const double started = now_seconds();
    modular_passes(n, requested_threads, lanes, residues);
    const U128 answer = reconstruct(residues, lanes);
    if (answer > bound) {
        die("ordinary CRT result exceeds rigorous upper bound");
    }
    const uint64_t evaluations = UINT64_C(1) << (2U * (unsigned)n - 1);
    int threads = requested_threads;
    if ((uint64_t)threads > evaluations) {
        threads = (int)evaluations;
    }
    fprintf(stderr,
            "322178_02: n=%d, ordinary-prime CRT, evaluations=%" PRIu64
            ", symmetry-orbits=%" PRIu64
            ", primes=%u, threads=%d, %.3f s\n",
            n, evaluations, symmetry_orbit_count(n), lanes, threads,
            now_seconds() - started);
    return answer;
}

static void verify_known(int n, U128 value)
{
    const unsigned count = (unsigned)(sizeof(known) / sizeof(known[0]));
    if ((unsigned)n >= count) {
        return;
    }
    U128 expected;
    if (!parse_u128(known[n], &expected) || value != expected) {
        fprintf(stderr, "error: A322178 mismatch at n=%d: got ", n);
        print_u128(stderr, value);
        fprintf(stderr, ", expected %s\n", known[n]);
        exit(EXIT_FAILURE);
    }
}

static uint64_t mod_power(uint64_t base, uint64_t exponent,
                          uint64_t modulus)
{
    uint64_t result = 1;
    while (exponent != 0) {
        if ((exponent & 1U) != 0) {
            result = (uint64_t)(((U128)result * base) % modulus);
        }
        exponent >>= 1;
        if (exponent != 0) {
            base = (uint64_t)(((U128)base * base) % modulus);
        }
    }
    return result;
}

static bool is_prime_u64(uint64_t value)
{
    static const uint32_t small_primes[] = {
        2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37
    };
    for (unsigned index = 0;
         index < sizeof(small_primes) / sizeof(small_primes[0]); ++index) {
        const uint64_t prime = small_primes[index];
        if (value % prime == 0) {
            return value == prime;
        }
    }
    if (value < 2) {
        return false;
    }
    uint64_t odd = value - 1;
    unsigned power_of_two = 0;
    while ((odd & 1U) == 0) {
        odd >>= 1;
        ++power_of_two;
    }
    /* This witness set makes Miller-Rabin deterministic for uint64_t. */
    static const uint64_t witnesses[] = {
        2, 325, 9375, 28178, 450775, 9780504, 1795265022
    };
    for (unsigned index = 0;
         index < sizeof(witnesses) / sizeof(witnesses[0]); ++index) {
        const uint64_t witness = witnesses[index] % value;
        if (witness == 0) {
            continue;
        }
        uint64_t sample = mod_power(witness, odd, value);
        if (sample == 1 || sample == value - 1) {
            continue;
        }
        bool composite = true;
        for (unsigned square = 1; square < power_of_two; ++square) {
            sample = (uint64_t)(((U128)sample * sample) % value);
            if (sample == value - 1) {
                composite = false;
                break;
            }
        }
        if (composite) {
            return false;
        }
    }
    return true;
}

static void verify_symmetry_and_parity_paths(const Prime prime[2])
{
    for (int n = 1; n <= 7; ++n) {
        const unsigned positions = 2U * (unsigned)n;
        const uint64_t assignments = UINT64_C(1) << (positions - 1);
        uint64_t represented = 0;
        uint64_t representatives = 0;
        for (uint64_t raw = 0; raw < assignments; ++raw) {
            const uint64_t signs = raw << 1;
            const uint64_t alternated = alternated_signs(signs, positions);
            const uint64_t reflected =
                normalized_reflection(signs, positions);
            if (alternated_signs(alternated, positions) != signs ||
                normalized_reflection(reflected, positions) != signs ||
                normalized_reflection(alternated, positions) !=
                    alternated_signs(reflected, positions) ||
                (alternated & 1U) != 0 || (reflected & 1U) != 0) {
                die("alternation/reflection group self-test failed");
            }
            unsigned orbit_size;
            if (symmetry_representative(signs, positions, &orbit_size)) {
                if (orbit_size != 2U && orbit_size != 4U) {
                    die("invalid alternation/reflection orbit size");
                }
                represented += orbit_size;
                ++representatives;
            }

            int edges[MAX_POSITIONS];
            int alternate_edges[MAX_POSITIONS];
            correlations(positions, signs, edges);
            correlations(positions, alternated, alternate_edges);
            for (unsigned distance = 1;
                 distance < positions; ++distance) {
                const int terms = (int)(positions - distance);
                if (edges[distance] < -terms || edges[distance] > terms ||
                    ((edges[distance] + terms) & 1) != 0 ||
                    alternate_edges[distance] !=
                        ((distance & 1U) != 0 ? -edges[distance] :
                                               edges[distance])) {
                    die("correlation/parity invariant self-test failed");
                }
            }

            uint64_t legacy[2], alternate_legacy[2], filtered[2];
            coefficient_two(n, positions, edges, prime, legacy);
            coefficient_two(n, positions, alternate_edges, prime,
                            alternate_legacy);
            coefficient_parity_two(n, positions, edges, prime, filtered);
            for (unsigned lane = 0; lane < 2; ++lane) {
                const uint64_t expected = (n & 1) != 0 ?
                    mod_sub(legacy[lane], alternate_legacy[lane],
                            prime[lane].modulus) :
                    mod_add(legacy[lane], alternate_legacy[lane],
                            prime[lane].modulus);
                const uint64_t doubled = mod_add(
                    filtered[lane], filtered[lane], prime[lane].modulus);
                if (expected != doubled) {
                    die("alternating-sign parity extraction self-test failed");
                }
            }
            if (coefficient_one(n, positions, edges, &prime[0]) != legacy[0] ||
                coefficient_parity_one(n, positions, edges, &prime[0]) !=
                    filtered[0]) {
                die("one/two-lane coefficient path self-test failed");
            }
        }
        if (represented != assignments ||
            representatives != symmetry_orbit_count(n)) {
            die("alternation/reflection orbit coverage self-test failed");
        }
    }
}

static void verify_constants_and_paths(void)
{
    Prime configured[MAX_LANES];
    for (unsigned lane = 0; lane < MAX_LANES; ++lane) {
        const uint64_t modulus = prime_values[lane];
        if (!is_prime_u64(modulus)) {
            die("configured ordinary modulus is not prime");
        }
        for (unsigned other = 0; other < lane; ++other) {
            if (gcd_u64(modulus, prime_values[other]) != 1) {
                die("configured ordinary primes are not coprime");
            }
        }
        Prime *const prime = &configured[lane];
        initialize_prime(prime, modulus);
        if (modulus * prime->montgomery_negative_inverse != UINT64_MAX) {
            die("ordinary-prime Montgomery inverse self-test failed");
        }
        uint64_t state = UINT64_C(0x9e3779b97f4a7c15) ^ modulus;
        for (unsigned multiplier = 0;
             multiplier <= MAX_POSITIONS; ++multiplier) {
            for (unsigned test = 0; test < 1000; ++test) {
                state = state * UINT64_C(6364136223846793005) + 1;
                const uint64_t sample = state % modulus;
                const uint64_t expected = (uint64_t)(
                    ((U128)sample * multiplier) % modulus);
                if (mod_multiply_small(sample, multiplier, prime) !=
                    expected) {
                    die("ordinary-prime Shoup self-test failed");
                }
            }
        }
        for (unsigned test = 0; test < 1000; ++test) {
            state = state * UINT64_C(6364136223846793005) + 1;
            const uint64_t left = state % modulus;
            state = state * UINT64_C(6364136223846793005) + 1;
            const uint64_t right = state % modulus;
            const uint64_t left_montgomery = (uint64_t)(
                ((U128)left << 64) % modulus);
            const uint64_t right_montgomery = (uint64_t)(
                ((U128)right << 64) % modulus);
            const uint64_t product_montgomery = montgomery_multiply(
                left_montgomery, right_montgomery, prime);
            const uint64_t actual =
                montgomery_multiply(product_montgomery, 1, prime);
            const uint64_t expected =
                (uint64_t)(((U128)left * right) % modulus);
            if (actual != expected) {
                die("ordinary-prime Montgomery self-test failed");
            }
        }
    }
    verify_symmetry_and_parity_paths(configured);
    const U128 product = (U128)prime_values[0] * prime_values[1];
    U128 expected18, expected19;
    if (!parse_u128("33512221839068180452468224000000", &expected18) ||
        !parse_u128("11466557255140264747179943833600000", &expected19) ||
        repeated_distance_bound(18) != expected18 ||
        repeated_distance_bound(19) != expected19 ||
        !(multiset_permutation_bound(11) < prime_values[0] &&
          multiset_permutation_bound(12) >= prime_values[0] &&
          reconstruction_bound(18) < product &&
          reconstruction_bound(19) < product)) {
        die("ordinary-prime CRT threshold self-test failed");
    }
    const U128 sample = product - 12345;
    uint64_t sample_residues[MAX_LANES] = {
        (uint64_t)(sample % prime_values[0]),
        (uint64_t)(sample % prime_values[1])
    };
    if (reconstruct(sample_residues, 2) != sample) {
        die("ordinary CRT reconstruction self-test failed");
    }
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
    verify_constants_and_paths();
    if (mode == MODE_CHECK) {
        for (int n = 0; n <= CHECK_N; ++n) {
            verify_known(n, a322178(n));
        }
        printf("ok: A322178 n=0..%d and ordinary-prime paths verified\n",
               CHECK_N);
        return EXIT_SUCCESS;
    }
    if (mode == MODE_TERM) {
        const U128 value = a322178(maximum);
        verify_known(maximum, value);
        if (write_bfile) {
            store_bfile_term(maximum, value);
        }
        printf("%d ", maximum);
        print_u128(stdout, value);
        putchar('\n');
        return EXIT_SUCCESS;
    }
    for (int n = 0; n <= maximum; ++n) {
        const U128 value = a322178(n);
        verify_known(n, value);
        if (write_bfile) {
            store_bfile_term(n, value);
        }
        printf("%d ", n);
        print_u128(stdout, value);
        putchar('\n');
        fflush(stdout);
    }
    return EXIT_SUCCESS;
}
