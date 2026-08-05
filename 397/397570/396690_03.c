/*
 * Spahn--Zeilberger tiling-partition formula for
 *
 *   Number of permutations p of [n+k] such that
 *   |p(i+1) - p(i)| != n for every 1 <= i < n+k.
 *
 * The default is k=4.  Term n is b_{1,n}(n+k).  For n>=1 this program
 * evaluates the same C_alpha tiling formula as 398599_03.c, with
 * permutation size N=n+k and forbidden difference s=n.
 *
 * Spahn and Zeilberger define C_alpha(N,d) as the number of tilings of
 * {1,...,N} by d-step chains having tile-size partition alpha.  Their
 * formula specializes here to
 *
 *   b_{1,n}(N) = sum_alpha C_alpha(N,1) C_alpha(N,n)
 *                    (-1)^(number_of_tiles-N)
 *                    product_j a_j! * 2^(a_2+a_3+...).
 *
 * Store h_l=a_{l+1}, the number of non-singleton tiles with l edges.
 * Put r=sum l*h_l and c=sum h_l, so a_1=N-r-c.  On a path of m vertices,
 * a fixed histogram occurs
 *
 *       binomial(m-r,c) c! / product_l h_l!
 *
 * times.  Convolution over the n residue paths constructs C_alpha(N,n);
 * C_alpha(N,1) is evaluated directly by the same displayed expression.
 * This does not construct algorithm 02's Q(H) or Tauraso q(H).
 *
 * The normal calculation does not enumerate permutations.  A small
 * independent subset DP is used only by --check.
 *
 * If n=0, a zero difference cannot occur between distinct permutation
 * values.  If k=0, the forbidden difference equals the permutation size.
 * Both cases therefore equal (n+k)!.
 *
 * Arbitrary-precision final values are provided by GMP.  For the common
 * n+k<=60 limit, all histogram-convolution coefficients sum to at most
 * 2^59 and fit rigorously in uint64_t.  Histogram factorials fit in
 * uint128_t, while quantities of permutation-factorial size remain in GMP.
 * Every allocation and fixed-width integer operation is checked.
 *
 * Reference:
 *   G. Spahn and D. Zeilberger,
 *   "Counting Permutations Where The Difference Between Entries Located
 *    r Places Apart Can never be s",
 *   https://doi.org/10.54550/ECA2023V3S2R10
 *
 * Build (Homebrew GMP on Apple silicon):
 *
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic \
 *     -I/opt/homebrew/include -L/opt/homebrew/lib \
 *     396690_03.c -lgmp -o 396690_03
 *
 * Usage:
 *
 *   ./396690_03                         # k=4, a(0)..a(6)
 *   ./396690_03 --k 4 --upto 6
 *   ./396690_03 --k 4 --term 6
 *   ./396690_03 --check                 # k=4, n=0..6
 *   ./396690_03 --check 8
 *
 * --upto prints a(0),...,a(MAX_N) as one comma-separated line.  No b-file
 * is created.
 */

#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <gmp.h>

#if ULONG_MAX < UINT64_MAX
#error "396690_03 requires a platform with 64-bit unsigned long"
#endif

#if !defined(__SIZEOF_INT128__)
#error "396690_03 requires unsigned __int128"
#endif

__extension__ typedef unsigned __int128 uint128_t;

#define DEFAULT_K 4
#define DEFAULT_MAX_N 6
#define DEFAULT_CHECK_N 6
#define MAX_SUPPORTED_TOTAL_SIZE 60
#define MAX_COMPONENT_COUNT (MAX_SUPPORTED_TOTAL_SIZE / 2)
#define MAX_HISTOGRAM_LENGTH (MAX_SUPPORTED_TOTAL_SIZE - 1)
#define KNOWN_MAX_N 8
#define MAX_BRUTE_SIZE (KNOWN_MAX_N + DEFAULT_K)

static const char *const known_k4_terms[KNOWN_MAX_N + 1] = {
    "24",
    "14",
    "152",
    "1392",
    "13824",
    "140160",
    "1543680",
    "18385920",
    "235791360"
};

typedef struct {
    uint8_t count[MAX_HISTOGRAM_LENGTH];
    uint64_t coefficient;
} HistogramTerm;

typedef struct {
    HistogramTerm *item;
    size_t count;
    size_t capacity;
} TermList;

