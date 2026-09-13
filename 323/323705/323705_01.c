/*
 * A323705
 *
 * Odd primes p such that ((p+1)/2)^(p-1) == 1 (mod p^2).
 * Writing (p+1)/2 = (1+p)/2 gives the faster equivalent test
 *
 *     q_p(2) = (2^(p-1)-1)/p == -1 (mod p),
 *
 * or, without forming the Fermat quotient,
 *
 *     2^(p-1) == 1-p (mod p^2).
 *
 * This program enumerates primes in an inclusive uint64_t interval with an
 * odd-only segmented sieve, then applies one modular exponentiation per
 * prime.  Segments are distributed dynamically among POSIX threads.
 *
 * Build (macOS/Homebrew or another pkg-config setup):
 *
 *   cc -O3 -march=native -std=c11 -Wall -Wextra -Wpedantic -pthread \
 *      323705_01.c -o 323705_01 $(pkg-config --cflags --libs gmp) -lm
 *
 * Usage:
 *
 *   ./323705_01 START END [THREADS [SEGMENT_ODDS]]
 *   ./323705_01 --self-test
 *
 * START and END are inclusive.  SEGMENT_ODDS is the number of odd integers
 * represented by one segment (default: 1048576).  Matches are written in
 * increasing order to stdout and appended (with OEIS indices) to
 * b323705_01.txt.  Existing terms are not duplicated.  Diagnostics and
 * progress go to stderr.
 */

#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <gmp.h>
#include <inttypes.h>
#include <limits.h>
#include <math.h>
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
/* Avoid pulling BSD-only typedefs through <sys/sysctl.h> in strict POSIX mode. */
extern int sysctlbyname(const char *, void *, size_t *, void *, size_t);
#endif

#define DEFAULT_SEGMENT_ODDS UINT64_C(1048576)
#define MAX_THREADS 1024u
#ifndef BFILE_PATH
#define BFILE_PATH "b323705_01.txt"
#endif

_Static_assert(sizeof(unsigned long) >= sizeof(uint64_t),
               "this program needs a 64-bit unsigned long for GMP");

typedef struct {
    uint32_t *values;
    size_t count;
} PrimeTable;

typedef struct {
    mpz_t base;
    mpz_t p;
    mpz_t modulus;
    mpz_t residue;
    mpz_t target;
} PowContext;

typedef struct {
    FILE *stream;
    uint64_t *terms;
    size_t count;
    size_t capacity;
    const char *path;
} BFile;

typedef struct {
    uint64_t first_odd;
    uint64_t odd_count;
    uint64_t segment_odds;
    uint64_t segment_count;
    const PrimeTable *base_primes;

    atomic_uint_fast64_t next_segment;
    atomic_uint_fast64_t completed_segments;
    atomic_uint_fast64_t primes_tested;

    pthread_mutex_t output_lock;
    pthread_cond_t output_ready;
    uint64_t next_segment_to_commit;
    double started_at;
    double last_report_at;
    bool print_matches;
    bool show_progress;
    BFile *bfile;

    uint64_t *matches;
    size_t match_count;
    size_t match_capacity;
    bool failed;
} SearchJob;

static void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static void *xmalloc(size_t size)
{
    void *p = malloc(size == 0 ? 1 : size);
    if (p == NULL) {
        die("out of memory");
    }
    return p;
}

static void *xcalloc(size_t count, size_t size)
{
    void *p = calloc(count == 0 ? 1 : count, size);
    if (p == NULL) {
        die("out of memory");
    }
    return p;
}

static void *xrealloc(void *old, size_t size)
{
    void *p = realloc(old, size == 0 ? 1 : size);
    if (p == NULL) {
        die("out of memory");
    }
    return p;
}

static double monotonic_seconds(void)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        die("clock_gettime failed");
    }
    return (double)ts.tv_sec + 1.0e-9 * (double)ts.tv_nsec;
}

static uint64_t parse_u64(const char *text, const char *name)
{
    char *end = NULL;
    unsigned long long value;

    if (text == NULL || *text == '\0' || *text == '-') {
        fprintf(stderr, "error: invalid %s: %s\n", name,
                text == NULL ? "(null)" : text);
        exit(EXIT_FAILURE);
    }
    errno = 0;
    value = strtoull(text, &end, 10);
    if (errno == ERANGE || end == text || *end != '\0' ||
        value > UINT64_MAX) {
        fprintf(stderr, "error: invalid %s: %s\n", name, text);
        exit(EXIT_FAILURE);
    }
    return (uint64_t)value;
}

