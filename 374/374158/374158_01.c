/*
 * A374158: least k having exactly n positive solutions of
 *
 *                    x^2 + 3 y^2 = k.
 *
 * This program does not scan every integer k.  It exhausts the much smaller
 * set of possible prime-exponent patterns, which is an exact check of the
 * whole interval (and in fact gives a(n) directly).
 *
 * Put D(k) = product (e_p+1), over primes p == 1 (mod 3).  For every k that
 * is representable,
 *
 *     R(k) = (C D(k) - S(k))/2,
 *
 * where C=1 if v_2(k)=0, C=3 if positive v_2(k) is even, and S=1 if k is a
 * square or three times a square (otherwise S=0).  Odd v_2(k), or an odd
 * exponent at an odd prime p == 2 (mod 3), gives no representations.
 *
 * Hence R(k)=n permits only D=2n+S for C=1 or D=(2n+S)/3 for C=3.
 * A multiplicative partition D=product(e_i+1) lists every possible exponent
 * multiset.  Its smallest integer assigns the largest e_i to 7, the next to
 * 13, etc.  Factors 3 and squares of inert primes only increase k; C=3 adds
 * the least possible factor 2^2.  Thus the search proves minimality without
 * iterating through all k below the answer.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic 374158_01.c \
 *       -o 374158_01 $(pkg-config --cflags --libs gmp)
 *
 * Examples:
 *   ./374158_01 --target 23 --limit 2^40 --verbose
 *   ./374158_01 --upto 23
 *   ./374158_01 --self-test 1000000
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
#define BFILE_NAME "b374158.txt"
#define BFILE_TEMP_NAME "b374158.txt.tmp"

typedef struct {
    bool found;
    mpz_t value;
    uint64_t d;
    unsigned c;
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
    for (uint64_t p = 7; count < MAX_FACTORS; p += 2)
        if (p % 3 == 1 && is_prime(p)) primes[count++] = p;
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

static void consider_partition(Search *search, uint64_t d, unsigned c,
                               bool axis)
{
    bool all_even = true;
    for (unsigned i = 0; i < search->factor_count; ++i)
        if (((search->factors[i] - 1) & 1) != 0) all_even = false;
    if (all_even != axis) return;

    mpz_t value, power;
    mpz_init_set_ui(value, c == 3 ? 4 : 1);
    mpz_init(power);
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
                          unsigned c, bool axis)
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

static void search_case(Search *search, uint64_t d, unsigned c, bool axis)
{
    if (d == 0) return;
    search->factor_count = 0;
    partition_rec(search, d, 2, d, c, axis);
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
    search_case(&search, twice, 1, false);
    search_case(&search, twice + 1, 1, true);
    if (twice % 3 == 0) search_case(&search, twice / 3, 3, false);
    if ((twice + 1) % 3 == 0)
        search_case(&search, (twice + 1) / 3, 3, true);
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
    if (v2 & 1) return 0;

    uint64_t d = 1;
    for (uint64_t p = 3; p <= remaining / p; p += 2) {
        if (remaining % p != 0) continue;
        unsigned exponent = 0;
        do {
            remaining /= p;
            ++exponent;
        } while (remaining % p == 0);
        if (p % 3 == 1) d *= exponent + 1;
        else if (p != 3 && (exponent & 1)) return 0;
    }
    if (remaining > 1) {
        if (remaining % 3 == 1) d *= 2;
        else if (remaining != 3) return 0;
    }

    unsigned s_axis = 0;
    const uint64_t root = isqrt_u64(k);
    if (root * root == k) ++s_axis;
    if (k % 3 == 0) {
        const uint64_t q = k / 3;
        const uint64_t root3 = isqrt_u64(q);
        if (root3 * root3 == q) ++s_axis;
    }
    return (unsigned)(((v2 == 0 ? 1 : 3) * d - s_axis) / 2);
}

static void self_test(uint64_t limit)
{
    if (limit > UINT32_MAX) die("self-test limit must be at most 2^32-1");
    uint16_t *direct = calloc((size_t)limit + 1, sizeof(*direct));
    if (direct == NULL) die("could not allocate self-test table");
    for (uint64_t y = 1; 3 * y * y <= limit; ++y)
        for (uint64_t x = 1; x * x <= limit - 3 * y * y; ++x)
            ++direct[x * x + 3 * y * y];

    for (uint64_t k = 0; k <= limit; ++k) {
        const unsigned got = representation_count_formula(k);
        if (got != direct[k]) {
            fprintf(stderr, "self-test failed at k=%" PRIu64
                    ": formula=%u direct=%u\n", k, got, direct[k]);
            free(direct);
            exit(EXIT_FAILURE);
        }
    }
    static const uint64_t expected[] = {
        0, 4, 91, 28, 196, 31213, 364, 9604, 53599, 2548, 470596,
        UINT64_C(3672178237), 6916, 33124, 29059303, 124852, 1983163,
        UINT64_C(3360173089), 48412, UINT64_C(55365148804), 18384457,
        6117748, 1623076
    };
    for (uint64_t n = 0; n < sizeof(expected) / sizeof(expected[0]); ++n) {
        Answer answer;
        answer_init(&answer);
        calculate_answer(n, &answer);
        if (!answer.found ||
            mpz_cmp_ui(answer.value, (unsigned long)expected[n]) != 0) {
            fprintf(stderr, "calculator regression failed at n=%" PRIu64 "\n",
                    n);
            answer_clear(&answer);
            free(direct);
            exit(EXIT_FAILURE);
        }
        answer_clear(&answer);
    }

    /* Values and upper bound recorded in the A374158 comments. */
    static const Regression oeis_regressions[] = {
        {11, UINT64_C(3672178237), true},
        {12, UINT64_C(6916), false},
        {13, UINT64_C(33124), false},
        {14, UINT64_C(29059303), false},
        {15, UINT64_C(124852), false},
        {16, UINT64_C(1983163), false},
        {18, UINT64_C(48412), false},
        {20, UINT64_C(18384457), false},
        {21, UINT64_C(6117748), false},
        {22, UINT64_C(1623076), false},
        {24, UINT64_C(214396), false},
        {27, UINT64_C(629356), false},
        {28, UINT64_C(900838393), false},
        {30, UINT64_C(2372188), false},
        {31, UINT64_C(79530724), false},
        {32, UINT64_C(85276009), false},
        {36, UINT64_C(1500772), false},
        {37, UINT64_C(274299844), false},
        {40, UINT64_C(11957764), false},
        {42, UINT64_C(116237212), false},
        {45, UINT64_C(30838444), false},
        {48, UINT64_C(7932652), false},
        {54, UINT64_C(19510036), false},
        {60, UINT64_C(73537828), false},
        {67, UINT64_C(585930436), false},
        {72, UINT64_C(55528564), false}
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
           PRIu64 ", calculator regressions a(0)..a(22) pass, and %zu "
           "A374158 comment regressions pass; exact structural checks pass "
           "through n=%" PRIu64 "\n",
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
        printf("case: C=%u, S=%u, D=%" PRIu64 "; factorization: ",
               answer->c, answer->axis ? 1U : 0U, answer->d);
        bool wrote = false;
        if (answer->c == 3) {
            printf("2^2");
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
