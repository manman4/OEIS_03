/*
 * A306634 -- exact, independent search (no OEIS b-file input).
 *
 * Search for k such that the Perrin number P(6*k+2) is divisible by 6*k+2,
 * where P(0)=3, P(1)=0, P(2)=2 and P(n)=P(n-2)+P(n-3).
 *
 * The search is exhaustive in the requested inclusive interval.  It uses
 * only necessary filters, so a term cannot be discarded accidentally:
 *
 *   1. P(n) modulo 2 has period 7, with zeros at 1, 2, 4 modulo 7.
 *   2. For several small d, the complete period of P(n) modulo d is made at
 *      startup.  If d divides n but P(n) is nonzero modulo d, then n cannot
 *      divide P(n).
 *   3. Every survivor receives the exact full congruence test.
 *
 * For the full test write n=2^a*q with q odd.  The 2-power part is computed
 * exactly with masked unsigned arithmetic.  The odd part uses Montgomery
 * multiplication and the Adams-Shanks six-term doubling identities.  Thus
 * there is no floating point, probabilistic test, unchecked overflow, or
 * dependency on a precomputed pseudoprime list.
 *
 * Segments are distributed dynamically over POSIX threads.  Matches are
 * collected and sorted, hence stdout is deterministic even with many threads.
 * Each output line is a value k (not a b-file record).
 *
 * Supported range: 0 <= k <= (2^63-3)/6.  Keeping n=6*k+2 below 2^63 makes
 * the Montgomery reduction proof and every range calculation straightforward.
 * This is far beyond the known fourth term 123725995972.
 *
 * Build:
 *   cc -O3 -march=native -std=c11 -Wall -Wextra -Wpedantic -pthread \
 *      306634_02.c -o 306634_02
 *
 * Usage:
 *   ./306634_02 START_K END_K [THREADS [SEGMENT_SIZE]]
 *   ./306634_02 --check K
 *   ./306634_02 --self-test
 *
 * Examples:
 *   ./306634_02 0 5000000
 *   ./306634_02 0 123725995972 12 1048576
 */

#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#if defined(__APPLE__)
extern int sysctlbyname(const char *, void *, size_t *, void *, size_t);
#endif

#if !defined(__SIZEOF_INT128__)
#error "306634_02.c requires unsigned __int128"
#endif

__extension__ typedef unsigned __int128 u128;

#define DEFAULT_SEGMENT UINT64_C(1048576)
#define MIN_SEGMENT UINT64_C(4096)
#define MAX_SEGMENT UINT64_C(16777216)
#define MAX_THREADS 256U
#define MAX_FILTER_PERIOD 5000U
#define MAX_FILTERS 40U
#define MAX_K ((UINT64_C(0x7fffffffffffffff) - 2) / 6)

typedef struct {
    uint32_t divisor;
    uint32_t period;
    uint32_t step;
    uint32_t k_residue;
    uint8_t *zero;
} Filter;

typedef struct {
    uint64_t modulus;
    uint64_t negative_inverse;
    uint64_t one;
} Montgomery;

typedef struct {
    uint64_t start;
    uint64_t end;
    uint64_t segment_size;
    uint64_t segment_count;
    unsigned thread_count;
    Filter filters[MAX_FILTERS];
    size_t filter_count;

    atomic_uint_fast64_t next_segment;
    atomic_uint_fast64_t completed_segments;
    atomic_uint_fast64_t next_report_segment;
    atomic_uint_fast64_t parity_survivors;
    atomic_uint_fast64_t full_tests;

    pthread_mutex_t match_lock;
    uint64_t *matches;
    size_t match_count;
    size_t match_capacity;
    double started_at;
} Search;

typedef struct {
    Search *search;
    uint8_t *alive;
} Worker;

static _Noreturn void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static void *xmalloc(size_t size)
{
    void *p = malloc(size == 0 ? 1 : size);
    if (p == NULL) die("out of memory");
    return p;
}

static uint64_t parse_u64(const char *text, uint64_t maximum,
                          const char *name)
{
    char *end = NULL;
    if (text == NULL || *text == '\0' || *text == '-') {
        fprintf(stderr, "error: invalid %s: %s\n", name,
                text == NULL ? "(null)" : text);
        exit(EXIT_FAILURE);
    }
    errno = 0;
    const uintmax_t value = strtoumax(text, &end, 10);
    if (errno == ERANGE || end == text || *end != '\0' || value > maximum) {
        fprintf(stderr, "error: %s must be in 0..%" PRIu64 ": %s\n",
                name, maximum, text);
        exit(EXIT_FAILURE);
    }
    return (uint64_t)value;
}

