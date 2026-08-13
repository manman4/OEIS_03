/*
 * Exact counts of partitions of {1,...,3*n} into unordered triples of
 * distinct values satisfying x+y=3*z.  This is a c=3 adaptation of the
 * recursive/pruning framework of Hercher and Niedermeyer, arXiv:2307.00303;
 * their order inequalities for x+y=z are not used because z need not be the
 * largest member here.
 *
 * Correctness of the recursion
 * ----------------------------
 * Let M be the largest remaining value.  M cannot be z: two other remaining
 * values are both smaller than M, so their sum is less than 2*M<3*M.  Hence M
 * is one of x,y.  After choosing z, the other addend is uniquely
 *
 *                         q = 3*z-M.
 *
 * Positivity and q<M give M/3<z<2*M/3.  Thus every completion has exactly one
 * branch for the triple containing M.  Induction on the number of triples
 * proves that every unordered partition is counted exactly once.
 *
 * Exact pruning
 * -------------
 * For a remaining set S of 3*m values with total T, let Z be its m values
 * used in the z role.  Summing x+y=3*z over a completion gives
 *
 *                    |Z|=m,       sum(Z)=T/4.                 (1)
 *
 * Consequently T must be divisible by 4.  An exact cardinality-aware subset
 * sum DP checks (1).  If L (respectively H) is the sum of the m smallest
 * (respectively largest) remaining values, then 4*L<=T<=4*H.  Equality on
 * the left fixes Z to the lower block and persists in descendants.  Equality
 * on the right is impossible because it would make the largest value M a z.
 *
 * In each triple let b=max(x,y).  Distinctness and x+y=3*z imply b>3*z/2.
 * The m values b form an m-subset of S, so H>=sum(b)>3*T/8.  Therefore
 * 8*H<=3*T is another exact impossibility test.  No probabilistic step is
 * used.
 *
 * At the root T=3*n*(3*n+1)/2 must be divisible by 4, which is equivalent to
 * n==0 or 5 (mod 8).  Values 1..63 use one uint64_t mask, hence MAX_N=21.
 * At every level there are at most n choices for z, so the result is at most
 * n^n; 21^21<2^93, safely within U128.  All additions are checked as well.
 *
 * Build:
 *   clang -O3 -march=native -std=c11 -Wall -Wextra -Wpedantic -pthread \
 *       397152_01.c -o 397152_01
 *
 * Results are atomically recorded in b397152_01.txt by default.  A --term
 * request is rejected before calculation if earlier b-file terms are missing;
 * use the prefix form or --no-bfile in that case.
 *   ./397152_01 8 --threads 8
 *   ./397152_01 --term 8 --threads 8
 *   ./397152_01 --check --threads 8
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
#include <time.h>
#include <unistd.h>

#if !defined(__SIZEOF_INT128__)
#error "397152_01.c requires unsigned __int128"
#endif
__extension__ typedef unsigned __int128 U128;

#define MAX_N 21
#define MAX_VALUES (3 * MAX_N)
#define MAX_THREADS 64
#define DEFAULT_N 8
#define DEFAULT_THREADS 4
#define MAX_TARGET ((MAX_VALUES * (MAX_VALUES + 1)) / 8)
#define TARGET_WORDS ((MAX_TARGET / 64) + 1)
#define MAX_ROOT_TASKS MAX_VALUES

_Static_assert(MAX_VALUES < 64, "search mask must fit in uint64_t");
_Static_assert(MAX_TARGET == 504, "unexpected subset-sum bound");

static int requested_threads = DEFAULT_THREADS;
static const char *output_path = "b397152_01.txt";
static bool write_bfile = true;
static _Atomic uint64_t live_state_chunks;
static _Atomic unsigned completed_tasks;
static pthread_mutex_t monitor_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t monitor_condition = PTHREAD_COND_INITIALIZER;
static bool monitor_finished;

static _Noreturn void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static int parse_bounded(const char *text, int low, int high,
                         const char *what)
{
    char *end = NULL;
    errno = 0;
    const long value = strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' ||
        value < low || value > high) {
        fprintf(stderr, "error: %s must be in %d..%d: %s\n",
                what, low, high, text);
        exit(EXIT_FAILURE);
    }
    return (int)value;
}

static double now_seconds(void)
{
    struct timespec value;
    if (clock_gettime(CLOCK_MONOTONIC, &value) != 0) {
        die("clock_gettime failed");
    }
    return (double)value.tv_sec + (double)value.tv_nsec / 1e9;
}

static bool add_u128(U128 *destination, U128 addend)
{
    const U128 old = *destination;
    *destination += addend;
    return *destination >= old;
}

static int print_u128(FILE *stream, U128 value)
{
    char digits[40];
    size_t length = 0;
    do {
        digits[length++] = (char)('0' + (unsigned)(value % 10));
        value /= 10;
    } while (value != 0);
    while (length != 0) {
        if (fputc(digits[--length], stream) == EOF) return -1;
    }
    return 0;
}

static void print_term(int n, U128 value)
{
    if (fprintf(stdout, "%d ", n) < 0 || print_u128(stdout, value) != 0 ||
        fputc('\n', stdout) == EOF || fflush(stdout) != 0) {
        die("cannot write result");
    }
}

static bool parse_u128(const char *text, U128 *result);

static void u128_text(U128 value, char text[40])
{
    char reverse[40];
    size_t length = 0;
    do {
        reverse[length++] = (char)('0' + (unsigned)(value % 10));
        value /= 10;
    } while (value != 0);
    for (size_t i = 0; i < length; ++i) text[i] = reverse[length - 1U - i];
    text[length] = '\0';
}

static int lock_bfile(void)
{
    char *path = malloc(strlen(output_path) + 6U);
    if (path == NULL) die("cannot allocate lock path");
    sprintf(path, "%s.lock", output_path);
    const int fd = open(path, O_RDWR | O_CREAT, 0666);
    free(path);
    if (fd < 0) die("cannot open b-file lock");
    struct flock lock = {.l_type=F_WRLCK, .l_whence=SEEK_SET};
    while (fcntl(fd, F_SETLKW, &lock) != 0) {
        if (errno != EINTR) die("cannot lock b-file");
    }
    return fd;
}

static void unlock_bfile(int fd)
{
    struct flock lock = {.l_type=F_UNLCK, .l_whence=SEEK_SET};
    if (fcntl(fd, F_SETLK, &lock) != 0 || close(fd) != 0) {
        die("cannot unlock b-file");
    }
}

/* Read a strict contiguous b-file prefix.  No uncomputed built-in values are
   inserted: only record_term() can extend this prefix. */
