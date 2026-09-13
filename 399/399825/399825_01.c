/*
 * A399825: numbers k such that
 *
 *   Sum_{j=0..k} (3*k+j)! / ((3*k-3*j)! * (4*j+1)!)
 *
 * is an integer.
 *
 * This is an exact independent search; it does not read an OEIS b-file.
 * Define the integer sequence V by
 *
 *   V(0)=4, V(1)=V(2)=0, V(3)=3,
 *   V(r)=V(r-3)+V(r-4)  (r>=4).
 *
 * Equivalently, V(r) is the sum of the r-th powers of the four roots of
 * x^4-x-1.  The logarithmic generating function
 *
 *   Sum_{r>=1} V(r)*z^r/r = -log(1-z^3-z^4)
 *
 * gives, by taking the coefficient of z^(12*k+3),
 *
 *   u(3,k) = V(12*k+3)/(12*k+3).
 *
 * Hence k is a term exactly when V(N)==0 (mod N), N=12*k+3.  This avoids
 * all factorials and rational arithmetic.
 *
 * The requested interval is searched exhaustively.  At startup, complete
 * periods of V modulo several small divisors d are constructed and checked.
 * If d|N but V(N)!=0 (mod d), the candidate is safely rejected.  Every
 * survivor is tested exactly by binary powering in
 *
 *   (Z/NZ)[x]/(x^4-x-1), where x^4=x+1.
 *
 * N is odd, so Montgomery multiplication makes the full test fast.  All
 * arithmetic is integer-only and overflow-controlled; there are no
 * probabilistic tests and no assumptions about the factorization of N.
 * Segments are shared dynamically by POSIX threads.  Matches are sorted
 * before being written to stdout, one value k per line.
 *
 * Build:
 *   cc -O3 -march=native -std=c11 -Wall -Wextra -Wpedantic -pthread \
 *      399825_01.c -o 399825_01
 *
 * Usage:
 *   ./399825_01 START_K END_K [THREADS [SEGMENT_SIZE]]
 *   ./399825_01 --check K
 *   ./399825_01 --self-test
 *
 * Examples:
 *   ./399825_01 0 600000
 *   ./399825_01 --check 528531
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
#error "399825_01.c requires unsigned __int128"
#endif

__extension__ typedef unsigned __int128 u128;

#define DEFAULT_SEGMENT UINT64_C(1048576)
#define MIN_SEGMENT UINT64_C(4096)
#define MAX_SEGMENT UINT64_C(16777216)
#define MAX_THREADS 256U
#define MAX_FILTERS 40U
#define MAX_FILTER_PERIOD 5000U
#define MAX_K ((UINT64_C(0x7fffffffffffffff) - 3) / 12)

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
    uint64_t c[4];
} Poly4;

typedef struct {
    uint64_t start;
    uint64_t end;
    uint64_t segment_size;
    uint64_t segment_count;
    Filter filters[MAX_FILTERS];
    size_t filter_count;

    atomic_uint_fast64_t next_segment;
    atomic_uint_fast64_t completed_segments;
    atomic_uint_fast64_t next_report_segment;
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
    void *result = malloc(size == 0 ? 1 : size);
    if (result == NULL) die("out of memory");
    return result;
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
        const uint32_t remainder = a % b;
        a = b;
        b = remainder;
    }
    return a;
}

typedef struct {
    uint32_t divisor;
    uint32_t period;
} FilterSpec;

/* Periods are not trusted blindly: make_filters verifies each one. */
static const FilterSpec filter_specs[] = {
    {3, 80}, {5, 312}, {7, 342}, {9, 240}, {11, 1330}, {13, 2196},
    {17, 96}, {25, 1560}, {27, 720}, {37, 1368}, {49, 2394},
    {53, 1404}, {67, 4488}, {71, 1008}, {73, 1332}, {79, 208},
    {81, 2160}, {83, 82}, {107, 954}, {113, 3192}, {191, 4560},
    {239, 3570}, {283, 282}, {289, 1632}, {643, 642}, {773, 772},
    {859, 858}, {1193, 1192}, {1301, 1300}, {1307, 1306},
    {1567, 1566}, {1789, 1788}, {1901, 1900}
};