static double monotonic_seconds(void)
{
    struct timespec value;
    if (clock_gettime(CLOCK_MONOTONIC, &value) != 0)
        die("clock_gettime failed");
    return (double)value.tv_sec + 1.0e-9 * (double)value.tv_nsec;
}

static unsigned default_thread_count(void)
{
#if defined(__APPLE__)
    int logical_cpus = 1;
    size_t size = sizeof(logical_cpus);
    if (sysctlbyname("hw.logicalcpu", &logical_cpus, &size, NULL, 0) != 0 ||
        logical_cpus < 1)
        return 1;
    const long count = logical_cpus;
#else
    const long count = sysconf(_SC_NPROCESSORS_ONLN);
#endif
    if (count < 1) return 1;
    if (count > (long)MAX_THREADS) return MAX_THREADS;
    return (unsigned)count;
}

static uint32_t gcd_u32(uint32_t a, uint32_t b)
{
    while (b != 0) {
        const uint32_t r = a % b;
        a = b;
        b = r;
    }
    return a;
}

/* The listed periods are checked from the recurrence at startup. */
typedef struct {
    uint32_t divisor;
    uint32_t period;
} FilterSpec;

static const FilterSpec filter_specs[] = {
    {4, 14}, {5, 24}, {7, 48}, {11, 120}, {13, 183}, {17, 288},
    {19, 180}, {23, 22}, {25, 120}, {29, 871}, {31, 993},
    {37, 1368}, {41, 1723}, {43, 231}, {47, 2257}, {53, 1404},
    {59, 58}, {61, 930}, {101, 100}, {121, 1320}, {137, 391},
    {167, 166}, {173, 172}, {211, 210}, {223, 111}, {271, 270},
    {307, 306}, {347, 173}, {449, 224}, {463, 231}, {593, 592}
};

static uint32_t solve_k_residue(uint32_t divisor, uint32_t step)
{
    for (uint32_t k = 0; k < step; ++k)
        if ((6 * (uint64_t)k + 2) % divisor == 0) return k;
    die("internal filter congruence has no solution");
}

static void make_filters(Search *search)
{
    const size_t count = sizeof(filter_specs) / sizeof(filter_specs[0]);
    if (count > MAX_FILTERS) die("too many filters");
    search->filter_count = count;

    for (size_t i = 0; i < count; ++i) {
        Filter *filter = &search->filters[i];
        filter->divisor = filter_specs[i].divisor;
        filter->period = filter_specs[i].period;
        if (filter->period > MAX_FILTER_PERIOD)
            die("filter period exceeds configured maximum");

        const uint32_t common = gcd_u32(6, filter->divisor);
        if (2 % common != 0) die("useless filter divisible by 3");
        filter->step = filter->divisor / common;
        filter->k_residue = solve_k_residue(filter->divisor, filter->step);
        filter->zero = xmalloc(filter->period);

        uint32_t a = 3 % filter->divisor;
        uint32_t b = 0;
        uint32_t c = 2 % filter->divisor;
        const uint32_t initial_a = a, initial_b = b, initial_c = c;
        for (uint32_t n = 0; n < filter->period; ++n) {
            filter->zero[n] = (uint8_t)(a == 0);
            const uint32_t next = (a + b) % filter->divisor;
            a = b;
            b = c;
            c = next;
            if (n + 1 < filter->period &&
                a == initial_a && b == initial_b && c == initial_c)
                die("a listed Perrin period is not minimal");
        }
        if (a != initial_a || b != initial_b || c != initial_c)
            die("a listed Perrin period is incorrect");
    }
}

static void free_filters(Search *search)
{
    for (size_t i = 0; i < search->filter_count; ++i) {
        free(search->filters[i].zero);
        search->filters[i].zero = NULL;
    }
}

static uint64_t add_mod(uint64_t a, uint64_t b, uint64_t modulus)
{
    return a >= modulus - b ? a - (modulus - b) : a + b;
}

static uint64_t sub_mod(uint64_t a, uint64_t b, uint64_t modulus)
{
    return a >= b ? a - b : modulus - (b - a);
}