typedef struct {
    bool initialized;
    TermList pattern;
} PatternCache;

typedef enum {
    MODE_UPTO,
    MODE_TERM
} OutputMode;

static int comparison_key_length = 0;

static void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static void *xcalloc(size_t count, size_t size)
{
    if (size != 0U && count > SIZE_MAX / size) {
        die("allocation size overflow");
    }
    void *pointer = calloc(count, size);
    if (pointer == NULL) {
        die("out of memory");
    }
    return pointer;
}

static void *xrealloc(void *old, size_t count, size_t size)
{
    if (size != 0U && count > SIZE_MAX / size) {
        die("allocation size overflow");
    }
    void *pointer = realloc(old, count * size);
    if (pointer == NULL) {
        die("out of memory");
    }
    return pointer;
}

static uint64_t add_u64(uint64_t left, uint64_t right)
{
    if (left > UINT64_MAX - right) {
        die("uint64_t addition overflow");
    }
    return left + right;
}

static uint64_t multiply_u64(uint64_t left, uint64_t right)
{
    if (left != 0U && right > UINT64_MAX / left) {
        die("uint64_t multiplication overflow");
    }
    return left * right;
}

static uint128_t multiply_u128(uint128_t left, uint128_t right)
{
    if (left != 0 && right > ~(uint128_t)0 / left) {
        die("uint128_t multiplication overflow");
    }
    return left * right;
}

static void mpz_set_u128(mpz_t result, uint128_t value)
{
    uint64_t high = (uint64_t)(value >> 64);
    uint64_t low = (uint64_t)value;
    mpz_set_ui(result, (unsigned long)high);
    mpz_mul_2exp(result, result, 64U);
    mpz_add_ui(result, result, (unsigned long)low);
}

static uint64_t mpz_get_u64_checked(const mpz_t value)
{
    if (mpz_sgn(value) < 0 || mpz_sizeinbase(value, 2) > 64U) {
        die("GMP value does not fit uint64_t");
    }
    return (uint64_t)mpz_get_ui(value);
}

static double monotonic_seconds(void)
{
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) != 0) {
        die("clock_gettime failed");
    }
    return (double)now.tv_sec + (double)now.tv_nsec / 1000000000.0;
}

static int parse_integer(const char *text, const char *label,
                         int minimum, int maximum)
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

static int parse_n(const char *text, const char *label)
{
    return parse_integer(text, label, 0, MAX_SUPPORTED_TOTAL_SIZE);
}

static int parse_k(const char *text)
{
    return parse_integer(text, "K", 0, MAX_SUPPORTED_TOTAL_SIZE);
}

static void validate_total_size(int n, int k)
{
    if (n > MAX_SUPPORTED_TOTAL_SIZE - k) {
        fprintf(stderr,
                "error: N+K must be at most %d: n=%d, k=%d\n",
                MAX_SUPPORTED_TOTAL_SIZE, n, k);
        exit(EXIT_FAILURE);
    }
}

static void list_init(TermList *list)
{
    list->item = NULL;
    list->count = 0U;
    list->capacity = 0U;
}

static void list_destroy(TermList *list)
{
    free(list->item);
    list->item = NULL;
    list->count = 0U;
    list->capacity = 0U;
}

static void list_add(TermList *list, const uint8_t *histogram,
                     int key_length, uint64_t coefficient)
{
    if (list->count == list->capacity) {
        size_t next = list->capacity == 0U ? 16U : list->capacity * 2U;
        if (next < list->capacity) {
            die("histogram list capacity overflow");
        }
        list->item = xrealloc(list->item, next, sizeof(*list->item));
        list->capacity = next;
    }

    HistogramTerm *term = &list->item[list->count++];
    memset(term->count, 0, sizeof(term->count));
    memcpy(term->count, histogram, (size_t)key_length);
    term->coefficient = coefficient;
}

static void make_factorials(
    uint128_t factorial[MAX_COMPONENT_COUNT + 1])
{
    factorial[0] = (uint128_t)1;
    for (int i = 1; i <= MAX_COMPONENT_COUNT; ++i) {
        factorial[i] = multiply_u128(factorial[i - 1], (uint128_t)i);
    }
}