static uint32_t solve_k_residue(uint32_t divisor, uint32_t step)
{
    for (uint32_t k = 0; k < step; ++k)
        if ((12 * (uint64_t)k + 3) % divisor == 0) return k;
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
        const uint32_t common = gcd_u32(12, filter->divisor);
        if (3 % common != 0)
            die("filter divisor never divides 12*k+3");
        filter->step = filter->divisor / common;
        filter->k_residue = solve_k_residue(filter->divisor, filter->step);
        filter->zero = xmalloc(filter->period);

        uint32_t state[4] = {4 % filter->divisor, 0, 0,
                             3 % filter->divisor};
        const uint32_t initial[4] = {state[0], state[1], state[2], state[3]};
        for (uint32_t n = 0; n < filter->period; ++n) {
            filter->zero[n] = (uint8_t)(state[0] == 0);
            const uint32_t next = (state[0] + state[1]) % filter->divisor;
            state[0] = state[1];
            state[1] = state[2];
            state[2] = state[3];
            state[3] = next;
            if (n + 1 < filter->period &&
                memcmp(state, initial, sizeof(initial)) == 0)
                die("a listed V period is not minimal");
        }
        if (memcmp(state, initial, sizeof(initial)) != 0)
            die("a listed V period is incorrect");
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

static Montgomery make_montgomery(uint64_t modulus)
{
    if ((modulus & 1) == 0 || modulus >= (UINT64_C(1) << 63))
        die("invalid Montgomery modulus");
    uint64_t inverse = 1;
    for (unsigned i = 0; i < 6; ++i)
        inverse *= 2 - modulus * inverse;

    Montgomery result;
    result.modulus = modulus;
    result.negative_inverse = 0 - inverse;
    result.one = (uint64_t)(((u128)1 << 64) % modulus);
    return result;
}

/* Return a*b/2^64 modulo modulus, with a,b<modulus. */
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
    if (overflow) return reduced - mont->modulus;
    return reduced >= mont->modulus ? reduced - mont->modulus : reduced;
}

static Poly4 poly_multiply(Poly4 a, Poly4 b, const Montgomery *mont)
{
    uint64_t d[7] = {0, 0, 0, 0, 0, 0, 0};
    for (unsigned i = 0; i < 4; ++i) {
        for (unsigned j = 0; j < 4; ++j) {
            d[i + j] = add_mod(d[i + j],
                montgomery_multiply(mont, a.c[i], b.c[j]), mont->modulus);
        }
    }
    Poly4 result;
    /* x^4=x+1, x^5=x^2+x, x^6=x^3+x^2. */
    result.c[0] = add_mod(d[0], d[4], mont->modulus);
    result.c[1] = add_mod(add_mod(d[1], d[4], mont->modulus), d[5],
                          mont->modulus);
    result.c[2] = add_mod(add_mod(d[2], d[5], mont->modulus), d[6],
                          mont->modulus);
    result.c[3] = add_mod(d[3], d[6], mont->modulus);
    return result;
}

static Poly4 poly_square(Poly4 a, const Montgomery *mont)
{
    uint64_t d[7];
    const uint64_t m = mont->modulus;
    const uint64_t a01 = montgomery_multiply(mont, a.c[0], a.c[1]);
    const uint64_t a02 = montgomery_multiply(mont, a.c[0], a.c[2]);
    const uint64_t a03 = montgomery_multiply(mont, a.c[0], a.c[3]);
    const uint64_t a12 = montgomery_multiply(mont, a.c[1], a.c[2]);
    const uint64_t a13 = montgomery_multiply(mont, a.c[1], a.c[3]);
    const uint64_t a23 = montgomery_multiply(mont, a.c[2], a.c[3]);
    d[0] = montgomery_multiply(mont, a.c[0], a.c[0]);
    d[1] = add_mod(a01, a01, m);
    d[2] = add_mod(add_mod(a02, a02, m),
                   montgomery_multiply(mont, a.c[1], a.c[1]), m);
    d[3] = add_mod(add_mod(a03, a03, m), add_mod(a12, a12, m), m);
    d[4] = add_mod(add_mod(a13, a13, m),
                   montgomery_multiply(mont, a.c[2], a.c[2]), m);
    d[5] = add_mod(a23, a23, m);
    d[6] = montgomery_multiply(mont, a.c[3], a.c[3]);

    Poly4 result;
    result.c[0] = add_mod(d[0], d[4], m);
    result.c[1] = add_mod(add_mod(d[1], d[4], m), d[5], m);
    result.c[2] = add_mod(add_mod(d[2], d[5], m), d[6], m);
    result.c[3] = add_mod(d[3], d[6], m);
    return result;
}