static int read_bfile(char values[MAX_N + 1][40])
{
    FILE *input = fopen(output_path, "r");
    if (input == NULL) {
        if (errno == ENOENT) return 0;
        die("cannot read b-file");
    }
    char line[256];
    int next = 0;
    while (fgets(line, sizeof(line), input) != NULL) {
        char *p = line;
        while (*p == ' ' || *p == '\t') ++p;
        if (*p == '\0' || *p == '\n' || *p == '#') continue;
        char number[40], extra;
        int index;
        if (sscanf(p, "%d %39s %c", &index, number, &extra) != 2 ||
            index != next || index < 0 || index > MAX_N) {
            fclose(input);
            die("b-file is malformed or has a gap");
        }
        U128 parsed;
        if (!parse_u128(number, &parsed)) {
            fclose(input);
            die("b-file contains an invalid value");
        }
        strcpy(values[next++], number);
    }
    if (ferror(input) || fclose(input) != 0) die("cannot finish reading b-file");
    return next;
}

static void require_recordable(int n)
{
    if (!write_bfile) return;
    char values[MAX_N + 1][40];
    const int fd = lock_bfile();
    const int prefix = read_bfile(values);
    unlock_bfile(fd);
    if (n > prefix) {
        fprintf(stderr,
                "error: %s currently ends before n=%d; compute the missing "
                "prefix first, or use --no-bfile\n", output_path, n);
        exit(EXIT_FAILURE);
    }
}

