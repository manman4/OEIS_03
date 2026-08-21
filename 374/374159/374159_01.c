/*
 * A374159: least k having exactly n positive solutions of
 *
 *                    x^2 + 7 y^2 = k.
 *
 * This program does not scan every integer k.  It exhausts the much smaller
 * set of possible prime-exponent patterns, which is an exact check of the
 * whole interval (and in fact gives a(n) directly).
 *
 * Put D(k) = product (e_p+1), over primes p whose residue modulo 7 is
 * 1, 2, or 4.  For every represented k, let e=v_2(k) and put
 *
 *     C(e) = 1       if e=0,
 *            0       if e=1,
 *            e-1     if e>=2.
 *
 * The number of positive solutions is
 *
 *     R(k) = (C(e) D(k) - A(k))/2,
 *
 * where A(k)=1 if k is a square or seven times a square, and A(k)=0
 * otherwise.  An odd exponent at a prime congruent to 3, 5, or 6 modulo 7
 * gives no representations.
 *
 * Hence C D=2n+A.  The program enumerates every divisor C of 2n (A=0)
 * and every divisor C of 2n+1 (A=1), together with every multiplicative
 * partition D=product(e_i+1).
 * A multiplicative partition D=product(e_i+1) lists every possible exponent
 * multiset.  Its smallest odd part assigns the largest e_i to 11, the next
 * to 23, then 29, 37, etc.  For C>=2 the least 2-adic factor is 2^(C+1).
 * Powers of 7 and squares of inert primes only increase k.  Thus the search
 * proves minimality without iterating through all k below the answer.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic 374159_01.c \
 *       -o 374159_01 $(pkg-config --cflags --libs gmp)
 *
 * Examples:
 *   ./374159_01 --target 23 --limit 2^40 --verbose
 *   ./374159_01 --upto 23
 *   ./374159_01 --self-test 1000000
 */

#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>

#define MAX_FACTORS 64
#define MAX_TARGET UINT64_C(1000000000)
#define GUARANTEED_UPTO UINT64_C(100)
#define MAX_LIMIT_EXPONENT UINT64_C(1000000)
#define DEFAULT_TARGET UINT64_C(23)
#define DEFAULT_TEST_LIMIT UINT64_C(1000000)
#define BFILE_NAME "b374159.txt"
#define BFILE_TEMP_NAME "b374159.txt.tmp"

typedef struct {
    bool found;
    mpz_t value;
    uint64_t d;
    uint64_t c;
    bool axis;
    unsigned exponent_count;
    uint64_t exponents[MAX_FACTORS];
} Answer;

typedef struct {
    uint64_t factors[MAX_FACTORS];
    unsigned factor_count;
    uint64_t primes[MAX_FACTORS];
    Answer best;
} Search;

typedef struct {
    uint64_t n;
    uint64_t value;
    bool upper_bound;
} Regression;

static _Noreturn void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static bool is_prime(uint64_t n)
{
    if (n < 2) return false;
    if ((n & 1) == 0) return n == 2;
    for (uint64_t d = 3; d <= n / d; d += 2)
        if (n % d == 0) return false;
    return true;
}

static void make_split_primes(uint64_t primes[MAX_FACTORS])
{
    unsigned count = 0;
    for (uint64_t p = 11; count < MAX_FACTORS; p += 2) {
        const uint64_t residue = p % 7;
        if ((residue == 1 || residue == 2 || residue == 4) && is_prime(p))
            primes[count++] = p;
    }
}

static void answer_init(Answer *answer)
{
    memset(answer, 0, sizeof(*answer));
    mpz_init(answer->value);
}

static void answer_clear(Answer *answer)
{
    mpz_clear(answer->value);
}

static void answer_copy(Answer *destination, const Answer *source)
{
    destination->found = source->found;
    mpz_set(destination->value, source->value);
    destination->d = source->d;
    destination->c = source->c;
    destination->axis = source->axis;
    destination->exponent_count = source->exponent_count;
    memcpy(destination->exponents, source->exponents,
           sizeof(destination->exponents));
}

static void print_bigint(FILE *stream, const mpz_t value)
{
    if (mpz_out_str(stream, 10, value) == 0)
        die("could not write a big integer");
}