static Montgomery make_montgomery(uint64_t modulus)
{
    if ((modulus & 1) == 0 || modulus >= (UINT64_C(1) << 63))
        die("invalid Montgomery modulus");

    /* Newton iteration gives modulus^(-1) modulo 2^64. */
    uint64_t inverse = 1;
    for (unsigned i = 0; i < 6; ++i)
        inverse *= 2 - modulus * inverse;

    Montgomery result;
    result.modulus = modulus;
    result.negative_inverse = 0 - inverse;
    result.one = (uint64_t)(((u128)1 << 64) % modulus);
    return result;
}

/* Return a*b/R modulo m, where R=2^64 and a,b<m. */
static uint64_t montgomery_multiply(const Montgomery *mont,
                                    uint64_t a, uint64_t b)
{
    const u128 product = (u128)a * b;
    const uint64_t low = (uint64_t)product;
    const uint64_t high = (uint64_t)(product >> 64);
    const uint64_t factor = low * mont->negative_inverse;
    const u128 correction = (u128)factor * mont->modulus;
    const uint64_t correction_low = (uint64_t)correction;
    const uint64_t correction_high = (uint64_t)(correction >> 64);

    const uint64_t low_sum = low + correction_low;
    const uint64_t carry = low_sum < low;
    uint64_t reduced = high + correction_high;
    bool overflow = reduced < high;
    const uint64_t before_carry = reduced;
    reduced += carry;
    overflow = overflow || reduced < before_carry;

    /* The mathematical result before subtraction is below 2*m. */
    if (overflow) return reduced - mont->modulus;
    return reduced >= mont->modulus ? reduced - mont->modulus : reduced;
}

static uint64_t montgomery_square(const Montgomery *mont, uint64_t value)
{
    return montgomery_multiply(mont, value, value);
}

/* Exact P(n)==0 (mod odd_modulus), using six neighboring Perrin values. */
static bool perrin_zero_odd(uint64_t n, uint64_t odd_modulus)
{
    if (odd_modulus == 1) return true;
    const Montgomery mont = make_montgomery(odd_modulus);
    uint64_t s[6], t[6];
    s[0] = mont.one;
    s[1] = odd_modulus - mont.one;
    s[2] = add_mod(add_mod(mont.one, mont.one, odd_modulus), mont.one,
                   odd_modulus);
    s[3] = s[2];
    s[4] = 0;
    s[5] = add_mod(mont.one, mont.one, odd_modulus);

    unsigned top = 63U - (unsigned)__builtin_clzll(n);
    while (top-- != 0) {
        for (unsigned i = 0; i < 6; ++i) {
            const uint64_t twice_opposite =
                add_mod(s[5 - i], s[5 - i], odd_modulus);
            t[i] = sub_mod(montgomery_square(&mont, s[i]),
                           twice_opposite, odd_modulus);
        }
        const uint64_t t01 = sub_mod(t[2], t[1], odd_modulus);
        const uint64_t t34 = sub_mod(t[5], t[4], odd_modulus);
        const uint64_t t45 = add_mod(t34, t[3], odd_modulus);
        if (((n >> top) & 1) != 0) {
            s[0] = t[0]; s[1] = t01; s[2] = t[1];
            s[3] = t[4]; s[4] = t45; s[5] = t[5];
        } else {
            s[0] = t01; s[1] = t[1];
            s[2] = add_mod(t01, t[0], odd_modulus);
            s[3] = t34; s[4] = t[4]; s[5] = t45;
        }
    }
    return s[4] == 0;
}

/* Exact P(n)==0 modulo a power of two.  Overflow is reduction mod 2^64. */
static bool perrin_zero_power_of_two(uint64_t n, uint64_t power_of_two)
{
    const uint64_t mask = power_of_two - 1;
    uint64_t s[6] = {1 & mask, (uint64_t)-1 & mask, 3 & mask,
                     3 & mask, 0, 2 & mask};
    uint64_t t[6];
    unsigned top = 63U - (unsigned)__builtin_clzll(n);
    while (top-- != 0) {
        for (unsigned i = 0; i < 6; ++i)
            t[i] = (s[i] * s[i] - 2 * s[5 - i]) & mask;
        const uint64_t t01 = (t[2] - t[1]) & mask;
        const uint64_t t34 = (t[5] - t[4]) & mask;
        const uint64_t t45 = (t34 + t[3]) & mask;
        if (((n >> top) & 1) != 0) {
            s[0] = t[0]; s[1] = t01; s[2] = t[1];
            s[3] = t[4]; s[4] = t45; s[5] = t[5];
        } else {
            s[0] = t01; s[1] = t[1]; s[2] = (t01 + t[0]) & mask;
            s[3] = t34; s[4] = t[4]; s[5] = t45;
        }
    }
    return s[4] == 0;
}