static void record_term(int n, U128 value)
{
    if (!write_bfile) return;
    char values[MAX_N + 1][40], text[40];
    u128_text(value, text);
    const int lock_fd = lock_bfile();
    int prefix = read_bfile(values);
    if (n < prefix) {
        if (strcmp(values[n], text) != 0) die("computed value disagrees with b-file");
        unlock_bfile(lock_fd);
        return;
    }
    if (n != prefix) die("b-file gap detected while recording");
    strcpy(values[prefix++], text);

    char *temporary = malloc(strlen(output_path) + 32U);
    if (temporary == NULL) die("cannot allocate temporary path");
    sprintf(temporary, "%s.tmp.%ld", output_path, (long)getpid());
    FILE *output = fopen(temporary, "w");
    if (output == NULL) die("cannot create temporary b-file");
    bool failed = false;
    for (int i = 0; i < prefix; ++i) {
        if (fprintf(output, "%d %s\n", i, values[i]) < 0) failed = true;
    }
    if (!failed && fflush(output) != 0) failed = true;
    if (!failed && fsync(fileno(output)) != 0) failed = true;
    if (fclose(output) != 0) failed = true;
    if (failed || rename(temporary, output_path) != 0) {
        unlink(temporary);
        free(temporary);
        die("cannot atomically update b-file");
    }
    free(temporary);
    unlock_bfile(lock_fd);
    fprintf(stderr, "397152_01: recorded computed term n=%d in %s\n",
            n, output_path);
}

static uint64_t full_mask(unsigned count)
{
    if (count == 0 || count >= 64) die("invalid full-mask size");
    return (UINT64_C(1) << count) - 1U;
}

static bool contains(uint64_t mask, unsigned value)
{
    return value != 0 && value <= MAX_VALUES &&
           (mask & (UINT64_C(1) << (value - 1U))) != 0;
}

static uint64_t without(uint64_t mask, unsigned value)
{
    if (!contains(mask, value)) die("attempt to remove absent value");
    return mask & ~(UINT64_C(1) << (value - 1U));
}

static unsigned largest(uint64_t mask)
{
    if (mask == 0) die("largest of empty mask");
    return 64U - (unsigned)__builtin_clzll(mask);
}

static int sum_smallest(uint64_t mask, unsigned count, unsigned *last)
{
    int sum = 0;
    unsigned value = 0;
    for (unsigned i = 0; i < count; ++i) {
        if (mask == 0) die("too few values in sum_smallest");
        value = (unsigned)__builtin_ctzll(mask) + 1U;
        mask &= mask - 1U;
        sum += (int)value;
    }
    if (last != NULL) *last = value;
    return sum;
}

static int sum_largest(uint64_t mask, unsigned count)
{
    int sum = 0;
    for (unsigned i = 0; i < count; ++i) {
        if (mask == 0) die("too few values in sum_largest");
        const unsigned value = largest(mask);
        mask &= ~(UINT64_C(1) << (value - 1U));
        sum += (int)value;
    }
    return sum;
}

/* Is there an exactly-'need'-element subset of mask with sum target? */
static bool role_sum_possible(uint64_t mask, unsigned need, unsigned target)
{
    if (need > MAX_N || target > MAX_TARGET) return false;
    uint64_t dp[MAX_N + 1][TARGET_WORDS] = {{0}};
    dp[0][0] = 1U;
    unsigned seen = 0;
    while (mask != 0) {
        const unsigned value = (unsigned)__builtin_ctzll(mask) + 1U;
        mask &= mask - 1U;
        if (seen < need) ++seen;
        const unsigned word_shift = value >> 6;
        const unsigned bit_shift = value & 63U;
        for (unsigned count = seen; count != 0; --count) {
            for (unsigned word = TARGET_WORDS; word-- > word_shift;) {
                uint64_t shifted =
                    dp[count - 1U][word - word_shift] << bit_shift;
                if (bit_shift != 0 && word > word_shift) {
                    shifted |= dp[count - 1U][word - word_shift - 1U] >>
                               (64U - bit_shift);
                }
                dp[count][word] |= shifted;
            }
        }
    }
    return ((dp[need][target >> 6] >> (target & 63U)) & 1U) != 0;
}

typedef struct {
    uint64_t states;
    uint64_t subset_prunes;
} SearchStats;