static BFile open_bfile(const char *path)
{
    BFile file;
    memset(&file, 0, sizeof(file));
    file.path = path;
    file.stream = fopen(path, "a+");
    if (file.stream == NULL) {
        fprintf(stderr, "error: cannot open %s: %s\n", path,
                strerror(errno));
        exit(EXIT_FAILURE);
    }
    rewind(file.stream);

    char line[256];
    uint64_t line_number = 0;
    while (fgets(line, sizeof(line), file.stream) != NULL) {
        ++line_number;
        const char *cursor = line;
        while (*cursor == ' ' || *cursor == '\t') {
            ++cursor;
        }
        if (*cursor == '\0' || *cursor == '\n' || *cursor == '#') {
            continue;
        }

        uint64_t index;
        uint64_t term;
        char extra;
        if (sscanf(cursor, " %" SCNu64 " %" SCNu64 " %c", &index, &term,
                   &extra) != 2 ||
            index != (uint64_t)file.count + 1 ||
            (file.count != 0 && term <= file.terms[file.count - 1])) {
            fprintf(stderr,
                    "error: malformed or non-increasing %s at line %" PRIu64
                    "\n",
                    path, line_number);
            fclose(file.stream);
            free(file.terms);
            exit(EXIT_FAILURE);
        }

        if (file.count == file.capacity) {
            file.capacity = file.capacity == 0 ? 8 : 2 * file.capacity;
            file.terms = xrealloc(file.terms,
                                  file.capacity * sizeof(*file.terms));
        }
        file.terms[file.count++] = term;
    }
    if (ferror(file.stream)) {
        fprintf(stderr, "error: cannot read %s\n", path);
        fclose(file.stream);
        free(file.terms);
        exit(EXIT_FAILURE);
    }
    if (fseek(file.stream, 0, SEEK_END) != 0) {
        fprintf(stderr, "error: cannot seek to the end of %s\n", path);
        fclose(file.stream);
        free(file.terms);
        exit(EXIT_FAILURE);
    }
    return file;
}

static void close_bfile(BFile *file)
{
    if (fclose(file->stream) != 0) {
        fprintf(stderr, "error: cannot close %s\n", file->path);
    }
    free(file->terms);
    memset(file, 0, sizeof(*file));
}

static bool bfile_contains(const BFile *file, uint64_t term)
{
    size_t low = 0;
    size_t high = file->count;
    while (low < high) {
        const size_t middle = low + (high - low) / 2;
        if (file->terms[middle] < term) {
            low = middle + 1;
        } else {
            high = middle;
        }
    }
    return low < file->count && file->terms[low] == term;
}

/* Called with SearchJob.output_lock held. */
static bool append_bfile(BFile *file, uint64_t term)
{
    if (file == NULL) {
        return true;
    }
    if (file->count != 0 && term <= file->terms[file->count - 1]) {
        if (bfile_contains(file, term)) {
            return true;
        }
        fprintf(stderr,
                "error: refusing to append out-of-order term %" PRIu64
                " to %s\n",
                term, file->path);
        return false;
    }

    const uint64_t index = (uint64_t)file->count + 1;
    if (fprintf(file->stream, "%" PRIu64 " %" PRIu64 "\n", index, term) <
            0 ||
        fflush(file->stream) != 0 || fsync(fileno(file->stream)) != 0) {
        fprintf(stderr, "error: cannot append term to %s: %s\n", file->path,
                strerror(errno));
        return false;
    }

    if (file->count == file->capacity) {
        file->capacity = file->capacity == 0 ? 8 : 2 * file->capacity;
        file->terms =
            xrealloc(file->terms, file->capacity * sizeof(*file->terms));
    }
    file->terms[file->count++] = term;
    return true;
}

