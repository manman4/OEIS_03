/*
 * A399278 (draft): least nonnegative k having exactly n nonnegative
 * integer solutions (x,y) satisfying
 *
 *                         x^2 + 6 y^2 = k.
 *
 * This is an exact prime-exponent search, not a scan up to a guessed bound.
 * The form has fundamental discriminant -24 and class group of order two.
 * For a represented positive integer k, write
 *
 *   D(k) = product (e_p+1),
 *
 * over split primes p.  The principal split primes are 1 or 7 (mod 24),
 * and the nonprincipal split primes are 5 or 11 (mod 24).  Primes congruent
 * to 13,17,19,23 (mod 24) are inert and must have even exponent.  If s and
 * t are the exponents of 2 and 3, representation by the principal form
 * requires
 *
 *   s + t + sum e_q == 0 (mod 2),  q == 5 or 11 (mod 24).
 *
 * Let A(k)=1 when k is a square or six times a square, and A(k)=0
 * otherwise. The number of nonnegative solutions for k>0 is
 *
 *   R(k) = (D(k)+A(k))/2.
 *
 * Thus only D=2*n with A=0 and D=2*n-1 with A=1 need be searched.
 * Every multiplicative partition D=product(e_i+1) gives one exponent
 * multiset.  Every assignment of that multiset to the two split-prime
 * classes is considered.  Within a class, decreasing exponents are put on
 * increasing primes, which is necessarily minimal.  If the nonprincipal
 * exponent sum is odd, the least ramified correction is one factor 2;
 * otherwise no correction is needed.  Inert factors and any further
 * ramified factors only increase k.  Hence no possible minimum is omitted.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic 399278_01.c \
 *       -o 399278_01 $(pkg-config --cflags --libs gmp)
 *
 * Examples:
 *   ./399278_01 --target 23 --verbose
 *   ./399278_01 --upto 100
 *   ./399278_01 --self-test 1000000
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
#define BFILE_NAME "b399278.txt"
#define BFILE_TEMP_NAME "b399278.txt.tmp"

typedef struct {
    bool found;
    mpz_t value;
    uint64_t d;
    bool axis;
    bool correction_two;
    unsigned prime_count;
    uint64_t primes[MAX_FACTORS];
    uint64_t exponents[MAX_FACTORS];
} Answer;

typedef struct {
    uint64_t factors[MAX_FACTORS];
    unsigned factor_count;
    uint64_t principal_primes[MAX_FACTORS];
    uint64_t nonprincipal_primes[MAX_FACTORS];
    Answer best;
} Search;

typedef struct {
    Search *search;
    uint64_t d;
    bool axis;
    unsigned group_count;
    uint64_t group_exponent[MAX_FACTORS];
    unsigned group_multiplicity[MAX_FACTORS];
    uint64_t principal_exponents[MAX_FACTORS];
    uint64_t nonprincipal_exponents[MAX_FACTORS];
    unsigned principal_count;
    unsigned nonprincipal_count;
} AssignmentSearch;

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

static void make_prime_lists(Search *search)
{
    unsigned principal_count = 0, nonprincipal_count = 0;
    for (uint64_t p = 5;
         principal_count < MAX_FACTORS ||
             nonprincipal_count < MAX_FACTORS;
         p += 2) {
        if (!is_prime(p)) continue;
        const uint64_t residue = p % 24;
        if ((residue == 1 || residue == 7) &&
            principal_count < MAX_FACTORS)
            search->principal_primes[principal_count++] = p;
        if ((residue == 5 || residue == 11) &&
            nonprincipal_count < MAX_FACTORS)
            search->nonprincipal_primes[nonprincipal_count++] = p;
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
    destination->axis = source->axis;
    destination->correction_two = source->correction_two;
    destination->prime_count = source->prime_count;
    memcpy(destination->primes, source->primes,
           sizeof(destination->primes));
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

static void store_if_better(Search *search, const mpz_t value, uint64_t d,
                            bool axis, bool correction_two,
                            const uint64_t primes[MAX_FACTORS],
                            const uint64_t exponents[MAX_FACTORS],
                            unsigned prime_count)
{
    if (search->best.found && mpz_cmp(value, search->best.value) >= 0)
        return;
    search->best.found = true;
    mpz_set(search->best.value, value);
    search->best.d = d;
    search->best.axis = axis;
    search->best.correction_two = correction_two;
    search->best.prime_count = prime_count;
    for (unsigned i = 0; i < prime_count; ++i) {
        search->best.primes[i] = primes[i];
        search->best.exponents[i] = exponents[i];
    }
}

static void consider_assignment(AssignmentSearch *assignment,
                                unsigned nonprincipal_parity)
{
    if (assignment->principal_count + assignment->nonprincipal_count >
        MAX_FACTORS)
        die("too many assigned exponents");

    const bool correction_two = nonprincipal_parity != 0;
    mpz_t value, power;
    mpz_init_set_ui(value, correction_two ? 2U : 1U);
    mpz_init(power);
    for (unsigned i = 0; i < assignment->principal_count; ++i) {
        mpz_ui_pow_ui(
            power,
            (unsigned long)assignment->search->principal_primes[i],
            (unsigned long)assignment->principal_exponents[i]);
        mpz_mul(value, value, power);
    }
    for (unsigned i = 0; i < assignment->nonprincipal_count; ++i) {
        mpz_ui_pow_ui(
            power,
            (unsigned long)assignment->search->nonprincipal_primes[i],
            (unsigned long)assignment->nonprincipal_exponents[i]);
        mpz_mul(value, value, power);
    }

    uint64_t primes[MAX_FACTORS] = {0};
    uint64_t exponents[MAX_FACTORS] = {0};
    unsigned pi = 0, qi = 0, count = 0;
    while (pi < assignment->principal_count ||
           qi < assignment->nonprincipal_count) {
        const bool take_nonprincipal =
            pi == assignment->principal_count ||
            (qi < assignment->nonprincipal_count &&
             assignment->search->nonprincipal_primes[qi] <
                 assignment->search->principal_primes[pi]);
        if (take_nonprincipal) {
            primes[count] =
                assignment->search->nonprincipal_primes[qi];
            exponents[count] = assignment->nonprincipal_exponents[qi];
            ++qi;
        } else {
            primes[count] = assignment->search->principal_primes[pi];
            exponents[count] = assignment->principal_exponents[pi];
            ++pi;
        }
        ++count;
    }
    store_if_better(assignment->search, value, assignment->d,
                    assignment->axis, correction_two,
                    primes, exponents, count);
    mpz_clear(power);
    mpz_clear(value);
}

static void assignment_rec(AssignmentSearch *assignment,
                           unsigned group_index,
                           unsigned nonprincipal_parity)
{
    if (group_index == assignment->group_count) {
        consider_assignment(assignment, nonprincipal_parity);
        return;
    }

    const uint64_t exponent = assignment->group_exponent[group_index];
    const unsigned multiplicity =
        assignment->group_multiplicity[group_index];
    const unsigned old_principal_count = assignment->principal_count;
    const unsigned old_nonprincipal_count =
        assignment->nonprincipal_count;

    for (unsigned to_nonprincipal = 0;
         to_nonprincipal <= multiplicity; ++to_nonprincipal) {
        assignment->principal_count = old_principal_count;
        assignment->nonprincipal_count = old_nonprincipal_count;
        for (unsigned i = 0; i < to_nonprincipal; ++i)
            assignment->nonprincipal_exponents
                [assignment->nonprincipal_count++] = exponent;
        for (unsigned i = to_nonprincipal; i < multiplicity; ++i)
            assignment->principal_exponents
                [assignment->principal_count++] = exponent;
        const unsigned next_parity =
            nonprincipal_parity ^
            (unsigned)((exponent & 1) && (to_nonprincipal & 1));
        assignment_rec(assignment, group_index + 1, next_parity);
    }
    assignment->principal_count = old_principal_count;
    assignment->nonprincipal_count = old_nonprincipal_count;
}

static void consider_partition(Search *search, uint64_t d, bool axis)
{
    uint64_t exponents[MAX_FACTORS] = {0};
    bool all_even = true;
    for (unsigned i = 0; i < search->factor_count; ++i) {
        const uint64_t exponent =
            search->factors[search->factor_count - 1 - i] - 1;
        exponents[i] = exponent;
        if ((exponent & 1) != 0) all_even = false;
    }
    if (all_even != axis) return;

    AssignmentSearch assignment;
    memset(&assignment, 0, sizeof(assignment));
    assignment.search = search;
    assignment.d = d;
    assignment.axis = axis;
    for (unsigned i = 0; i < search->factor_count; ++i) {
        if (assignment.group_count == 0 ||
            assignment.group_exponent[assignment.group_count - 1] !=
                exponents[i]) {
            const unsigned group = assignment.group_count++;
            assignment.group_exponent[group] = exponents[i];
            assignment.group_multiplicity[group] = 1;
        } else {
            ++assignment.group_multiplicity[assignment.group_count - 1];
        }
    }
    assignment_rec(&assignment, 0, 0);
}

static void partition_rec(Search *search, uint64_t remaining,
                          uint64_t minimum_factor, uint64_t d, bool axis)
{
    if (remaining == 1) {
        consider_partition(search, d, axis);
        return;
    }
    if (search->factor_count == MAX_FACTORS)
        die("too many multiplicative-partition factors");

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
    if (d == 0) die("D must be positive");
    search->factor_count = 0;
    partition_rec(search, d, 2, d, axis);
}

static void calculate_answer(uint64_t n, Answer *answer)
{
    Search search;
    memset(&search, 0, sizeof(search));
    answer_init(&search.best);
    make_prime_lists(&search);

    if (n == 0 || n == 1) {
        search.best.found = true;
        mpz_set_ui(search.best.value, n == 0 ? 2U : 0U);
    } else {
        const uint64_t twice = 2 * n;
        search_case(&search, twice, false);
        search_case(&search, twice - 1, true);
    }
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

static bool is_square_u64(uint64_t n)
{
    const uint64_t root = isqrt_u64(n);
    return root * root == n;
}

static uint64_t representation_count_q6(uint64_t k)
{
    if (k == 0) return 1;
    uint64_t remaining = k;
    uint64_t ramified_parity = 0;
    while ((remaining & 1) == 0) {
        remaining >>= 1;
        ramified_parity ^= 1;
    }
    while (remaining % 3 == 0) {
        remaining /= 3;
        ramified_parity ^= 1;
    }

    uint64_t d = 1;
    uint64_t nonprincipal_parity = 0;
    for (uint64_t p = 5; p <= remaining / p; p += 2) {
        if (remaining % p != 0) continue;
        uint64_t exponent = 0;
        do {
            remaining /= p;
            ++exponent;
        } while (remaining % p == 0);
        const uint64_t residue = p % 24;
        if (residue == 1 || residue == 7) {
            d *= exponent + 1;
        } else if (residue == 5 || residue == 11) {
            d *= exponent + 1;
            nonprincipal_parity ^= exponent & 1;
        } else if ((exponent & 1) != 0) {
            return 0;
        }
    }
    if (remaining > 1) {
        const uint64_t residue = remaining % 24;
        if (residue == 1 || residue == 7) {
            d *= 2;
        } else if (residue == 5 || residue == 11) {
            d *= 2;
            nonprincipal_parity ^= 1;
        } else {
            return 0;
        }
    }
    if ((ramified_parity ^ nonprincipal_parity) != 0) return 0;

    const uint64_t axis =
        is_square_u64(k) || (k % 6 == 0 && is_square_u64(k / 6));
    return (d + axis) / 2;
}

static void self_test(uint64_t limit)
{
    if (limit > UINT32_MAX)
        die("self-test limit must be at most 2^32-1");
    if (limit > (uint64_t)(SIZE_MAX / sizeof(uint32_t) - 1))
        die("self-test table is too large for size_t");
    uint32_t *direct = calloc((size_t)limit + 1, sizeof(*direct));
    if (direct == NULL) die("could not allocate self-test table");

    for (uint64_t y = 0; 6 * y * y <= limit; ++y) {
        const uint64_t y_term = 6 * y * y;
        for (uint64_t x = 0; x * x <= limit - y_term; ++x)
            ++direct[x * x + y_term];
    }

    uint64_t minima[GUARANTEED_UPTO + 1];
    for (uint64_t n = 0; n <= GUARANTEED_UPTO; ++n)
        minima[n] = UINT64_MAX;
    for (uint64_t k = 0; k <= limit; ++k) {
        const uint64_t got = representation_count_q6(k);
        if (got != direct[k]) {
            fprintf(stderr, "self-test failed at k=%" PRIu64
                    ": formula=%" PRIu64 " direct=%" PRIu32 "\n",
                    k, got, direct[k]);
            free(direct);
            exit(EXIT_FAILURE);
        }
        if (got <= GUARANTEED_UPTO && minima[got] == UINT64_MAX)
            minima[got] = k;
    }

    /* Independently checked by direct enumeration through k=5000000. */
    static const Regression known_regressions[] = {
        {0, 2}, {1, 0}, {2, 25}, {3, 175}, {4, 385}, {5, 1225},
        {6, 2695}, {7, 109375}, {8, 9625}, {9, 21175}, {10, 96250},
        {11, 765625}, {12, 55825}, {13, 1500625}, {14, 148225},
        {15, 529375}, {16, 298375}, {18, 390775}, {20, 1395625},
        {23, 3705625}, {24, 1730575}, {27, 4594975}
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
        if (n == 0 || n == 1) {
            passed = passed &&
                     mpz_cmp_ui(answer.value, n == 0 ? 2U : 0U) == 0;
        } else {
            uint64_t reconstructed_d = 1;
            bool all_even = true;
            uint64_t nonprincipal_parity = 0;
            mpz_t reconstructed, power;
            mpz_init_set_ui(reconstructed,
                            answer.correction_two ? 2U : 1U);
            mpz_init(power);
            for (unsigned i = 0; i < answer.prime_count; ++i) {
                reconstructed_d *= answer.exponents[i] + 1;
                if ((answer.exponents[i] & 1) != 0) all_even = false;
                const uint64_t residue = answer.primes[i] % 24;
                if (residue == 5 || residue == 11)
                    nonprincipal_parity ^= answer.exponents[i] & 1;
                else if (residue != 1 && residue != 7)
                    passed = false;
                mpz_ui_pow_ui(power, (unsigned long)answer.primes[i],
                              (unsigned long)answer.exponents[i]);
                mpz_mul(reconstructed, reconstructed, power);
            }
            passed = passed && reconstructed_d == answer.d &&
                     all_even == answer.axis &&
                     answer.correction_two ==
                         (nonprincipal_parity != 0) &&
                     (reconstructed_d + (answer.axis ? 1U : 0U)) / 2 == n &&
                     mpz_cmp(reconstructed, answer.value) == 0;
            mpz_clear(power);
            mpz_clear(reconstructed);
        }

        if (passed && mpz_fits_ulong_p(answer.value)) {
            const unsigned long value = mpz_get_ui(answer.value);
            passed = (uint64_t)value == value &&
                     representation_count_q6((uint64_t)value) == n;
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
           PRIu64 "; all minima within that range and %zu stored "
           "regressions agree, and exact "
           "structural checks pass through n=%" PRIu64 "\n",
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
                   " nonnegative representations\n", n);
        } else {
            printf("the least solution is within the checked bound ");
            print_bigint(stdout, limit);
            putchar('\n');
        }
    }
    if (verbose && n >= 2) {
        printf("A=%u, D=%" PRIu64 "; factorization: ",
               answer->axis ? 1U : 0U, answer->d);
        bool wrote = false;
        if (answer->correction_two) {
            printf("2^1");
            wrote = true;
        }
        for (unsigned i = 0; i < answer->prime_count; ++i) {
            if (wrote) printf(" * ");
            printf("%" PRIu64 "^%" PRIu64,
                   answer->primes[i], answer->exponents[i]);
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
