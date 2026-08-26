/*
 * A349257: largest integer representable as
 *
 *                  sum_{k=1..n} k / p(k),
 *
 * where p is a permutation of [n].
 *
 * Writing q=p^{-1} gives the equivalent sum sum_{j=1..n} q(j)/j.
 * Let L=lcm(1,...,n).  The search therefore uses only integers:
 *
 *                  sum q(j)*(L/j) = L*t.
 *
 * Candidate integers t are tested downwards from the unrestricted assignment
 * maximum.  A depth-first exact-assignment search is pruned by the
 * rearrangement-inequality bounds, divisibility by the gcd of the unassigned
 * weights, and a permutation-lattice congruence.  The next denominator is
 * chosen by minimum remaining values; in particular, prime and prime-power
 * divisibility restrictions are exposed near the root.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic 349257_01.c -o 349257_01
 *
 * Usage:
 *   ./349257_01                 # print a(0)..a(19)
 *   ./349257_01 --upto 19
 *   ./349257_01 --term 19
 *   ./349257_01 --term 19 --witness
 *   ./349257_01 --check
 */

#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_N 31
#define DEFAULT_MAX_N 19
#define KNOWN_MAX_N 19
#define DIRECT_CHECK_MAX_N 10

_Static_assert(MAX_N <= 31, "uint32_t masks support at most n=31");

static const uint64_t known[KNOWN_MAX_N + 1] = {
    UINT64_C(0),  UINT64_C(1),  UINT64_C(2),  UINT64_C(3),
    UINT64_C(6),  UINT64_C(7),  UINT64_C(10), UINT64_C(11),
    UINT64_C(15), UINT64_C(18), UINT64_C(21), UINT64_C(22),
    UINT64_C(27), UINT64_C(28), UINT64_C(32), UINT64_C(36),
    UINT64_C(40), UINT64_C(41), UINT64_C(46), UINT64_C(47)
};

typedef struct {
    int n;
    uint64_t lcm;
    uint64_t weight[MAX_N + 1];
    int inverse[MAX_N + 1];
    uint64_t nodes;
} Search;

typedef enum {
    MODE_UPTO,
    MODE_TERM
} OutputMode;

static _Noreturn void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static double monotonic_seconds(void)
{
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) != 0)
        die("clock_gettime failed");
    return (double)now.tv_sec + (double)now.tv_nsec / 1000000000.0;
}

static uint64_t gcd_u64(uint64_t a, uint64_t b)
{
    while (b != 0) {
        const uint64_t remainder = a % b;
        a = b;
        b = remainder;
    }
    return a;
}

static int parse_n(const char *text)
{
    errno = 0;
    char *end = NULL;
    const uintmax_t value = strtoumax(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' || value > MAX_N) {
        fprintf(stderr, "error: n must be in 0..%d: %s\n", MAX_N, text);
        exit(EXIT_FAILURE);
    }
    return (int)value;
}

static uint64_t make_lcm(int n)
{
    uint64_t value = 1;
    for (uint64_t k = 2; k <= (uint64_t)n; ++k) {
        const uint64_t divisor = gcd_u64(value, k);
        if (value > UINT64_MAX / (k / divisor))
            die("LCM overflow");
        value *= k / divisor;
    }
    return value;
}

static unsigned bit_count(uint32_t mask)
{
#if defined(__clang__) || defined(__GNUC__)
    return (unsigned)__builtin_popcount(mask);
#else
    unsigned count = 0;
    while (mask != 0) {
        mask &= mask - 1U;
        ++count;
    }
    return count;
#endif
}

static int first_value(uint32_t mask)
{
#if defined(__clang__) || defined(__GNUC__)
    return (int)__builtin_ctz(mask) + 1;
#else
    int value = 1;
    while ((mask & 1U) == 0) {
        mask >>= 1U;
        ++value;
    }
    return value;
#endif
}