static void parse_bigint_decimal(mpz_t result, const char *text,
                                 const char *name)
{
    if (*text == '\0') die("empty integer argument");
    for (const unsigned char *p = (const unsigned char *)text; *p; ++p) {
        if (*p < '0' || *p > '9') {
            fprintf(stderr, "error: invalid %s: %s\n", name, text);
            exit(EXIT_FAILURE);
        }
    }
    if (mpz_set_str(result, text, 10) != 0)
        die("could not parse a nonnegative integer");
}

static uint64_t parse_u64(const char *text, uint64_t maximum,
                          const char *name);

static void parse_limit(mpz_t result, const char *text)
{
    const char *caret = strchr(text, '^');
    if (caret == NULL) {
        parse_bigint_decimal(result, text, "limit");
        return;
    }
    if (caret == text || caret[1] == '\0' || strchr(caret + 1, '^') != NULL)
        die("limit power must have the form B^E");
    char base_text[40];
    const size_t length = (size_t)(caret - text);
    if (length >= sizeof(base_text)) die("power base is too long");
    memcpy(base_text, text, length);
    base_text[length] = '\0';
    mpz_t base;
    mpz_init(base);
    parse_bigint_decimal(base, base_text, "power base");
    const uint64_t exponent =
        parse_u64(caret + 1, MAX_LIMIT_EXPONENT, "power exponent");
    mpz_pow_ui(result, base, (unsigned long)exponent);
    mpz_clear(base);
}

static uint64_t parse_u64(const char *text, uint64_t maximum,
                          const char *name)
{
    errno = 0;
    char *end = NULL;
    const uintmax_t value = strtoumax(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' || value > maximum) {
        fprintf(stderr, "error: %s must be in 0..%" PRIu64 ": %s\n",
                name, maximum, text);
        exit(EXIT_FAILURE);
    }
    return (uint64_t)value;
}

static void consider_partition(Search *search, uint64_t d, uint64_t c,
                               bool axis)
{
    bool all_even = true;
    for (unsigned i = 0; i < search->factor_count; ++i)
        if (((search->factors[i] - 1) & 1) != 0) all_even = false;
    if (axis) {
        if (!all_even) return;
    } else if ((c & 1) != 0 && all_even) {
        return;
    }

    mpz_t value, power;
    mpz_init_set_ui(value, 1);
    mpz_init(power);
    if (c >= 2) {
        mpz_ui_pow_ui(value, 2, (unsigned long)(c + 1));
    }
    for (unsigned i = 0; i < search->factor_count; ++i) {
        const uint64_t exponent =
            search->factors[search->factor_count - 1 - i] - 1;
        mpz_ui_pow_ui(power, (unsigned long)search->primes[i],
                      (unsigned long)exponent);
        mpz_mul(value, value, power);
    }
    if (!search->best.found || mpz_cmp(value, search->best.value) < 0) {
        search->best.found = true;
        mpz_set(search->best.value, value);
        search->best.d = d;
        search->best.c = c;
        search->best.axis = axis;
        search->best.exponent_count = search->factor_count;
        for (unsigned i = 0; i < search->factor_count; ++i)
            search->best.exponents[i] =
                search->factors[search->factor_count - 1 - i] - 1;
    }
    mpz_clear(power);
    mpz_clear(value);
}

static void partition_rec(Search *search, uint64_t remaining,
                          uint64_t minimum_factor, uint64_t d,
                          uint64_t c, bool axis)
{
    if (remaining == 1) {
        consider_partition(search, d, c, axis);
        return;
    }
    if (search->factor_count == MAX_FACTORS) die("too many partition factors");

    for (uint64_t factor = minimum_factor; factor <= remaining / factor;
         ++factor) {
        if (remaining % factor != 0) continue;
        search->factors[search->factor_count++] = factor;
        partition_rec(search, remaining / factor, factor, d, c, axis);
        --search->factor_count;
    }
    if (remaining >= minimum_factor) {
        search->factors[search->factor_count++] = remaining;
        partition_rec(search, 1, remaining, d, c, axis);
        --search->factor_count;
    }
}

static void search_case(Search *search, uint64_t d, uint64_t c, bool axis)
{
    if (d == 0) return;
    search->factor_count = 0;
    partition_rec(search, d, 2, d, c, axis);
}

