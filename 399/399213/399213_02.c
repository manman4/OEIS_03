/*
 * A399213, version 02: least nonnegative k having exactly n
 * positive-integer pairs
 * (x,y) satisfying
 *
 *                         x^2 + 8 y^2 = k.
 *
 * This is an exact prime-exponent search, not a scan up to a guessed bound.
 * Put R_8(k)=#{(x,y)>0 : x^2+8y^2=k} and define R_2 similarly for
 * x^2+2y^2.  Every represented k is of exactly one of these types:
 *
 *   odd:  k == 1 (mod 8), and R_8(k)=R_2(k);
 *   even: k=4*u,             and R_8(k)=R_2(u).
 *
 * For represented u, let
 *
 *   D(u) = product (e_p+1),  p == 1 or 3 (mod 8),
 *
 * and let A(u)=1 when u is a square or twice a square, otherwise 0.
 * Then R_2(u)=(D(u)-A(u))/2.  Therefore only D=2*n (A=0) and
 * D=2*n+1 (A=1) need be searched.
 *
 * A multiplicative partition D=product(e_i+1) enumerates every exponent
 * multiset.  For the even type, the exponents are assigned in decreasing
 * order to 3,11,17,19,... .  For the odd type, every assignment to primes
 * 1 or 3 (mod 8) having an even sum of exponents at the 3 (mod 8) primes
 * is considered.  Within each residue class, decreasing exponents go on
 * increasing primes, so no possible minimum is omitted.
 *
 * This version additionally uses the proved theorem
 *
 *                     9 | a(n) unless n = 2.
 *
 * The exceptional value a(2)=33 is installed directly.  For every other
 * positive index, candidates are constructed explicitly as 9*u: the
 * exponent assigned to the least split prime 3 must be at least 2.  This
 * removes all candidates that the theorem proves cannot be minimal while
 * retaining the complete set of possible answers.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic 399213_02.c \
 *       -o 399213_02 $(pkg-config --cflags --libs gmp)
 *
 * Examples:
 *   ./399213_02 --target 23 --verbose
 *   ./399213_02 --upto 100
 *   ./399213_02 --self-test 1000000
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
#define BFILE_NAME "b399213_02.txt"
#define BFILE_TEMP_NAME "b399213_02.txt.tmp"

typedef enum {
    SOURCE_INITIAL,
    SOURCE_ODD,
    SOURCE_EVEN_FOUR_TIMES
} AnswerSource;

typedef struct {
    bool found;
    mpz_t value;
    uint64_t d;
    bool axis;
    bool factor_four;
    AnswerSource source;
    unsigned prime_count;
    uint64_t primes[MAX_FACTORS];
    uint64_t exponents[MAX_FACTORS];
} Answer;

typedef struct {
    uint64_t factors[MAX_FACTORS];
    unsigned factor_count;
    uint64_t split_primes[MAX_FACTORS];
    uint64_t p_primes[MAX_FACTORS]; /* 1 (mod 8). */
    uint64_t q_primes[MAX_FACTORS]; /* 3 (mod 8). */
    Answer best;
} Search;

