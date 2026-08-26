/*
 * A349257 -- parallel p-adic branch-and-bound implementation.
 *
 * Find the largest integer of the form
 *
 *                      sum_{k=1..n} k/p(k),
 *
 * where p is a permutation of [n].  This is deliberately independent of
 * 349257_01.c.  It does not test candidate answers downwards and does not use
 * residual-target gcd or permutation-lattice congruence pruning.
 *
 * Put q=p^{-1}, L=lcm(1,...,n), and w_j=L/j.  We maximize
 *
 *                      S = sum_j q(j)*w_j
 *
 * directly.  S/L is integral exactly when S is zero modulo every maximal
 * prime power r^a dividing L.  For a fixed prime r, terms with r not dividing
 * j vanish modulo r^a.  The search maintains these small p-adic residues and
 * rejects a branch as soon as the last denominator belonging to a constraint
 * makes that constraint nonzero.  An assignment upper bound proves that a
 * branch cannot improve the best integral value already found.
 *
 * With --threads T, a deterministic prefix expansion creates independent
 * frontier jobs.  Workers own all mutable search state; only the incumbent
 * value is atomic, and its witness is protected by a mutex.  Parallelism can
 * change which maximizing witness is retained, but never the sequence value.
 * For the currently supported n<=31 the serial version is normally faster;
 * the threaded path is included as an independently checkable implementation
 * for larger searches and experiments.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic 349257_02.c \
 *       -o 349257_02 -pthread
 *
 * Usage:
 *   ./349257_02                         # a(0)..a(25), serial
 *   ./349257_02 --upto 25 --threads 8
 *   ./349257_02 --term 25 --threads 8 --witness --verbose
 *   ./349257_02 --check
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

#define MAX_N 31
#define DEFAULT_MAX_N 25
#define KNOWN_MAX_N 19
#define DIRECT_CHECK_MAX_N 10
#define MAX_CONSTRAINTS 16
#define MAX_THREADS 64
#define JOBS_PER_THREAD 8
#define MAX_JOBS 65536

_Static_assert(MAX_N <= 31, "uint32_t masks support at most n=31");

static const uint64_t known[KNOWN_MAX_N + 1] = {
    UINT64_C(0),  UINT64_C(1),  UINT64_C(2),  UINT64_C(3),
    UINT64_C(6),  UINT64_C(7),  UINT64_C(10), UINT64_C(11),
    UINT64_C(15), UINT64_C(18), UINT64_C(21), UINT64_C(22),
    UINT64_C(27), UINT64_C(28), UINT64_C(32), UINT64_C(36),
    UINT64_C(40), UINT64_C(41), UINT64_C(46), UINT64_C(47)
};

typedef struct {
    uint32_t denominator_mask;
    uint32_t coefficient[MAX_N + 1];
    uint32_t modulus;
    uint32_t prime;
} Constraint;

typedef struct {
    uint32_t denominators;
    uint32_t numerators;
    uint64_t scaled_sum;
    uint32_t residue[MAX_CONSTRAINTS];
    uint8_t inverse[MAX_N + 1];
} Job;

typedef struct {
    int n;
    unsigned constraint_count;
    uint64_t lcm;
    uint64_t weight[MAX_N + 1];
    Constraint constraint[MAX_CONSTRAINTS];

    _Atomic uint64_t best;
    pthread_mutex_t witness_mutex;
    uint8_t best_inverse[MAX_N + 1];
    _Atomic uint64_t nodes;

    Job *jobs;
    size_t job_count;
    size_t desired_jobs;
    _Atomic size_t next_job;
} Shared;

typedef struct {
    Shared *shared;
    uint32_t residue[MAX_CONSTRAINTS];
    uint8_t inverse[MAX_N + 1];
    uint64_t nodes;
} Worker;

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

static unsigned bit_count(uint32_t mask)
{
#if defined(__clang__) || defined(__GNUC__)
    return (unsigned)__builtin_popcount(mask);
#else
    unsigned result = 0;
    while (mask != 0) {
        mask &= mask - 1U;
        ++result;
    }
    return result;
#endif
}

static int first_index(uint32_t mask)
{
#if defined(__clang__) || defined(__GNUC__)
    return (int)__builtin_ctz(mask) + 1;
#else
    int result = 1;
    while ((mask & 1U) == 0) {
        mask >>= 1U;
        ++result;
    }
    return result;
#endif
}

static bool is_prime(unsigned value)
{
    if (value < 2U) return false;
    if ((value & 1U) == 0) return value == 2U;
    for (unsigned divisor = 3; divisor <= value / divisor; divisor += 2U)
        if (value % divisor == 0) return false;
    return true;
}

static uint64_t make_lcm(int n)
{
    uint64_t value = 1;
    for (uint64_t k = 2; k <= (uint64_t)n; ++k) {
        const uint64_t divisor = gcd_u64(value, k);
        if (value > UINT64_MAX / (k / divisor)) die("LCM overflow");
        value *= k / divisor;
    }
    return value;
}

static int parse_integer(const char *text, const char *label,
                         int minimum, int maximum)
{
    errno = 0;
    char *end = NULL;
    const intmax_t value = strtoimax(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' ||
        value < minimum || value > maximum) {
        fprintf(stderr, "error: %s must be in %d..%d: %s\n",
                label, minimum, maximum, text);
        exit(EXIT_FAILURE);
    }
    return (int)value;
}

static int parse_n(const char *text)
{
    return parse_integer(text, "n", 0, MAX_N);
}

static unsigned parse_threads(const char *text)
{
    return (unsigned)parse_integer(text, "threads", 1, MAX_THREADS);
}

static uint32_t full_mask(int n)
{
    return (UINT32_C(1) << n) - 1U;
}

static void shared_init(Shared *shared, int n)
{
    memset(shared, 0, sizeof(*shared));
    shared->n = n;
    shared->lcm = make_lcm(n);
    for (int j = 1; j <= n; ++j)
        shared->weight[j] = shared->lcm / (uint64_t)j;

    for (unsigned prime = 2; prime <= (unsigned)n; ++prime) {
        if (!is_prime(prime)) continue;
        if (shared->constraint_count == MAX_CONSTRAINTS)
            die("too many prime constraints");

        Constraint *constraint =
            &shared->constraint[shared->constraint_count++];
        constraint->prime = prime;
        uint32_t modulus = prime;
        while (modulus <= (uint32_t)n / prime) modulus *= prime;
        constraint->modulus = modulus;
        for (int j = 1; j <= n; ++j) {
            if ((unsigned)j % prime != 0) continue;
            constraint->denominator_mask |= UINT32_C(1) << (j - 1);
            constraint->coefficient[j] =
                (uint32_t)(shared->weight[j] % modulus);
        }
    }

    atomic_init(&shared->best, (uint64_t)n);
    atomic_init(&shared->nodes, 0);
    atomic_init(&shared->next_job, 0);
    if (pthread_mutex_init(&shared->witness_mutex, NULL) != 0)
        die("pthread_mutex_init failed");
    for (int j = 1; j <= n; ++j)
        shared->best_inverse[j] = (uint8_t)j;
}

static void shared_clear(Shared *shared)
{
    free(shared->jobs);
    if (pthread_mutex_destroy(&shared->witness_mutex) != 0)
        die("pthread_mutex_destroy failed");
}

/* Rearrangement-inequality upper bound for all remaining assignments. */
static uint64_t maximum_completion(const Shared *shared,
                                   uint32_t denominators,
                                   uint32_t numerators)
{
    int values[MAX_N];
    int count = 0;
    for (int value = 1; value <= shared->n; ++value)
        if ((numerators & (UINT32_C(1) << (value - 1))) != 0)
            values[count++] = value;

    uint64_t result = 0;
    int rank = 0;
    for (int j = 1; j <= shared->n; ++j) {
        if ((denominators & (UINT32_C(1) << (j - 1))) == 0) continue;
        result += (uint64_t)values[count - 1 - rank] * shared->weight[j];
        ++rank;
    }
    return result;
}