static bool is_a306634(uint64_t k)
{
    const uint64_t n = 6 * k + 2;
    const unsigned twos = (unsigned)__builtin_ctzll(n);
    const uint64_t power_of_two = UINT64_C(1) << twos;
    if (!perrin_zero_power_of_two(n, power_of_two)) return false;
    return perrin_zero_odd(n, n >> twos);
}

/* A slower, structurally independent reference: power x^n mod x^3-x-1. */
typedef struct {
    uint64_t c[3];
} Poly;

static uint64_t reference_multiply(uint64_t a, uint64_t b, uint64_t modulus)
{
    return (uint64_t)(((u128)a * b) % modulus);
}

static Poly reference_poly_multiply(Poly a, Poly b, uint64_t modulus)
{
    uint64_t d[5] = {0, 0, 0, 0, 0};
    for (unsigned i = 0; i < 3; ++i)
        for (unsigned j = 0; j < 3; ++j)
            d[i + j] = add_mod(d[i + j],
                reference_multiply(a.c[i], b.c[j], modulus), modulus);
    Poly r;
    r.c[0] = add_mod(d[0], d[3], modulus);
    r.c[1] = add_mod(add_mod(d[1], d[3], modulus), d[4], modulus);
    r.c[2] = add_mod(d[2], d[4], modulus);
    return r;
}

static uint64_t reference_perrin_mod(uint64_t n, uint64_t modulus)
{
    Poly result = {{1, 0, 0}}, power = {{0, 1, 0}};
    uint64_t exponent = n;
    while (exponent != 0) {
        if ((exponent & 1) != 0)
            result = reference_poly_multiply(result, power, modulus);
        exponent >>= 1;
        if (exponent != 0)
            power = reference_poly_multiply(power, power, modulus);
    }
    uint64_t value = reference_multiply(3, result.c[0], modulus);
    return add_mod(value, reference_multiply(2, result.c[2], modulus),
                   modulus);
}

static uint64_t first_at_or_after(uint64_t lower, uint32_t residue,
                                  uint32_t step)
{
    const uint32_t lower_residue = (uint32_t)(lower % step);
    const uint32_t advance =
        residue >= lower_residue ? residue - lower_residue
                                 : step - (lower_residue - residue);
    return lower + advance;
}

static void apply_filter(uint8_t *alive, uint64_t lower, uint64_t length,
                         const Filter *filter)
{
    const uint64_t upper = lower + length - 1;
    uint64_t k = first_at_or_after(lower, filter->k_residue, filter->step);
    if (k > upper) return;
    uint32_t n_mod_period =
        (uint32_t)((6 * (k % filter->period) + 2) % filter->period);
    const uint32_t delta =
        (uint32_t)((6 * (uint64_t)filter->step) % filter->period);

    for (;;) {
        const size_t index = (size_t)(k - lower);
        if (alive[index] && !filter->zero[n_mod_period]) alive[index] = 0;
        if (upper - k < filter->step) break;
        k += filter->step;
        n_mod_period += delta;
        if (n_mod_period >= filter->period)
            n_mod_period %= filter->period;
    }
}

static void record_match(Search *search, uint64_t k)
{
    if (pthread_mutex_lock(&search->match_lock) != 0)
        die("cannot lock match list");
    if (search->match_count == search->match_capacity) {
        const size_t new_capacity = search->match_capacity == 0
                                  ? 16 : 2 * search->match_capacity;
        if (new_capacity < search->match_capacity ||
            new_capacity > SIZE_MAX / sizeof(*search->matches))
            die("match list is too large");
        uint64_t *new_matches =
            realloc(search->matches, new_capacity * sizeof(*new_matches));
        if (new_matches == NULL) die("cannot grow match list");
        search->matches = new_matches;
        search->match_capacity = new_capacity;
    }
    search->matches[search->match_count++] = k;
    fprintf(stderr, "match: k=%" PRIu64 "\n", k);
    if (pthread_mutex_unlock(&search->match_lock) != 0)
        die("cannot unlock match list");
}

