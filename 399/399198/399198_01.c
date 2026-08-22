/*
 * A399198: least k having exactly n nonnegative solutions of
 *
 *                    x^2 + 4 y^2 = k.
 *
 * This program does not scan every integer k.  It exhausts the much smaller
 * set of possible prime-exponent patterns, which is an exact check of all
 * possible values (and therefore gives a(n) directly).
 *
 * Put D(k) = product (e_p+1), over odd primes p == 1 (mod 4).  An odd
 * exponent at a prime p == 3 (mod 4) makes k unrepresentable.  If e=v_2(k),
 * the number T(k) of all signed integer solutions is
 *
 *     T(k) = 2 D(k)   if e=0,
 *            0        if e=1,
 *            4 D(k)   if e>=2.
 *
 * There are two signed solutions on the x-axis when k is a square, and two
 * on the y-axis when k/4 is a square.  Put A(k) equal to the number (0, 1,
 * or 2) of these axis conditions.  Each nonaxis nonnegative solution has
 * four signed variants, while each axis condition contributes one
 * nonnegative solution.  Hence
 *
 *     R(k) = (T(k) + 2 A(k))/4.
 *
 * Consequently R(k)=n has exactly the following four candidate types:
 *
 *   odd, nonsquare:        D=2n;
 *   odd, square:           D=2n-1;
 *   divisible by 4, A=0:  D=n;
 *   divisible by 4, A=2:  D=n-1.
 *
 * A multiplicative partition D=product(e_i+1) lists every possible exponent
 * multiset.  The largest e_i is assigned to 5, the next to 13, then 17, 29,
 * etc.  For an even nonaxis candidate, the least 2-adic factor is 4 when
 * some e_i is odd and 8 when all e_i are even.  An even two-axis candidate
 * has 2-adic factor 4.  Squares of primes == 3 (mod 4) only increase k.
 * Thus the search proves minimality without relying on conjectural data.
 * The exceptional initial values are a(0)=2 and a(1)=0.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic 399198_01.c \
 *       -o 399198_01 $(pkg-config --cflags --libs gmp)
 *
 * Examples:
 *   ./399198_01 --target 23 --verbose
 *   ./399198_01 --upto 100
 *   ./399198_01 --self-test 1000000
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
#define BFILE_NAME "b399198.txt"
#define BFILE_TEMP_NAME "b399198.txt.tmp"

typedef enum {
    ODD_NONAXIS,
    ODD_AXIS,
    EVEN_NONAXIS,
    EVEN_AXIS
} CandidateType;

typedef struct {
    bool found;
    mpz_t value;
    uint64_t d;
    CandidateType type;
    unsigned two_exponent;
    unsigned axis_count;
    unsigned exponent_count;
    uint64_t exponents[MAX_FACTORS];
} Answer;

typedef struct {
    uint64_t factors[MAX_FACTORS];
    unsigned factor_count;
    uint64_t primes[MAX_FACTORS];
    Answer best;
} Search;

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
    for (uint64_t p = 5; count < MAX_FACTORS; p += 2)
        if (p % 4 == 1 && is_prime(p))
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
    destination->type = source->type;
    destination->two_exponent = source->two_exponent;
    destination->axis_count = source->axis_count;
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

static const char *candidate_type_name(CandidateType type)
{
    switch (type) {
    case ODD_NONAXIS: return "odd nonsquare";
    case ODD_AXIS: return "odd square";
    case EVEN_NONAXIS: return "divisible by 4, nonaxis";
    case EVEN_AXIS: return "divisible by 4, two axes";
    }
    die("invalid candidate type");
}

static void consider_partition(Search *search, uint64_t d,
                               CandidateType type)
{
    bool all_even = true;
    for (unsigned i = 0; i < search->factor_count; ++i)
        if (((search->factors[i] - 1) & 1) != 0) all_even = false;

    unsigned two_exponent = 0;
    unsigned axis_count = 0;
    switch (type) {
    case ODD_NONAXIS:
        if (all_even) return;
        break;
    case ODD_AXIS:
        if (!all_even) return;
        axis_count = 1;
        break;
    case EVEN_NONAXIS:
        two_exponent = all_even ? 3U : 2U;
        break;
    case EVEN_AXIS:
        if (!all_even) return;
        two_exponent = 2;
        axis_count = 2;
        break;
    }

    mpz_t value, power;
    mpz_init_set_ui(value, 1);
    mpz_init(power);
    if (two_exponent != 0)
        mpz_ui_pow_ui(value, 2, (unsigned long)two_exponent);
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
        search->best.type = type;
        search->best.two_exponent = two_exponent;
        search->best.axis_count = axis_count;
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
                          CandidateType type)
{
    if (remaining == 1) {
        consider_partition(search, d, type);
        return;
    }
    if (search->factor_count == MAX_FACTORS)
        die("too many partition factors");

    for (uint64_t factor = minimum_factor; factor <= remaining / factor;
         ++factor) {
        if (remaining % factor != 0) continue;
        search->factors[search->factor_count++] = factor;
        partition_rec(search, remaining / factor, factor, d, type);
        --search->factor_count;
    }
    if (remaining >= minimum_factor) {
        search->factors[search->factor_count++] = remaining;
        partition_rec(search, 1, remaining, d, type);
        --search->factor_count;
    }
}

static void search_case(Search *search, uint64_t d, CandidateType type)
{
    if (d == 0) return;
    search->factor_count = 0;
    partition_rec(search, d, 2, d, type);
}

static void calculate_answer(uint64_t n, Answer *answer)
{
    Search search;
    memset(&search, 0, sizeof(search));
    answer_init(&search.best);
    make_split_primes(search.primes);
    if (n == 0) {
        search.best.found = true;
        mpz_set_ui(search.best.value, 2);
        answer_copy(answer, &search.best);
        answer_clear(&search.best);
        return;
    }
    if (n == 1) {
        search.best.found = true;
        mpz_set_ui(search.best.value, 0);
        answer_copy(answer, &search.best);
        answer_clear(&search.best);
        return;
    }
    search_case(&search, 2 * n, ODD_NONAXIS);
    search_case(&search, 2 * n - 1, ODD_AXIS);
    search_case(&search, n, EVEN_NONAXIS);
    search_case(&search, n - 1, EVEN_AXIS);
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
    if (k == 0) return 1;
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
        if (p % 4 == 1)
            d *= exponent + 1;
        else if ((exponent & 1) != 0)
            return 0;
    }
    if (remaining > 1) {
        if (remaining % 4 == 1)
            d *= 2;
        else
            return 0;
    }

    unsigned axis_count = 0;
    const uint64_t root = isqrt_u64(k);
    if (root * root == k) ++axis_count;
    if (k % 4 == 0) {
        const uint64_t quarter = k / 4;
        const uint64_t quarter_root = isqrt_u64(quarter);
        if (quarter_root * quarter_root == quarter) ++axis_count;
    }
    const uint64_t c = v2 == 0 ? 1 : 2;
    return (unsigned)((c * d + axis_count) / 2);
}

static void self_test(uint64_t limit)
{
    if (limit > UINT32_MAX) die("self-test limit must be at most 2^32-1");
    uint32_t *direct = calloc((size_t)limit + 1, sizeof(*direct));
    if (direct == NULL) die("could not allocate self-test table");
    for (uint64_t y = 0; 4 * y * y <= limit; ++y)
        for (uint64_t x = 0; x * x <= limit - 4 * y * y; ++x)
            ++direct[x * x + 4 * y * y];

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

    static const uint64_t initial_values[] = {
        2, 0, 4, 200, 100, 4225, 1300, 125000, 4420, 33800, 16900
    };
    for (uint64_t n = 0;
         n < sizeof(initial_values) / sizeof(initial_values[0]); ++n) {
        Answer answer;
        answer_init(&answer);
        calculate_answer(n, &answer);
        if (!answer.found ||
            mpz_cmp_ui(answer.value, (unsigned long)initial_values[n]) != 0) {
            fprintf(stderr, "initial-value regression failed at n=%" PRIu64
                    "\n", n);
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
        if (n >= 2) {
            uint64_t reconstructed_d = 1;
            bool all_even = true;
            for (unsigned i = 0; i < answer.exponent_count; ++i) {
                reconstructed_d *= answer.exponents[i] + 1;
                if ((answer.exponents[i] & 1) != 0) all_even = false;
            }
            const uint64_t c = answer.two_exponent == 0 ? 1 : 2;
            const uint64_t numerator =
                c * reconstructed_d + answer.axis_count;
            passed = passed && reconstructed_d == answer.d &&
                     numerator == 2 * n;
            switch (answer.type) {
            case ODD_NONAXIS:
                passed = passed && answer.two_exponent == 0 &&
                         answer.axis_count == 0 && !all_even;
                break;
            case ODD_AXIS:
                passed = passed && answer.two_exponent == 0 &&
                         answer.axis_count == 1 && all_even;
                break;
            case EVEN_NONAXIS:
                passed = passed && answer.axis_count == 0 &&
                         answer.two_exponent == (all_even ? 3U : 2U);
                break;
            case EVEN_AXIS:
                passed = passed && answer.two_exponent == 2 &&
                         answer.axis_count == 2 && all_even;
                break;
            }
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
           PRIu64 "; every computed minimum in that interval agrees, "
           "initial values agree, and structural checks pass through n=%"
           PRIu64 "\n", limit, GUARANTEED_UPTO);
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
            printf(" with exactly %" PRIu64 " nonnegative representations\n",
                   n);
        } else {
            printf("the least solution is within the checked bound ");
            print_bigint(stdout, limit);
            putchar('\n');
        }
    }
    if (verbose && n < 2) {
        printf("special initial case: k=%u has %s\n",
               n == 0 ? 2U : 0U,
               n == 0 ? "no nonnegative solutions"
                      : "the single solution (0,0)");
        return;
    }
    if (verbose) {
        uint64_t primes[MAX_FACTORS];
        make_split_primes(primes);
        printf("case: %s, A=%u, D=%" PRIu64 "; factorization: ",
               candidate_type_name(answer->type), answer->axis_count,
               answer->d);
        bool wrote = false;
        if (answer->two_exponent != 0) {
            printf("2^%u", answer->two_exponent);
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
        /* Exclusive creation prevents concurrent runs or a preexisting
         * symbolic link from clobbering an unrelated file. */
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