static unsigned default_thread_count(void)
{
#if defined(__APPLE__)
    int logical_cpus = 1;
    size_t size = sizeof(logical_cpus);
    if (sysctlbyname("hw.logicalcpu", &logical_cpus, &size, NULL, 0) != 0 ||
        logical_cpus < 1) {
        return 1;
    }
    long n = logical_cpus;
#else
    long n = sysconf(_SC_NPROCESSORS_ONLN);
#endif
    if (n < 1) {
        return 1;
    }
    if (n > (long)MAX_THREADS) {
        return MAX_THREADS;
    }
    return (unsigned)n;
}

static uint64_t isqrt_u64(uint64_t n)
{
    uint64_t r = (uint64_t)sqrtl((long double)n);

    while (r != 0 && r > n / r) {
        --r;
    }
    while (r < UINT32_MAX && r + 1 <= n / (r + 1)) {
        ++r;
    }
    return r;
}

/* All odd primes <= limit. */
static PrimeTable make_prime_table(uint64_t limit)
{
    PrimeTable table = {NULL, 0};
    if (limit < 3) {
        return table;
    }

    const uint64_t odd_count64 = (limit - 1) / 2;
    if (odd_count64 > SIZE_MAX) {
        die("base-prime sieve is too large for this machine");
    }
    const size_t odd_count = (size_t)odd_count64;
    uint8_t *composite = xcalloc(odd_count, sizeof(*composite));

    for (size_t i = 0; i < odd_count; ++i) {
        const uint64_t p = 2 * (uint64_t)i + 3;
        if (p > limit / p) {
            break;
        }
        if (composite[i] != 0) {
            continue;
        }
        const uint64_t first = (p * p - 3) / 2;
        for (uint64_t j = first; j < odd_count64; j += p) {
            composite[(size_t)j] = 1;
        }
    }

    size_t count = 0;
    for (size_t i = 0; i < odd_count; ++i) {
        count += composite[i] == 0;
    }
    table.values = xmalloc(count * sizeof(*table.values));
    table.count = count;

    size_t out = 0;
    for (size_t i = 0; i < odd_count; ++i) {
        if (composite[i] == 0) {
            table.values[out++] = (uint32_t)(2 * (uint64_t)i + 3);
        }
    }
    free(composite);
    return table;
}

static void free_prime_table(PrimeTable *table)
{
    free(table->values);
    table->values = NULL;
    table->count = 0;
}

static void pow_context_init(PowContext *ctx)
{
    mpz_init_set_ui(ctx->base, 2);
    mpz_init(ctx->p);
    mpz_init(ctx->modulus);
    mpz_init(ctx->residue);
    mpz_init(ctx->target);
}

static void pow_context_clear(PowContext *ctx)
{
    mpz_clear(ctx->base);
    mpz_clear(ctx->p);
    mpz_clear(ctx->modulus);
    mpz_clear(ctx->residue);
    mpz_clear(ctx->target);
}

#if defined(__SIZEOF_INT128__)
static uint64_t mul_mod_u64(uint64_t a, uint64_t b, uint64_t modulus)
{
    return (uint64_t)(((__uint128_t)a * b) % modulus);
}

static uint64_t pow_mod_u64(uint64_t base, uint64_t exponent,
                            uint64_t modulus)
{
    uint64_t result = 1;
    while (exponent != 0) {
        if ((exponent & 1) != 0) {
            result = mul_mod_u64(result, base, modulus);
        }
        exponent >>= 1;
        if (exponent != 0) {
            base = mul_mod_u64(base, base, modulus);
        }
    }
    return result;
}
#endif

static bool is_a323705(uint64_t p, PowContext *ctx)
{
#if defined(__SIZEOF_INT128__)
    if (p <= UINT32_MAX) {
        const uint64_t modulus = p * p;
        return pow_mod_u64(2, p - 1, modulus) == modulus - p + 1;
    }
#endif

    mpz_set_ui(ctx->p, (unsigned long)p);
    mpz_mul(ctx->modulus, ctx->p, ctx->p);
    mpz_powm_ui(ctx->residue, ctx->base, (unsigned long)(p - 1),
                ctx->modulus);
    mpz_sub(ctx->target, ctx->modulus, ctx->p);
    mpz_add_ui(ctx->target, ctx->target, 1);
    return mpz_cmp(ctx->residue, ctx->target) == 0;
}