static bool can_improve(const Shared *shared, uint32_t denominators,
                        uint32_t numerators, uint64_t scaled_sum)
{
    const uint64_t upper =
        scaled_sum + maximum_completion(shared, denominators, numerators);
    const uint64_t incumbent =
        atomic_load_explicit(&shared->best, memory_order_relaxed);
    return upper / shared->lcm > incumbent;
}

/*
 * Return the next denominator.  A denominator that is last in a p-adic
 * constraint is preferred because its candidate numerators can be filtered
 * immediately.  Otherwise work on the smallest unfinished constraint.  In
 * that constraint, coefficients with a large gcd against the modulus are
 * assigned first, tending to leave a unit coefficient for the final step.
 */
static int choose_denominator(const Worker *worker, uint32_t denominators,
                              uint32_t numerators, bool *impossible)
{
    const Shared *shared = worker->shared;
    uint32_t forced = 0;
    int active_constraint = -1;
    unsigned smallest_group = MAX_N + 1U;

    *impossible = false;
    for (unsigned c = 0; c < shared->constraint_count; ++c) {
        const Constraint *constraint = &shared->constraint[c];
        const uint32_t remaining =
            denominators & constraint->denominator_mask;
        const unsigned count = bit_count(remaining);
        if (count == 0U) {
            if (worker->residue[c] != 0U) {
                *impossible = true;
                return 0;
            }
        } else if (count == 1U) {
            forced |= remaining;
        } else if (count < smallest_group ||
                   (count == smallest_group &&
                    (active_constraint < 0 ||
                     constraint->modulus >
                         shared->constraint[active_constraint].modulus))) {
            smallest_group = count;
            active_constraint = (int)c;
        }
    }

    int best_denominator = 0;
    int best_candidate_count = shared->n + 1;
    if (forced != 0) {
        uint32_t scan = forced;
        while (scan != 0) {
            const int denominator = first_index(scan);
            scan &= scan - 1U;
            int candidate_count = 0;
            for (int value = 1; value <= shared->n; ++value) {
                const uint32_t bit = UINT32_C(1) << (value - 1);
                if ((numerators & bit) == 0) continue;
                bool allowed = true;
                const uint32_t denominator_bit =
                    UINT32_C(1) << (denominator - 1);
                for (unsigned c = 0; c < shared->constraint_count; ++c) {
                    const Constraint *constraint = &shared->constraint[c];
                    if ((constraint->denominator_mask & denominator_bit) == 0)
                        continue;
                    if (bit_count(denominators &
                                  constraint->denominator_mask) != 1U)
                        continue;
                    const uint32_t residue =
                        (worker->residue[c] +
                         (uint32_t)value *
                             constraint->coefficient[denominator]) %
                        constraint->modulus;
                    if (residue != 0U) {
                        allowed = false;
                        break;
                    }
                }
                if (allowed) ++candidate_count;
            }
            if (candidate_count < best_candidate_count ||
                (candidate_count == best_candidate_count &&
                 (best_denominator == 0 || denominator < best_denominator))) {
                best_candidate_count = candidate_count;
                best_denominator = denominator;
            }
        }
        if (best_candidate_count == 0) *impossible = true;
        return best_denominator;
    }

    if (active_constraint >= 0) {
        const Constraint *constraint =
            &shared->constraint[active_constraint];
        uint64_t best_gcd = 0;
        uint32_t scan = denominators & constraint->denominator_mask;
        while (scan != 0) {
            const int denominator = first_index(scan);
            scan &= scan - 1U;
            const uint64_t divisor =
                gcd_u64(constraint->coefficient[denominator],
                        constraint->modulus);
            if (best_denominator == 0 || divisor > best_gcd ||
                (divisor == best_gcd && denominator < best_denominator)) {
                best_gcd = divisor;
                best_denominator = denominator;
            }
        }
        return best_denominator;
    }

    return first_index(denominators);
}