/* (available)_component_count / product_l h_l! */
static uint64_t tiling_multiplicity(
    int available, int component_count, uint128_t histogram_factor,
    const uint128_t factorial[MAX_COMPONENT_COUNT + 1],
    mpz_t numerator, mpz_t divisor)
{
    if (available < component_count || component_count < 0 ||
        component_count > MAX_COMPONENT_COUNT) {
        die("invalid tiling multiplicity arguments");
    }

    mpz_bin_uiui(numerator, (unsigned long)available,
                 (unsigned long)component_count);
    mpz_set_u128(divisor, factorial[component_count]);
    mpz_mul(numerator, numerator, divisor);
    mpz_set_u128(divisor, histogram_factor);
    if (!mpz_divisible_p(numerator, divisor)) {
        die("nonintegral tiling multiplicity");
    }
    mpz_divexact(numerator, numerator, divisor);
    return mpz_get_u64_checked(numerator);
}

static void generate_patterns_recursive(
    TermList *patterns, int key_length, int path_size, int length,
    int edge_count, int component_count,
    uint8_t histogram[MAX_HISTOGRAM_LENGTH],
    const uint128_t factorial[MAX_COMPONENT_COUNT + 1],
    mpz_t numerator, mpz_t divisor)
{
    if (length >= path_size) {
        uint128_t histogram_factor = (uint128_t)1;
        for (int i = 0; i < path_size - 1; ++i) {
            histogram_factor = multiply_u128(
                histogram_factor, factorial[histogram[i]]);
        }
        uint64_t coefficient = tiling_multiplicity(
            path_size - edge_count, component_count, histogram_factor,
            factorial, numerator, divisor);
        list_add(patterns, histogram, key_length, coefficient);
        return;
    }

    int remaining_vertices = path_size - edge_count - component_count;
    int maximum = remaining_vertices / (length + 1);
    for (int count = 0; count <= maximum; ++count) {
        histogram[length - 1] = (uint8_t)count;
        generate_patterns_recursive(
            patterns, key_length, path_size, length + 1,
            edge_count + length * count, component_count + count,
            histogram, factorial, numerator, divisor);
    }
}

static void generate_patterns(
    TermList *patterns, int key_length, int path_size,
    const uint128_t factorial[MAX_COMPONENT_COUNT + 1])
{
    uint8_t histogram[MAX_HISTOGRAM_LENGTH] = {0};
    mpz_t numerator;
    mpz_t divisor;
    mpz_init(numerator);
    mpz_init(divisor);
    list_init(patterns);
    generate_patterns_recursive(
        patterns, key_length, path_size, 1, 0, 0, histogram,
        factorial, numerator, divisor);
    mpz_clear(divisor);
    mpz_clear(numerator);
}

static int compare_histograms(const void *left, const void *right)
{
    const HistogramTerm *a = left;
    const HistogramTerm *b = right;
    return memcmp(a->count, b->count, (size_t)comparison_key_length);
}

static void convolve_path(TermList *current, const TermList *patterns,
                          int key_length)
{
    if (patterns->count != 0U &&
        current->count > SIZE_MAX / patterns->count) {
        die("histogram convolution size overflow");
    }
    size_t candidate_count = current->count * patterns->count;
    HistogramTerm *candidate =
        xcalloc(candidate_count, sizeof(*candidate));

    size_t position = 0U;
    for (size_t i = 0; i < current->count; ++i) {
        for (size_t j = 0; j < patterns->count; ++j) {
            HistogramTerm *term = &candidate[position++];
            for (int length = 0; length < key_length; ++length) {
                unsigned count = (unsigned)current->item[i].count[length] +
                                 patterns->item[j].count[length];
                if (count > UINT8_MAX) {
                    die("histogram count overflow");
                }
                term->count[length] = (uint8_t)count;
            }
            term->coefficient = multiply_u64(
                current->item[i].coefficient,
                patterns->item[j].coefficient);
        }
    }
    if (position != candidate_count) {
        die("internal convolution length mismatch");
    }

    comparison_key_length = key_length;
    qsort(candidate, candidate_count, sizeof(*candidate),
          compare_histograms);

    size_t following_count = 0U;
    for (size_t read = 0; read < candidate_count; ++read) {
        if (following_count != 0U &&
            memcmp(candidate[following_count - 1U].count,
                   candidate[read].count, (size_t)key_length) == 0) {
            candidate[following_count - 1U].coefficient = add_u64(
                candidate[following_count - 1U].coefficient,
                candidate[read].coefficient);
        } else {
            if (following_count != read) {
                candidate[following_count] = candidate[read];
            }
            ++following_count;
        }
    }

    list_destroy(current);
    current->item = candidate;
    current->count = following_count;
    current->capacity = candidate_count;
}

