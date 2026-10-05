/*
 * 399386_01.c: numbers k such that k and k*k use only digits {5,6,7,9}.
 * Digits need not all occur.  Search is exact and independent of OEIS data.
 *
 * Build/run from the directory containing this file:
 *   cc -O3 -march=native -std=c11 -Wall -Wextra -Wpedantic -pthread \
 *      399386_01.c -o 399386_01
 *   ./399386_01                         # smallest match with 1..40 k digits
 *   ./399386_01 40 --all --threads 8     # all matches with 1..40 k digits
 *   ./399386_01 --min-digits 41 --max-digits 44 --all --threads 8
 *   ./399386_01 --check 567
 *   ./399386_01 --self-test
 *
 * The inclusive bounds refer to k, not k*k.  Maximum: k=256 digits,
 * square=512 digits.  All lengths share one suffix search tree.  Default
 * mode exhausts the whole interval, then prints its smallest match; --all
 * prints all matches in numeric order.  There is no 100-term cutoff.
 * Matches are written as k only, one per line, to stdout.  Candidates and
 * progress are on stderr.  On SIGINT/SIGTERM, report INCOMPLETE, return 130,
 * and do not print an unconfirmed sorted result.  Candidate messages are
 * valid matches but do not prove minimality/completeness.  No checkpoints.
 * Exit 0 means completed, including when there are no matches; 1 is error.
 * Large bounds may take substantial time; existence of another term is
 * not assumed.  Known terms and b-files are not used to prune the search.
 *
 * Proof of enumeration/pruning:
 * At depth n, a[0..n-1] are allowed low digits of k.  Uncarried square
 * coefficients are c[j]=sum(a[u]*a[v],u+v=j).  Appending d changes
 * c[n+u] by 2*d*a[u] for u<n and c[2*n] by d*d.  The next square digit
 * is (c[n]+carry+2*d*a[0]) mod 10 for n>=1, and d*d mod 10 for n=0.
 * A lookup table rejects forbidden next digits before coefficient updates.
 * Later k digits cannot alter already checked square digits, so rejection
 * loses no valid completion.  At every requested length, processing the
 * remaining coefficients and carries checks the full square.  The root
 * checks short terms through the split depth once; workers check each
 * deeper term once and skip rechecking their initial split prefix.  Thus
 * all requested lengths are covered with neither duplicate nor missing
 * terms.  Sorting by length then lexicographic order is numeric sorting.
 * c[j]<=81*N and carried columns<=90*N; uint32_t is ample for N<=256.
 * Diagnostic counters saturate at UINT64_MAX.  --self-test independently
 * enumerates all allowed words through 9 digits using uint64_t squaring,
 * comparing every match and suffix count for 1 and 4 threads.  It tests
 * single lengths, combined lengths, and lower digit bounds across the split.
 */

#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <inttypes.h>
#include <pthread.h>
#include <signal.h>
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

#define MAX_DIGITS 256U
#define SPLIT_DIGITS 8U
#define MAX_THREADS 256U
static unsigned alphabet[4];
static bool allowed[10];
static char set_name[5];
static unsigned char extensions[10][10];
static unsigned char unit_extensions;
static const unsigned char first_bit[16] = {0,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0};
static volatile sig_atomic_t interrupted;

typedef struct { unsigned char a[SPLIT_DIGITS]; } Task;
typedef struct {
    unsigned length, split;
    Task *tasks;
    size_t task_count, task_capacity, next_task, done;
    char **matches;
    size_t count, capacity;
    uint64_t leaves, nodes;
    bool quiet, all_lengths;
    unsigned min_length;
    pthread_mutex_t lock;
} Search;
typedef struct {
    Search *search;
    unsigned char a[MAX_DIGITS];
    uint32_t c[2 * MAX_DIGITS];
    uint64_t leaves, nodes;
} Worker;

