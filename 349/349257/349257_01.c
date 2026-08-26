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
 * weights, and a permutation-lattice congruence.
 *
 * There is also one exact congruence for each prime r.  If P is the largest
 * power of r not exceeding n, integrality requires
 *
 *       sum_{r|j} q(j)*(L/j) == 0 (mod P).
 *
 * Terms with r not dividing j vanish modulo P.  The search maintains these
 * residues and finishes the smallest pending prime congruence first.  This is
 * a mathematical ordering/pruning fact; the algorithm remains a downward
 * exact-target search, independently of 349257_02.c's direct maximization.
 * If n itself is prime, its congruence forces q(n)=n, so a(n)=a(n-1)+1;
 * that recurrence also gives a(83) without constructing an overflowing LCM.
 * With multiple threads, a bounded deterministic prefix expansion produces
 * disjoint exact-target subtrees.  Workers have private mutable search state.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic 349257_01.c \
 *       -o 349257_01 -pthread
 *
 * Usage:
 *   ./349257_01                 # print a(0)..a(19)
 *   ./349257_01 --upto 19
 *   ./349257_01 --term 19
 *   ./349257_01 --term 74 --threads 8 --verbose
 *   ./349257_01 --term 19 --witness
 *   ./349257_01 --check
 *
 * The default and --upto take an exclusive writer lock and atomically replace
 * b349257_01.txt through a unique temporary file.  --term and --check do not
 * modify the b-file.
 */

#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define MAX_N 83
#define MAX_EXACT_N 82
#define DEFAULT_MAX_N 19
#define KNOWN_MAX_N 19
#define DIRECT_CHECK_MAX_N 10
#define MAX_CONSTRAINTS 23
#define DEFAULT_THREADS 8
#define MAX_THREADS 64
#define JOBS_PER_THREAD 8
#define MAX_JOBS 8192
#define BFILE_NAME "b349257_01.txt"
#define BFILE_TEMP_TEMPLATE BFILE_NAME ".tmp.XXXXXX"
#define BFILE_LOCK_NAME BFILE_NAME ".lock"

_Static_assert(MAX_N <= 127, "128-bit masks support at most n=127");
_Static_assert(MAX_EXACT_N <= 82,
               "128-bit scaled assignment sums support at most n=82");

typedef __uint128_t u128;
typedef u128 mask_t;

typedef struct {
    mask_t denominator_mask;
    uint32_t coefficient[MAX_N + 1];
    uint32_t modulus;
} PrimeConstraint;

static const uint64_t known[KNOWN_MAX_N + 1] = {
    UINT64_C(0),  UINT64_C(1),  UINT64_C(2),  UINT64_C(3),
    UINT64_C(6),  UINT64_C(7),  UINT64_C(10), UINT64_C(11),
    UINT64_C(15), UINT64_C(18), UINT64_C(21), UINT64_C(22),
    UINT64_C(27), UINT64_C(28), UINT64_C(32), UINT64_C(36),
    UINT64_C(40), UINT64_C(41), UINT64_C(46), UINT64_C(47)
};

typedef struct {
    int n;
    u128 lcm;
    u128 weight[MAX_N + 1];
    PrimeConstraint constraint[MAX_CONSTRAINTS];
    uint32_t residue[MAX_CONSTRAINTS];
    unsigned constraint_count;
    int inverse[MAX_N + 1];
    uint64_t nodes;
    _Atomic bool *cancel;
} Search;

typedef struct {
    mask_t denominators;
    mask_t numerators;
    u128 residual;
    uint32_t residue[MAX_CONSTRAINTS];
    int inverse[MAX_N + 1];
} Job;

typedef struct {
    Search prototype;
    Job *jobs;
    size_t job_count;
    size_t desired_jobs;
    _Atomic size_t next_job;
    _Atomic bool found;
    _Atomic uint64_t nodes;
    pthread_mutex_t witness_mutex;
    int witness[MAX_N + 1];
} ParallelTarget;

typedef struct {
    ParallelTarget *shared;
} Worker;

typedef struct {
    int denominator;
    u128 remaining_gcd;
    int maximizing_value;
} Branch;

typedef enum {
    MODE_UPTO,
    MODE_TERM
} OutputMode;

static char bfile_temp_name[] = BFILE_TEMP_TEMPLATE;
static int bfile_lock_descriptor = -1;

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

static u128 gcd_u128(u128 a, u128 b)
{
    while (b != 0) {
        const u128 remainder = a % b;
        a = b;
        b = remainder;
    }
    return a;
}