typedef struct {
    Search *search;
    uint64_t d;
    bool axis;
    unsigned group_count;
    uint64_t group_exponent[MAX_FACTORS];
    unsigned group_multiplicity[MAX_FACTORS];
    uint64_t p_exponents[MAX_FACTORS];
    uint64_t q_exponents[MAX_FACTORS];
    unsigned p_count;
    unsigned q_count;
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
    unsigned split_count = 0, p_count = 0, q_count = 0;
    for (uint64_t p = 3;
         split_count < MAX_FACTORS || p_count < MAX_FACTORS ||
         q_count < MAX_FACTORS;
         p += 2) {
        if (!is_prime(p)) continue;
        const uint64_t residue = p % 8;
        if (residue != 1 && residue != 3) continue;
        if (split_count < MAX_FACTORS)
            search->split_primes[split_count++] = p;
        if (residue == 1 && p_count < MAX_FACTORS)
            search->p_primes[p_count++] = p;
        if (residue == 3 && q_count < MAX_FACTORS)
            search->q_primes[q_count++] = p;
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
    destination->factor_four = source->factor_four;
    destination->source = source->source;
    destination->prime_count = source->prime_count;
    memcpy(destination->primes, source->primes,
           sizeof(destination->primes));
    memcpy(destination->exponents, source->exponents,
           sizeof(destination->exponents));
}

static const char *source_name(AnswerSource source)
{
    switch (source) {
    case SOURCE_INITIAL: return "initial a(0)";
    case SOURCE_ODD: return "odd k == 1 (mod 8)";
    case SOURCE_EVEN_FOUR_TIMES: return "k=4*u from x^2+2*y^2=u";
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

static void store_if_better(Search *search, const mpz_t value, uint64_t d,
                            bool axis, bool factor_four,
                            AnswerSource source,
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
    search->best.factor_four = factor_four;
    search->best.source = source;
    search->best.prime_count = prime_count;
    for (unsigned i = 0; i < prime_count; ++i) {
        search->best.primes[i] = primes[i];
        search->best.exponents[i] = exponents[i];
    }
}

static void consider_unrestricted(Search *search,
                                  const uint64_t exponents[MAX_FACTORS],
                                  unsigned exponent_count, uint64_t d,
                                  bool axis, bool factor_four,
                                  AnswerSource source)
{
    /* By 9 | a(n), the exponent of the first split prime 3 is at least 2. */
    if (exponent_count == 0 || exponents[0] < 2) return;

    mpz_t value, power;
    mpz_init_set_ui(value, factor_four ? 36U : 9U);
    mpz_init(power);
    for (unsigned i = 0; i < exponent_count; ++i) {
        const uint64_t exponent_in_cofactor =
            exponents[i] - (i == 0 ? 2U : 0U);
        mpz_ui_pow_ui(power, (unsigned long)search->split_primes[i],
                      (unsigned long)exponent_in_cofactor);
        mpz_mul(value, value, power);
    }
    store_if_better(search, value, d, axis, factor_four, source,
                    search->split_primes, exponents, exponent_count);
    mpz_clear(power);
    mpz_clear(value);
}

static void consider_assignment(AssignmentSearch *assignment)
{
    if (assignment->p_count + assignment->q_count > MAX_FACTORS)
        die("too many assigned exponents");
    /* The first Q-type prime is 3.  Construct only candidates 9*u. */
    if (assignment->q_count == 0 || assignment->q_exponents[0] < 2)
        return;

    mpz_t value, power;
    mpz_init_set_ui(value, 9);
    mpz_init(power);
    for (unsigned i = 0; i < assignment->p_count; ++i) {
        mpz_ui_pow_ui(power,
                      (unsigned long)assignment->search->p_primes[i],
                      (unsigned long)assignment->p_exponents[i]);
        mpz_mul(value, value, power);
    }
    for (unsigned i = 0; i < assignment->q_count; ++i) {
        const uint64_t exponent_in_cofactor =
            assignment->q_exponents[i] - (i == 0 ? 2U : 0U);
        mpz_ui_pow_ui(power,
                      (unsigned long)assignment->search->q_primes[i],
                      (unsigned long)exponent_in_cofactor);
        mpz_mul(value, value, power);
    }

    uint64_t primes[MAX_FACTORS] = {0};
    uint64_t exponents[MAX_FACTORS] = {0};
    unsigned pi = 0, qi = 0, count = 0;
    while (pi < assignment->p_count || qi < assignment->q_count) {
        const bool take_q =
            pi == assignment->p_count ||
            (qi < assignment->q_count &&
             assignment->search->q_primes[qi] <
                 assignment->search->p_primes[pi]);
        if (take_q) {
            primes[count] = assignment->search->q_primes[qi];
            exponents[count] = assignment->q_exponents[qi];
            ++qi;
        } else {
            primes[count] = assignment->search->p_primes[pi];
            exponents[count] = assignment->p_exponents[pi];
            ++pi;
        }
        ++count;
    }
    store_if_better(assignment->search, value, assignment->d,
                    assignment->axis, false, SOURCE_ODD,
                    primes, exponents, count);
    mpz_clear(power);
    mpz_clear(value);
}

static void assignment_rec(AssignmentSearch *assignment,
                           unsigned group_index, unsigned parity)
{
    if (group_index == assignment->group_count) {
        if (parity == 0) consider_assignment(assignment);
        return;
    }

    const uint64_t exponent =
        assignment->group_exponent[group_index];
    const unsigned multiplicity =
        assignment->group_multiplicity[group_index];
    const unsigned old_p_count = assignment->p_count;
    const unsigned old_q_count = assignment->q_count;

    for (unsigned to_q = 0; to_q <= multiplicity; ++to_q) {
        assignment->p_count = old_p_count;
        assignment->q_count = old_q_count;
        for (unsigned i = 0; i < to_q; ++i)
            assignment->q_exponents[assignment->q_count++] = exponent;
        for (unsigned i = to_q; i < multiplicity; ++i)
            assignment->p_exponents[assignment->p_count++] = exponent;
        const unsigned next_parity =
            parity ^ (unsigned)((exponent & 1) && (to_q & 1));
        assignment_rec(assignment, group_index + 1, next_parity);
    }
    assignment->p_count = old_p_count;
    assignment->q_count = old_q_count;
}

static void consider_constrained(Search *search,
                                 const uint64_t exponents[MAX_FACTORS],
                                 unsigned exponent_count, uint64_t d,
                                 bool axis)
{
    AssignmentSearch assignment;
    memset(&assignment, 0, sizeof(assignment));
    assignment.search = search;
    assignment.d = d;
    assignment.axis = axis;

    for (unsigned i = 0; i < exponent_count; ++i) {
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

    /* Odd square candidates automatically satisfy k == 1 (mod 8). */
    if (axis)
        consider_unrestricted(search, exponents, search->factor_count,
                              d, axis, false, SOURCE_ODD);
    else
        consider_constrained(search, exponents, search->factor_count,
                             d, axis);

    /* Every x^2+2y^2 candidate u also gives the even candidate k=4*u. */
    consider_unrestricted(search, exponents, search->factor_count,
                          d, axis, true, SOURCE_EVEN_FOUR_TIMES);
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

    if (n == 0) {
        search.best.found = true;
        search.best.source = SOURCE_INITIAL;
        mpz_set_ui(search.best.value, 0);
    } else if (n == 2) {
        /* The unique exception to the divisibility theorem: 33=3*11. */
        search.best.found = true;
        search.best.d = 4;
        search.best.axis = false;
        search.best.source = SOURCE_ODD;
        search.best.prime_count = 2;
        search.best.primes[0] = 3;
        search.best.exponents[0] = 1;
        search.best.primes[1] = 11;
        search.best.exponents[1] = 1;
        mpz_set_ui(search.best.value, 33);
    } else {
        const uint64_t twice = 2 * n;
        search_case(&search, twice, false);
        search_case(&search, twice + 1, true);
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

static uint64_t representation_count_q2(uint64_t k)
{
    if (k == 0) return 0;
    uint64_t remaining = k;
    while ((remaining & 1) == 0) remaining >>= 1;

    uint64_t d = 1;
    for (uint64_t p = 3; p <= remaining / p; p += 2) {
        if (remaining % p != 0) continue;
        uint64_t exponent = 0;
        do {
            remaining /= p;
            ++exponent;
        } while (remaining % p == 0);
        if (p % 8 == 1 || p % 8 == 3)
            d *= exponent + 1;
        else if ((exponent & 1) != 0)
            return 0;
    }
    if (remaining > 1) {
        if (remaining % 8 == 1 || remaining % 8 == 3)
            d *= 2;
        else
            return 0;
    }

    uint64_t axis = 0;
    const uint64_t root = isqrt_u64(k);
    if (root * root == k) ++axis;
    if ((k & 1) == 0) {
        const uint64_t half = k / 2;
        const uint64_t half_root = isqrt_u64(half);
        if (half_root * half_root == half) ++axis;
    }
    return (d - axis) / 2;
}

static uint64_t representation_count_q8(uint64_t k)
{
    if (k == 0) return 0;
    if ((k & 1) != 0) {
        if (k % 8 != 1) return 0;
        return representation_count_q2(k);
    }
    if (k % 4 != 0) return 0;
    return representation_count_q2(k / 4);
}

static void self_test(uint64_t limit)
{
    if (limit > UINT32_MAX)
        die("self-test limit must be at most 2^32-1");
    uint32_t *direct = calloc((size_t)limit + 1, sizeof(*direct));
    if (direct == NULL) die("could not allocate self-test table");

    for (uint64_t y = 1; 8 * y * y <= limit; ++y) {
        const uint64_t y_term = 8 * y * y;
        for (uint64_t x = 1; x * x <= limit - y_term; ++x)
            ++direct[x * x + y_term];
    }

    uint64_t minima[GUARANTEED_UPTO + 1];
    for (uint64_t n = 0; n <= GUARANTEED_UPTO; ++n)
        minima[n] = UINT64_MAX;
    for (uint64_t k = 0; k <= limit; ++k) {
        const uint64_t got = representation_count_q8(k);
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

    static const Regression known_regressions[] = {
        {0, 0}, {1, 9}, {2, 33}, {3, 153}, {4, 297}, {5, 1377},
        {6, 1881}, {7, 9801}, {8, 5049}, {9, 18513}, {10, 16929},
        {11, 1003833}, {12, 31977}, {13, 314721}, {14, 152361},
        {15, 166617}, {16, 207009}, {17, 10673289}, {18, 543609},
        {19, 64304361}, {20, 287793}, {21, 1499553}, {22, 2832489},
        {24, 1055241}, {25, 20160657}, {27, 6683193},
        {28, 2590137}, {30, 4892481}
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
            passed = passed && answer.source == SOURCE_INITIAL &&
                     mpz_cmp_ui(answer.value, 0) == 0;
        } else {
            uint64_t reconstructed_d = 1;
            bool all_even = true;
            uint64_t q_parity = 0;
            for (unsigned i = 0; i < answer.prime_count; ++i) {
                reconstructed_d *= answer.exponents[i] + 1;
                if ((answer.exponents[i] & 1) != 0) all_even = false;
                if (answer.primes[i] % 8 == 3)
                    q_parity ^= answer.exponents[i] & 1;
            }
            passed = passed && reconstructed_d == answer.d &&
                     all_even == answer.axis &&
                     (reconstructed_d - (answer.axis ? 1U : 0U)) / 2 == n;
            if (answer.source == SOURCE_ODD)
                passed = passed && !answer.factor_four && q_parity == 0 &&
                         mpz_fdiv_ui(answer.value, 8) == 1;
            else if (answer.source == SOURCE_EVEN_FOUR_TIMES)
                passed = passed && answer.factor_four &&
                         mpz_divisible_ui_p(answer.value, 4) != 0;
            else
                passed = false;
        }

        if (n == 2)
            passed = passed && mpz_cmp_ui(answer.value, 33) == 0;
        else
            passed = passed && mpz_divisible_ui_p(answer.value, 9) != 0;

        if (passed && mpz_fits_ulong_p(answer.value)) {
            const unsigned long value = mpz_get_ui(answer.value);
            passed = (uint64_t)value == value &&
                     representation_count_q8((uint64_t)value) == n;
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
           "regressions agree, the 9-divisibility theorem holds, and exact "
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
                   " positive representations\n", n);
        } else {
            printf("the least solution is within the checked bound ");
            print_bigint(stdout, limit);
            putchar('\n');
        }
    }
    if (verbose && n != 0) {
        printf("source: %s; underlying A=%u, D=%" PRIu64
               "; factorization: ",
               source_name(answer->source), answer->axis ? 1U : 0U,
               answer->d);
        bool wrote = false;
        if (answer->factor_four) {
            printf("2^2");
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
