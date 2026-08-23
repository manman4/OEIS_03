/*
 * A374091: least nonnegative k having exactly n ordered positive solutions of
 *
 *                    x^2 + 3*x*y + y^2 = k.
 *
 * This program does not scan every integer k.  It exhausts the much smaller
 * set of possible prime-exponent patterns, which is an exact check of the
 * whole interval (and in fact gives a(n) directly).
 *
 * Put D(k) = product (e_p+1), over primes p == 1 or 4 (mod 5).  If k is
 * representable, its number R(k) of ordered solutions with x,y > 0 is
 *
 *     R(k) = D(k) - S(k),
 *
 * where S(k)=1 if k is a square, and S(k)=0 otherwise.  An odd exponent
 * at a prime p == 2 or 3 (mod 5) gives no representations.  The ramified
 * prime 5 does not affect D(k).
 *
 * Here alpha=(3+sqrt(5))/2 and x^2+3*x*y+y^2=N(x+alpha*y) in Z[alpha].
 * This real quadratic ring has class number one.  Its totally positive
 * norm-one units are the powers of alpha.  The cone x,y >= 0 is a
 * fundamental domain for their action on totally positive generators.
 * Thus the D(k) ideals of norm k give D(k) representatives in this cone,
 * with its two boundary rays identified.  A boundary representative exists
 * exactly when k is a square; removing it gives R(k)=D(k)-S(k).
 *
 * Hence R(k)=n permits only D=n when S=0 or D=n+1 when S=1.
 * A multiplicative partition D=product(e_i+1) lists every possible exponent
 * multiset.  Its smallest represented part assigns the largest e_i to 11,
 * the next to 19, then 29, 31, etc.  If all e_i are even but S=0 is required,
 * multiplying by 5 gives the least nonsquare without changing D.  Squares of
 * inert primes only increase k.  Thus the search proves minimality without iterating
 * through all k below the answer.  The exceptional initial value is a(0)=0.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic 374091_01.c \
 *       -o 374091_01 $(pkg-config --cflags --libs gmp)
 *
 * Examples:
 *   ./374091_01 --target 10 --verbose
 *   ./374091_01 --upto 100
 *   ./374091_01 --self-test 1000000
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
#define GUARANTEED_UPTO UINT64_C(1000)
#define MAX_LIMIT_EXPONENT UINT64_C(1000000)
#define DEFAULT_TARGET UINT64_C(10)
#define DEFAULT_TEST_LIMIT UINT64_C(1000000)
#define BFILE_NAME "b374091.txt"
#define BFILE_TEMP_NAME "b374091.txt.tmp"

typedef struct {
    bool found;
    mpz_t value;
    uint64_t d;
    bool axis;
    bool factor_five;
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
    for (uint64_t p = 11; count < MAX_FACTORS; p += 2)
        if ((p % 5 == 1 || p % 5 == 4) && is_prime(p))
            primes[count++] = p;
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
    destination->axis = source->axis;
    destination->factor_five = source->factor_five;
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

static void consider_partition(Search *search, uint64_t d, bool axis)
{
    bool all_even = true;
    for (unsigned i = 0; i < search->factor_count; ++i)
        if (((search->factors[i] - 1) & 1) != 0) all_even = false;
    if (axis && !all_even) return;

    mpz_t value, power;
    const bool factor_five = !axis && all_even;
    mpz_init_set_ui(value, factor_five ? 5 : 1);
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
        search->best.axis = axis;
        search->best.factor_five = factor_five;
        search->best.exponent_count = search->factor_count;
        for (unsigned i = 0; i < search->factor_count; ++i)
            search->best.exponents[i] =
                search->factors[search->factor_count - 1 - i] - 1;
    }
    mpz_clear(power);
    mpz_clear(value);
}

static void partition_rec(Search *search, uint64_t remaining,
                          uint64_t minimum_factor, uint64_t d, bool axis)
{
    if (remaining == 1) {
        consider_partition(search, d, axis);
        return;
    }
    if (search->factor_count == MAX_FACTORS) die("too many partition factors");

    for (uint64_t factor = minimum_factor; factor <= remaining / factor;
         ++factor) {
        if (remaining % factor != 0) continue;
        search->factors[search->factor_count++] = factor;
        partition_rec(search, remaining / factor, factor, d, axis);
        --search->factor_count;
    }
    if (remaining >= minimum_factor) {
        search->factors[search->factor_count++] = remaining;
        partition_rec(search, 1, remaining, d, axis);
        --search->factor_count;
    }
}

static void search_case(Search *search, uint64_t d, bool axis)
{
    if (d == 0) return;
    search->factor_count = 0;
    partition_rec(search, d, 2, d, axis);
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
    search_case(&search, n, false);
    search_case(&search, n + 1, true);
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
    while (remaining % 5 == 0) remaining /= 5;

    unsigned exponent_two = 0;
    while ((remaining & 1) == 0) {
        remaining >>= 1;
        ++exponent_two;
    }
    if ((exponent_two & 1) != 0) return 0;

    uint64_t d = 1;
    for (uint64_t p = 3; p <= remaining / p; p += 2) {
        if (remaining % p != 0) continue;
        unsigned exponent = 0;
        do {
            remaining /= p;
            ++exponent;
        } while (remaining % p == 0);
        if (p % 5 == 1 || p % 5 == 4) d *= exponent + 1;
        else if (exponent & 1) return 0;
    }
    if (remaining > 1) {
        if (remaining % 5 == 1 || remaining % 5 == 4) d *= 2;
        else return 0;
    }

    const uint64_t root = isqrt_u64(k);
    const unsigned square = root * root == k;
    return (unsigned)(d - square);
}

static void self_test(uint64_t limit)
{
    if (limit > UINT32_MAX) die("self-test limit must be at most 2^32-1");
    if (limit > (uint64_t)(SIZE_MAX / sizeof(uint32_t) - 1))
        die("self-test table is too large for size_t");
    uint32_t *direct = calloc((size_t)limit + 1, sizeof(*direct));
    if (direct == NULL) die("could not allocate self-test table");
    uint64_t split_primes[MAX_FACTORS];
    make_split_primes(split_primes);
    for (uint64_t y = 1; y * y + 3 * y + 1 <= limit; ++y) {
        const uint64_t y_square = y * y;
        for (uint64_t x = 1;
             x * x + 3 * x * y <= limit - y_square; ++x)
            ++direct[x * x + 3 * x * y + y_square];
    }

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

    static const Regression oeis_regressions[] = {
        {0, 0}, {1, 5}, {2, 11}, {3, 605}, {4, 209},
        {5, 73205}, {6, 2299}, {7, 8857805}, {8, 6061},
        {9, 218405}, {10, 278179}
    };
    for (size_t i = 0;
         i < sizeof(oeis_regressions) / sizeof(oeis_regressions[0]); ++i) {
        Answer answer;
        answer_init(&answer);
        calculate_answer(oeis_regressions[i].n, &answer);
        const bool passed =
            answer.found &&
            mpz_cmp_ui(answer.value,
                       (unsigned long)oeis_regressions[i].value) == 0;
        if (!passed) {
            fprintf(stderr, "OEIS regression failed at a(%" PRIu64
                    ") = %" PRIu64 "\n",
                    oeis_regressions[i].n, oeis_regressions[i].value);
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
        if (n == 0) {
            passed = passed && mpz_cmp_ui(answer.value, 0) == 0;
        } else {
            uint64_t reconstructed_d = 1;
            bool all_even = true;
            for (unsigned i = 0; i < answer.exponent_count; ++i)
                reconstructed_d *= answer.exponents[i] + 1;
            for (unsigned i = 0; i < answer.exponent_count; ++i)
                if (answer.exponents[i] & 1) all_even = false;
            const uint64_t reconstructed_count =
                reconstructed_d - (answer.axis ? 1 : 0);
            mpz_t reconstructed, power;
            mpz_init_set_ui(reconstructed, answer.factor_five ? 5U : 1U);
            mpz_init(power);
            for (unsigned i = 0; i < answer.exponent_count; ++i) {
                mpz_ui_pow_ui(power, (unsigned long)split_primes[i],
                              (unsigned long)answer.exponents[i]);
                mpz_mul(reconstructed, reconstructed, power);
            }
            passed = passed && reconstructed_d == answer.d &&
                     reconstructed_count == n &&
                     answer.factor_five == (!answer.axis && all_even) &&
                     mpz_cmp(reconstructed, answer.value) == 0;
            if ((n & 1) != 0)
                passed = passed && !answer.axis && answer.factor_five;
            mpz_clear(power);
            mpz_clear(reconstructed);
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
            printf(" with exactly %" PRIu64
                   " ordered positive representations\n", n);
        } else {
            printf("the least solution is within the checked bound ");
            print_bigint(stdout, limit);
            putchar('\n');
        }
    }
    if (verbose && n != 0) {
        uint64_t primes[MAX_FACTORS];
        make_split_primes(primes);
        printf("case: S=%u, D=%" PRIu64 "; factorization: ",
               answer->axis ? 1U : 0U, answer->d);
        bool wrote = false;
        if (answer->factor_five) {
            printf("5");
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
        bfile = fopen(BFILE_TEMP_NAME, "wx");
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