static void search_divisor_cases(Search *search, uint64_t total, bool axis)
{
    for (uint64_t divisor = 1; divisor <= total / divisor; ++divisor) {
        if (total % divisor != 0) continue;
        search_case(search, total / divisor, divisor, axis);
        const uint64_t paired = total / divisor;
        if (paired != divisor)
            search_case(search, divisor, paired, axis);
    }
}

static void calculate_answer(uint64_t n, Answer *answer)
{
    Search search;
    memset(&search, 0, sizeof(search));
    answer_init(&search.best);
    make_split_primes(search.primes);
    if (n == 0) {
        search.best.found = true;
        mpz_set_ui(search.best.value, 0);
        answer_copy(answer, &search.best);
        answer_clear(&search.best);
        return;
    }
    const uint64_t twice = 2 * n;
    search_divisor_cases(&search, twice, false);
    search_divisor_cases(&search, twice + 1, true);
    answer_copy(answer, &search.best);
    answer_clear(&search.best);
}

static uint64_t isqrt_u64(uint64_t n)
{
    uint64_t lo = 0, hi = UINT64_C(1) << 32;
    while (lo + 1 < hi) {
        const uint64_t mid = lo + (hi - lo) / 2;
        if (mid <= n / mid) lo = mid;
        else hi = mid;
    }
    return lo;
}

static unsigned representation_count_formula(uint64_t k)
{
    if (k == 0) return 0;
    uint64_t remaining = k;
    unsigned v2 = 0;
    while ((remaining & 1) == 0) {
        remaining >>= 1;
        ++v2;
    }
    if (v2 == 1) return 0;

    uint64_t d = 1;
    for (uint64_t p = 3; p <= remaining / p; p += 2) {
        if (remaining % p != 0) continue;
        unsigned exponent = 0;
        do {
            remaining /= p;
            ++exponent;
        } while (remaining % p == 0);
        const uint64_t residue = p % 7;
        if (residue == 1 || residue == 2 || residue == 4)
            d *= exponent + 1;
        else if (p != 7 && (exponent & 1))
            return 0;
    }
    if (remaining > 1) {
        const uint64_t residue = remaining % 7;
        if (residue == 1 || residue == 2 || residue == 4)
            d *= 2;
        else if (remaining != 7)
            return 0;
    }

    unsigned s_axis = 0;
    const uint64_t root = isqrt_u64(k);
    if (root * root == k) ++s_axis;
    if (k % 7 == 0) {
        const uint64_t q = k / 7;
        const uint64_t root7 = isqrt_u64(q);
        if (root7 * root7 == q) ++s_axis;
    }
    const uint64_t c = v2 == 0 ? 1 : v2 - 1;
    return (unsigned)((c * d - s_axis) / 2);
}