/* Return V(exponent) in Montgomery representation modulo modulus. */
static uint64_t v_montgomery(uint64_t exponent, const Montgomery *mont)
{
    Poly4 result = {{mont->one, 0, 0, 0}};
    Poly4 power = {{0, mont->one, 0, 0}};
    uint64_t e = exponent;
    while (e != 0) {
        if ((e & 1) != 0) result = poly_multiply(result, power, mont);
        e >>= 1;
        if (e != 0) power = poly_square(power, mont);
    }

    /* Tr(1)=4, Tr(x)=Tr(x^2)=0, Tr(x^3)=3. */
    uint64_t trace = result.c[0];
    trace = add_mod(trace, trace, mont->modulus);
    trace = add_mod(trace, trace, mont->modulus);
    const uint64_t thrice_c3 = add_mod(
        add_mod(result.c[3], result.c[3], mont->modulus), result.c[3],
        mont->modulus);
    return add_mod(trace, thrice_c3, mont->modulus);
}

static bool is_a399825(uint64_t k)
{
    const uint64_t n = 12 * k + 3;
    const Montgomery mont = make_montgomery(n);
    return v_montgomery(n, &mont) == 0;
}

/* Independent division-based implementation used only by --self-test. */
static uint64_t reference_multiply(uint64_t a, uint64_t b, uint64_t modulus)
{
    return (uint64_t)(((u128)a * b) % modulus);
}

static Poly4 reference_poly_multiply(Poly4 a, Poly4 b, uint64_t modulus)
{
    uint64_t d[7] = {0, 0, 0, 0, 0, 0, 0};
    for (unsigned i = 0; i < 4; ++i)
        for (unsigned j = 0; j < 4; ++j)
            d[i + j] = add_mod(d[i + j],
                reference_multiply(a.c[i], b.c[j], modulus), modulus);
    Poly4 result;
    result.c[0] = add_mod(d[0], d[4], modulus);
    result.c[1] = add_mod(add_mod(d[1], d[4], modulus), d[5], modulus);
    result.c[2] = add_mod(add_mod(d[2], d[5], modulus), d[6], modulus);
    result.c[3] = add_mod(d[3], d[6], modulus);
    return result;
}

static uint64_t reference_v_mod(uint64_t exponent, uint64_t modulus)
{
    Poly4 result = {{1, 0, 0, 0}}, power = {{0, 1, 0, 0}};
    uint64_t e = exponent;
    while (e != 0) {
        if ((e & 1) != 0)
            result = reference_poly_multiply(result, power, modulus);
        e >>= 1;
        if (e != 0) power = reference_poly_multiply(power, power, modulus);
    }
    uint64_t trace = reference_multiply(4, result.c[0], modulus);
    return add_mod(trace, reference_multiply(3, result.c[3], modulus),
                   modulus);
}

static uint64_t first_at_or_after(uint64_t lower, uint32_t residue,
                                  uint32_t step)
{
    const uint32_t lower_residue = (uint32_t)(lower % step);
    const uint32_t advance = residue >= lower_residue
                           ? residue - lower_residue
                           : step - (lower_residue - residue);
    return lower + advance;
}