static void die(const char *s)
{
    fprintf(stderr, "error: %s\n", s);
    exit(1);
}
static void *resize(void *p, size_t n)
{
    void *q = realloc(p, n);
    if (!q) die("out of memory");
    return q;
}
static bool good(unsigned d) { return d < 10 && allowed[d]; }
static void set_digits(unsigned a, unsigned b, unsigned c, unsigned d)
{
    unsigned v[4] = {a,b,c,d};
    memset(allowed, 0, sizeof(allowed));
    for (unsigned i = 0; i < 4; ++i) {
        alphabet[i] = v[i];
        allowed[v[i]] = true;
        set_name[i] = (char)('0' + v[i]);
    }
    set_name[4] = '\0';
    unit_extensions = 0;
    memset(extensions, 0, sizeof(extensions));
    for (unsigned j=0; j<4; ++j) {
        unsigned digit = alphabet[j];
        if (good(digit*digit % 10)) unit_extensions |= (unsigned char)(1U << j);
        for (unsigned unit=0; unit<10; ++unit)
        for (unsigned residue=0; residue<10; ++residue)
            if (good((residue + 2*unit*digit) % 10))
                extensions[unit][residue] |= (unsigned char)(1U << j);
    }
}
static uint64_t add_count(uint64_t a, uint64_t b)
{
    return UINT64_MAX - a < b ? UINT64_MAX : a + b;
}
static void on_signal(int sig) { (void)sig; interrupted = 1; }
static double seconds(void)
{
    struct timespec t;
    if (clock_gettime(CLOCK_MONOTONIC, &t)) die("clock_gettime failed");
    return (double)t.tv_sec + (double)t.tv_nsec * 1e-9;
}
static unsigned number(const char *s, unsigned max)
{
    if (!*s) die("empty numeric argument");
    for (const char *p = s; *p; ++p)
        if (*p < '0' || *p > '9') die("expected a positive decimal integer");
    char *end;
    errno = 0;
    unsigned long v = strtoul(s, &end, 10);
    if (errno || *end || v < 1 || v > max) die("numeric argument out of range");
    return (unsigned)v;
}
static void change(Worker *w, unsigned n, unsigned d, bool add)
{
    for (unsigned i = 0; i < n; ++i) {
        uint32_t v = 2U * d * w->a[i];
        if (add) w->c[n+i] += v;
        else w->c[n+i] -= v;
    }
    if (add) w->c[2*n] += d*d;
    else w->c[2*n] -= d*d;
}
static bool upper_good(const Worker *w, unsigned n, uint32_t carry)
{
    for (unsigned i = n; i < 2*n-1; ++i) {
        uint32_t t = w->c[i] + carry;
        if (!good(t % 10)) return false;
        carry = t / 10;
    }
    while (carry) {
        if (!good(carry % 10)) return false;
        carry /= 10;
    }
    return true;
}
static void record(Worker *w, unsigned n)
{
    Search *s = w->search;
    char *value = resize(NULL, n + 1U);
    for (unsigned i = 0; i < n; ++i)
        value[i] = (char)('0' + w->a[n - 1U - i]);
    value[n] = '\0';
    pthread_mutex_lock(&s->lock);
    if (s->count == s->capacity) {
        if (s->capacity > SIZE_MAX / (2 * sizeof(*s->matches)))
            die("too many matches to store");
        s->capacity = s->capacity ? 2*s->capacity : 16;
        s->matches = resize(s->matches, s->capacity * sizeof(*s->matches));
    }
    s->matches[s->count++] = value;
    if (!s->quiet) fprintf(stderr, "candidate: %s (interval still incomplete)\n", value);
    pthread_mutex_unlock(&s->lock);
}
static void dfs(Worker *w, unsigned n, uint32_t carry, bool tasks, bool examine)
{
    Search *s = w->search;
    if (interrupted) return;
    w->nodes = add_count(w->nodes, 1);
    /* The root enumerates short terms once, including each split prefix.
       Workers start with examine=false so that split terms are not duplicated. */
    if (examine && n != 0) {
        if (n == s->length) w->leaves = add_count(w->leaves, 1);
        if (n >= s->min_length && (s->all_lengths || n == s->length) && upper_good(w, n, carry)) record(w, n);
    }
    if (tasks && n == s->split) {
        if (s->task_count == s->task_capacity) {
            s->task_capacity = s->task_capacity ? 2*s->task_capacity : 128;
            s->tasks = resize(s->tasks, s->task_capacity * sizeof(*s->tasks));
        }
        memcpy(s->tasks[s->task_count++].a, w->a, n);
        return;
    }
    if (n == s->length) return;
    uint32_t base = w->c[n] + carry;
    unsigned mask = n ? extensions[w->a[0]][base % 10] : unit_extensions;
    while (mask) {
        unsigned j = first_bit[mask];
        mask &= mask-1;
        unsigned d = alphabet[j];
        uint32_t t = base + (n ? 2U*d*w->a[0] : d*d);
        w->a[n] = (unsigned char)d;
        change(w, n, d, true);
        dfs(w, n+1, t/10, tasks, true);
        change(w, n, d, false);
    }
}
static void *work(void *arg)
{
    Search *s = arg;
    Worker w = {.search = s};
    for (;;) {
        pthread_mutex_lock(&s->lock);
        size_t task = s->next_task++;
        pthread_mutex_unlock(&s->lock);
        if (interrupted || task >= s->task_count) break;
        memset(w.c, 0, sizeof(w.c));
        uint32_t carry = 0;
        for (unsigned n = 0; n < s->split; ++n) {
            unsigned d = s->tasks[task].a[n];
            w.a[n] = (unsigned char)d;
            change(&w, n, d, true);
            carry = (w.c[n] + carry) / 10;
        }
        dfs(&w, s->split, carry, false, false);
        pthread_mutex_lock(&s->lock);
        if (!interrupted) ++s->done;
        if (!s->quiet && (s->done % 16 == 0 || s->done == s->task_count))
            fprintf(stderr, "%s up to %u digits: %zu/%zu suffix tasks complete\n",
                    set_name, s->length, s->done, s->task_count);
        pthread_mutex_unlock(&s->lock);
    }
    pthread_mutex_lock(&s->lock);
    s->leaves = add_count(s->leaves, w.leaves);
    s->nodes = add_count(s->nodes, w.nodes);
    pthread_mutex_unlock(&s->lock);
    return NULL;
}
static int compare(const void *a, const void *b)
{
    const char *x = *(char *const *)a, *y = *(char *const *)b;
    size_t nx = strlen(x), ny = strlen(y);
    return nx < ny ? -1 : (nx > ny ? 1 : strcmp(x,y));
}
static void run(Search *s, unsigned n, unsigned threads, bool quiet, unsigned min_length, bool all_lengths)
{
    memset(s, 0, sizeof(*s));
    s->length = n;
    s->split = n < SPLIT_DIGITS ? n : SPLIT_DIGITS;
    s->quiet = quiet;
    s->all_lengths = all_lengths;
    s->min_length = min_length;
    if (pthread_mutex_init(&s->lock, NULL)) die("mutex initialization failed");
    Worker root = {.search = s};
    dfs(&root, 0, 0, true, true);
    s->nodes = root.nodes;
    s->leaves = root.leaves;
    pthread_t ids[MAX_THREADS];
    if (s->task_count < threads) threads = (unsigned)s->task_count;
    for (unsigned i = 0; i < threads; ++i)
        if (pthread_create(&ids[i], NULL, work, s)) die("pthread_create failed");
    for (unsigned i = 0; i < threads; ++i)
        if (pthread_join(ids[i], NULL)) die("pthread_join failed");
    if (s->count) qsort(s->matches, s->count, sizeof(*s->matches), compare);
}
static void release(Search *s)
{
    for (size_t i = 0; i < s->count; ++i) free(s->matches[i]);
    free(s->matches);
    free(s->tasks);
    pthread_mutex_destroy(&s->lock);
}
static bool integer_good(uint64_t k)
{
    do {
        if (!good((unsigned)(k % 10))) return false;
        k /= 10;
    } while (k);
    return true;
}
static void oracle(unsigned left, uint64_t k, uint64_t modulus,
                   uint64_t *leaves, char values[1024][32], size_t *count)
{
    if (left) {
        for (unsigned j = 0; j < 4; ++j) {
            if (k == 0 && alphabet[j] == 0) continue;
            oracle(left-1, 10*k + alphabet[j], modulus, leaves, values, count);
        }
        return;
    }
    uint64_t square = k*k;
    uint64_t suffix = square % modulus;
    bool ok = true;
    for (uint64_t p = modulus; p > 1; p /= 10) {
        if (!good((unsigned)(suffix % 10))) ok = false;
        suffix /= 10;
    }
    if (ok) ++*leaves;
    if (integer_good(square)) {
        if (*count >= 1024) die("self-test oracle capacity exceeded");
        snprintf(values[(*count)++], 32, "%" PRIu64, k);
    }
}
static void self_test(void)
{
    uint64_t modulus = 1;
    char cumulative[4096][32];
    size_t total = 0;
    for (unsigned n=1; n<=9; ++n) {
        modulus *= 10;
        char values[1024][32];
        size_t count = 0;
        uint64_t leaves = 0;
        oracle(n, 0, modulus, &leaves, values, &count);
        if (total+count>4096) die("self-test capacity exceeded");
        for (size_t i=0; i<count; ++i) strcpy(cumulative[total++], values[i]);
        for (unsigned threads=1; threads<=4; threads*=4) {
            Search s;
            run(&s, n, threads, true, 1, false);
            if (s.leaves!=leaves || s.count!=count) die("self-test count mismatch");
            for (size_t i=0; i<count; ++i)
                if (strcmp(s.matches[i],values[i])) die("self-test match mismatch");
            release(&s);
            for (unsigned first=1; first<=n; ++first) {
                run(&s, n, threads, true, first, true);
                size_t i=0;
                for (size_t j=0; j<total; ++j) {
                    if (strlen(cumulative[j])<first) continue;
                    if (i>=s.count || strcmp(s.matches[i++], cumulative[j]))
                        die("self-test interval match mismatch");
                }
                if (i!=s.count) die("self-test interval count mismatch");
                release(&s);
            }
        }
    }
    fprintf(stderr,"self-test passed: exhaustive 1..9 digits, 1/4 threads, all digit intervals\n");
}
static int check(const char *k)
{
    size_t n = strlen(k);
    if (!n || n > MAX_DIGITS || k[0] == '0') die("check expects 1..256 digits, no leading zero");
    uint32_t c[2*MAX_DIGITS] = {0};
    bool ok = true;
    for (size_t i = 0; i < n; ++i) {
        if (k[i] < '0' || k[i] > '9') die("check expects decimal digits");
        if (!good((unsigned)(k[i] - '0'))) ok = false;
    }
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            c[i+j] += (uint32_t)(k[n-1-i]-'0') * (uint32_t)(k[n-1-j]-'0');
    for (size_t i = 0; i < 2*n-1; ++i) {
        c[i+1] += c[i]/10;
        c[i] %= 10;
    }
    size_t len = 2*n;
    while (len > 1 && c[len-1] == 0) --len;
    fprintf(stderr, "%s^2 = ", k);
    for (size_t i = len; i > 0; --i) {
        fprintf(stderr, "%u", c[i-1]);
        if (!good(c[i-1])) ok = false;
    }
    fprintf(stderr, "\n%s\n", ok ? "match" : "not a match");
    if (ok && (puts(k) == EOF || fflush(stdout))) die("stdout write failed");
    return 0;
}
static void usage(void)
{
    fprintf(stderr,"Usage: ./399386_01 [N] [--min-digits N] [--max-digits N] [--threads T] [--all]\n"
                   "       ./399386_01 --check K | --self-test | --help\n"
                   "Defaults: k digits 1..40; maximum=256; no term-count cutoff.\n");
}
int main(int argc, char **argv)
{
    set_digits(5,6,7,9);
    unsigned first=1, last=40;
    bool all=false, bound_given=false;
    long cpus=1;
#if defined(__APPLE__)
    int cpu_count=1;
    size_t cpu_size=sizeof(cpu_count);
    if (sysctlbyname("hw.logicalcpu", &cpu_count, &cpu_size, NULL, 0)==0) cpus=cpu_count;
#elif defined(_SC_NPROCESSORS_ONLN)
    cpus=sysconf(_SC_NPROCESSORS_ONLN);
#endif
    unsigned threads=cpus>0 && cpus<=MAX_THREADS ? (unsigned)cpus : 1;
    if (argc==2 && !strcmp(argv[1],"--help")) { usage(); return 0; }
    if (argc==2 && !strcmp(argv[1],"--self-test")) { self_test(); return 0; }
    if (argc==3 && !strcmp(argv[1],"--check")) return check(argv[2]);
    for (int i=1; i<argc; ++i) {
        if (!strcmp(argv[i],"--all")) { all=true; continue; }
        if (argv[i][0]!='-') {
            if (bound_given) die("maximum digit bound specified twice");
            last=number(argv[i],MAX_DIGITS); bound_given=true;
            continue;
        }
        if (i+1>=argc) die("missing option value");
        if (!strcmp(argv[i],"--threads")) threads=number(argv[++i],MAX_THREADS);
        else if (!strcmp(argv[i],"--min-digits")) first=number(argv[++i],MAX_DIGITS);
        else if (!strcmp(argv[i],"--max-digits")) {
            if (bound_given) die("maximum digit bound specified twice");
            last=number(argv[++i],MAX_DIGITS); bound_given=true;
        } else { usage(); die("unknown option"); }
    }
    if (first>last) die("min-digits exceeds max-digits");
    if (signal(SIGINT,on_signal)==SIG_ERR || signal(SIGTERM,on_signal)==SIG_ERR)
        die("signal handler installation failed");
    fprintf(stderr,"search: k digits %u..%u inclusive, %u threads\n",first,last,threads);
    double started=seconds();
    Search s;
    run(&s,last,threads,false,first,true);
    if (interrupted) {
        fprintf(stderr,"requested %u..%u digit interval INCOMPLETE; %zu candidates; restart interval\n",first,last,s.count);
        release(&s); return 130;
    }
    size_t emit=all ? s.count : (s.count ? 1 : 0);
    for (size_t i=0; i<emit; ++i)
        if (puts(s.matches[i])==EOF || fflush(stdout)) die("stdout write failed");
    fprintf(stderr,"requested %u..%u digit interval COMPLETE: matches=%zu, nodes=%" PRIu64 ", %.3f s\n",
            first,last,s.count,s.nodes,seconds()-started);
    release(&s);
    return 0;
}
