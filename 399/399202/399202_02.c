/*
 * A399202, version 02: least k having exactly n ordered nonnegative
 * solutions of
 *
 *                    x^2 + y^2 = k.
 *
 * This version uses the proved offset-safe formulas
 *
 *   a(n) = min(A018782(n), A018782(n-1))   (even n >= 2),
 *   a(n) = 2*A018782(n)                    (odd n >= 3).
 *
 * A018782 has offset 1.  Both n and n-1 are therefore valid indices for
 * every n>=2; no conversion to the offset-0 indexing of A006339 is needed.
 *
 * A018782(r) is the least integer with D=r, where D=product(e_p+1) over
 * primes p == 1 (mod 4).  It is computed exactly by enumerating every
 * multiplicative partition of r.
 * Thus this program does not depend on stored OEIS tables or conjectures.
 *
 * The exceptional initial values are a(0)=3 and a(1)=0.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic 399202_02.c \
 *       -o 399202_02 $(pkg-config --cflags --libs gmp)
 *
 * Examples:
 *   ./399202_02 --target 23 --verbose
 *   ./399202_02 --upto 100
 *   ./399202_02 --self-test 1000000
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
#define BFILE_NAME "b399202_02.txt"
#define BFILE_TEMP_NAME "b399202_02.txt.tmp"

typedef enum {
    ANSWER_INITIAL_ZERO,
    ANSWER_INITIAL_ONE,
    ANSWER_ODD_A018782,
    ANSWER_EVEN_A018782_N,
    ANSWER_EVEN_A018782_N_MINUS_1
} AnswerSource;

typedef struct {
    bool found;
    mpz_t value;
    unsigned exponent_count;
    uint64_t exponents[MAX_FACTORS];
} AuxValue;

typedef struct {
    bool found;
    mpz_t value;
    uint64_t d;
    AnswerSource source;
    bool factor_two;
    unsigned exponent_count;
    uint64_t exponents[MAX_FACTORS];
} Answer;

typedef struct {
    uint64_t factors[MAX_FACTORS];
    unsigned factor_count;
    uint64_t primes[MAX_FACTORS];
    AuxValue best;
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

static void aux_init(AuxValue *value)
{
    memset(value, 0, sizeof(*value));
    mpz_init(value->value);
}

static void aux_clear(AuxValue *value)
{
    mpz_clear(value->value);
}

static void aux_copy(AuxValue *destination, const AuxValue *source)
{
    destination->found = source->found;
    mpz_set(destination->value, source->value);
    destination->exponent_count = source->exponent_count;
    memcpy(destination->exponents, source->exponents,
           sizeof(destination->exponents));
}

static const char *answer_source_name(AnswerSource source)
{
    switch (source) {
    case ANSWER_INITIAL_ZERO: return "initial a(0)";
    case ANSWER_INITIAL_ONE: return "initial a(1)";
    case ANSWER_ODD_A018782: return "2*A018782(n)";
    case ANSWER_EVEN_A018782_N: return "A018782(n)";
    case ANSWER_EVEN_A018782_N_MINUS_1: return "A018782(n-1)";
    }
    die("invalid answer source");
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

static void consider_partition(Search *search)
{
    mpz_t value, power;
    mpz_init_set_ui(value, 1);
    mpz_init(power);
    uint64_t exponents[MAX_FACTORS] = {0};

    for (unsigned i = 0; i < search->factor_count; ++i) {
        const uint64_t exponent =
            search->factors[search->factor_count - 1 - i] - 1;
        exponents[i] = exponent;
        mpz_ui_pow_ui(power, (unsigned long)search->primes[i],
                      (unsigned long)exponent);
        mpz_mul(value, value, power);
    }

    if (!search->best.found || mpz_cmp(value, search->best.value) < 0) {
        search->best.found = true;
        mpz_set(search->best.value, value);
        search->best.exponent_count = search->factor_count;
        memcpy(search->best.exponents, exponents,
               sizeof(search->best.exponents));
    }
    mpz_clear(power);
    mpz_clear(value);
}

static void partition_rec(Search *search, uint64_t remaining,
                          uint64_t minimum_factor)
{
    if (remaining == 1) {
        consider_partition(search);
        return;
    }
    if (search->factor_count == MAX_FACTORS)
        die("too many partition factors");

    for (uint64_t factor = minimum_factor; factor <= remaining / factor;
         ++factor) {
        if (remaining % factor != 0) continue;
        search->factors[search->factor_count++] = factor;
        partition_rec(search, remaining / factor, factor);
        --search->factor_count;
    }
    if (remaining >= minimum_factor) {
        search->factors[search->factor_count++] = remaining;
        partition_rec(search, 1, remaining);
        --search->factor_count;
    }
}

static void search_auxiliary(uint64_t d, AuxValue *result)
{
    if (d == 0) die("auxiliary D value must be positive");

    Search search;
    memset(&search, 0, sizeof(search));
    aux_init(&search.best);
    make_split_primes(search.primes);
    partition_rec(&search, d, 2);
    if (!search.best.found)
        die("internal auxiliary search found no exponent pattern");
    aux_copy(result, &search.best);
    aux_clear(&search.best);
}

/* A018782 has offset 1: index must be at least 1. */
static void calculate_a018782(uint64_t index, AuxValue *result)
{
    if (index < 1) die("A018782 index must be at least 1");
    search_auxiliary(index, result);
}

