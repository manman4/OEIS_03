/*
 * A398599 and b_{1,s}(n) -- path-polynomial inclusion-exclusion.
 *
 * Count permutations p of {1,...,n} satisfying
 *
 *        |p(i+1) - p(i)| != s,   1 <= i < n.
 *
 * The forbidden-value graph has vertices 1,...,n and edges {j,j+s}.
 * It is the disjoint union of the s residue-class paths.  For a selected
 * edge set E with k edges and c nonempty connected components, collapse
 * each component to one block.  Exactly
 *
 *                         2^c (n-k)!
 *
 * permutations contain all selected adjacencies: every path component has
 * two orientations, and collapsing k edges leaves n-k blocks.  Therefore
 * inclusion-exclusion only needs k and c, not the individual component
 * lengths used by 398599_02.c and 398599_03.c.
 *
 * Let F_L(x) enumerate edge subsets of a path with L edges, with weight x
 * per selected edge and weight 2 per nonempty component.  A two-state
 * decomposition according to whether the last edge is selected gives
 *
 *   F_0(x)=1,  F_1(x)=1+2x,
 *   F_L(x)=(1+x)F_{L-1}(x)+xF_{L-2}(x).
 *
 * Write n=q*s+t, 0<=t<s.  There are t residue paths with q edges and s-t
 * paths with q-1 edges.  Hence, for n>=s,
 *
 *   P_{n,s}(x)=F_q(x)^t F_{q-1}(x)^(s-t),
 *
 *   b_{1,s}(n)=sum_k (-1)^k (n-k)! [x^k]P_{n,s}(x).
 *
 * This is a direct, rigorous inclusion-exclusion derivation.  It is a
 * separate implementation from the q/histogram convolution in 02 and the
 * tiling-partition formula in 03.  Polynomial coefficients are bounded by
 * 3^(n-s), which is below 2^128 for n<=60 and s>=1.  Checked unsigned
 * 128-bit arithmetic holds those coefficients, and GMP holds the final
 * alternating sum.
 *
 * A range run writes b398599_04_sS_part.txt, flushes every completed term,
 * and atomically renames it to b398599_04_sS.txt after successful
 * completion.  Thus its output cannot collide with 02, 03, or another s.
 *
 * Build:
 *
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic \
 *     -I/opt/homebrew/include -L/opt/homebrew/lib \
 *     398599_04.c -lgmp -o 398599_04
 *
 * Usage:
 *   ./398599_04 --upto 30
 *   ./398599_04 --term 30
 *   ./398599_04 --s 4 --upto 30
 *   ./398599_04 --check
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
#error "398599_04 requires a platform with 64-bit unsigned long"
#endif

#if !defined(__SIZEOF_INT128__)
#error "398599_04 requires unsigned __int128"
#endif

__extension__ typedef unsigned __int128 uint128_t;

#define DEFAULT_S 3
#define DEFAULT_MAX_N 20
#define DEFAULT_CHECK_N 30
#define MAX_SUPPORTED_N 60
#define MAX_SUPPORTED_S 128
#define KNOWN_MAX_N 30
#define BRUTE_CHECK_MAX_N 11
#define GENERAL_CHECK_MAX_S 8

static const char *const known_s3[KNOWN_MAX_N + 1] = {
    "1", "1", "2", "6", "12", "48", "240", "1392", "9936",
    "82416", "771888", "8081808", "93273936", "1174442160",
    "16012371024", "234865643952", "3686260363152",
    "61627946597424", "1093150647044880", "20501844922196976",
    "405313119744707088", "8423466060115604208",
    "183582624178893950736", "4186520700374895202800",
    "99697398784352956533264", "2474726013785813664809712",
    "63922390767402268644275472", "1715494609619150973923837424",
    "47765601143125284027333699600",
    "1378019056802003925032691803376",
    "41140957260201936774910629270288"
};

typedef struct {
    int degree;
    uint128_t coefficient[MAX_SUPPORTED_N + 1];
} Polynomial;

typedef enum {
    MODE_UPTO,
    MODE_TERM
} OutputMode;

static void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static void *xmalloc_array(size_t count, size_t size)
{
    if (size != 0U && count > SIZE_MAX / size) {
        die("allocation size overflow");
    }
    void *pointer = malloc(count * size);
    if (pointer == NULL) {
        die("out of memory");
    }
    return pointer;
}

static double monotonic_seconds(void)
{
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) != 0) {
        die("clock_gettime failed");
    }
    return (double)now.tv_sec + (double)now.tv_nsec / 1000000000.0;
}

static uint128_t add_u128(uint128_t left, uint128_t right)
{
    if (left > ~(uint128_t)0 - right) {
        die("unsigned 128-bit addition overflow");
    }
    return left + right;
}

static uint128_t multiply_u128(uint128_t left, uint128_t right)
{
    if (left != 0 && right > ~(uint128_t)0 / left) {
        die("unsigned 128-bit multiplication overflow");
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

static void polynomial_zero(Polynomial *polynomial)
{
    polynomial->degree = 0;
    memset(polynomial->coefficient, 0, sizeof(polynomial->coefficient));
}

static void make_path_polynomials(
    Polynomial path[MAX_SUPPORTED_N + 1], int maximum_length)
{
    for (int i = 0; i <= maximum_length; ++i) {
        polynomial_zero(&path[i]);
    }
    path[0].coefficient[0] = UINT64_C(1);
    if (maximum_length == 0) {
        return;
    }

    path[1].degree = 1;
    path[1].coefficient[0] = UINT64_C(1);
    path[1].coefficient[1] = UINT64_C(2);

    for (int length = 2; length <= maximum_length; ++length) {
        path[length].degree = length;
        for (int k = 0; k <= length; ++k) {
            uint128_t value = path[length - 1].coefficient[k];
            if (k > 0) {
                value = add_u128(
                    value, path[length - 1].coefficient[k - 1]);
                value = add_u128(
                    value, path[length - 2].coefficient[k - 1]);
            }
            path[length].coefficient[k] = value;
        }
    }
}

static void polynomial_multiply(Polynomial *result,
                                const Polynomial *left,
                                const Polynomial *right,
                                int maximum_degree)
{
    uint128_t accumulator[MAX_SUPPORTED_N + 1] = {0};
    int degree = left->degree + right->degree;
    if (degree > maximum_degree) {
        degree = maximum_degree;
    }

    for (int i = 0; i <= left->degree; ++i) {
        int maximum_j = right->degree;
        if (i + maximum_j > maximum_degree) {
            maximum_j = maximum_degree - i;
        }
        for (int j = 0; j <= maximum_j; ++j) {
            uint128_t product = multiply_u128(
                left->coefficient[i], right->coefficient[j]);
            accumulator[i + j] =
                add_u128(accumulator[i + j], product);
        }
    }

    polynomial_zero(result);
    result->degree = degree;
    for (int k = 0; k <= degree; ++k) {
        result->coefficient[k] = accumulator[k];
    }
}

static void make_forbidden_graph_polynomial(Polynomial *result,
                                            int n, int s)
{
    if (n <= s) {
        polynomial_zero(result);
        result->coefficient[0] = UINT64_C(1);
        return;
    }

    int quotient = n / s;
    int remainder = n % s;
    Polynomial path[MAX_SUPPORTED_N + 1];
    make_path_polynomials(path, quotient);

    Polynomial current;
    polynomial_zero(&current);
    current.coefficient[0] = UINT64_C(1);
    int maximum_degree = n - s;

    for (int residue = 0; residue < s; ++residue) {
        int edge_count = residue < remainder ? quotient : quotient - 1;
        Polynomial following;
        polynomial_multiply(&following, &current, &path[edge_count],
                            maximum_degree);
        current = following;
    }
    *result = current;

    if (result->degree != maximum_degree) {
        die("forbidden-graph polynomial has the wrong degree");
    }

    uint128_t sum = 0;
    for (int k = 0; k <= result->degree; ++k) {
        sum = add_u128(sum, result->coefficient[k]);
    }
    uint128_t bound = 1;
    for (int edge = 0; edge < maximum_degree; ++edge) {
        bound = multiply_u128(bound, 3U);
    }
    if (sum > bound) {
        die("path-polynomial coefficient bound failed");
    }
}

static void compute_formula(mpz_t result, int n, int s,
                            int *polynomial_degree)
{
    Polynomial graph;
    make_forbidden_graph_polynomial(&graph, n, s);
    *polynomial_degree = graph.degree;

    mpz_set_ui(result, 0UL);
    mpz_t term;
    mpz_t coefficient;
    mpz_init(term);
    mpz_init(coefficient);
    for (int k = 0; k <= graph.degree; ++k) {
        mpz_fac_ui(term, (unsigned long)(n - k));
        mpz_set_u128(coefficient, graph.coefficient[k]);
        mpz_mul(term, term, coefficient);
        if ((k & 1) == 0) {
            mpz_add(result, result, term);
        } else {
            mpz_sub(result, result, term);
        }
    }
    mpz_clear(coefficient);
    mpz_clear(term);

    mpz_t upper;
    mpz_init(upper);
    mpz_fac_ui(upper, (unsigned long)n);
    if (mpz_sgn(result) < 0 || mpz_cmp(result, upper) > 0) {
        mpz_clear(upper);
        die("formula result is outside 0..n!");
    }
    mpz_clear(upper);
}

static void verify_known(const mpz_t value, int n, int s)
{
    if (s != DEFAULT_S || n > KNOWN_MAX_N) {
        return;
    }
    mpz_t expected;
    mpz_init(expected);
    if (mpz_set_str(expected, known_s3[n], 10) != 0 ||
        mpz_cmp(value, expected) != 0) {
        gmp_fprintf(stderr,
                    "error: A398599 mismatch at n=%d: got %Zd, "
                    "expected %s\n",
                    n, value, known_s3[n]);
        mpz_clear(expected);
        exit(EXIT_FAILURE);
    }
    mpz_clear(expected);
}

static void compute_value(mpz_t result, int n, int s, bool verbose)
{
    double started = monotonic_seconds();
    int degree = 0;
    compute_formula(result, n, s, &degree);
    verify_known(result, n, s);
    if (verbose) {
        fprintf(stderr,
                "398599_04: s=%d, n=%d, polynomial degree=%d, %.6f s\n",
                s, n, degree, monotonic_seconds() - started);
    }
}

static uint64_t brute_recursive(int n, int s, uint32_t full,
                                uint64_t *memo, uint32_t used, int last)
{
    if (used == full) {
        return UINT64_C(1);
    }
    size_t index = (size_t)used * (size_t)(n + 1) + (size_t)last;
    if (memo[index] != UINT64_MAX) {
        return memo[index];
    }

    uint64_t total = 0;
    uint32_t available = full ^ used;
    while (available != 0U) {
        uint32_t bit = available & (0U - available);
        available ^= bit;
        int value = __builtin_ctz(bit);
        if (last < n) {
            int difference = value - last;
            if (difference == s || difference == -s) {
                continue;
            }
        }
        total += brute_recursive(n, s, full, memo, used | bit, value);
    }
    memo[index] = total;
    return total;
}

static uint64_t brute_count(int n, int s)
{
    size_t state_count = ((size_t)1U << n) * (size_t)(n + 1);
    uint64_t *memo = xmalloc_array(state_count, sizeof(*memo));
    for (size_t i = 0; i < state_count; ++i) {
        memo[i] = UINT64_MAX;
    }
    uint32_t full = ((uint32_t)1U << n) - 1U;
    uint64_t result = brute_recursive(n, s, full, memo, 0U, n);
    free(memo);
    return result;
}

static int check_implementation(int maximum_n)
{
    mpz_t value;
    mpz_init(value);
    for (int n = 0; n <= maximum_n; ++n) {
        compute_value(value, n, DEFAULT_S, false);
    }

    int brute_maximum = maximum_n < BRUTE_CHECK_MAX_N ?
                        maximum_n : BRUTE_CHECK_MAX_N;
    for (int s = 1; s <= GENERAL_CHECK_MAX_S; ++s) {
        for (int n = 0; n <= brute_maximum; ++n) {
            compute_formula(value, n, s, &(int){0});
            uint64_t direct = brute_count(n, s);
            if (mpz_cmp_ui(value, (unsigned long)direct) != 0) {
                gmp_fprintf(stderr,
                            "error: DP mismatch at s=%d, n=%d: "
                            "formula=%Zd, DP=%llu\n",
                            s, n, value, (unsigned long long)direct);
                mpz_clear(value);
                return EXIT_FAILURE;
            }
        }
    }
    mpz_clear(value);
    printf("ok: A398599 verified for n=0..%d; independent subset DP "
           "for s=1..%d, n=0..%d\n",
           maximum_n, GENERAL_CHECK_MAX_S, brute_maximum);
    return EXIT_SUCCESS;
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

static void print_value(const mpz_t value)
{
    if (mpz_out_str(stdout, 10, value) == 0U) {
        die("could not write result");
    }
}

static int write_b_file(int maximum_n, int s)
{
    char part_name[80];
    char final_name[80];
    int part_length = snprintf(part_name, sizeof(part_name),
                               "b398599_04_s%d_part.txt", s);
    int final_length = snprintf(final_name, sizeof(final_name),
                                "b398599_04_s%d.txt", s);
    if (part_length < 0 || (size_t)part_length >= sizeof(part_name) ||
        final_length < 0 || (size_t)final_length >= sizeof(final_name)) {
        die("b-file name is too long");
    }

    FILE *file = fopen(part_name, "w");
    if (file == NULL) {
        fprintf(stderr, "error: cannot open %s: %s\n",
                part_name, strerror(errno));
        return EXIT_FAILURE;
    }

    mpz_t value;
    mpz_init(value);
    for (int n = 0; n <= maximum_n; ++n) {
        compute_value(value, n, s, false);
        if (gmp_fprintf(file, "%d %Zd\n", n, value) < 0 ||
            fflush(file) != 0) {
            fprintf(stderr, "error: cannot write %s: %s\n",
                    part_name, strerror(errno));
            mpz_clear(value);
            (void)fclose(file);
            return EXIT_FAILURE;
        }
    }
    mpz_clear(value);

    if (fclose(file) != 0) {
        fprintf(stderr, "error: cannot close %s: %s\n",
                part_name, strerror(errno));
        return EXIT_FAILURE;
    }
    if (rename(part_name, final_name) != 0) {
        fprintf(stderr, "error: cannot rename %s to %s: %s\n",
                part_name, final_name, strerror(errno));
        return EXIT_FAILURE;
    }
    printf("wrote %s (n=0..%d, s=%d)\n", final_name, maximum_n, s);
    return EXIT_SUCCESS;
}

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s [MAX_N]\n"
            "       %s --term N\n"
            "       %s --upto MAX_N\n"
            "       %s --s S --term N\n"
            "       %s --s S --upto MAX_N\n"
            "       %s --check [MAX_N]\n"
            "S defaults to %d; MAX_N defaults to %d.\n"
            "A range run writes b398599_04_sS.txt in the current directory.\n",
            program, program, program, program, program, program,
            DEFAULT_S, DEFAULT_MAX_N);
}

int main(int argc, char **argv)
{
    if (argc == 2 &&
        (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
        usage(argv[0]);
        return EXIT_SUCCESS;
    }
    if (argc >= 2 && strcmp(argv[1], "--check") == 0) {
        if (argc > 3) {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
        int maximum = argc == 3 ?
            parse_integer(argv[2], "MAX_N", 0, KNOWN_MAX_N) :
            DEFAULT_CHECK_N;
        return check_implementation(maximum);
    }

    int s = DEFAULT_S;
    int n = -1;
    bool have_s = false;
    OutputMode mode = MODE_UPTO;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--s") == 0) {
            if (have_s || i + 1 >= argc) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            s = parse_integer(argv[++i], "S", 1, MAX_SUPPORTED_S);
            have_s = true;
        } else if (strcmp(argv[i], "--term") == 0 ||
                   strcmp(argv[i], "--upto") == 0) {
            if (n >= 0 || i + 1 >= argc) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            mode = strcmp(argv[i], "--term") == 0 ? MODE_TERM : MODE_UPTO;
            n = parse_integer(argv[++i],
                              mode == MODE_TERM ? "N" : "MAX_N",
                              0, MAX_SUPPORTED_N);
        } else {
            if (argv[i][0] == '-' || n >= 0) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            n = parse_integer(argv[i], "MAX_N", 0, MAX_SUPPORTED_N);
            mode = MODE_UPTO;
        }
    }
    if (n < 0) {
        n = DEFAULT_MAX_N;
    }
    if (mode == MODE_UPTO) {
        return write_b_file(n, s);
    }

    mpz_t value;
    mpz_init(value);
    compute_value(value, n, s, true);
    printf("%d ", n);
    print_value(value);
    putchar('\n');
    mpz_clear(value);
    return EXIT_SUCCESS;
}