static void apply_filter(uint8_t *alive, uint64_t lower, uint64_t length,
                         const Filter *filter)
{
    const uint64_t upper = lower + length - 1;
    uint64_t k = first_at_or_after(lower, filter->k_residue, filter->step);
    if (k > upper) return;
    uint32_t index_mod_period =
        (uint32_t)((12 * (k % filter->period) + 3) % filter->period);
    const uint32_t delta =
        (uint32_t)((12 * (uint64_t)filter->step) % filter->period);

    for (;;) {
        const size_t index = (size_t)(k - lower);
        if (alive[index] && !filter->zero[index_mod_period]) alive[index] = 0;
        if (upper - k < filter->step) break;
        k += filter->step;
        index_mod_period += delta;
        if (index_mod_period >= filter->period)
            index_mod_period %= filter->period;
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
        uint64_t *replacement =
            realloc(search->matches, new_capacity * sizeof(*replacement));
        if (replacement == NULL) die("cannot grow match list");
        search->matches = replacement;
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
    memset(worker->alive, 1, (size_t)length);
    for (size_t i = 0; i < search->filter_count; ++i)
        apply_filter(worker->alive, lower, length, &search->filters[i]);

    uint64_t tests = 0;
    for (uint64_t i = 0; i < length; ++i) {
        if (!worker->alive[i]) continue;
        ++tests;
        const uint64_t k = lower + i;
        if (is_a399825(k)) record_match(search, k);
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
            fprintf(stderr, "progress: %.1f%%, %.3f M k/s\n",
                    100.0 * (double)completed /
                        (double)search->segment_count,
                    elapsed > 0
                        ? (double)completed * (double)search->segment_size /
                          elapsed / 1.0e6
                        : 0.0);
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
    atomic_init(&search.next_segment, 0);
    atomic_init(&search.completed_segments, 0);
    atomic_init(&search.next_report_segment, 1024);
    atomic_init(&search.full_tests, 0);
    if (pthread_mutex_init(&search.match_lock, NULL) != 0)
        die("cannot initialize match mutex");
    make_filters(&search);

    pthread_t *ids = xmalloc(threads * sizeof(*ids));
    Worker *workers = xmalloc(threads * sizeof(*workers));
    fprintf(stderr,
            "A399825 exact search: k=%" PRIu64 "..%" PRIu64
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
    const uint64_t tested = atomic_load(&search.full_tests);
    fprintf(stderr,
            "done: candidates=%" PRIu64 ", full-tests=%" PRIu64
            ", matches=%zu, %.3f s, %.3f M k/s\n",
            total, tested, search.match_count, elapsed,
            elapsed > 0 ? (double)total / elapsed / 1.0e6 : 0.0);

    free_filters(&search);
    if (pthread_mutex_destroy(&search.match_lock) != 0)
        die("cannot destroy match mutex");
    free(search.matches);
    free(workers);
    free(ids);
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

    uint64_t state = UINT64_C(39982501);
    for (unsigned i = 0; i < 2000; ++i) {
        const uint64_t k = splitmix64(&state) % (MAX_K + 1);
        const uint64_t n = 12 * k + 3;
        const Montgomery mont = make_montgomery(n);
        const uint64_t in_montgomery = v_montgomery(n, &mont);
        const uint64_t fast = montgomery_multiply(&mont, in_montgomery, 1);
        const uint64_t reference = reference_v_mod(n, n);
        if (fast != reference)
            die("Montgomery and reference polynomial powers disagree");
    }

    static const uint64_t terms[] = {
        0, UINT64_C(3006), UINT64_C(401781), UINT64_C(528531)
    };
    for (size_t i = 0; i < sizeof(terms) / sizeof(terms[0]); ++i)
        if (!is_a399825(terms[i])) die("known term failed self-test");
    static const uint64_t nonterms[] = {
        1, 2, 3, 100, 3005, 3007, 401780, 401782, 528530, 528532
    };
    for (size_t i = 0; i < sizeof(nonterms) / sizeof(nonterms[0]); ++i)
        if (is_a399825(nonterms[i])) die("known nonterm failed self-test");
    puts("ok: periods, 2000 exact Montgomery/reference comparisons, and "
         "known A399825 terms verified");
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
        const uint64_t n = 12 * k + 3;
        const bool match = is_a399825(k);
        printf("k=%" PRIu64 ", 12*k+3=%" PRIu64 ": %s\n", k, n,
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