static void self_test(uint64_t limit)
{
    if (limit > UINT32_MAX) die("self-test limit must be at most 2^32-1");
    uint16_t *direct = calloc((size_t)limit + 1, sizeof(*direct));
    if (direct == NULL) die("could not allocate self-test table");
    for (uint64_t y = 1; 7 * y * y <= limit; ++y)
        for (uint64_t x = 1; x * x <= limit - 7 * y * y; ++x)
            ++direct[x * x + 7 * y * y];

    uint64_t minima[GUARANTEED_UPTO + 1];
    for (uint64_t n = 0; n <= GUARANTEED_UPTO; ++n)
        minima[n] = UINT64_MAX;
    for (uint64_t k = 0; k <= limit; ++k) {
        const unsigned got = representation_count_formula(k);
        if (got != direct[k]) {
            fprintf(stderr, "self-test failed at k=%" PRIu64
                    ": formula=%u direct=%u\n", k, got, direct[k]);
            free(direct);
            exit(EXIT_FAILURE);
        }
        if (got <= GUARANTEED_UPTO && minima[got] == UINT64_MAX)
            minima[got] = k;
    }

    /* Values recorded in the A374159 data and comments. */
    static const Regression oeis_regressions[] = {
        {0, 0, false}, {1, 8, false}, {2, 32, false},
        {3, 128, false}, {4, 352, false}, {5, 704, false},
        {6, 1408, false}, {7, 2816, false}, {8, 5632, false},
        {9, 11264, false}, {10, 16192, false}, {11, 45056, false},
        {12, 32384, false}, {13, 123904, false}, {14, 64768, false},
        {15, 178112, false}, {16, 129536, false},
        {17, 2883584, false}, {18, 259072, false},
        {19, 1982464, false}, {20, 469568, false},
        {21, 712448, false}, {22, 1036288, false},
        {23, 184549376, false}, {24, 939136, false},
        {25, 21551552, false}, {26, 4145152, false},
        {27, 2849792, false}, {28, 1878272, false},
        {29, 11811160064, false}, {30, 5165248, false},
        {31, 16386304, false}, {32, 3756544, false},
        {33, 11399168, false}, {34, 66322432, false},
        {35, 86206208, false}, {36, 7513088, false}
    };
    for (size_t i = 0;
         i < sizeof(oeis_regressions) / sizeof(oeis_regressions[0]); ++i) {
        const Regression *regression = &oeis_regressions[i];
        Answer answer;
        answer_init(&answer);
        calculate_answer(regression->n, &answer);
        const int comparison =
            mpz_cmp_ui(answer.value, (unsigned long)regression->value);
        const bool passed =
            answer.found &&
            (regression->upper_bound ? comparison <= 0 : comparison == 0);
        if (!passed) {
            fprintf(stderr, "OEIS regression failed at a(%" PRIu64 ")",
                    regression->n);
            if (regression->upper_bound) {
                fprintf(stderr, " <= ");
            } else {
                fprintf(stderr, " = ");
            }
            fprintf(stderr, "%" PRIu64 "\n", regression->value);
            answer_clear(&answer);
            free(direct);
            exit(EXIT_FAILURE);
        }
        answer_clear(&answer);
    }

    for (uint64_t n = 0; n <= GUARANTEED_UPTO; ++n) {
        Answer answer;
        answer_init(&answer);
        calculate_answer(n, &answer);
        bool passed = answer.found;
        if (n != 0) {
            uint64_t reconstructed_d = 1;
            for (unsigned i = 0; i < answer.exponent_count; ++i)
                reconstructed_d *= answer.exponents[i] + 1;
            const uint64_t numerator =
                answer.c * reconstructed_d - (answer.axis ? 1 : 0);
            passed = passed && reconstructed_d == answer.d &&
                     numerator == 2 * n;
        }
        if (passed && mpz_fits_ulong_p(answer.value)) {
            const unsigned long value = mpz_get_ui(answer.value);
            passed = (uint64_t)value == value &&
                     representation_count_formula((uint64_t)value) == n;
            if ((uint64_t)value <= limit)
                passed = passed && minima[n] == (uint64_t)value;
        }
        if (!passed) {
            fprintf(stderr,
                    "guaranteed-range structural test failed at n=%" PRIu64
                    "\n", n);
            answer_clear(&answer);
            free(direct);
            exit(EXIT_FAILURE);
        }
        answer_clear(&answer);
    }
    free(direct);
    printf("self-test passed: direct enumeration agrees for 0 <= k <= %"
           PRIu64 "; all minima within that range and %zu OEIS regressions "
           "agree, and exact structural checks pass through n=%" PRIu64 "\n",
           limit, sizeof(oeis_regressions) / sizeof(oeis_regressions[0]),
           GUARANTEED_UPTO);
}

static void print_answer(uint64_t n, const Answer *answer, bool have_limit,
                         const mpz_t limit, bool verbose)
{
    printf("a(%" PRIu64 ") = ", n);
    print_bigint(stdout, answer->value);
    putchar('\n');
    if (have_limit) {
        if (mpz_cmp(answer->value, limit) > 0) {
            printf("there is no k <= ");
            print_bigint(stdout, limit);
            printf(" with exactly %" PRIu64 " positive representations\n", n);
        } else {
            printf("the least solution is within the checked bound ");
            print_bigint(stdout, limit);
            putchar('\n');
        }
    }
    if (verbose && n != 0) {
        uint64_t primes[MAX_FACTORS];
        make_split_primes(primes);
        printf("case: C=%" PRIu64 ", A=%u, D=%" PRIu64
               "; factorization: ",
               answer->c, answer->axis ? 1U : 0U, answer->d);
        bool wrote = false;
        if (answer->c >= 2) {
            printf("2^%" PRIu64, answer->c + 1);
            wrote = true;
        }
        for (unsigned i = 0; i < answer->exponent_count; ++i) {
            if (wrote) printf(" * ");
            printf("%" PRIu64 "^%" PRIu64, primes[i], answer->exponents[i]);
            wrote = true;
        }
        if (!wrote) putchar('1');
        putchar('\n');
    }
}