static void tiling_coefficients(
    TermList *result, int size, int step,
    const uint128_t factorial[MAX_COMPONENT_COUNT + 1])
{
    int key_length = size - 1;
    uint8_t zero[MAX_HISTOGRAM_LENGTH] = {0};
    list_init(result);
    list_add(result, zero, key_length, UINT64_C(1));

    PatternCache cache[MAX_SUPPORTED_TOTAL_SIZE + 1];
    memset(cache, 0, sizeof(cache));

    for (int residue = 1; residue <= step; ++residue) {
        int path_size = (size - residue) / step + 1;
        if (path_size == 1) {
            continue;
        }
        if (!cache[path_size].initialized) {
            generate_patterns(&cache[path_size].pattern, key_length,
                              path_size, factorial);
            cache[path_size].initialized = true;
        }
        convolve_path(result, &cache[path_size].pattern, key_length);
    }

    for (int path_size = 0;
         path_size <= MAX_SUPPORTED_TOTAL_SIZE; ++path_size) {
        if (cache[path_size].initialized) {
            list_destroy(&cache[path_size].pattern);
        }
    }

    uint64_t coefficient_sum = UINT64_C(0);
    for (size_t i = 0; i < result->count; ++i) {
        coefficient_sum = add_u64(coefficient_sum,
                                  result->item[i].coefficient);
    }
    int exponent = size - step;
    if (exponent < 0 || exponent >= 64 ||
        coefficient_sum != (UINT64_C(1) << exponent)) {
        die("tiling coefficients do not sum to 2^(size-step)");
    }
}

static uint128_t histogram_factorial(
    const uint8_t *histogram, int key_length,
    const uint128_t factorial[MAX_COMPONENT_COUNT + 1],
    int *edge_count, int *component_count)
{
    uint128_t product = (uint128_t)1;
    *edge_count = 0;
    *component_count = 0;
    for (int i = 0; i < key_length; ++i) {
        int count = histogram[i];
        *edge_count += (i + 1) * count;
        *component_count += count;
        product = multiply_u128(product, factorial[count]);
    }
    return product;
}

static void multiply_by_u128(mpz_t value, uint128_t factor,
                             mpz_t temporary)
{
    mpz_set_u128(temporary, factor);
    mpz_mul(value, value, temporary);
}

static void compute_tiling_formula(mpz_t result, int size, int step,
                                   size_t *histogram_count)
{
    if (size <= step) {
        mpz_fac_ui(result, (unsigned long)size);
        *histogram_count = 1U;
        return;
    }
    if (step <= 0 || size > MAX_SUPPORTED_TOTAL_SIZE) {
        die("invalid tiling-formula arguments");
    }

    uint128_t factorial[MAX_COMPONENT_COUNT + 1];
    make_factorials(factorial);

    TermList output;
    tiling_coefficients(&output, size, step, factorial);
    *histogram_count = output.count;
    mpz_set_ui(result, 0UL);

    mpz_t term;
    mpz_t factor;
    mpz_init(term);
    mpz_init(factor);

    int key_length = size - 1;
    for (size_t i = 0; i < output.count; ++i) {
        int edge_count;
        int component_count;
        uint128_t hist_factor = histogram_factorial(
            output.item[i].count, key_length, factorial,
            &edge_count, &component_count);
        int singleton_count = size - edge_count - component_count;
        if (singleton_count < 0) {
            die("invalid tile histogram");
        }

        /* C_alpha(size,1) for this same tile-size partition alpha. */
        uint64_t input_coefficient = tiling_multiplicity(
            size - edge_count, component_count, hist_factor,
            factorial, term, factor);

        mpz_set_ui(term, (unsigned long)output.item[i].coefficient);
        multiply_by_u128(term, hist_factor, factor);
        multiply_by_u128(term, (uint128_t)input_coefficient, factor);
        mpz_fac_ui(factor, (unsigned long)singleton_count);
        mpz_mul(term, term, factor);
        mpz_mul_2exp(term, term, (mp_bitcnt_t)component_count);

        if ((edge_count & 1) == 0) {
            mpz_add(result, result, term);
        } else {
            mpz_sub(result, result, term);
        }
    }

    mpz_clear(factor);
    mpz_clear(term);
    list_destroy(&output);

    mpz_t upper;
    mpz_init(upper);
    mpz_fac_ui(upper, (unsigned long)size);
    if (mpz_sgn(result) < 0 || mpz_cmp(result, upper) > 0) {
        mpz_clear(upper);
        die("formula result is outside 0..size!");
    }
    mpz_clear(upper);
}