static bool candidate_allowed(const Worker *worker, uint32_t denominators,
                              int denominator, int value)
{
    const Shared *shared = worker->shared;
    const uint32_t denominator_bit = UINT32_C(1) << (denominator - 1);
    for (unsigned c = 0; c < shared->constraint_count; ++c) {
        const Constraint *constraint = &shared->constraint[c];
        if ((constraint->denominator_mask & denominator_bit) == 0) continue;
        if (bit_count(denominators & constraint->denominator_mask) != 1U)
            continue;
        const uint32_t residue =
            (worker->residue[c] +
             (uint32_t)value * constraint->coefficient[denominator]) %
            constraint->modulus;
        if (residue != 0U) return false;
    }
    return true;
}

static void assign_residues(Worker *worker, int denominator, int value,
                            uint32_t old[MAX_CONSTRAINTS])
{
    Shared *shared = worker->shared;
    const uint32_t denominator_bit = UINT32_C(1) << (denominator - 1);
    for (unsigned c = 0; c < shared->constraint_count; ++c) {
        old[c] = worker->residue[c];
        const Constraint *constraint = &shared->constraint[c];
        if ((constraint->denominator_mask & denominator_bit) == 0) continue;
        worker->residue[c] =
            (worker->residue[c] +
             (uint32_t)value * constraint->coefficient[denominator]) %
            constraint->modulus;
    }
}