static U128 count_partitions(uint64_t mask, unsigned m, int total,
                             bool lower_forced, SearchStats *stats)
{
    if ((unsigned)__builtin_popcountll(mask) != 3U * m) {
        die("recursion cardinality invariant failed");
    }
    if (stats->states == UINT64_MAX) die("state counter overflow");
    ++stats->states;
    if ((stats->states & UINT64_C(65535)) == 0) {
        atomic_fetch_add_explicit(&live_state_chunks, UINT64_C(65536),
                                  memory_order_relaxed);
    }
    if (m == 0) {
        if (mask != 0 || total != 0) die("invalid terminal state");
        return 1;
    }
    if (total <= 0 || (total & 3) != 0) return 0;

    unsigned lower_last;
    const int lower_sum = sum_smallest(mask, m, &lower_last);
    if (4 * lower_sum > total) return 0;
    const bool lower_equality = 4 * lower_sum == total;
    if (lower_forced && !lower_equality) {
        die("lower-role invariant failed");
    }
    if (lower_equality) lower_forced = true;

    const int upper_sum = sum_largest(mask, m);
    if (4 * upper_sum <= total) {
        /* '<' violates (1); equality forces largest(mask) to be a z. */
        return 0;
    }
    if (8 * upper_sum <= 3 * total) return 0;

    if (!lower_forced &&
        !role_sum_possible(mask, m, (unsigned)total / 4U)) {
        if (stats->subset_prunes == UINT64_MAX) die("prune counter overflow");
        ++stats->subset_prunes;
        return 0;
    }

    const unsigned maximum = largest(mask);
    const unsigned first_z = maximum / 3U + 1U;
    unsigned last_z = (2U * maximum - 1U) / 3U;
    if (lower_forced && last_z > lower_last) last_z = lower_last;

    U128 answer = 0;
    for (unsigned z = first_z; z <= last_z; ++z) {
        const unsigned q = 3U * z - maximum;
        if (q == z || !contains(mask, z) || !contains(mask, q)) continue;
        if (lower_forced && q <= lower_last) continue;

        uint64_t child = without(mask, maximum);
        child = without(child, z);
        child = without(child, q);
        const U128 addend = count_partitions(
            child, m - 1U, total - 4 * (int)z, lower_forced, stats);
        if (!add_u128(&answer, addend)) die("answer overflowed U128");
    }
    return answer;
}

typedef struct {
    uint64_t mask;
    unsigned m;
    int total;
} RootTask;

typedef struct {
    _Atomic unsigned next;
    unsigned count;
    RootTask tasks[MAX_ROOT_TASKS];
} RootSchedule;

typedef struct {
    RootSchedule *schedule;
    U128 answer;
    SearchStats stats;
} Worker;

static unsigned build_root_tasks(int n, RootTask tasks[MAX_ROOT_TASKS])
{
    const unsigned m = (unsigned)n;
    const unsigned maximum = 3U * m;
    const uint64_t mask = full_mask(maximum);
    const int total = (int)(maximum * (maximum + 1U) / 2U);
    unsigned count = 0;
    for (unsigned z = maximum / 3U + 1U;
         z <= (2U * maximum - 1U) / 3U; ++z) {
        const unsigned q = 3U * z - maximum;
        if (q == z) continue;
        if (count == MAX_ROOT_TASKS) die("root-task array overflow");
        uint64_t child = without(mask, maximum);
        child = without(child, z);
        child = without(child, q);
        tasks[count++] = (RootTask){child, m - 1U,
                                    total - 4 * (int)z};
    }
    return count;
}

static void *worker_main(void *argument)
{
    Worker *worker = argument;
    for (;;) {
        const unsigned index = atomic_fetch_add_explicit(
            &worker->schedule->next, 1U, memory_order_relaxed);
        if (index >= worker->schedule->count) break;
        const RootTask *task = &worker->schedule->tasks[index];
        const U128 addend = count_partitions(
            task->mask, task->m, task->total, false, &worker->stats);
        if (!add_u128(&worker->answer, addend)) die("worker U128 overflow");
        atomic_fetch_add_explicit(&completed_tasks, 1U, memory_order_relaxed);
    }
    return NULL;
}

typedef struct {
    int n;
    unsigned task_count;
    double started;
} MonitorArgument;