static void write_bfile_term(FILE *stream, uint64_t n, const Answer *answer)
{
    if (fprintf(stream, "%" PRIu64 " ", n) < 0)
        die("could not write a b-file index");
    print_bigint(stream, answer->value);
    if (fputc('\n', stream) == EOF)
        die("could not write a b-file term");
}

static void usage(const char *program)
{
    fprintf(stderr,
        "usage: %s [--target N] [--limit L] [--verbose]\n"
        "       %s --upto N [--limit L] [--no-bfile]\n"
        "       %s --self-test [K]\n"
        "--upto writes " BFILE_NAME " unless --no-bfile is given.\n"
        "Exact b-file output is guaranteed through at least n=%" PRIu64 ".\n"
        "L may be decimal or a power such as 2^40.\n",
        program, program, program, GUARANTEED_UPTO);
}

int main(int argc, char **argv)
{
    uint64_t target = DEFAULT_TARGET;
    bool upto = false, have_limit = false, verbose = false, test = false;
    bool write_bfile = true;
    uint64_t test_limit = DEFAULT_TEST_LIMIT;
    mpz_t limit;
    mpz_init(limit);

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--target") == 0 && i + 1 < argc) {
            target = parse_u64(argv[++i], MAX_TARGET, "target");
        } else if (strcmp(argv[i], "--upto") == 0 && i + 1 < argc) {
            target = parse_u64(argv[++i], MAX_TARGET, "upto");
            upto = true;
        } else if (strcmp(argv[i], "--limit") == 0 && i + 1 < argc) {
            parse_limit(limit, argv[++i]);
            have_limit = true;
        } else if (strcmp(argv[i], "--verbose") == 0) {
            verbose = true;
        } else if (strcmp(argv[i], "--no-bfile") == 0) {
            write_bfile = false;
        } else if (strcmp(argv[i], "--self-test") == 0) {
            test = true;
            if (i + 1 < argc && argv[i + 1][0] != '-')
                test_limit = parse_u64(argv[++i], UINT32_MAX,
                                       "self-test limit");
        } else if (strcmp(argv[i], "--help") == 0 ||
                   strcmp(argv[i], "-h") == 0) {
            usage(argv[0]);
            mpz_clear(limit);
            return EXIT_SUCCESS;
        } else {
            usage(argv[0]);
            mpz_clear(limit);
            return EXIT_FAILURE;
        }
    }
    if (test) {
        self_test(test_limit);
        mpz_clear(limit);
        return EXIT_SUCCESS;
    }

    FILE *bfile = NULL;
    if (upto && write_bfile) {
        bfile = fopen(BFILE_TEMP_NAME, "w");
        if (bfile == NULL) {
            fprintf(stderr, "error: could not open %s: %s\n",
                    BFILE_TEMP_NAME, strerror(errno));
            mpz_clear(limit);
            return EXIT_FAILURE;
        }
    }
    const uint64_t first = upto ? 0 : target;
    for (uint64_t n = first; n <= target; ++n) {
        Answer answer;
        answer_init(&answer);
        calculate_answer(n, &answer);
        if (!answer.found) die("internal search found no exponent pattern");
        print_answer(n, &answer, have_limit, limit, verbose);
        if (bfile != NULL) write_bfile_term(bfile, n, &answer);
        answer_clear(&answer);
    }
    if (bfile != NULL) {
        if (fclose(bfile) != 0) {
            fprintf(stderr, "error: could not close %s: %s\n",
                    BFILE_TEMP_NAME, strerror(errno));
            mpz_clear(limit);
            return EXIT_FAILURE;
        }
        if (rename(BFILE_TEMP_NAME, BFILE_NAME) != 0) {
            fprintf(stderr, "error: could not replace %s: %s\n",
                    BFILE_NAME, strerror(errno));
            mpz_clear(limit);
            return EXIT_FAILURE;
        }
        printf("wrote %s\n", BFILE_NAME);
    }
    mpz_clear(limit);
    return EXIT_SUCCESS;
}