static void restore_residues(Worker *worker,
                             const uint32_t old[MAX_CONSTRAINTS])
{
    for (unsigned c = 0; c < worker->shared->constraint_count; ++c)
        worker->residue[c] = old[c];
}

static void record_solution(Worker *worker, uint64_t scaled_sum)
{
    Shared *shared = worker->shared;
    if (scaled_sum % shared->lcm != 0)
        die("p-adic constraints accepted a nonintegral sum");
    const uint64_t value = scaled_sum / shared->lcm;
    uint64_t incumbent =
        atomic_load_explicit(&shared->best, memory_order_relaxed);
    while (value > incumbent &&
           !atomic_compare_exchange_weak_explicit(
               &shared->best, &incumbent, value,
               memory_order_acq_rel, memory_order_relaxed)) {
    }
    if (value <= incumbent) return;

    if (pthread_mutex_lock(&shared->witness_mutex) != 0)
        die("pthread_mutex_lock failed");
    if (atomic_load_explicit(&shared->best, memory_order_acquire) == value)
        memcpy(shared->best_inverse, worker->inverse,
               sizeof(shared->best_inverse));
    if (pthread_mutex_unlock(&shared->witness_mutex) != 0)
        die("pthread_mutex_unlock failed");
}

static void maximize(Worker *worker, uint32_t denominators,
                     uint32_t numerators, uint64_t scaled_sum)
{
    ++worker->nodes;
    if (denominators == 0) {
        record_solution(worker, scaled_sum);
        return;
    }
    if (!can_improve(worker->shared, denominators, numerators, scaled_sum))
        return;

    bool impossible;
    const int denominator =
        choose_denominator(worker, denominators, numerators, &impossible);
    if (impossible) return;
    const uint32_t denominator_bit = UINT32_C(1) << (denominator - 1);

    for (int value = worker->shared->n; value >= 1; --value) {
        const uint32_t numerator_bit = UINT32_C(1) << (value - 1);
        if ((numerators & numerator_bit) == 0) continue;
        if (!candidate_allowed(worker, denominators, denominator, value))
            continue;

        uint32_t old[MAX_CONSTRAINTS];
        assign_residues(worker, denominator, value, old);
        worker->inverse[denominator] = (uint8_t)value;
        maximize(worker, denominators ^ denominator_bit,
                 numerators ^ numerator_bit,
                 scaled_sum +
                     (uint64_t)value * worker->shared->weight[denominator]);
        restore_residues(worker, old);
        worker->inverse[denominator] = 0;
    }
}

static void append_job(Worker *builder, uint32_t denominators,
                       uint32_t numerators, uint64_t scaled_sum)
{
    Shared *shared = builder->shared;
    if (shared->job_count == MAX_JOBS) die("parallel frontier is too large");
    Job *job = &shared->jobs[shared->job_count++];
    job->denominators = denominators;
    job->numerators = numerators;
    job->scaled_sum = scaled_sum;
    memcpy(job->residue, builder->residue, sizeof(job->residue));
    memcpy(job->inverse, builder->inverse, sizeof(job->inverse));
}