static void set_answer_from_aux(Answer *answer, const AuxValue *aux,
                                uint64_t d, AnswerSource source,
                                bool factor_two)
{
    answer->found = aux->found;
    answer->d = d;
    answer->source = source;
    answer->factor_two = factor_two;
    answer->exponent_count = aux->exponent_count;

    mpz_set(answer->value, aux->value);
    if (factor_two) mpz_mul_ui(answer->value, answer->value, 2);

    for (unsigned i = 0; i < aux->exponent_count; ++i)
        answer->exponents[i] = aux->exponents[i];
}

static void calculate_answer(uint64_t n, Answer *answer)
{
    if (n == 0) {
        answer->found = true;
        answer->source = ANSWER_INITIAL_ZERO;
        mpz_set_ui(answer->value, 3);
        return;
    }
    if (n == 1) {
        answer->found = true;
        answer->source = ANSWER_INITIAL_ONE;
        mpz_set_ui(answer->value, 0);
        return;
    }

    if ((n & 1) != 0) {
        AuxValue current;
        aux_init(&current);
        calculate_a018782(n, &current);
        set_answer_from_aux(answer, &current, n, ANSWER_ODD_A018782,
                            true);
        aux_clear(&current);
        return;
    }

    AuxValue current, previous;
    aux_init(&current);
    aux_init(&previous);
    calculate_a018782(n, &current);
    calculate_a018782(n - 1, &previous);

    if (mpz_cmp(current.value, previous.value) < 0)
        set_answer_from_aux(answer, &current, n,
                            ANSWER_EVEN_A018782_N, false);
    else
        set_answer_from_aux(answer, &previous, n - 1,
                            ANSWER_EVEN_A018782_N_MINUS_1, false);

    aux_clear(&previous);
    aux_clear(&current);
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
    while ((remaining & 1) == 0) remaining >>= 1;

    uint64_t d = 1;
    for (uint64_t p = 3; p <= remaining / p; p += 2) {
        if (remaining % p != 0) continue;
        unsigned exponent = 0;
        do {
            remaining /= p;
            ++exponent;
        } while (remaining % p == 0);
        if (p % 4 == 1) d *= exponent + 1;
        else if (exponent & 1) return 0;
    }
    if (remaining > 1) {
        if (remaining % 4 == 1) d *= 2;
        else return 0;
    }

    const uint64_t root = isqrt_u64(k);
    const unsigned square = root * root == k;
    return (unsigned)(d + square);
}

