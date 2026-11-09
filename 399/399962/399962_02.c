/* A399962: largest subset of {1,...,n} with at most two representations
 * of each sum x+y, counting x<=y (including x=y).
 */
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    size_t n;
    size_t target;
    size_t *selected;
    unsigned char *counts;
} Search;

static bool complete(Search *search, size_t start, size_t size)
{
    if (size == search->target)
        return true;

    /* Leave enough interior elements to reach the target size. */
    size_t last = search->n - (search->target - size);
    for (size_t x = start; x <= last; ++x) {
        if (search->counts[2 * x] == 2)
            continue;
        size_t i = 0;
        while (i < size && search->counts[x + search->selected[i]] < 2)
            ++i;
        if (i != size)
            continue;

        /* The new sums x+y are distinct, including 2*x. */
        ++search->counts[2 * x];
        for (i = 0; i < size; ++i)
            ++search->counts[x + search->selected[i]];
        search->selected[size] = x;
        if (complete(search, x + 1, size + 1))
            return true;
        for (i = 0; i < size; ++i)
            --search->counts[x + search->selected[i]];
        --search->counts[2 * x];
    }
    return false;
}


typedef struct {
    uint64_t low;
    uint64_t high;
} SumMask;

static uint64_t low_bits(size_t k)
{
    return k == 64 ? UINT64_MAX : (UINT64_C(1) << k) - 1;
}

/* Bit y-1 in the result represents the saturated sum a+y. */
static uint64_t conflicts(SumMask saturated, size_t a)
{
    size_t shift = a - 1;
    return shift == 0 ? saturated.low :
        (saturated.low >> shift) | ((saturated.high & low_bits(shift)) << (64 - shift));
}

static void add_sum(Search *search, size_t sum, SumMask *newly_full,
                    uint64_t *diagonal)
{
    if (++search->counts[sum] == 2) {
        size_t bit = sum - 2;
        if (bit < 64)
            newly_full->low |= UINT64_C(1) << bit;
        else
            newly_full->high |= UINT64_C(1) << (bit - 64);
        if (sum % 2 == 0)
            *diagonal |= UINT64_C(1) << (sum / 2 - 1);
    }
}

/* available contains exactly the individually feasible interior extensions
 * in the remaining ordered search range and chosen reflection orientation.
 */
static bool complete_mask(Search *search, uint64_t available, size_t size,
                          SumMask saturated)
{
    size_t needed = search->target - size;
    if (needed == 0)
        return true;
    if (needed == 1)
        return available != 0;

    while ((size_t)__builtin_popcountll(available) >= needed) {
        size_t x = (size_t)__builtin_ctzll(available) + 1;
        available &= available - 1;
        uint64_t next = available;

        /* Reflection about (n+1)/2 swaps the first and last interior
         * points. Keep the orientation where their sum is <= n+1.
         */
        if (size == 2)
            next &= low_bits(search->n + 1 - x);
        if ((size_t)__builtin_popcountll(next) < needed - 1)
            continue;

        SumMask newly_full = {0, 0};
        uint64_t diagonal = 0;
        add_sum(search, 2 * x, &newly_full, &diagonal);
        for (size_t i = 0; i < size; ++i)
            add_sum(search, x + search->selected[i], &newly_full, &diagonal);

        SumMask all_full = {saturated.low | newly_full.low,
                            saturated.high | newly_full.high};
        next &= ~(diagonal | conflicts(all_full, x));
        if (newly_full.low != 0 || newly_full.high != 0) {
            uint64_t forbidden = 0;
            for (size_t i = 0; i < size; ++i)
                forbidden |= conflicts(newly_full, search->selected[i]);
            next &= ~forbidden;
        }

        search->selected[size] = x;
        if (complete_mask(search, next, size + 1, all_full))
            return true;
        for (size_t i = 0; i < size; ++i)
            --search->counts[x + search->selected[i]];
        --search->counts[2 * x];
    }
    return false;
}

static bool find_increase(size_t n, size_t target, bool *found)
{
    if (target > SIZE_MAX / sizeof(size_t))
        return false;

    Search search = {n, target, NULL, NULL};
    search.selected = malloc(target * sizeof *search.selected);
    search.counts = calloc(2 * n + 1, sizeof *search.counts);
    if (search.selected == NULL || search.counts == NULL) {
        free(search.selected);
        free(search.counts);
        return false;
    }

    /* A set larger than the previous maximum must contain n.
     * If it omitted 1, translating it down by 1 would give a set of
     * the same size in {1,...,n-1}, with unchanged sum multiplicities.
     */
    search.selected[0] = 1;
    search.selected[1] = n;
    search.counts[2] = 1;
    search.counts[1 + n] = 1;
    search.counts[2 * n] = 1;
    if (n <= 64) {
        SumMask saturated = {0, 0};
        *found = complete_mask(&search, low_bits(n - 1) & ~UINT64_C(1),
                               2, saturated);
    } else {
        *found = complete(&search, 2, 2);
    }

    free(search.selected);
    free(search.counts);
    return true;
}

static bool parse_terms(const char *arg, size_t *terms)
{
    if (*arg < '0' || *arg > '9')
        return false;
    char *end;
    errno = 0;
    uintmax_t value = strtoumax(arg, &end, 10);
    if (errno != 0 || *end != '\0' || value == 0 ||
        value > (SIZE_MAX - 1) / 2)
        return false;
    *terms = (size_t)value;
    return true;
}

int main(int argc, char **argv)
{
    size_t terms = 32;
    if (argc > 2 || (argc == 2 && !parse_terms(argv[1], &terms))) {
        fprintf(stderr, "Usage: %s [positive-term-count]\n", argv[0]);
        return EXIT_FAILURE;
    }

    size_t maximum = 1;
    for (size_t n = 1; n <= terms; ++n) {
        /* Deleting n from a feasible set gives a(n) <= a(n-1)+1. */
        if (n > 1) {
            bool found;
            if (!find_increase(n, maximum + 1, &found)) {
                fprintf(stderr, "\nCannot allocate search memory for n=%zu.\n", n);
                return EXIT_FAILURE;
            }
            if (found)
                ++maximum;
        }
        if (printf("%s%zu", n == 1 ? "" : ", ", maximum) < 0 ||
            fflush(stdout) == EOF)
            return EXIT_FAILURE;
    }
    return putchar('\n') == EOF || fflush(stdout) == EOF
        ? EXIT_FAILURE : EXIT_SUCCESS;
}