static void *monitor_main(void *argument)
{
    const MonitorArgument *monitor = argument;
    if (pthread_mutex_lock(&monitor_mutex) != 0) die("monitor lock failed");
    while (!monitor_finished) {
        struct timespec deadline;
        if (clock_gettime(CLOCK_REALTIME, &deadline) != 0) {
            die("monitor clock_gettime failed");
        }
        deadline.tv_sec += 10;
        const int error = pthread_cond_timedwait(
            &monitor_condition, &monitor_mutex, &deadline);
        if (error != 0 && error != ETIMEDOUT) die("monitor wait failed");
        if (!monitor_finished && error == ETIMEDOUT) {
            fprintf(stderr,
                    "397152_01: heartbeat n=%d, root-tasks=%u/%u, "
                    "states>=%" PRIu64 ", %.1f s\n",
                    monitor->n,
                    atomic_load_explicit(&completed_tasks,
                                         memory_order_relaxed),
                    monitor->task_count,
                    atomic_load_explicit(&live_state_chunks,
                                         memory_order_relaxed),
                    now_seconds() - monitor->started);
        }
    }
    if (pthread_mutex_unlock(&monitor_mutex) != 0) die("monitor unlock failed");
    return NULL;
}

static U128 sequence_term(int n)
{
    if (n == 0) return 1;
    if (n % 8 != 0 && n % 8 != 5) return 0;

    RootSchedule schedule = {0};
    atomic_init(&schedule.next, 0U);
    schedule.count = build_root_tasks(n, schedule.tasks);
    if (schedule.count == 0) return 0;
    int threads = requested_threads;
    if ((unsigned)threads > schedule.count) threads = (int)schedule.count;

    Worker *workers = calloc((size_t)threads, sizeof(*workers));
    pthread_t *ids = calloc((size_t)threads, sizeof(*ids));
    if (workers == NULL || ids == NULL) {
        free(workers); free(ids); die("cannot allocate workers");
    }
    const double started = now_seconds();
    atomic_store_explicit(&live_state_chunks, 0, memory_order_relaxed);
    atomic_store_explicit(&completed_tasks, 0, memory_order_relaxed);
    if (pthread_mutex_lock(&monitor_mutex) != 0) die("monitor lock failed");
    monitor_finished = false;
    if (pthread_mutex_unlock(&monitor_mutex) != 0) die("monitor unlock failed");
    const MonitorArgument monitor_argument = {n, schedule.count, started};
    pthread_t monitor_id;
    int error = pthread_create(&monitor_id, NULL, monitor_main,
                               (void *)&monitor_argument);
    if (error != 0) {
        fprintf(stderr, "error: pthread_create monitor: %s\n", strerror(error));
        exit(EXIT_FAILURE);
    }
    for (int id = 0; id < threads; ++id) {
        workers[id].schedule = &schedule;
        error = pthread_create(&ids[id], NULL, worker_main, &workers[id]);
        if (error != 0) {
            fprintf(stderr, "error: pthread_create: %s\n", strerror(error));
            exit(EXIT_FAILURE);
        }
    }

    U128 answer = 0;
    uint64_t states = 1, subset_prunes = 0;
    for (int id = 0; id < threads; ++id) {
        const int error = pthread_join(ids[id], NULL);
        if (error != 0) {
            fprintf(stderr, "error: pthread_join: %s\n", strerror(error));
            exit(EXIT_FAILURE);
        }
        if (!add_u128(&answer, workers[id].answer)) die("final U128 overflow");
        if (UINT64_MAX - states < workers[id].stats.states ||
            UINT64_MAX - subset_prunes < workers[id].stats.subset_prunes) {
            die("statistics counter overflow");
        }
        states += workers[id].stats.states;
        subset_prunes += workers[id].stats.subset_prunes;
    }
    if (pthread_mutex_lock(&monitor_mutex) != 0) die("monitor lock failed");
    monitor_finished = true;
    if (pthread_cond_signal(&monitor_condition) != 0) {
        die("monitor signal failed");
    }
    if (pthread_mutex_unlock(&monitor_mutex) != 0) die("monitor unlock failed");
    error = pthread_join(monitor_id, NULL);
    if (error != 0) {
        fprintf(stderr, "error: pthread_join monitor: %s\n", strerror(error));
        exit(EXIT_FAILURE);
    }
    free(workers);
    free(ids);
    fprintf(stderr,
            "397152_01: n=%d, maximum-first exact pruning, "
            "root-tasks=%u, states=%" PRIu64 ", subset-prunes=%" PRIu64
            ", threads=%d, %.3f s\n",
            n, schedule.count, states, subset_prunes, threads,
            now_seconds() - started);
    return answer;
}