static void sieve_segment(uint8_t *composite, uint64_t low, uint64_t count,
                          const PrimeTable *base_primes)
{
    memset(composite, 0, (size_t)count);
    const uint64_t high = low + 2 * (count - 1);

    for (size_t i = 0; i < base_primes->count; ++i) {
        const uint64_t p = base_primes->values[i];
        if (p > high / p) {
            break;
        }

        uint64_t first;
        const uint64_t p2 = p * p;
        if (p2 >= low) {
            first = p2;
        } else {
            const uint64_t remainder = low % p;
            const uint64_t delta = remainder == 0 ? 0 : p - remainder;
            if (delta > UINT64_MAX - low) {
                continue;
            }
            first = low + delta;
            if ((first & 1) == 0) {
                if (p > UINT64_MAX - first) {
                    continue;
                }
                first += p;
            }
        }

        if (first > high) {
            continue;
        }
        uint64_t j = (first - low) / 2;
        while (j < count) {
            composite[(size_t)j] = 1;
            if (p > UINT64_MAX - j) {
                break;
            }
            j += p;
        }
    }
}

/* Called with output_lock held. */
static void record_match_locked(SearchJob *job, uint64_t p)
{
    if (job->match_count == job->match_capacity) {
        job->match_capacity =
            job->match_capacity == 0 ? 8 : 2 * job->match_capacity;
        job->matches = xrealloc(
            job->matches, job->match_capacity * sizeof(*job->matches));
    }
    job->matches[job->match_count++] = p;

    if (job->print_matches) {
        printf("%" PRIu64 "\n", p);
        fflush(stdout);
    }
    if (!append_bfile(job->bfile, p)) {
        job->failed = true;
    }
}

static void commit_segment(SearchJob *job, uint64_t segment,
                           const uint64_t *matches, size_t match_count)
{
    pthread_mutex_lock(&job->output_lock);
    while (!job->failed && segment != job->next_segment_to_commit) {
        pthread_cond_wait(&job->output_ready, &job->output_lock);
    }
    if (!job->failed) {
        for (size_t i = 0; i < match_count; ++i) {
            record_match_locked(job, matches[i]);
            if (job->failed) {
                break;
            }
        }
        ++job->next_segment_to_commit;
    }
    pthread_cond_broadcast(&job->output_ready);
    pthread_mutex_unlock(&job->output_lock);
}

static void maybe_report_progress(SearchJob *job, uint64_t completed)
{
    if (!job->show_progress) {
        return;
    }

    const double now = monotonic_seconds();
    pthread_mutex_lock(&job->output_lock);
    if (completed == job->segment_count || now - job->last_report_at >= 5.0) {
        job->last_report_at = now;
        const uint64_t tested = atomic_load_explicit(
            &job->primes_tested, memory_order_relaxed);
        const double elapsed = now - job->started_at;
        const double percent = job->segment_count == 0
                                   ? 100.0
                                   : 100.0 * (double)completed /
                                         (double)job->segment_count;
        fprintf(stderr,
                "%6.2f%%  segments=%" PRIu64 "/%" PRIu64
                "  primes=%" PRIu64 "  elapsed=%.1fs\n",
                percent, completed, job->segment_count, tested, elapsed);
        fflush(stderr);
    }
    pthread_mutex_unlock(&job->output_lock);
}

static void *search_worker(void *argument)
{
    SearchJob *job = argument;
    uint8_t *composite = xmalloc((size_t)job->segment_odds);
    uint64_t *segment_matches = NULL;
    size_t segment_match_capacity = 0;
    PowContext pow_ctx;
    pow_context_init(&pow_ctx);

    for (;;) {
        const uint64_t segment = atomic_fetch_add_explicit(
            &job->next_segment, 1, memory_order_relaxed);
        if (segment >= job->segment_count) {
            break;
        }

        const uint64_t offset = segment * job->segment_odds;
        const uint64_t remaining = job->odd_count - offset;
        const uint64_t count = remaining < job->segment_odds
                                   ? remaining
                                   : job->segment_odds;
        const uint64_t low = job->first_odd + 2 * offset;

        sieve_segment(composite, low, count, job->base_primes);

        uint64_t local_prime_count = 0;
        size_t segment_match_count = 0;
        for (uint64_t i = 0; i < count; ++i) {
            if (composite[(size_t)i] != 0) {
                continue;
            }
            const uint64_t p = low + 2 * i;
            ++local_prime_count;
            if (is_a323705(p, &pow_ctx)) {
                if (segment_match_count == segment_match_capacity) {
                    segment_match_capacity = segment_match_capacity == 0
                                                 ? 2
                                                 : 2 * segment_match_capacity;
                    segment_matches = xrealloc(
                        segment_matches,
                        segment_match_capacity * sizeof(*segment_matches));
                }
                segment_matches[segment_match_count++] = p;
            }
        }
        atomic_fetch_add_explicit(&job->primes_tested, local_prime_count,
                                  memory_order_relaxed);
        commit_segment(job, segment, segment_matches, segment_match_count);
        const uint64_t completed = atomic_fetch_add_explicit(
                                       &job->completed_segments, 1,
                                       memory_order_relaxed) +
                                   1;
        maybe_report_progress(job, completed);
    }

    pow_context_clear(&pow_ctx);
    free(segment_matches);
    free(composite);
    return NULL;
}