static bool is_prime(unsigned value)
{
    if (value < 2U) return false;
    if ((value & 1U) == 0) return value == 2U;
    for (unsigned divisor = 3; divisor <= value / divisor; divisor += 2U)
        if (value % divisor == 0U) return false;
    return true;
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

static unsigned parse_threads(const char *text)
{
    errno = 0;
    char *end = NULL;
    const uintmax_t value = strtoumax(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' || value < 1U ||
        value > MAX_THREADS) {
        fprintf(stderr, "error: threads must be in 1..%d: %s\n",
                MAX_THREADS, text);
        exit(EXIT_FAILURE);
    }
    return (unsigned)value;
}

static u128 make_lcm(int n)
{
    u128 value = 1;
    for (u128 k = 2; k <= (u128)n; ++k) {
        const u128 divisor = gcd_u128(value, k);
        if (value > ~(u128)0 / (k / divisor))
            die("LCM overflow");
        value *= k / divisor;
    }
    return value;
}

static unsigned bit_count(mask_t mask)
{
#if defined(__clang__) || defined(__GNUC__)
    return (unsigned)__builtin_popcountll((uint64_t)mask) +
           (unsigned)__builtin_popcountll((uint64_t)(mask >> 64));
#else
    unsigned count = 0;
    while (mask != 0) {
        mask &= mask - 1U;
        ++count;
    }
    return count;
#endif
}

static int first_value(mask_t mask)
{
#if defined(__clang__) || defined(__GNUC__)
    const uint64_t low = (uint64_t)mask;
    if (low != 0) return (int)__builtin_ctzll(low) + 1;
    return (int)__builtin_ctzll((uint64_t)(mask >> 64)) + 65;
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
static void assignment_bounds(const Search *search, mask_t denominators,
                              mask_t numerators, u128 *minimum,
                              u128 *maximum)
{
    int ascending[MAX_N] = {0};
    int count = 0;
    for (int value = 1; value <= search->n; ++value)
        if ((numerators & ((mask_t)1 << (value - 1))) != 0)
            ascending[count++] = value;
    if ((unsigned)count != bit_count(denominators))
        die("assignment masks have different sizes");

    u128 low = 0;
    u128 high = 0;
    int rank = 0;
    /* j increases, so L/j decreases. */
    for (int j = 1; j <= search->n; ++j) {
        if ((denominators & ((mask_t)1 << (j - 1))) == 0) continue;
        if (rank >= count) die("assignment bound rank overflow");
        low += (u128)ascending[rank] * search->weight[j];
        high += (u128)ascending[count - 1 - rank] * search->weight[j];
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
static bool lattice_congruence(const Search *search, mask_t denominators,
                               mask_t numerators, u128 residual)
{
    if (bit_count(numerators) < 2U) return true;

    u128 base = 0;
    u128 numerator_gcd = 0;
    u128 weight_gcd = 0;
    int first_numerator = 0;
    u128 first_weight = 0;
    mask_t number_scan = numerators;

    while (number_scan != 0) {
        const int value = first_value(number_scan);
        number_scan &= number_scan - 1U;
        if (first_numerator == 0)
            first_numerator = value;
        else
            numerator_gcd = gcd_u128(numerator_gcd,
                                     (u128)(value - first_numerator));
    }

    int numerator_index = 0;
    int ordered_numerators[MAX_N];
    for (int value = 1; value <= search->n; ++value)
        if ((numerators & ((mask_t)1 << (value - 1))) != 0)
            ordered_numerators[numerator_index++] = value;

    int index = 0;
    for (int j = 1; j <= search->n; ++j) {
        if ((denominators & ((mask_t)1 << (j - 1))) == 0) continue;
        const u128 weight = search->weight[j];
        base += (u128)ordered_numerators[index++] * weight;
        if (first_weight == 0)
            first_weight = weight;
        else {
            const u128 difference = first_weight > weight
                                            ? first_weight - weight
                                            : weight - first_weight;
            weight_gcd = gcd_u128(weight_gcd, difference);
        }
    }

    const u128 modulus = numerator_gcd * weight_gcd;
    return modulus == 0 || residual % modulus == base % modulus;
}

static u128 gcd_without(const Search *search, mask_t denominators,
                        int omitted)
{
    u128 result = 0;
    for (int j = 1; j <= search->n; ++j) {
        if (j == omitted ||
            (denominators & ((mask_t)1 << (j - 1))) == 0)
            continue;
        result = gcd_u128(result, search->weight[j]);
    }
    return result;
}

static bool prime_constraints_consistent(const Search *search,
                                         mask_t denominators)
{
    for (unsigned c = 0; c < search->constraint_count; ++c)
        if ((denominators & search->constraint[c].denominator_mask) == 0 &&
            search->residue[c] != 0U)
            return false;
    return true;
}

/* Enforce a prime congruence as soon as its final denominator is assigned. */
static bool prime_candidate_allowed(const Search *search,
                                    mask_t denominators, int denominator,
                                    int value)
{
    const mask_t denominator_bit = (mask_t)1 << (denominator - 1);
    for (unsigned c = 0; c < search->constraint_count; ++c) {
        const PrimeConstraint *constraint = &search->constraint[c];
        if ((constraint->denominator_mask & denominator_bit) == 0) continue;
        if (bit_count(denominators & constraint->denominator_mask) != 1U)
            continue;
        if ((search->residue[c] +
             (uint32_t)value * constraint->coefficient[denominator]) %
                constraint->modulus != 0U)
            return false;
    }
    return true;
}

static void add_prime_residues(Search *search, int denominator, int value,
                               uint32_t old[MAX_CONSTRAINTS])
{
    const mask_t denominator_bit = (mask_t)1 << (denominator - 1);
    for (unsigned c = 0; c < search->constraint_count; ++c) {
        old[c] = search->residue[c];
        const PrimeConstraint *constraint = &search->constraint[c];
        if ((constraint->denominator_mask & denominator_bit) == 0) continue;
        search->residue[c] =
            (search->residue[c] +
             (uint32_t)value * constraint->coefficient[denominator]) %
            constraint->modulus;
    }
}

static void restore_prime_residues(Search *search,
                                   const uint32_t old[MAX_CONSTRAINTS])
{
    for (unsigned c = 0; c < search->constraint_count; ++c)
        search->residue[c] = old[c];
}

/*
 * Finish the smallest pending prime congruence first.  Within it, assign the
 * coefficient having the largest gcd with the modulus; this tends to leave a
 * unit coefficient for the final, immediately checkable assignment.
 */
static int prime_preferred_denominator(const Search *search,
                                       mask_t denominators)
{
    int active_constraint = -1;
    unsigned smallest_group = MAX_N + 1U;
    for (unsigned c = 0; c < search->constraint_count; ++c) {
        const unsigned count = bit_count(
            denominators & search->constraint[c].denominator_mask);
        if (count == 0U) continue;
        if (count < smallest_group ||
            (count == smallest_group &&
             (active_constraint < 0 ||
              search->constraint[c].modulus >
                  search->constraint[active_constraint].modulus))) {
            smallest_group = count;
            active_constraint = (int)c;
        }
    }
    if (active_constraint < 0) return 0;

    const PrimeConstraint *constraint =
        &search->constraint[active_constraint];
    int best_denominator = 0;
    u128 best_divisor = 0;
    mask_t scan = denominators & constraint->denominator_mask;
    while (scan != 0) {
        const int denominator = first_value(scan);
        scan &= scan - 1U;
        const u128 divisor = gcd_u128(
            constraint->coefficient[denominator], constraint->modulus);
        if (best_denominator == 0 || divisor > best_divisor ||
            (divisor == best_divisor && denominator < best_denominator)) {
            best_denominator = denominator;
            best_divisor = divisor;
        }
    }
    return best_denominator;
}

static bool prepare_branch(Search *search, mask_t denominators,
                           mask_t numerators, u128 residual, Branch *branch)
{
    u128 minimum, maximum;
    assignment_bounds(search, denominators, numerators, &minimum, &maximum);
    if (residual < minimum || residual > maximum) return false;
    if (!lattice_congruence(search, denominators, numerators, residual))
        return false;
    if (!prime_constraints_consistent(search, denominators)) return false;

    int chosen_denominator = 0;
    u128 chosen_gcd = 0;
    int smallest_candidate_count = search->n + 1;
    const int prime_preference =
        prime_preferred_denominator(search, denominators);

    for (int j = 1; j <= search->n; ++j) {
        if ((denominators & ((mask_t)1 << (j - 1))) == 0) continue;
        if (prime_preference != 0 && j != prime_preference) continue;
        const u128 remaining_gcd = gcd_without(search, denominators, j);
        int candidate_count = 0;
        for (int value = 1; value <= search->n; ++value) {
            if ((numerators & ((mask_t)1 << (value - 1))) == 0) continue;
            if (!prime_candidate_allowed(search, denominators, j, value))
                continue;
            const u128 term = (u128)value * search->weight[j];
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
    if (smallest_candidate_count == 0 || chosen_denominator == 0)
        return false;

    const mask_t denominator_bit =
        (mask_t)1 << (chosen_denominator - 1);
    unsigned maximizing_rank =
        bit_count(denominators & (denominator_bit - 1U));
    int maximizing_value = 0;
    for (int value = search->n; value >= 1; --value) {
        if ((numerators & ((mask_t)1 << (value - 1))) == 0) continue;
        if (maximizing_rank == 0U) {
            maximizing_value = value;
            break;
        }
        --maximizing_rank;
    }
    branch->denominator = chosen_denominator;
    branch->remaining_gcd = chosen_gcd;
    branch->maximizing_value = maximizing_value;
    return true;
}

static int ordered_candidates(const Search *search, mask_t denominators,
                              mask_t numerators, u128 residual,
                              const Branch *branch, int result[MAX_N])
{
    int count = 0;
    for (int order = 0; order < 2 * search->n; ++order) {
        const int distance = (order + 1) / 2;
        const int value = (order & 1) == 0
                              ? branch->maximizing_value - distance
                              : branch->maximizing_value + distance;
        if (value < 1 || value > search->n) continue;
        const mask_t numerator_bit = (mask_t)1 << (value - 1);
        if ((numerators & numerator_bit) == 0) continue;
        if (!prime_candidate_allowed(search, denominators,
                                     branch->denominator, value))
            continue;
        const u128 term = (u128)value * search->weight[branch->denominator];
        if (term > residual) continue;
        if (branch->remaining_gcd == 0) {
            if (term != residual) continue;
        } else if ((residual - term) % branch->remaining_gcd != 0) {
            continue;
        }
        result[count++] = value;
    }
    return count;
}

static bool exact_assignment(Search *search, mask_t denominators,
                             mask_t numerators, u128 residual)
{
    if (search->cancel != NULL &&
        atomic_load_explicit(search->cancel, memory_order_relaxed))
        return false;
    ++search->nodes;
    if (denominators == 0) return residual == 0;

    Branch branch;
    if (!prepare_branch(search, denominators, numerators, residual, &branch))
        return false;
    int candidates[MAX_N];
    const int candidate_count = ordered_candidates(
        search, denominators, numerators, residual, &branch, candidates);
    const mask_t denominator_bit =
        (mask_t)1 << (branch.denominator - 1);
    for (int index = 0; index < candidate_count; ++index) {
        const int value = candidates[index];
        const mask_t numerator_bit = (mask_t)1 << (value - 1);
        const u128 term = (u128)value * search->weight[branch.denominator];
        uint32_t old_residue[MAX_CONSTRAINTS] = {0};
        add_prime_residues(search, branch.denominator, value, old_residue);
        search->inverse[branch.denominator] = value;
        if (exact_assignment(search, denominators ^ denominator_bit,
                             numerators ^ numerator_bit, residual - term)) {
            restore_prime_residues(search, old_residue);
            return true;
        }
        restore_prime_residues(search, old_residue);
    }
    search->inverse[branch.denominator] = 0;
    return false;
}

static void append_job(ParallelTarget *shared, const Search *builder,
                       mask_t denominators, mask_t numerators, u128 residual)
{
    if (shared->job_count == MAX_JOBS)
        die("parallel frontier is too large");
    Job *job = &shared->jobs[shared->job_count++];
    job->denominators = denominators;
    job->numerators = numerators;
    job->residual = residual;
    memcpy(job->residue, builder->residue, sizeof(job->residue));
    memcpy(job->inverse, builder->inverse, sizeof(job->inverse));
}

/* Build a disjoint deterministic frontier without solving an early branch. */
static void build_frontier(ParallelTarget *shared, Search *builder,
                           mask_t denominators, mask_t numerators,
                           u128 residual, unsigned depth)
{
    ++builder->nodes;
    if (denominators == 0) {
        if (residual == 0 &&
            prime_constraints_consistent(builder, denominators)) {
            memcpy(shared->witness, builder->inverse,
                   sizeof(shared->witness));
            atomic_store_explicit(&shared->found, true,
                                  memory_order_release);
        }
        return;
    }

    Branch branch;
    if (!prepare_branch(builder, denominators, numerators, residual, &branch))
        return;
    if (depth == 0U || shared->job_count >= shared->desired_jobs) {
        append_job(shared, builder, denominators, numerators, residual);
        return;
    }

    int candidates[MAX_N];
    const int candidate_count = ordered_candidates(
        builder, denominators, numerators, residual, &branch, candidates);
    const mask_t denominator_bit =
        (mask_t)1 << (branch.denominator - 1);
    for (int index = 0; index < candidate_count; ++index) {
        const int value = candidates[index];
        const mask_t numerator_bit = (mask_t)1 << (value - 1);
        const u128 term = (u128)value * builder->weight[branch.denominator];
        uint32_t old_residue[MAX_CONSTRAINTS] = {0};
        add_prime_residues(builder, branch.denominator, value, old_residue);
        builder->inverse[branch.denominator] = value;
        build_frontier(shared, builder, denominators ^ denominator_bit,
                       numerators ^ numerator_bit, residual - term,
                       depth - 1U);
        restore_prime_residues(builder, old_residue);
        builder->inverse[branch.denominator] = 0;
        if (atomic_load_explicit(&shared->found, memory_order_acquire)) return;
    }
}

static void *worker_main(void *argument)
{
    Worker *worker = argument;
    ParallelTarget *shared = worker->shared;
    uint64_t nodes = 0;

    for (;;) {
        if (atomic_load_explicit(&shared->found, memory_order_acquire)) break;
        const size_t index = atomic_fetch_add_explicit(
            &shared->next_job, 1U, memory_order_relaxed);
        if (index >= shared->job_count) break;

        const Job *job = &shared->jobs[index];
        Search search = shared->prototype;
        search.nodes = 0;
        search.cancel = &shared->found;
        memcpy(search.residue, job->residue, sizeof(search.residue));
        memcpy(search.inverse, job->inverse, sizeof(search.inverse));
        const bool solved = exact_assignment(
            &search, job->denominators, job->numerators, job->residual);
        nodes += search.nodes;
        if (!solved) continue;

        if (pthread_mutex_lock(&shared->witness_mutex) != 0)
            die("pthread_mutex_lock failed");
        if (!atomic_load_explicit(&shared->found, memory_order_relaxed)) {
            memcpy(shared->witness, search.inverse,
                   sizeof(shared->witness));
            atomic_store_explicit(&shared->found, true,
                                  memory_order_release);
        }
        if (pthread_mutex_unlock(&shared->witness_mutex) != 0)
            die("pthread_mutex_unlock failed");
        break;
    }
    atomic_fetch_add_explicit(&shared->nodes, nodes, memory_order_relaxed);
    return NULL;
}

static bool solve_target(Search *search, mask_t denominators,
                         mask_t numerators, u128 residual,
                         unsigned thread_count, size_t *job_count)
{
    if (thread_count == 1U) {
        if (job_count != NULL) *job_count = 1U;
        search->cancel = NULL;
        return exact_assignment(search, denominators, numerators, residual);
    }

    ParallelTarget shared;
    memset(&shared, 0, sizeof(shared));
    shared.prototype = *search;
    shared.prototype.nodes = 0;
    shared.prototype.cancel = NULL;
    shared.jobs = calloc(MAX_JOBS, sizeof(*shared.jobs));
    if (shared.jobs == NULL) die("could not allocate parallel frontier");
    shared.desired_jobs = (size_t)thread_count * JOBS_PER_THREAD;
    atomic_init(&shared.next_job, 0U);
    atomic_init(&shared.found, false);
    atomic_init(&shared.nodes, 0U);
    if (pthread_mutex_init(&shared.witness_mutex, NULL) != 0)
        die("pthread_mutex_init failed");

    Search builder = shared.prototype;
    builder.nodes = 0;
    for (unsigned depth = 1; depth <= (unsigned)search->n; ++depth) {
        shared.job_count = 0;
        memset(builder.residue, 0, sizeof(builder.residue));
        memset(builder.inverse, 0, sizeof(builder.inverse));
        build_frontier(&shared, &builder, denominators, numerators, residual,
                       depth);
        if (atomic_load_explicit(&shared.found, memory_order_acquire) ||
            shared.job_count == 0 ||
            shared.job_count >= shared.desired_jobs ||
            depth == (unsigned)search->n)
            break;
    }

    if (!atomic_load_explicit(&shared.found, memory_order_acquire) &&
        shared.job_count != 0) {
        const unsigned workers = shared.job_count < thread_count
                                     ? (unsigned)shared.job_count
                                     : thread_count;
        pthread_t threads[MAX_THREADS];
        Worker worker[MAX_THREADS];
        atomic_store_explicit(&shared.next_job, 0U, memory_order_relaxed);
        for (unsigned i = 0; i < workers; ++i) {
            worker[i].shared = &shared;
            if (pthread_create(&threads[i], NULL, worker_main, &worker[i]) !=
                0)
                die("pthread_create failed");
        }
        for (unsigned i = 0; i < workers; ++i)
            if (pthread_join(threads[i], NULL) != 0)
                die("pthread_join failed");
    }

    const bool solved =
        atomic_load_explicit(&shared.found, memory_order_acquire);
    if (solved)
        memcpy(search->inverse, shared.witness, sizeof(search->inverse));
    search->nodes += builder.nodes +
        atomic_load_explicit(&shared.nodes, memory_order_relaxed);
    if (job_count != NULL) *job_count = shared.job_count;
    free(shared.jobs);
    if (pthread_mutex_destroy(&shared.witness_mutex) != 0)
        die("pthread_mutex_destroy failed");
    return solved;
}

static uint64_t compute_term(int n, unsigned thread_count,
                             int witness[MAX_N + 1], uint64_t *node_count,
                             size_t *job_count)
{
    if (n == 0) {
        if (witness != NULL) memset(witness, 0, sizeof(int) * (MAX_N + 1));
        if (node_count != NULL) *node_count = 0;
        if (job_count != NULL) *job_count = 0;
        return 0;
    }
    if (n > MAX_EXACT_N) die("internal exact-search limit exceeded");

    Search search;
    memset(&search, 0, sizeof(search));
    search.n = n;
    search.lcm = make_lcm(n);
    for (int j = 1; j <= n; ++j)
        search.weight[j] = search.lcm / (u128)j;
    for (unsigned prime = 2; prime <= (unsigned)n; ++prime) {
        if (!is_prime(prime)) continue;
        if (search.constraint_count == MAX_CONSTRAINTS)
            die("too many prime constraints");
        PrimeConstraint *constraint =
            &search.constraint[search.constraint_count++];
        uint32_t modulus = prime;
        while (modulus <= (uint32_t)n / prime) modulus *= prime;
        constraint->modulus = modulus;
        for (int j = (int)prime; j <= n; j += (int)prime) {
            constraint->denominator_mask |= (mask_t)1 << (j - 1);
            constraint->coefficient[j] =
                (uint32_t)(search.weight[j] % modulus);
        }
    }

    const mask_t full_mask = ((mask_t)1 << n) - 1U;
    u128 minimum, maximum;
    assignment_bounds(&search, full_mask, full_mask, &minimum, &maximum);
    (void)minimum;

    for (uint64_t target = (uint64_t)(maximum / search.lcm);; --target) {
        memset(search.inverse, 0, sizeof(search.inverse));
        memset(search.residue, 0, sizeof(search.residue));
        if (solve_target(&search, full_mask, full_mask, target * search.lcm,
                         thread_count, job_count)) {
            if (witness != NULL)
                memcpy(witness, search.inverse, sizeof(search.inverse));
            if (node_count != NULL) *node_count = search.nodes;
            return target;
        }
        if (target == 0) die("internal search failure");
    }
}

/* Use the prime recurrence, reusing n-1 during a sequential --upto run. */
static uint64_t compute_output_term(
    int n, unsigned thread_count, bool have_previous,
    uint64_t previous_value, const int previous_witness[MAX_N + 1],
    int witness[MAX_N + 1], uint64_t *node_count, size_t *job_count,
    bool *prime_step)
{
    *prime_step = false;
    if (!is_prime((unsigned)n))
        return compute_term(n, thread_count, witness, node_count, job_count);

    uint64_t value;
    if (have_previous) {
        value = previous_value;
        if (witness != NULL)
            memcpy(witness, previous_witness,
                   sizeof(int) * (MAX_N + 1));
        if (node_count != NULL) *node_count = 0;
        if (job_count != NULL) *job_count = 0;
    } else {
        value = compute_term(n - 1, thread_count, witness, node_count,
                             job_count);
    }
    if (witness != NULL) witness[n] = n;
    *prime_step = true;
    return value + 1U;
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
    const u128 lcm = make_lcm(n);
    int inverse[MAX_N];
    for (int i = 0; i < n; ++i) inverse[i] = i + 1;

    uint64_t best = 0;
    do {
        u128 scaled_sum = 0;
        for (int j = 1; j <= n; ++j)
            scaled_sum += (u128)inverse[j - 1] * (lcm / (u128)j);
        if (scaled_sum % lcm == 0 && scaled_sum / lcm > best)
            best = scaled_sum / lcm;
    } while (next_permutation(inverse, n));
    return best;
}

static void verify_witness(const int inverse[MAX_N + 1], int n,
                           uint64_t target)
{
    mask_t seen = 0;
    for (int j = 1; j <= n; ++j) {
        const int value = inverse[j];
        if (value < 1 || value > n) die("invalid witness value");
        const mask_t bit = (mask_t)1 << (value - 1);
        if ((seen & bit) != 0) die("duplicate witness value");
        seen |= bit;
    }

    int exact_n = n;
    uint64_t exact_target = target;
    if (n > MAX_EXACT_N) {
        /* The only supported step beyond the u128 search range is prime. */
        if (!is_prime((unsigned)n) || target == 0 || inverse[n] != n)
            die("invalid prime-recurrence witness");
        exact_n = n - 1;
        exact_target = target - 1U;
    }

    const u128 lcm = make_lcm(exact_n);
    u128 scaled_sum = 0;
    for (int j = 1; j <= exact_n; ++j)
        scaled_sum += (u128)inverse[j] * (lcm / (u128)j);
    if (scaled_sum != (u128)exact_target * lcm)
        die("witness sum mismatch");
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

static void release_bfile_lock(void)
{
    if (bfile_lock_descriptor < 0) return;
    const int saved_error = errno;
    const struct flock lock = {
        .l_type = F_UNLCK,
        .l_whence = SEEK_SET,
        .l_start = 0,
        .l_len = 0
    };
    (void)fcntl(bfile_lock_descriptor, F_SETLK, &lock);
    (void)close(bfile_lock_descriptor);
    bfile_lock_descriptor = -1;
    errno = saved_error;
}

static void discard_bfile_temp(void)
{
    const int saved_error = errno;
    (void)unlink(bfile_temp_name);
    release_bfile_lock();
    errno = saved_error;
}

static FILE *open_bfile(void)
{
    bfile_lock_descriptor =
        open(BFILE_LOCK_NAME, O_CREAT | O_RDWR, 0666);
    if (bfile_lock_descriptor < 0) {
        fprintf(stderr, "error: cannot open %s: %s\n",
                BFILE_LOCK_NAME, strerror(errno));
        exit(EXIT_FAILURE);
    }
    const struct flock lock = {
        .l_type = F_WRLCK,
        .l_whence = SEEK_SET,
        .l_start = 0,
        .l_len = 0
    };
    if (fcntl(bfile_lock_descriptor, F_SETLK, &lock) != 0) {
        const int saved_error = errno;
        release_bfile_lock();
        if (saved_error == EACCES || saved_error == EAGAIN)
            fprintf(stderr, "error: another writer holds %s\n",
                    BFILE_LOCK_NAME);
        else
            fprintf(stderr, "error: cannot lock %s: %s\n",
                    BFILE_LOCK_NAME, strerror(saved_error));
        exit(EXIT_FAILURE);
    }

    const int descriptor = mkstemp(bfile_temp_name);
    if (descriptor < 0) {
        const int saved_error = errno;
        release_bfile_lock();
        fprintf(stderr, "error: cannot create %s: %s\n",
                BFILE_TEMP_TEMPLATE, strerror(saved_error));
        exit(EXIT_FAILURE);
    }

    struct stat existing;
    mode_t mode;
    if (stat(BFILE_NAME, &existing) == 0) {
        mode = existing.st_mode & 0777;
    } else if (errno == ENOENT) {
        const mode_t mask = umask(0);
        (void)umask(mask);
        mode = 0666 & ~mask;
    } else {
        const int saved_error = errno;
        (void)close(descriptor);
        errno = saved_error;
        discard_bfile_temp();
        fprintf(stderr, "error: cannot inspect %s: %s\n",
                BFILE_NAME, strerror(saved_error));
        exit(EXIT_FAILURE);
    }
    if (fchmod(descriptor, mode) != 0) {
        const int saved_error = errno;
        (void)close(descriptor);
        errno = saved_error;
        discard_bfile_temp();
        fprintf(stderr, "error: cannot set permissions on %s: %s\n",
                bfile_temp_name, strerror(saved_error));
        exit(EXIT_FAILURE);
    }

    FILE *stream = fdopen(descriptor, "w");
    if (stream == NULL) {
        const int saved_error = errno;
        (void)close(descriptor);
        errno = saved_error;
        discard_bfile_temp();
        fprintf(stderr, "error: cannot open %s: %s\n",
                bfile_temp_name, strerror(saved_error));
        exit(EXIT_FAILURE);
    }
    return stream;
}

static void write_bfile_term(FILE *stream, int n, uint64_t value)
{
    if (fprintf(stream, "%d %" PRIu64 "\n", n, value) < 0 ||
        fflush(stream) != 0) {
        const int saved_error = errno == 0 ? EIO : errno;
        (void)fclose(stream);
        errno = saved_error;
        discard_bfile_temp();
        fprintf(stderr, "error: cannot write %s: %s\n",
                bfile_temp_name, strerror(saved_error));
        exit(EXIT_FAILURE);
    }
}

static void finish_bfile(FILE *stream)
{
    bool failed = false;
    int saved_error = 0;
    if (fflush(stream) != 0) {
        failed = true;
        saved_error = errno;
    }
    const int descriptor = fileno(stream);
    if (descriptor < 0) {
        failed = true;
        if (saved_error == 0) saved_error = errno;
    } else if (!failed && fsync(descriptor) != 0) {
        failed = true;
        saved_error = errno;
    }
    if (fclose(stream) != 0) {
        failed = true;
        if (saved_error == 0) saved_error = errno;
    }
    if (failed) {
        if (saved_error == 0) saved_error = EIO;
        errno = saved_error;
        discard_bfile_temp();
        fprintf(stderr, "error: cannot finalize %s: %s\n",
                bfile_temp_name, strerror(saved_error));
        exit(EXIT_FAILURE);
    }
    if (rename(bfile_temp_name, BFILE_NAME) != 0) {
        const int rename_error = errno;
        errno = rename_error;
        discard_bfile_temp();
        fprintf(stderr, "error: cannot replace %s: %s\n",
                BFILE_NAME, strerror(rename_error));
        exit(EXIT_FAILURE);
    }
    release_bfile_lock();
}

static void usage(FILE *stream, const char *program)
{
    fprintf(stream,
            "Usage: %s [--upto N | --term N | --check] [--threads T] "
            "[--witness] [--verbose]\n",
            program);
}

int main(int argc, char **argv)
{
    OutputMode mode = MODE_UPTO;
    int limit = DEFAULT_MAX_N;
    unsigned threads = DEFAULT_THREADS;
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
        } else if (strcmp(argv[i], "--threads") == 0) {
            if (i + 1 == argc) {
                usage(stderr, argv[0]);
                return EXIT_FAILURE;
            }
            threads = parse_threads(argv[++i]);
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

    FILE *bfile = mode == MODE_UPTO && !check ? open_bfile() : NULL;
    const int first = mode == MODE_TERM ? limit : 0;
    bool have_previous = false;
    uint64_t previous_value = 0;
    int previous_witness[MAX_N + 1] = {0};
    for (int n = first; n <= limit; ++n) {
        int witness[MAX_N + 1] = {0};
        uint64_t nodes = 0;
        size_t jobs = 0;
        bool prime_step = false;
        const double started = monotonic_seconds();
        const uint64_t value = compute_output_term(
            n, threads, mode == MODE_UPTO && have_previous,
            previous_value, previous_witness, witness, &nodes, &jobs,
            &prime_step);
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

        if (bfile != NULL) write_bfile_term(bfile, n, value);

        previous_value = value;
        memcpy(previous_witness, witness, sizeof(previous_witness));
        have_previous = true;

        if (mode == MODE_TERM)
            printf("%" PRIu64 "\n", value);
        else
            printf("%s%" PRIu64, n == 0 ? "" : ", ", value);

        if (fflush(stdout) != 0) die("could not flush stdout");

        if (verbose)
            fprintf(stderr,
                    "349257_01: n=%d, a(n)=%" PRIu64
                    ", method=%s, threads=%u, jobs=%zu, nodes=%" PRIu64
                    ", %.3f s\n",
                    n, value,
                    prime_step ? "prime-recurrence" : "exact-search",
                    threads, jobs, nodes,
                    monotonic_seconds() - started);
        if (show_witness) print_witness(witness, n);
    }
    if (mode == MODE_UPTO) {
        if (putchar('\n') == EOF || fflush(stdout) != 0)
            die("could not finish stdout");
    }
    if (bfile != NULL) finish_bfile(bfile);

    if (check)
        fprintf(stderr,
                "check passed: A349257(0..%d), direct permutations through "
                "n=%d\n",
                KNOWN_MAX_N, DIRECT_CHECK_MAX_N);
    return EXIT_SUCCESS;
}