static void search_segment(Worker *worker, uint64_t segment_index)
{
    Search *search = worker->search;
    const uint64_t lower =
        search->start + segment_index * search->segment_size;
    const uint64_t remaining = search->end - lower + 1;
    const uint64_t length = remaining < search->segment_size
                          ? remaining : search->segment_size;
    memset(worker->alive, 0, (size_t)length);

    static const uint32_t allowed_mod_7[] = {0, 1, 5};
    uint64_t parity_count = 0;
    for (size_t r = 0; r < 3; ++r) {
        uint64_t k = first_at_or_after(lower, allowed_mod_7[r], 7);
        const uint64_t upper = lower + length - 1;
        while (k <= upper) {
            worker->alive[k - lower] = 1;
            ++parity_count;
            if (upper - k < 7) break;
            k += 7;
        }
    }
    atomic_fetch_add_explicit(&search->parity_survivors, parity_count,
                              memory_order_relaxed);

    for (size_t i = 0; i < search->filter_count; ++i)
        apply_filter(worker->alive, lower, length, &search->filters[i]);

    uint64_t tests = 0;
    for (uint64_t i = 0; i < length; ++i) {
        if (!worker->alive[i]) continue;
        ++tests;
        const uint64_t k = lower + i;
        if (is_a306634(k)) record_match(search, k);
    }
    atomic_fetch_add_explicit(&search->full_tests, tests,
                              memory_order_relaxed);
}

static void *worker_main(void *argument)
{
    Worker *worker = argument;
    Search *search = worker->search;
    for (;;) {
        const uint64_t segment = atomic_fetch_add_explicit(
            &search->next_segment, 1, memory_order_relaxed);
        if (segment >= search->segment_count) break;
        search_segment(worker, segment);
        const uint64_t completed = atomic_fetch_add_explicit(
            &search->completed_segments, 1, memory_order_relaxed) + 1;
        uint64_t report = atomic_load_explicit(&search->next_report_segment,
                                               memory_order_relaxed);
        if (completed >= report && report < search->segment_count &&
            atomic_compare_exchange_strong_explicit(
                &search->next_report_segment, &report, report + 1024,
                memory_order_relaxed, memory_order_relaxed)) {
            const double elapsed = monotonic_seconds() - search->started_at;
            fprintf(stderr,
                    "progress: %.1f%%, %.3f M k/s, full-tests=%" PRIuFAST64
                    "\n",
                    100.0 * (double)completed /
                        (double)search->segment_count,
                    elapsed > 0
                        ? (double)completed * (double)search->segment_size /
                          elapsed / 1.0e6
                        : 0.0,
                    atomic_load_explicit(&search->full_tests,
                                         memory_order_relaxed));
        }
    }
    return NULL;
}

static int compare_u64(const void *left, const void *right)
{
    const uint64_t a = *(const uint64_t *)left;
    const uint64_t b = *(const uint64_t *)right;
    return a > b ? 1 : a < b ? -1 : 0;
}

static void run_search(uint64_t start, uint64_t end, unsigned threads,
                       uint64_t segment_size)
{
    Search search;
    memset(&search, 0, sizeof(search));
    search.start = start;
    search.end = end;
    search.segment_size = segment_size;
    search.segment_count = (end - start) / segment_size + 1;
    search.thread_count = threads;
    atomic_init(&search.next_segment, 0);
    atomic_init(&search.completed_segments, 0);
    atomic_init(&search.next_report_segment, 1024);
    atomic_init(&search.parity_survivors, 0);
    atomic_init(&search.full_tests, 0);
    if (pthread_mutex_init(&search.match_lock, NULL) != 0)
        die("cannot initialize match mutex");
    make_filters(&search);

    pthread_t *ids = xmalloc(threads * sizeof(*ids));
    Worker *workers = xmalloc(threads * sizeof(*workers));
    fprintf(stderr,
            "A306634 exact search: k=%" PRIu64 "..%" PRIu64
            ", threads=%u, segment=%" PRIu64 ", filters=%zu\n",
            start, end, threads, segment_size, search.filter_count);
    search.started_at = monotonic_seconds();

    for (unsigned i = 0; i < threads; ++i) {
        workers[i].search = &search;
        workers[i].alive = xmalloc((size_t)segment_size);
        if (pthread_create(&ids[i], NULL, worker_main, &workers[i]) != 0)
            die("cannot create worker thread");
    }
    for (unsigned i = 0; i < threads; ++i) {
        if (pthread_join(ids[i], NULL) != 0) die("cannot join worker thread");
        free(workers[i].alive);
    }
    const double elapsed = monotonic_seconds() - search.started_at;

    qsort(search.matches, search.match_count, sizeof(search.matches[0]),
          compare_u64);
    for (size_t i = 0; i < search.match_count; ++i)
        printf("%" PRIu64 "\n", search.matches[i]);
    if (fflush(stdout) != 0) die("cannot flush output");

    const uint64_t total = end - start + 1;
    const uint64_t parity = atomic_load(&search.parity_survivors);
    const uint64_t tested = atomic_load(&search.full_tests);
    fprintf(stderr,
            "done: candidates=%" PRIu64 ", parity-pass=%" PRIu64
            ", full-tests=%" PRIu64 ", matches=%zu, %.3f s, %.3f M k/s\n",
            total, parity, tested, search.match_count, elapsed,
            elapsed > 0 ? (double)total / elapsed / 1.0e6 : 0.0);

    free_filters(&search);
    if (pthread_mutex_destroy(&search.match_lock) != 0)
        die("cannot destroy match mutex");
    free(workers);
    free(ids);
    free(search.matches);
}