static int compare_u64(const void *left, const void *right)
{
    const uint64_t a = *(const uint64_t *)left;
    const uint64_t b = *(const uint64_t *)right;
    return (a > b) - (a < b);
}

static uint64_t *search_interval(uint64_t start, uint64_t end,
                                 unsigned threads, uint64_t segment_odds,
                                 bool print_matches, bool show_progress,
                                 BFile *bfile,
                                 size_t *match_count_out,
                                 uint64_t *primes_tested_out)
{
    *match_count_out = 0;
    *primes_tested_out = 0;
    if (start > end || end < 3) {
        return NULL;
    }
    if (start < 3) {
        start = 3;
    }
    if ((start & 1) == 0) {
        ++start;
    }
    if ((end & 1) == 0) {
        --end;
    }
    if (start > end) {
        return NULL;
    }

    const uint64_t odd_count = (end - start) / 2 + 1;
    const uint64_t segment_count =
        odd_count / segment_odds + (odd_count % segment_odds != 0);
    if (threads > segment_count) {
        threads = (unsigned)segment_count;
    }
    if (threads == 0) {
        threads = 1;
    }

    const uint64_t root = isqrt_u64(end);
    const double sieve_started = monotonic_seconds();
    PrimeTable base_primes = make_prime_table(root);
    if (show_progress) {
        fprintf(stderr,
                "range=[%" PRIu64 ", %" PRIu64 "]  threads=%u"
                "  segment_odds=%" PRIu64 "\n",
                start, end, threads, segment_odds);
        fprintf(stderr,
                "base sieve: %zu odd primes <= %" PRIu64 " (%.3fs)\n",
                base_primes.count, root,
                monotonic_seconds() - sieve_started);
    }

    SearchJob job;
    memset(&job, 0, sizeof(job));
    job.first_odd = start;
    job.odd_count = odd_count;
    job.segment_odds = segment_odds;
    job.segment_count = segment_count;
    job.base_primes = &base_primes;
    job.print_matches = print_matches;
    job.show_progress = show_progress;
    job.bfile = bfile;
    job.started_at = monotonic_seconds();
    job.last_report_at = job.started_at;
    atomic_init(&job.next_segment, 0);
    atomic_init(&job.completed_segments, 0);
    atomic_init(&job.primes_tested, 0);
    if (pthread_mutex_init(&job.output_lock, NULL) != 0) {
        die("pthread_mutex_init failed");
    }
    if (pthread_cond_init(&job.output_ready, NULL) != 0) {
        die("pthread_cond_init failed");
    }

    pthread_t *thread_ids = xmalloc(threads * sizeof(*thread_ids));
    unsigned created = 0;
    for (; created < threads; ++created) {
        if (pthread_create(&thread_ids[created], NULL, search_worker, &job) !=
            0) {
            job.failed = true;
            break;
        }
    }
    for (unsigned i = 0; i < created; ++i) {
        if (pthread_join(thread_ids[i], NULL) != 0) {
            job.failed = true;
        }
    }
    free(thread_ids);

    qsort(job.matches, job.match_count, sizeof(*job.matches), compare_u64);
    *match_count_out = job.match_count;
    *primes_tested_out = atomic_load_explicit(&job.primes_tested,
                                               memory_order_relaxed);

    pthread_cond_destroy(&job.output_ready);
    pthread_mutex_destroy(&job.output_lock);
    free_prime_table(&base_primes);
    if (job.failed) {
        free(job.matches);
        die("parallel search failed");
    }
    return job.matches;
}