static void verify_known_value(const mpz_t value, int n, int k)
{
    if (k != DEFAULT_K || n > KNOWN_MAX_N) {
        return;
    }

    mpz_t expected;
    mpz_init(expected);
    if (mpz_set_str(expected, known_k4_terms[n], 10) != 0) {
        mpz_clear(expected);
        die("invalid built-in decimal value");
    }
    if (mpz_cmp(value, expected) != 0) {
        gmp_fprintf(stderr,
                    "error: fixed-k mismatch at k=%d, n=%d: got %Zd, "
                    "expected %s\n",
                    k, n, value, known_k4_terms[n]);
        mpz_clear(expected);
        exit(EXIT_FAILURE);
    }
    mpz_clear(expected);
}

static void compute_value(mpz_t result, int n, int k, bool verbose)
{
    double started = monotonic_seconds();
    int size = n + k;
    size_t histogram_count = 0U;

    if (n == 0 || k == 0) {
        mpz_fac_ui(result, (unsigned long)size);
        histogram_count = 1U;
    } else {
        compute_tiling_formula(result, size, n, &histogram_count);
    }

    verify_known_value(result, n, k);
    if (verbose) {
        fprintf(stderr,
                "396690_03: k=%d, n=%d, size=%d, "
                "forbidden difference=%d, tiling histograms=%zu, "
                "Spahn--Zeilberger tiling-partition formula, %.3f s\n",
                k, n, size, n, histogram_count,
                monotonic_seconds() - started);
    }
}

static void print_value(const mpz_t value)
{
    if (mpz_out_str(stdout, 10, value) == 0U) {
        die("could not write result");
    }
}

static int print_sequence(int maximum_n, int k)
{
    mpz_t value;
    mpz_init(value);
    for (int n = 0; n <= maximum_n; ++n) {
        compute_value(value, n, k, false);
        if (n != 0) {
            fputs(", ", stdout);
        }
        print_value(value);
    }
    putchar('\n');
    mpz_clear(value);
    return EXIT_SUCCESS;
}

static uint64_t brute_recursive(int size, int difference, uint32_t full,
                                uint64_t *memo, uint32_t used, int last)
{
    if (used == full) {
        return UINT64_C(1);
    }
    size_t index = (size_t)used * (size_t)(size + 1) + (size_t)last;
    if (memo[index] != UINT64_MAX) {
        return memo[index];
    }

    uint64_t total = UINT64_C(0);
    uint32_t available = full ^ used;
    while (available != 0U) {
        uint32_t bit = available & (0U - available);
        available ^= bit;
        int value = __builtin_ctz(bit);
        if (last < size) {
            int delta = value - last;
            if (delta == difference || delta == -difference) {
                continue;
            }
        }
        total = add_u64(total, brute_recursive(
            size, difference, full, memo, used | bit, value));
    }
    memo[index] = total;
    return total;
}

static uint64_t brute_count(int size, int difference)
{
    if (size < 0 || size > MAX_BRUTE_SIZE) {
        die("independent DP size exceeds its checked exact range");
    }
    size_t mask_count = (size_t)1U << size;
    size_t state_count = mask_count * (size_t)(size + 1);
    uint64_t *memo = xrealloc(NULL, state_count, sizeof(*memo));
    for (size_t i = 0; i < state_count; ++i) {
        memo[i] = UINT64_MAX;
    }
    uint32_t full = ((uint32_t)1U << size) - 1U;
    uint64_t result = brute_recursive(
        size, difference, full, memo, 0U, size);
    free(memo);
    return result;
}