static uint64_t splitmix64(uint64_t *state)
{
    uint64_t z = (*state += UINT64_C(0x9e3779b97f4a7c15));
    z = (z ^ (z >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    z = (z ^ (z >> 27)) * UINT64_C(0x94d049bb133111eb);
    return z ^ (z >> 31);
}

static void self_test(void)
{
    Search filters;
    memset(&filters, 0, sizeof(filters));
    make_filters(&filters);
    free_filters(&filters);

    uint64_t state = UINT64_C(30663402);
    for (unsigned i = 0; i < 2000; ++i) {
        const uint64_t k = splitmix64(&state) % (MAX_K + 1);
        const uint64_t n = 6 * k + 2;
        const bool fast = is_a306634(k);
        const bool reference = reference_perrin_mod(n, n) == 0;
        if (fast != reference) die("fast and reference Perrin tests disagree");
    }

    static const uint64_t terms[] = {
        0, UINT64_C(2755452), UINT64_C(4570452),
        UINT64_C(123725995972)
    };
    for (size_t i = 0; i < sizeof(terms) / sizeof(terms[0]); ++i)
        if (!is_a306634(terms[i])) die("known term failed self-test");
    static const uint64_t nonterms[] = {
        1, 2, 3, 100, 2755451, 2755453, 4570451, 4570453,
        UINT64_C(123725995971), UINT64_C(123725995973)
    };
    for (size_t i = 0; i < sizeof(nonterms) / sizeof(nonterms[0]); ++i)
        if (is_a306634(nonterms[i])) die("known nonterm failed self-test");
    puts("ok: filters, Montgomery arithmetic, 2000 independent comparisons, "
         "and known terms verified");
}

static void usage(FILE *stream, const char *program)
{
    fprintf(stream,
            "usage:\n"
            "  %s START_K END_K [THREADS [SEGMENT_SIZE]]\n"
            "  %s --check K\n"
            "  %s --self-test\n",
            program, program, program);
}

int main(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "--self-test") == 0) {
        self_test();
        return EXIT_SUCCESS;
    }
    if (argc == 3 && strcmp(argv[1], "--check") == 0) {
        const uint64_t k = parse_u64(argv[2], MAX_K, "K");
        const uint64_t n = 6 * k + 2;
        const bool match = is_a306634(k);
        printf("k=%" PRIu64 ", n=%" PRIu64 ": %s\n", k, n,
               match ? "term" : "not a term");
        return match ? EXIT_SUCCESS : 1;
    }
    if (argc < 3 || argc > 5) {
        usage(stderr, argv[0]);
        return EXIT_FAILURE;
    }

    const uint64_t start = parse_u64(argv[1], MAX_K, "START_K");
    const uint64_t end = parse_u64(argv[2], MAX_K, "END_K");
    if (start > end) die("START_K must not exceed END_K");
    const unsigned threads = argc >= 4
        ? (unsigned)parse_u64(argv[3], MAX_THREADS, "THREADS")
        : default_thread_count();
    if (threads == 0) die("THREADS must be positive");
    const uint64_t segment_size = argc == 5
        ? parse_u64(argv[4], MAX_SEGMENT, "SEGMENT_SIZE")
        : DEFAULT_SEGMENT;
    if (segment_size < MIN_SEGMENT)
        die("SEGMENT_SIZE is too small");

    run_search(start, end, threads, segment_size);
    return EXIT_SUCCESS;
}