static void build_frontier(Worker *builder, uint32_t denominators,
                           uint32_t numerators, uint64_t scaled_sum,
                           unsigned depth)
{
    if (!can_improve(builder->shared, denominators, numerators, scaled_sum))
        return;
    if (depth == 0U || builder->shared->job_count >=
                           builder->shared->desired_jobs) {
        append_job(builder, denominators, numerators, scaled_sum);
        return;
    }

    bool impossible;
    const int denominator =
        choose_denominator(builder, denominators, numerators, &impossible);
    if (impossible) return;
    const uint32_t denominator_bit = UINT32_C(1) << (denominator - 1);
    for (int value = builder->shared->n; value >= 1; --value) {
        const uint32_t numerator_bit = UINT32_C(1) << (value - 1);
        if ((numerators & numerator_bit) == 0) continue;
        if (!candidate_allowed(builder, denominators, denominator, value))
            continue;
        uint32_t old[MAX_CONSTRAINTS];
        assign_residues(builder, denominator, value, old);
        builder->inverse[denominator] = (uint8_t)value;
        build_frontier(builder, denominators ^ denominator_bit,
                       numerators ^ numerator_bit,
                       scaled_sum + (uint64_t)value *
                                        builder->shared->weight[denominator],
                       depth - 1U);
        restore_residues(builder, old);
        builder->inverse[denominator] = 0;
    }
}

static void *worker_main(void *argument)
{
    Worker worker;
    memset(&worker, 0, sizeof(worker));
    worker.shared = argument;

    for (;;) {
        const size_t index = atomic_fetch_add_explicit(
            &worker.shared->next_job, 1, memory_order_relaxed);
        if (index >= worker.shared->job_count) break;
        const Job *job = &worker.shared->jobs[index];
        memcpy(worker.residue, job->residue, sizeof(worker.residue));
        memcpy(worker.inverse, job->inverse, sizeof(worker.inverse));
        maximize(&worker, job->denominators, job->numerators,
                 job->scaled_sum);
    }
    atomic_fetch_add_explicit(&worker.shared->nodes, worker.nodes,
                              memory_order_relaxed);
    return NULL;
}

static uint64_t compute_term(int n, unsigned thread_count,
                             uint8_t witness[MAX_N + 1],
                             uint64_t *node_count, size_t *job_count)
{
    if (n == 0) {
        if (witness != NULL) memset(witness, 0, MAX_N + 1U);
        if (node_count != NULL) *node_count = 0;
        if (job_count != NULL) *job_count = 0;
        return 0;
    }

    Shared shared;
    shared_init(&shared, n);
    const uint32_t mask = full_mask(n);

    if (thread_count == 1U) {
        Worker worker;
        memset(&worker, 0, sizeof(worker));
        worker.shared = &shared;
        maximize(&worker, mask, mask, 0);
        atomic_store_explicit(&shared.nodes, worker.nodes,
                              memory_order_relaxed);
    } else {
        shared.jobs = calloc(MAX_JOBS, sizeof(*shared.jobs));
        if (shared.jobs == NULL) die("could not allocate frontier jobs");
        shared.desired_jobs = (size_t)thread_count * JOBS_PER_THREAD;

        Worker builder;
        memset(&builder, 0, sizeof(builder));
        builder.shared = &shared;
        for (unsigned depth = 1; depth <= (unsigned)n; ++depth) {
            shared.job_count = 0;
            memset(builder.residue, 0, sizeof(builder.residue));
            memset(builder.inverse, 0, sizeof(builder.inverse));
            build_frontier(&builder, mask, mask, 0, depth);
            if (shared.job_count >= thread_count || depth == (unsigned)n)
                break;
        }
        if (shared.job_count != 0) {
            pthread_t threads[MAX_THREADS];
            atomic_store_explicit(&shared.next_job, 0, memory_order_relaxed);
            for (unsigned i = 0; i < thread_count; ++i)
                if (pthread_create(&threads[i], NULL, worker_main, &shared) !=
                    0)
                    die("pthread_create failed");
            for (unsigned i = 0; i < thread_count; ++i)
                if (pthread_join(threads[i], NULL) != 0)
                    die("pthread_join failed");
        }
    }

    const uint64_t result =
        atomic_load_explicit(&shared.best, memory_order_acquire);
    if (witness != NULL)
        memcpy(witness, shared.best_inverse, sizeof(shared.best_inverse));
    if (node_count != NULL)
        *node_count = atomic_load_explicit(&shared.nodes, memory_order_relaxed);
    if (job_count != NULL) *job_count = shared.job_count;
    shared_clear(&shared);
    return result;
}