static const char *const known[] = {
    "1", "0", "0", "0", "0", "4", "0", "0", "86"
};

static bool parse_u128(const char *text, U128 *result)
{
    const U128 maximum = ~(U128)0;
    U128 value = 0;
    if (*text == '\0') return false;
    while (*text != '\0') {
        if (*text < '0' || *text > '9') return false;
        const unsigned digit = (unsigned)(*text++ - '0');
        if (value > (maximum - digit) / 10U) return false;
        value = 10U * value + digit;
    }
    *result = value;
    return true;
}

static void verify_known(int n, U128 value)
{
    if ((size_t)n >= sizeof(known) / sizeof(known[0])) return;
    U128 expected;
    if (!parse_u128(known[n], &expected)) die("invalid built-in known term");
    if (value != expected) {
        fprintf(stderr, "error: known-term mismatch n=%d: got ", n);
        print_u128(stderr, value);
        fprintf(stderr, ", expected %s\n", known[n]);
        exit(EXIT_FAILURE);
    }
}

static void internal_self_check(void)
{
    const uint64_t mask = full_mask(MAX_VALUES);
    if ((unsigned)__builtin_popcountll(mask) != MAX_VALUES ||
        largest(mask) != MAX_VALUES ||
        sum_smallest(mask, MAX_N, NULL) != 231 ||
        sum_largest(mask, MAX_N) != 1113) {
        die("mask boundary self-check failed");
    }
    if (!role_sum_possible(full_mask(15), 5, 30) ||
        role_sum_possible(full_mask(15), 5, 5)) {
        die("role subset-sum self-check failed");
    }
}

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s [N] [--threads T] [--output FILE|--no-bfile]\n"
            "       %s --term N [--threads T] [--output FILE|--no-bfile]\n"
            "       %s --check [--threads T] [--output FILE|--no-bfile]\n"
            "N prints a(0)..a(N); --term prints only a(N).\n",
            program, program, program);
}

int main(int argc, char **argv)
{
    int n = DEFAULT_N;
    bool term_only = false, check = false, positional = false;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--threads") == 0) {
            if (++i == argc) die("--threads needs an argument");
            requested_threads = parse_bounded(
                argv[i], 1, MAX_THREADS, "threads");
        } else if (strcmp(argv[i], "--output") == 0) {
            if (++i == argc || argv[i][0] == '\0') die("--output needs a file");
            output_path = argv[i];
            write_bfile = true;
        } else if (strcmp(argv[i], "--no-bfile") == 0) {
            write_bfile = false;
        } else if (strcmp(argv[i], "--term") == 0) {
            if (term_only || positional || ++i == argc) {
                die("invalid --term usage");
            }
            term_only = true;
            n = parse_bounded(argv[i], 0, MAX_N, "N");
        } else if (strcmp(argv[i], "--check") == 0) {
            if (check || term_only || positional) die("invalid --check usage");
            check = true;
            n = (int)(sizeof(known) / sizeof(known[0])) - 1;
        } else if (argv[i][0] == '-') {
            usage(argv[0]);
            return EXIT_FAILURE;
        } else {
            if (positional || term_only || check) die("multiple N arguments");
            positional = true;
            n = parse_bounded(argv[i], 0, MAX_N, "N");
        }
    }

    internal_self_check();
    if (term_only) {
        require_recordable(n);
        const U128 value = sequence_term(n);
        verify_known(n, value);
        print_term(n, value);
        record_term(n, value);
        return EXIT_SUCCESS;
    }
    for (int k = 0; k <= n; ++k) {
        require_recordable(k);
        const U128 value = sequence_term(k);
        verify_known(k, value);
        print_term(k, value);
        record_term(k, value);
    }
    if (check) fprintf(stderr, "397152_01: self-check passed\n");
    return EXIT_SUCCESS;
}