static int check_implementation(int maximum_n)
{
    if (maximum_n > KNOWN_MAX_N) {
        fprintf(stderr, "error: CHECK_N must be in 0..%d: %d\n",
                KNOWN_MAX_N, maximum_n);
        return EXIT_FAILURE;
    }

    mpz_t value;
    mpz_init(value);
    for (int n = 0; n <= maximum_n; ++n) {
        compute_value(value, n, DEFAULT_K, false);
        if (n == 0) {
            continue;
        }
        uint64_t direct = brute_count(n + DEFAULT_K, n);
        if (mpz_cmp_ui(value, (unsigned long)direct) != 0) {
            gmp_fprintf(stderr,
                        "error: independent DP mismatch at k=%d, n=%d: "
                        "formula=%Zd, DP=%lu\n",
                        DEFAULT_K, n, value, (unsigned long)direct);
            mpz_clear(value);
            return EXIT_FAILURE;
        }
    }
    mpz_clear(value);

    printf("ok: tiling-partition formula agrees with built-in "
           "k=%d terms for n=0..%d and independent subset DP for "
           "n=1..%d\n",
           DEFAULT_K, maximum_n, maximum_n);
    return EXIT_SUCCESS;
}

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s [MAX_N]\n"
            "       %s --term N\n"
            "       %s --upto MAX_N\n"
            "       %s --k K [MAX_N]\n"
            "       %s --k K --term N\n"
            "       %s --k K --upto MAX_N\n"
            "       %s --check [CHECK_N]\n"
            "\n"
            "K defaults to %d and may be 0..%d.\n"
            "MAX_N defaults to %d and may be 0..%d.\n"
            "N+K must be at most %d.\n"
            "Term n counts permutations of [n+k] avoiding difference n.\n"
            "--upto fixes k and prints a(0)..a(MAX_N), comma-separated.\n"
            "Normal calculation uses the Spahn--Zeilberger "
            "tiling-partition formula.\n"
            "No b-file is created.\n",
            program, program, program, program, program, program, program,
            DEFAULT_K, MAX_SUPPORTED_TOTAL_SIZE,
            DEFAULT_MAX_N, MAX_SUPPORTED_TOTAL_SIZE,
            MAX_SUPPORTED_TOTAL_SIZE);
}

int main(int argc, char **argv)
{
    const char *program = argv[0];
    if (argc == 2 &&
        (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
        usage(program);
        return EXIT_SUCCESS;
    }

    if (argc >= 2 && strcmp(argv[1], "--check") == 0) {
        if (argc > 3) {
            usage(program);
            return EXIT_FAILURE;
        }
        int maximum_n = argc == 3 ?
            parse_integer(argv[2], "CHECK_N", 0, KNOWN_MAX_N) :
            DEFAULT_CHECK_N;
        return check_implementation(maximum_n);
    }

    int k = DEFAULT_K;
    int n = -1;
    bool have_k = false;
    OutputMode mode = MODE_UPTO;

    for (int argument = 1; argument < argc; ++argument) {
        const char *text = argv[argument];
        if (strcmp(text, "--k") == 0) {
            if (have_k || argument + 1 >= argc) {
                usage(program);
                return EXIT_FAILURE;
            }
            k = parse_k(argv[++argument]);
            have_k = true;
        } else if (strcmp(text, "--term") == 0 ||
                   strcmp(text, "--upto") == 0) {
            if (n >= 0 || argument + 1 >= argc) {
                usage(program);
                return EXIT_FAILURE;
            }
            mode = strcmp(text, "--term") == 0 ? MODE_TERM : MODE_UPTO;
            n = parse_n(argv[++argument],
                        mode == MODE_TERM ? "N" : "MAX_N");
        } else {
            if (text[0] == '-' || n >= 0) {
                usage(program);
                return EXIT_FAILURE;
            }
            n = parse_n(text, "MAX_N");
            mode = MODE_UPTO;
        }
    }

    if (n < 0) {
        n = DEFAULT_MAX_N;
    }
    validate_total_size(n, k);
    if (mode == MODE_UPTO) {
        return print_sequence(n, k);
    }

    mpz_t value;
    mpz_init(value);
    compute_value(value, n, k, true);
    printf("%d ", n);
    print_value(value);
    putchar('\n');
    mpz_clear(value);
    return EXIT_SUCCESS;
}