static bool next_permutation(int values[MAX_N], int n)
{
    int i = n - 2;
    while (i >= 0 && values[i] >= values[i + 1]) --i;
    if (i < 0) return false;
    int j = n - 1;
    while (values[j] <= values[i]) --j;
    int temporary = values[i];
    values[i] = values[j];
    values[j] = temporary;
    for (int left = i + 1, right = n - 1; left < right;
         ++left, --right) {
        temporary = values[left];
        values[left] = values[right];
        values[right] = temporary;
    }
    return true;
}

static uint64_t direct_term(int n)
{
    if (n == 0) return 0;
    const uint64_t lcm = make_lcm(n);
    int inverse[MAX_N];
    for (int i = 0; i < n; ++i) inverse[i] = i + 1;
    uint64_t best = 0;
    do {
        uint64_t sum = 0;
        for (int j = 1; j <= n; ++j)
            sum += (uint64_t)inverse[j - 1] * (lcm / (uint64_t)j);
        if (sum % lcm == 0 && sum / lcm > best) best = sum / lcm;
    } while (next_permutation(inverse, n));
    return best;
}

static void verify_value(uint64_t value, int n)
{
    if (n <= KNOWN_MAX_N && value != known[n]) {
        fprintf(stderr,
                "error: A349257 mismatch at n=%d: got %" PRIu64
                ", expected %" PRIu64 "\n",
                n, value, known[n]);
        exit(EXIT_FAILURE);
    }
}

static void verify_witness(const uint8_t inverse[MAX_N + 1], int n,
                           uint64_t target)
{
    const uint64_t lcm = make_lcm(n);
    uint32_t seen = 0;
    uint64_t sum = 0;
    for (int j = 1; j <= n; ++j) {
        const unsigned value = inverse[j];
        if (value < 1U || value > (unsigned)n) die("invalid witness value");
        const uint32_t bit = UINT32_C(1) << (value - 1U);
        if ((seen & bit) != 0) die("duplicate witness value");
        seen |= bit;
        sum += (uint64_t)value * (lcm / (uint64_t)j);
    }
    if (sum != target * lcm) die("witness sum mismatch");
}

static void print_witness(const uint8_t inverse[MAX_N + 1], int n)
{
    uint8_t permutation[MAX_N + 1] = {0};
    for (int j = 1; j <= n; ++j) permutation[inverse[j]] = (uint8_t)j;
    fprintf(stderr, "q=p^-1: [");
    for (int j = 1; j <= n; ++j)
        fprintf(stderr, "%s%u", j == 1 ? "" : ",", inverse[j]);
    fprintf(stderr, "]\np:      [");
    for (int k = 1; k <= n; ++k)
        fprintf(stderr, "%s%u", k == 1 ? "" : ",", permutation[k]);
    fprintf(stderr, "]\n");
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
    unsigned threads = 1;
    bool check = false;
    bool witness_requested = false;
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
            witness_requested = true;
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
    if (witness_requested && mode != MODE_TERM)
        die("--witness requires --term N");

    const int first = mode == MODE_TERM ? limit : 0;
    for (int n = first; n <= limit; ++n) {
        uint8_t witness[MAX_N + 1] = {0};
        uint64_t nodes = 0;
        size_t jobs = 0;
        const double started = monotonic_seconds();
        const uint64_t value =
            compute_term(n, threads, witness, &nodes, &jobs);
        verify_value(value, n);
        if (n != 0) verify_witness(witness, n, value);
        if (check && n <= DIRECT_CHECK_MAX_N) {
            const uint64_t direct = direct_term(n);
            if (direct != value) {
                fprintf(stderr,
                        "error: direct mismatch at n=%d: p-adic=%" PRIu64
                        ", direct=%" PRIu64 "\n",
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
                    "349257_02: n=%d, a(n)=%" PRIu64
                    ", threads=%u, jobs=%zu, nodes=%" PRIu64 ", %.3f s\n",
                    n, value, threads, jobs, nodes,
                    monotonic_seconds() - started);
        if (witness_requested) print_witness(witness, n);
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