/* Exact minimum and maximum assignments for the two remaining sets. */
static void assignment_bounds(const Search *search, uint32_t denominators,
                              uint32_t numerators, uint64_t *minimum,
                              uint64_t *maximum)
{
    int ascending[MAX_N];
    int count = 0;
    for (int value = 1; value <= search->n; ++value)
        if ((numerators & (UINT32_C(1) << (value - 1))) != 0)
            ascending[count++] = value;

    uint64_t low = 0;
    uint64_t high = 0;
    int rank = 0;
    /* j increases, so L/j decreases. */
    for (int j = 1; j <= search->n; ++j) {
        if ((denominators & (UINT32_C(1) << (j - 1))) == 0) continue;
        low += (uint64_t)ascending[rank] * search->weight[j];
        high += (uint64_t)ascending[count - 1 - rank] * search->weight[j];
        ++rank;
    }
    *minimum = low;
    *maximum = high;
}

/*
 * Every assignment sum is congruent to a fixed assignment modulo
 *
 *   gcd(a-b : a,b in numerators) * gcd(x-y : x,y in weights).
 *
 * This follows because exchanging a,b between weights x,y changes a sum by
 * (a-b)*(x-y).  It is only a necessary test, so it cannot remove a solution.
 */
static bool lattice_congruence(const Search *search, uint32_t denominators,
                               uint32_t numerators, uint64_t residual)
{
    if (bit_count(numerators) < 2U) return true;

    uint64_t base = 0;
    uint64_t numerator_gcd = 0;
    uint64_t weight_gcd = 0;
    int first_numerator = 0;
    uint64_t first_weight = 0;
    uint32_t number_scan = numerators;

    while (number_scan != 0) {
        const int value = first_value(number_scan);
        number_scan &= number_scan - 1U;
        if (first_numerator == 0)
            first_numerator = value;
        else
            numerator_gcd = gcd_u64(numerator_gcd,
                                    (uint64_t)(value - first_numerator));
    }

    int numerator_index = 0;
    int ordered_numerators[MAX_N];
    for (int value = 1; value <= search->n; ++value)
        if ((numerators & (UINT32_C(1) << (value - 1))) != 0)
            ordered_numerators[numerator_index++] = value;

    int index = 0;
    for (int j = 1; j <= search->n; ++j) {
        if ((denominators & (UINT32_C(1) << (j - 1))) == 0) continue;
        const uint64_t weight = search->weight[j];
        base += (uint64_t)ordered_numerators[index++] * weight;
        if (first_weight == 0)
            first_weight = weight;
        else {
            const uint64_t difference = first_weight > weight
                                            ? first_weight - weight
                                            : weight - first_weight;
            weight_gcd = gcd_u64(weight_gcd, difference);
        }
    }

    const uint64_t modulus = numerator_gcd * weight_gcd;
    return modulus == 0 || residual % modulus == base % modulus;
}

static uint64_t gcd_without(const Search *search, uint32_t denominators,
                            int omitted)
{
    uint64_t result = 0;
    for (int j = 1; j <= search->n; ++j) {
        if (j == omitted ||
            (denominators & (UINT32_C(1) << (j - 1))) == 0)
            continue;
        result = gcd_u64(result, search->weight[j]);
    }
    return result;
}

static bool exact_assignment(Search *search, uint32_t denominators,
                             uint32_t numerators, uint64_t residual)
{
    ++search->nodes;
    if (denominators == 0) return residual == 0;

    uint64_t minimum, maximum;
    assignment_bounds(search, denominators, numerators, &minimum, &maximum);
    if (residual < minimum || residual > maximum) return false;
    if (!lattice_congruence(search, denominators, numerators, residual))
        return false;

    int chosen_denominator = 0;
    uint64_t chosen_gcd = 0;
    int smallest_candidate_count = search->n + 1;

    for (int j = 1; j <= search->n; ++j) {
        if ((denominators & (UINT32_C(1) << (j - 1))) == 0) continue;
        const uint64_t remaining_gcd = gcd_without(search, denominators, j);
        int candidate_count = 0;
        for (int value = 1; value <= search->n; ++value) {
            if ((numerators & (UINT32_C(1) << (value - 1))) == 0) continue;
            const uint64_t term = (uint64_t)value * search->weight[j];
            if (term > residual) continue;
            if (remaining_gcd == 0) {
                if (term == residual) ++candidate_count;
            } else if ((residual - term) % remaining_gcd == 0) {
                ++candidate_count;
            }
        }
        if (candidate_count < smallest_candidate_count ||
            (candidate_count == smallest_candidate_count &&
             (chosen_denominator == 0 || j < chosen_denominator))) {
            chosen_denominator = j;
            chosen_gcd = remaining_gcd;
            smallest_candidate_count = candidate_count;
        }
    }

    if (smallest_candidate_count == 0) return false;

    const uint32_t denominator_bit =
        UINT32_C(1) << (chosen_denominator - 1);
    /* Descending order first follows the unrestricted maximizing assignment. */
    for (int value = search->n; value >= 1; --value) {
        const uint32_t numerator_bit = UINT32_C(1) << (value - 1);
        if ((numerators & numerator_bit) == 0) continue;
        const uint64_t term =
            (uint64_t)value * search->weight[chosen_denominator];
        if (term > residual) continue;
        if (chosen_gcd == 0) {
            if (term != residual) continue;
        } else if ((residual - term) % chosen_gcd != 0) {
            continue;
        }

        search->inverse[chosen_denominator] = value;
        if (exact_assignment(search, denominators ^ denominator_bit,
                             numerators ^ numerator_bit, residual - term))
            return true;
    }
    search->inverse[chosen_denominator] = 0;
    return false;
}