static int self_test(void)
{
    static const uint64_t known[] = {
        UINT64_C(1897121), UINT64_C(52368101), UINT64_C(126233057),
    };
    static const uint64_t nonmembers[] = {
        UINT64_C(3), UINT64_C(29), UINT64_C(37), UINT64_C(3373),
        UINT64_C(2001907169), UINT64_C(4294967311),
    };

    PowContext ctx;
    pow_context_init(&ctx);
    for (size_t i = 0; i < sizeof(known) / sizeof(known[0]); ++i) {
        if (!is_a323705(known[i], &ctx)) {
            fprintf(stderr, "self-test failed: known term %" PRIu64 "\n",
                    known[i]);
            pow_context_clear(&ctx);
            return EXIT_FAILURE;
        }
    }
    for (size_t i = 0; i < sizeof(nonmembers) / sizeof(nonmembers[0]); ++i) {
        if (is_a323705(nonmembers[i], &ctx)) {
            fprintf(stderr, "self-test failed: nonmember %" PRIu64 "\n",
                    nonmembers[i]);
            pow_context_clear(&ctx);
            return EXIT_FAILURE;
        }
    }
    pow_context_clear(&ctx);

    size_t count = 0;
    uint64_t tested = 0;
    uint64_t *found = search_interval(3, 2000000, 2, 8192, false, false,
                                      NULL, &count, &tested);
    static const uint64_t expected_small[] = {
        UINT64_C(1897121),
    };
    const size_t expected_count =
        sizeof(expected_small) / sizeof(expected_small[0]);
    bool ok = count == expected_count;
    for (size_t i = 0; ok && i < expected_count; ++i) {
        ok = found[i] == expected_small[i];
    }
    free(found);

    if (!ok || tested != 148932) {
        fprintf(stderr,
                "self-test failed: segmented search found %zu matches and "
                "tested %" PRIu64 " primes\n",
                count, tested);
        return EXIT_FAILURE;
    }

    fprintf(stderr,
            "self-test passed (three known terms; sieve through 2000000)\n");
    return EXIT_SUCCESS;
}

static void print_usage(const char *program)
{
    fprintf(stderr,
            "usage: %s START END [THREADS [SEGMENT_ODDS]]\n"
            "       %s --self-test\n",
            program, program);
}

int main(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "--self-test") == 0) {
        return self_test();
    }
    if (argc < 3 || argc > 5) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    const uint64_t start = parse_u64(argv[1], "START");
    const uint64_t end = parse_u64(argv[2], "END");
    if (start > end) {
        die("START must not exceed END");
    }

    unsigned threads = default_thread_count();
    if (argc >= 4) {
        const uint64_t parsed = parse_u64(argv[3], "THREADS");
        if (parsed == 0 || parsed > MAX_THREADS) {
            die("THREADS must be in 1..1024");
        }
        threads = (unsigned)parsed;
    }

    uint64_t segment_odds = DEFAULT_SEGMENT_ODDS;
    if (argc >= 5) {
        segment_odds = parse_u64(argv[4], "SEGMENT_ODDS");
        if (segment_odds == 0 || segment_odds > SIZE_MAX) {
            die("SEGMENT_ODDS is too large for this machine");
        }
    }

    const double started = monotonic_seconds();
    size_t match_count = 0;
    uint64_t primes_tested = 0;
    BFile bfile = open_bfile(BFILE_PATH);
    fprintf(stderr, "b-file: %s (%zu existing terms)\n", BFILE_PATH,
            bfile.count);
    uint64_t *matches = search_interval(start, end, threads, segment_odds,
                                        true, true, &bfile, &match_count,
                                        &primes_tested);
    free(matches);
    close_bfile(&bfile);

    fprintf(stderr,
            "done: tested %" PRIu64 " primes, found %zu match%s in %.3fs\n",
            primes_tested, match_count, match_count == 1 ? "" : "es",
            monotonic_seconds() - started);
    return EXIT_SUCCESS;
}