static void self_test(uint64_t limit)
{
    if (limit > UINT32_MAX) die("self-test limit must be at most 2^32-1");
    uint32_t *direct = calloc((size_t)limit + 1, sizeof(*direct));
    if (direct == NULL) die("could not allocate self-test table");
    for (uint64_t y = 0; y * y <= limit; ++y)
        for (uint64_t x = 0; x * x <= limit - y * y; ++x)
            ++direct[x * x + y * y];

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

    /* A018782 has offset 1; in particular A018782(1)=1. */
    static const Regression a018782_regressions[] = {
        {1, 1}, {2, 5}, {3, 25}, {4, 65}, {5, 625},
        {6, 325}, {7, 15625}, {8, 1105}, {9, 4225}, {10, 8125}
    };
    for (size_t i = 0;
         i < sizeof(a018782_regressions) /
             sizeof(a018782_regressions[0]); ++i) {
        AuxValue value;
        aux_init(&value);
        calculate_a018782(a018782_regressions[i].n, &value);
        const bool passed =
            value.found &&
            mpz_cmp_ui(value.value,
                       (unsigned long)a018782_regressions[i].value) == 0;
        aux_clear(&value);
        if (!passed) {
            fprintf(stderr, "A018782 offset regression failed at n=%"
                    PRIu64 "\n", a018782_regressions[i].n);
            free(direct);
            exit(EXIT_FAILURE);
        }
    }

    static const Regression known_regressions[] = {
        {0, 3}, {1, 0}, {2, 1}, {3, 50}, {4, 25}, {5, 1250},
        {6, 325}, {7, 31250}, {8, 1105}, {9, 8450}, {10, 4225}
    };
    for (size_t i = 0;
         i < sizeof(known_regressions) / sizeof(known_regressions[0]); ++i) {
        Answer answer;
        answer_init(&answer);
        calculate_answer(known_regressions[i].n, &answer);
        const bool passed =
            answer.found &&
            mpz_cmp_ui(answer.value,
                       (unsigned long)known_regressions[i].value) == 0;
        if (!passed) {
            fprintf(stderr, "stored regression failed at a(%" PRIu64
                    ") = %" PRIu64 "\n",
                    known_regressions[i].n, known_regressions[i].value);
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
            passed = passed && answer.source == ANSWER_INITIAL_ZERO;
        } else if (n == 1) {
            passed = passed && answer.source == ANSWER_INITIAL_ONE;
        } else {
            uint64_t reconstructed_d = 1;
            bool all_even = true;
            for (unsigned i = 0; i < answer.exponent_count; ++i)
                reconstructed_d *= answer.exponents[i] + 1;
            for (unsigned i = 0; i < answer.exponent_count; ++i)
                if (answer.exponents[i] & 1) all_even = false;
            const bool square = !answer.factor_two && all_even;
            const uint64_t reconstructed_count =
                reconstructed_d + (square ? 1 : 0);
            passed = passed && reconstructed_d == answer.d &&
                     reconstructed_count == n;
            switch (answer.source) {
            case ANSWER_ODD_A018782:
                passed = passed && (n & 1) != 0 && answer.d == n &&
                         answer.factor_two && all_even && !square;
                break;
            case ANSWER_EVEN_A018782_N:
                passed = passed && (n & 1) == 0 && answer.d == n &&
                         !answer.factor_two && !all_even && !square;
                break;
            case ANSWER_EVEN_A018782_N_MINUS_1:
                passed = passed && (n & 1) == 0 && answer.d == n - 1 &&
                         !answer.factor_two && all_even && square;
                break;
            case ANSWER_INITIAL_ZERO:
            case ANSWER_INITIAL_ONE:
                passed = false;
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
           PRIu64 "; the A018782 offset guard and %zu stored "
           "regressions agree, and exact structural checks pass through n=%"
           PRIu64 "\n",
           limit, sizeof(known_regressions) / sizeof(known_regressions[0]),
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
                   " ordered nonnegative representations\n", n);
        } else {
            printf("the least solution is within the checked bound ");
            print_bigint(stdout, limit);
            putchar('\n');
        }
    }
    if (verbose && n < 2) {
        printf("special initial case: k=%u has %s\n",
               n == 0 ? 3U : 0U,
               n == 0 ? "no nonnegative solutions"
                      : "the single solution (0,0)");
        return;
    }
    if (verbose) {
        uint64_t primes[MAX_FACTORS];
        make_split_primes(primes);
        const bool square =
            answer->source == ANSWER_EVEN_A018782_N_MINUS_1;
        printf("formula source: %s; S=%u, D=%" PRIu64
               "; factorization: ",
               answer_source_name(answer->source), square ? 1U : 0U,
               answer->d);
        bool wrote = false;
        if (answer->factor_two) {
            printf("2");
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