static uint64_t compute_term(int n, int witness[MAX_N + 1],
                             uint64_t *node_count)
{
    if (n == 0) {
        if (witness != NULL) memset(witness, 0, sizeof(int) * (MAX_N + 1));
        if (node_count != NULL) *node_count = 0;
        return 0;
    }

    Search search;
    memset(&search, 0, sizeof(search));
    search.n = n;
    search.lcm = make_lcm(n);
    for (int j = 1; j <= n; ++j)
        search.weight[j] = search.lcm / (uint64_t)j;

    const uint32_t full_mask = (UINT32_C(1) << n) - 1U;
    uint64_t minimum, maximum;
    assignment_bounds(&search, full_mask, full_mask, &minimum, &maximum);
    (void)minimum;

    for (uint64_t target = maximum / search.lcm;; --target) {
        memset(search.inverse, 0, sizeof(search.inverse));
        if (exact_assignment(&search, full_mask, full_mask,
                             target * search.lcm)) {
            if (witness != NULL)
                memcpy(witness, search.inverse, sizeof(search.inverse));
            if (node_count != NULL) *node_count = search.nodes;
            return target;
        }
        if (target == 0) die("internal search failure");
    }
}

static void verify_term(uint64_t value, int n)
{
    if (n <= KNOWN_MAX_N && value != known[n]) {
        fprintf(stderr,
                "error: A349257 mismatch at n=%d: got %" PRIu64
                ", expected %" PRIu64 "\n",
                n, value, known[n]);
        exit(EXIT_FAILURE);
    }
}

static bool next_permutation(int values[MAX_N], int n)
{
    int i = n - 2;
    while (i >= 0 && values[i] >= values[i + 1]) --i;
    if (i < 0) return false;

    int j = n - 1;
    while (values[j] <= values[i]) --j;
    const int temporary = values[i];
    values[i] = values[j];
    values[j] = temporary;

    for (int left = i + 1, right = n - 1; left < right;
         ++left, --right) {
        const int swap = values[left];
        values[left] = values[right];
        values[right] = swap;
    }
    return true;
}

/* Definition-level reference used only by --check. */
static uint64_t direct_term(int n)
{
    if (n == 0) return 0;
    const uint64_t lcm = make_lcm(n);
    int inverse[MAX_N];
    for (int i = 0; i < n; ++i) inverse[i] = i + 1;

    uint64_t best = 0;
    do {
        uint64_t scaled_sum = 0;
        for (int j = 1; j <= n; ++j)
            scaled_sum += (uint64_t)inverse[j - 1] * (lcm / (uint64_t)j);
        if (scaled_sum % lcm == 0 && scaled_sum / lcm > best)
            best = scaled_sum / lcm;
    } while (next_permutation(inverse, n));
    return best;
}

static void verify_witness(const int inverse[MAX_N + 1], int n,
                           uint64_t target)
{
    const uint64_t lcm = make_lcm(n);
    uint64_t scaled_sum = 0;
    uint32_t seen = 0;
    for (int j = 1; j <= n; ++j) {
        const int value = inverse[j];
        if (value < 1 || value > n) die("invalid witness value");
        const uint32_t bit = UINT32_C(1) << (value - 1);
        if ((seen & bit) != 0) die("duplicate witness value");
        seen |= bit;
        scaled_sum += (uint64_t)value * (lcm / (uint64_t)j);
    }
    if (scaled_sum != target * lcm) die("witness sum mismatch");
}

static void print_witness(const int inverse[MAX_N + 1], int n)
{
    int permutation[MAX_N + 1] = {0};
    for (int denominator = 1; denominator <= n; ++denominator)
        permutation[inverse[denominator]] = denominator;

    fprintf(stderr, "q=p^-1: [");
    for (int j = 1; j <= n; ++j)
        fprintf(stderr, "%s%d", j == 1 ? "" : ",", inverse[j]);
    fprintf(stderr, "]\np:      [");
    for (int k = 1; k <= n; ++k)
        fprintf(stderr, "%s%d", k == 1 ? "" : ",", permutation[k]);
    fprintf(stderr, "]\n");
}

static void usage(FILE *stream, const char *program)
{
    fprintf(stream,
            "Usage: %s [--upto N | --term N | --check] [--witness] "
            "[--verbose]\n",
            program);
}

int main(int argc, char **argv)
{
    OutputMode mode = MODE_UPTO;
    int limit = DEFAULT_MAX_N;
    bool check = false;
    bool show_witness = false;
    bool verbose = false;
    bool mode_seen = false;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--upto") == 0 ||
            strcmp(argv[i], "--term") == 0) {
            if (mode_seen || i + 1 == argc) {
                usage(stderr, argv[0]);
                return EXIT_FAILURE;
            }
            mode = strcmp(argv[i], "--term") == 0 ? MODE_TERM : MODE_UPTO;
            limit = parse_n(argv[++i]);
            mode_seen = true;
        } else if (strcmp(argv[i], "--check") == 0) {
            if (mode_seen) {
                usage(stderr, argv[0]);
                return EXIT_FAILURE;
            }
            check = true;
            limit = KNOWN_MAX_N;
            mode = MODE_UPTO;
            mode_seen = true;
        } else if (strcmp(argv[i], "--witness") == 0) {
            show_witness = true;
        } else if (strcmp(argv[i], "--verbose") == 0) {
            verbose = true;
        } else if (strcmp(argv[i], "--help") == 0 ||
                   strcmp(argv[i], "-h") == 0) {
            usage(stdout, argv[0]);
            return EXIT_SUCCESS;
        } else {
            fprintf(stderr, "error: unknown option: %s\n", argv[i]);
            usage(stderr, argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (show_witness && mode != MODE_TERM)
        die("--witness requires --term N");

    const int first = mode == MODE_TERM ? limit : 0;
    for (int n = first; n <= limit; ++n) {
        int witness[MAX_N + 1] = {0};
        uint64_t nodes = 0;
        const double started = monotonic_seconds();
        const uint64_t value = compute_term(n, witness, &nodes);
        verify_term(value, n);
        if (n != 0) verify_witness(witness, n, value);

        if (check && n <= DIRECT_CHECK_MAX_N) {
            const uint64_t direct = direct_term(n);
            if (direct != value) {
                fprintf(stderr,
                        "error: direct permutation mismatch at n=%d: "
                        "search=%" PRIu64 ", direct=%" PRIu64 "\n",
                        n, value, direct);
                return EXIT_FAILURE;
            }
        }

        if (mode == MODE_TERM)
            printf("%" PRIu64 "\n", value);
        else
            printf("%s%" PRIu64, n == 0 ? "" : ", ", value);

        if (fflush(stdout) != 0) die("could not flush stdout");

        if (verbose)
            fprintf(stderr,
                    "349257_01: n=%d, a(n)=%" PRIu64
                    ", nodes=%" PRIu64 ", %.3f s\n",
                    n, value, nodes, monotonic_seconds() - started);
        if (show_witness) print_witness(witness, n);
    }
    if (mode == MODE_UPTO) {
        if (putchar('\n') == EOF || fflush(stdout) != 0)
            die("could not finish stdout");
    }

    if (check)
        fprintf(stderr,
                "check passed: A349257(0..%d), direct permutations through "
                "n=%d\n",
                KNOWN_MAX_N, DIRECT_CHECK_MAX_N);
    return EXIT_SUCCESS;
}
