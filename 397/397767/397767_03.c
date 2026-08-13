/*
 * A397767 -- exact prime-sum-class sparse DP with meet in the middle.
 *
 * Count partitions of {1,...,4*n} into unordered four-element subsets which
 * can be split into two pairs having the same prime sum.  If a<b<c<d, then
 *
 *     a+b < c+d,                 a+c < b+d,
 *
 * so the only possible equal-sum pairing is {a,d},{b,c}.  In particular its
 * common sum is unique.
 *
 * For a prime p, put
 *
 *     E_p = {{i,p-i}: 1 <= i < p-i <= 4*n}.
 *
 * The edges of E_p are mutually disjoint.  Choosing 2*r of these edges and
 * grouping them into r unordered pairs of edges gives (2*r-1)!! valid blocks
 * of common sum p.  Hence the mask polynomial
 *
 *   F_p = sum_{T subset E_p, |T| even} (|T|-1)!! X_{V(T)},
 *
 * with the empty term assigned weight 1, describes exactly all blocks whose
 * common sum is p.  Mask multiplication is allowed only for disjoint masks.
 *
 * The prime classes are split deterministically into two balanced groups L
 * and R.  A sparse DP forms
 *
 *     F_L = product_{p in L} F_p,       F_R = product_{p in R} F_p.
 *
 * If C_L(M) and C_R(M) are their mask coefficients and U is the full mask,
 * the required answer is exactly
 *
 *     sum_M C_L(M) C_R(U xor M).
 *
 * Indeed, every final partition has a unique common prime for every block,
 * so it has one unique decomposition between L and R.  Conversely every
 * complementary pair of DP states is a valid partition.  This proves both
 * completeness and absence of overcounting.  The split changes only running
 * time, not the value.
 *
 * The open-addressing tables compare the complete 64-bit mask.  A collision
 * only lengthens probing; no state is discarded, approximated, or evicted.
 * Thus this is a deterministic exact algorithm, independent of hash luck,
 * thread count, and the memory addresses chosen by the system.  Reflection
 * symmetry is deliberately not used, because i -> 4*n+1-i changes a common
 * sum and does not preserve primality.
 *
 * MAX_N=12 uses 48 mask bits.  A coefficient on a 4*k-element mask is at most
 * the unrestricted number (4*k)!/(24^k*k!) of four-set partitions.  This is
 * increasing here, and its n=12 value has 120 bits, so U128 suffices.  Every
 * DP addition, multiplication, and final join is nevertheless checked.  Hash
 * allocations and subset tables obey --memory-limit-mib and fail cleanly
 * before exceeding it.  A failed or interrupted run never records a term.
 *
 * Build:
 *   clang -O3 -march=native -std=c11 -Wall -Wextra -Wpedantic -pthread \
 *       397767_03.c -o 397767_03
 *
 * OEIS offset is 1.  A positional N computes a(1),...,a(N).  --term computes
 * only a(N), and needs an existing contiguous b-file prefix unless
 * --no-bfile is supplied.  Completed terms are atomically recorded in
 * b397767_03.txt.
 *
 *   ./397767_03 8 --threads 8
 *   ./397767_03 --term 9 --threads 8 --memory-limit-mib 8192 --no-bfile
 *   ./397767_03 --check --threads 8 --no-bfile
 *
 * L and R are built concurrently when at least two threads are requested.
 * The complement join uses up to --threads workers.  DP memory in the status
 * messages includes both hash maps and temporary subset tables.
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
#error "397767_03.c requires unsigned __int128"
#endif
__extension__ typedef unsigned __int128 U128;

#define MAX_N 12
#define MAX_VALUES (4 * MAX_N)
#define MAX_CLASSES 64
#define MIN_N 1
#define MAX_THREADS 64
#define DEFAULT_N 8
#define DEFAULT_THREADS 4
#define DEFAULT_MEMORY_MIB 4096
#define MIN_MEMORY_MIB 64
#define MAX_MEMORY_MIB 32768
#define JOIN_CHUNK 65536U

_Static_assert(MAX_VALUES < 64, "used-element mask must fit in uint64_t");
_Static_assert(2 * MAX_N < 32, "edge-subset mask must fit in uint32_t");

typedef struct {
    uint64_t *keys;
    U128 *values;
    size_t capacity;
    size_t count;
    size_t grow_at;
} StateMap;

typedef struct {
    unsigned sum;
    unsigned edge_count;
    uint64_t edges[2 * MAX_N];
} PrimeClass;

typedef struct {
    int n;
    const char *name;
    const PrimeClass **classes;
    unsigned class_count;
    StateMap result;
    U128 transitions;
    double started;
} BuildArgument;

typedef struct {
    const StateMap *scan;
    const StateMap *lookup;
    uint64_t full;
    _Atomic size_t next_slot;
    _Atomic size_t processed_slots;
} JoinSchedule;

typedef struct {
    JoinSchedule *schedule;
    U128 answer;
    size_t lookups;
    size_t matches;
} JoinWorker;

typedef struct {
    int n;
    const JoinSchedule *schedule;
    double started;
} JoinMonitor;

static const char *output_path = "b397767_03.txt";
static bool write_bfile = true;
static int requested_threads = DEFAULT_THREADS;
static size_t memory_limit;
static size_t memory_used;
static size_t peak_memory;

static pthread_mutex_t memory_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
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

static size_t parse_memory_mib(const char *text)
{
    char *end = NULL;
    errno = 0;
    const unsigned long value = strtoul(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' ||
        value < MIN_MEMORY_MIB || value > MAX_MEMORY_MIB) {
        fprintf(stderr, "error: memory-limit-mib must be in %d..%d: %s\n",
                MIN_MEMORY_MIB, MAX_MEMORY_MIB, text);
        exit(EXIT_FAILURE);
    }
    return (size_t)value;
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

static bool multiply_u128(U128 a, U128 b, U128 *product)
{
    const U128 maximum = ~(U128)0;
    if (a != 0 && b > maximum / a) return false;
    *product = a * b;
    return true;
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

/* Read a strict contiguous prefix beginning at the OEIS offset.  No built-in
   value is inserted: only a successfully completed computation is recorded. */
static int read_bfile(char values[MAX_N + 1][40])
{
    FILE *input = fopen(output_path, "r");
    if (input == NULL) {
        if (errno == ENOENT) return MIN_N;
        die("cannot read b-file");
    }
    char line[256];
    int next = MIN_N;
    while (fgets(line, sizeof(line), input) != NULL) {
        char *p = line;
        while (*p == ' ' || *p == '\t') ++p;
        if (*p == '\0' || *p == '\n' || *p == '#') continue;
        int index;
        char number[40], extra;
        if (sscanf(p, "%d %39s %c", &index, number, &extra) != 2 ||
            index != next || index < MIN_N || index > MAX_N) {
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
    for (int i = MIN_N; i < prefix; ++i) {
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
    fprintf(stderr, "397767_03: recorded computed term n=%d in %s\n",
            n, output_path);
}

static uint64_t mix64(uint64_t x)
{
    x ^= x >> 30;
    x *= UINT64_C(0xbf58476d1ce4e5b9);
    x ^= x >> 27;
    x *= UINT64_C(0x94d049bb133111eb);
    return x ^ (x >> 31);
}

static void memory_snapshot(size_t *used, size_t *peak)
{
    if (pthread_mutex_lock(&memory_mutex) != 0) die("memory lock failed");
    *used = memory_used;
    *peak = peak_memory;
    if (pthread_mutex_unlock(&memory_mutex) != 0) die("memory unlock failed");
}

static void account_allocation(size_t bytes)
{
    if (pthread_mutex_lock(&memory_mutex) != 0) die("memory lock failed");
    if (bytes > memory_limit || memory_used > memory_limit - bytes) {
        const size_t used = memory_used;
        if (pthread_mutex_unlock(&memory_mutex) != 0) die("memory unlock failed");
        fprintf(stderr,
                "error: DP needs another %.2f MiB but the --memory-limit-mib "
                "budget is exhausted (used %.2f MiB of %.2f MiB)\n",
                (double)bytes / 1048576.0, (double)used / 1048576.0,
                (double)memory_limit / 1048576.0);
        exit(EXIT_FAILURE);
    }
    memory_used += bytes;
    if (memory_used > peak_memory) peak_memory = memory_used;
    if (pthread_mutex_unlock(&memory_mutex) != 0) die("memory unlock failed");
}

static void *budget_malloc(size_t bytes)
{
    account_allocation(bytes);
    void *memory = malloc(bytes);
    if (memory == NULL) {
        if (pthread_mutex_lock(&memory_mutex) != 0) die("memory lock failed");
        memory_used -= bytes;
        if (pthread_mutex_unlock(&memory_mutex) != 0) die("memory unlock failed");
        die("memory allocation failed");
    }
    return memory;
}

static void budget_free(void *memory, size_t bytes)
{
    free(memory);
    if (pthread_mutex_lock(&memory_mutex) != 0) die("memory lock failed");
    if (bytes > memory_used) die("internal memory accounting error");
    memory_used -= bytes;
    if (pthread_mutex_unlock(&memory_mutex) != 0) die("memory unlock failed");
}

static size_t capacity_for_expected(size_t expected)
{
    size_t capacity = 16;
    while (capacity - capacity / 4U < expected) {
        if (capacity > SIZE_MAX / 2U) die("hash capacity overflow");
        capacity *= 2U;
    }
    return capacity;
}

static void map_init(StateMap *map, size_t expected)
{
    const size_t capacity = capacity_for_expected(expected);
    if (capacity > SIZE_MAX / sizeof(*map->keys) ||
        capacity > SIZE_MAX / sizeof(*map->values)) {
        die("hash allocation size overflow");
    }
    const size_t key_bytes = capacity * sizeof(*map->keys);
    const size_t value_bytes = capacity * sizeof(*map->values);
    map->keys = budget_malloc(key_bytes);
    map->values = budget_malloc(value_bytes);
    memset(map->keys, 0xff, key_bytes);
    /* Values at empty-key slots are never read, but zero-initializing them
       also makes that invariant explicit to sanitizers and static analyzers. */
    memset(map->values, 0, value_bytes);
    map->capacity = capacity;
    map->count = 0;
    map->grow_at = capacity - capacity / 4U;
}

static void map_destroy(StateMap *map)
{
    if (map->capacity != 0) {
        budget_free(map->keys, map->capacity * sizeof(*map->keys));
        budget_free(map->values, map->capacity * sizeof(*map->values));
    }
    *map = (StateMap){0};
}

static void map_insert_new(StateMap *map, uint64_t key, U128 value)
{
    size_t slot = (size_t)mix64(key) & (map->capacity - 1U);
    while (map->keys[slot] != UINT64_MAX) {
        slot = (slot + 1U) & (map->capacity - 1U);
    }
    map->keys[slot] = key;
    map->values[slot] = value;
    ++map->count;
}

static void map_grow(StateMap *map)
{
    if (map->capacity > SIZE_MAX / 2U) die("hash capacity overflow");
    StateMap larger;
    map_init(&larger, map->grow_at + 1U);
    for (size_t i = 0; i < map->capacity; ++i) {
        if (map->keys[i] != UINT64_MAX) {
            map_insert_new(&larger, map->keys[i], map->values[i]);
        }
    }
    map_destroy(map);
    *map = larger;
}

static void map_add(StateMap *map, uint64_t key, U128 addend)
{
    for (;;) {
        size_t slot = (size_t)mix64(key) & (map->capacity - 1U);
        while (map->keys[slot] != UINT64_MAX && map->keys[slot] != key) {
            slot = (slot + 1U) & (map->capacity - 1U);
        }
        if (map->keys[slot] == key) {
            if (!add_u128(&map->values[slot], addend)) {
                die("DP value overflowed U128");
            }
            return;
        }
        if (map->count >= map->grow_at) {
            map_grow(map);
            continue;
        }
        map->keys[slot] = key;
        map->values[slot] = addend;
        ++map->count;
        return;
    }
}

static bool map_get(const StateMap *map, uint64_t key, U128 *value)
{
    size_t slot = (size_t)mix64(key) & (map->capacity - 1U);
    while (map->keys[slot] != UINT64_MAX) {
        if (map->keys[slot] == key) {
            *value = map->values[slot];
            return true;
        }
        slot = (slot + 1U) & (map->capacity - 1U);
    }
    return false;
}

static unsigned make_sum_edges(unsigned maximum, unsigned sum,
                               uint64_t edges[2 * MAX_N])
{
    unsigned count = 0;
    const unsigned first = sum > maximum ? sum - maximum : 1U;
    for (unsigned a = first; a < sum - a; ++a) {
        const unsigned b = sum - a;
        if (b > maximum) continue;
        if (count == 2U * MAX_N) die("too many equal-sum pairs");
        edges[count++] = (UINT64_C(1) << (a - 1U)) |
                         (UINT64_C(1) << (b - 1U));
    }
    return count;
}

static unsigned generate_prime_classes(int n, PrimeClass classes[MAX_CLASSES])
{
    const unsigned maximum = 4U * (unsigned)n;
    const unsigned sieve_max = 2U * MAX_VALUES;
    bool prime[2 * MAX_VALUES + 1];
    for (unsigned value = 0; value <= sieve_max; ++value) {
        prime[value] = value >= 2U;
    }
    for (unsigned divisor = 2; divisor <= sieve_max / divisor; ++divisor) {
        if (!prime[divisor]) continue;
        for (unsigned multiple = divisor * divisor;
             multiple <= sieve_max; multiple += divisor) {
            prime[multiple] = false;
        }
    }

    unsigned count = 0;
    for (unsigned sum = 3; sum <= 2U * maximum; ++sum) {
        if (!prime[sum]) continue;
        PrimeClass item = {.sum = sum};
        item.edge_count = make_sum_edges(maximum, sum, item.edges);
        if (item.edge_count < 2) continue;
        if (count == MAX_CLASSES) die("too many prime classes");
        classes[count++] = item;
    }
    return count;
}

static int compare_class_weight_desc(const void *left, const void *right)
{
    const PrimeClass *const *a = left;
    const PrimeClass *const *b = right;
    if ((*a)->edge_count != (*b)->edge_count) {
        return (*a)->edge_count > (*b)->edge_count ? -1 : 1;
    }
    return (*a)->sum < (*b)->sum ? -1 : (*a)->sum != (*b)->sum;
}

static int compare_class_weight_asc(const void *left, const void *right)
{
    const PrimeClass *const *a = left;
    const PrimeClass *const *b = right;
    if ((*a)->edge_count != (*b)->edge_count) {
        return (*a)->edge_count < (*b)->edge_count ? -1 : 1;
    }
    return (*a)->sum < (*b)->sum ? -1 : (*a)->sum != (*b)->sum;
}

/* A class with e edges has 2^(e-1) even local choices, so e-1 is its
   logarithmic branch weight.  Greedy bin packing balances that weight. */
static void split_classes(PrimeClass classes[MAX_CLASSES], unsigned count,
                          const PrimeClass *left[MAX_CLASSES],
                          unsigned *left_count,
                          const PrimeClass *right[MAX_CLASSES],
                          unsigned *right_count,
                          unsigned *left_weight, unsigned *right_weight)
{
    const PrimeClass *ordered[MAX_CLASSES];
    for (unsigned i = 0; i < count; ++i) ordered[i] = &classes[i];
    qsort(ordered, count, sizeof(*ordered), compare_class_weight_desc);
    *left_count = *right_count = 0;
    *left_weight = *right_weight = 0;
    for (unsigned i = 0; i < count; ++i) {
        const unsigned weight = ordered[i]->edge_count - 1U;
        if (*left_weight <= *right_weight) {
            left[(*left_count)++] = ordered[i];
            *left_weight += weight;
        } else {
            right[(*right_count)++] = ordered[i];
            *right_weight += weight;
        }
    }
    qsort(left, *left_count, sizeof(*left), compare_class_weight_asc);
    qsort(right, *right_count, sizeof(*right), compare_class_weight_asc);
}

static uint64_t *make_subset_vertices(const PrimeClass *item, size_t *bytes)
{
    if (item->edge_count >= sizeof(size_t) * 8U) {
        die("subset table is too large");
    }
    const size_t length = (size_t)1U << item->edge_count;
    if (length > SIZE_MAX / sizeof(uint64_t)) die("subset table overflow");
    *bytes = length * sizeof(uint64_t);
    uint64_t *vertices = budget_malloc(*bytes);
    vertices[0] = 0;
    for (size_t subset = 1; subset < length; ++subset) {
        const size_t bit = subset & (~subset + 1U);
        vertices[subset] = vertices[subset ^ bit] |
            item->edges[(unsigned)__builtin_ctzll((uint64_t)bit)];
    }
    return vertices;
}

/* weights[2*r] = (2*r-1)!! and weights[0] = 1. */
static void make_pairing_weights(uint64_t weights[2 * MAX_N + 1])
{
    weights[0] = 1;
    for (unsigned selected = 2; selected <= 2U * MAX_N; selected += 2U) {
        const uint64_t factor = selected - 1U;
        if (weights[selected - 2U] > UINT64_MAX / factor) {
            die("pairing-weight overflow");
        }
        weights[selected] = weights[selected - 2U] * factor;
    }
}

static void log_completed_class(const BuildArgument *argument,
                                const PrimeClass *item, const StateMap *map,
                                U128 class_transitions)
{
    size_t used, peak;
    char transition_text[40];
    memory_snapshot(&used, &peak);
    u128_text(class_transitions, transition_text);
    if (pthread_mutex_lock(&log_mutex) != 0) die("log lock failed");
    fprintf(stderr,
            "397767_03: n=%d, side=%s, prime=%u, pairs=%u, states=%zu, "
            "transitions=%s, allocated=%.2f/%.2f GiB, peak=%.2f GiB, %.1f s\n",
            argument->n, argument->name, item->sum,
            item->edge_count, map->count, transition_text,
            (double)used / 1073741824.0,
            (double)memory_limit / 1073741824.0,
            (double)peak / 1073741824.0,
            now_seconds() - argument->started);
    if (pthread_mutex_unlock(&log_mutex) != 0) die("log unlock failed");
}

static void process_class(BuildArgument *argument, const PrimeClass *item,
                          StateMap *current)
{
    uint64_t weights[2 * MAX_N + 1] = {0};
    make_pairing_weights(weights);
    size_t subset_bytes;
    uint64_t *subset_vertices = make_subset_vertices(item, &subset_bytes);

    if (current->count > (SIZE_MAX - 16U) / 2U) {
        die("expected state count overflow");
    }
    StateMap next;
    map_init(&next, 2U * current->count + 16U);
    U128 class_transitions = 0;
    size_t processed = 0;
    double next_heartbeat = now_seconds() + 10.0;

    for (size_t slot = 0; slot < current->capacity; ++slot) {
        const uint64_t state = current->keys[slot];
        if (state == UINT64_MAX) continue;
        uint32_t available = 0;
        for (unsigned edge = 0; edge < item->edge_count; ++edge) {
            if ((state & item->edges[edge]) == 0) {
                available |= UINT32_C(1) << edge;
            }
        }
        uint32_t subset = available;
        for (;;) {
            const unsigned selected = (unsigned)__builtin_popcount(subset);
            if ((selected & 1U) == 0) {
                U128 addend;
                if (!multiply_u128(current->values[slot],
                                   (U128)weights[selected], &addend)) {
                    die("DP transition overflowed U128");
                }
                map_add(&next, state | subset_vertices[subset], addend);
                if (!add_u128(&class_transitions, 1)) {
                    die("transition counter overflowed U128");
                }
            }
            if (subset == 0) break;
            subset = (subset - 1U) & available;
        }

        ++processed;
        if ((processed & 65535U) == 0 && now_seconds() >= next_heartbeat) {
            size_t used, peak;
            char transition_text[40];
            memory_snapshot(&used, &peak);
            u128_text(class_transitions, transition_text);
            if (pthread_mutex_lock(&log_mutex) != 0) die("log lock failed");
            fprintf(stderr,
                    "397767_03: heartbeat n=%d, side=%s, prime=%u, "
                    "states=%zu/%zu, next=%zu, transitions=%s, "
                    "allocated=%.2f/%.2f GiB, peak=%.2f GiB, %.1f s\n",
                    argument->n, argument->name, item->sum, processed,
                    current->count, next.count, transition_text,
                    (double)used / 1073741824.0,
                    (double)memory_limit / 1073741824.0,
                    (double)peak / 1073741824.0,
                    now_seconds() - argument->started);
            if (pthread_mutex_unlock(&log_mutex) != 0) die("log unlock failed");
            next_heartbeat = now_seconds() + 10.0;
        }
    }

    budget_free(subset_vertices, subset_bytes);
    map_destroy(current);
    *current = next;
    if (!add_u128(&argument->transitions, class_transitions)) {
        die("total transition counter overflowed U128");
    }
    log_completed_class(argument, item, current, class_transitions);
}

static void *build_side(void *pointer)
{
    BuildArgument *argument = pointer;
    StateMap current;
    map_init(&current, 1);
    map_insert_new(&current, 0, 1);
    argument->transitions = 0;
    for (unsigned i = 0; i < argument->class_count; ++i) {
        process_class(argument, argument->classes[i], &current);
    }
    argument->result = current;
    return NULL;
}

static void *join_worker_main(void *pointer)
{
    JoinWorker *worker = pointer;
    JoinSchedule *schedule = worker->schedule;
    for (;;) {
        const size_t begin = atomic_fetch_add_explicit(
            &schedule->next_slot, JOIN_CHUNK, memory_order_relaxed);
        if (begin >= schedule->scan->capacity) break;
        size_t end = begin + JOIN_CHUNK;
        if (end < begin || end > schedule->scan->capacity) {
            end = schedule->scan->capacity;
        }
        for (size_t slot = begin; slot < end; ++slot) {
            const uint64_t state = schedule->scan->keys[slot];
            if (state == UINT64_MAX) continue;
            if (worker->lookups == SIZE_MAX) die("join lookup counter overflow");
            ++worker->lookups;
            U128 complement;
            if (!map_get(schedule->lookup, schedule->full ^ state,
                         &complement)) {
                continue;
            }
            if (worker->matches == SIZE_MAX) die("join match counter overflow");
            ++worker->matches;
            U128 product;
            if (!multiply_u128(schedule->scan->values[slot], complement,
                               &product) ||
                !add_u128(&worker->answer, product)) {
                die("final join overflowed U128");
            }
        }
        atomic_fetch_add_explicit(&schedule->processed_slots, end - begin,
                                  memory_order_relaxed);
    }
    return NULL;
}

static void *join_monitor_main(void *pointer)
{
    const JoinMonitor *monitor = pointer;
    if (pthread_mutex_lock(&monitor_mutex) != 0) die("monitor lock failed");
    while (!monitor_finished) {
        struct timespec deadline;
        if (clock_gettime(CLOCK_REALTIME, &deadline) != 0) {
            die("monitor clock failed");
        }
        deadline.tv_sec += 10;
        const int error = pthread_cond_timedwait(
            &monitor_condition, &monitor_mutex, &deadline);
        if (error != 0 && error != ETIMEDOUT) die("monitor wait failed");
        if (!monitor_finished && error == ETIMEDOUT) {
            const size_t done = atomic_load_explicit(
                &monitor->schedule->processed_slots, memory_order_relaxed);
            if (pthread_mutex_lock(&log_mutex) != 0) die("log lock failed");
            fprintf(stderr,
                    "397767_03: heartbeat n=%d, join-slots=%zu/%zu, %.1f s\n",
                    monitor->n, done, monitor->schedule->scan->capacity,
                    now_seconds() - monitor->started);
            if (pthread_mutex_unlock(&log_mutex) != 0) die("log unlock failed");
        }
    }
    if (pthread_mutex_unlock(&monitor_mutex) != 0) die("monitor unlock failed");
    return NULL;
}

static U128 join_maps(int n, const StateMap *left, const StateMap *right,
                      uint64_t full, size_t *lookups, size_t *matches,
                      int *active_threads, double started)
{
    const StateMap *scan = left->count <= right->count ? left : right;
    const StateMap *lookup = scan == left ? right : left;
    JoinSchedule schedule = {
        .scan = scan, .lookup = lookup, .full = full
    };
    atomic_init(&schedule.next_slot, 0);
    atomic_init(&schedule.processed_slots, 0);

    int threads = requested_threads;
    if ((size_t)threads > scan->count) threads = (int)scan->count;
    if (threads < 1) threads = 1;
    *active_threads = threads;
    JoinWorker *workers = calloc((size_t)threads, sizeof(*workers));
    pthread_t *ids = calloc((size_t)threads, sizeof(*ids));
    if (workers == NULL || ids == NULL) {
        free(workers);
        free(ids);
        die("cannot allocate join workers");
    }
    for (int i = 0; i < threads; ++i) workers[i].schedule = &schedule;

    if (pthread_mutex_lock(&monitor_mutex) != 0) die("monitor lock failed");
    monitor_finished = false;
    if (pthread_mutex_unlock(&monitor_mutex) != 0) die("monitor unlock failed");
    const JoinMonitor monitor = {n, &schedule, started};
    pthread_t monitor_id;
    int error = pthread_create(&monitor_id, NULL, join_monitor_main,
                               (void *)&monitor);
    if (error != 0) die("cannot create join monitor");
    for (int i = 0; i < threads; ++i) {
        error = pthread_create(&ids[i], NULL, join_worker_main, &workers[i]);
        if (error != 0) die("cannot create join worker");
    }

    U128 answer = 0;
    *lookups = *matches = 0;
    for (int i = 0; i < threads; ++i) {
        if (pthread_join(ids[i], NULL) != 0) die("cannot join join worker");
        if (!add_u128(&answer, workers[i].answer)) die("join sum overflowed U128");
        if (SIZE_MAX - *lookups < workers[i].lookups ||
            SIZE_MAX - *matches < workers[i].matches) {
            die("join statistics overflow");
        }
        *lookups += workers[i].lookups;
        *matches += workers[i].matches;
    }
    if (pthread_mutex_lock(&monitor_mutex) != 0) die("monitor lock failed");
    monitor_finished = true;
    if (pthread_cond_signal(&monitor_condition) != 0) die("monitor signal failed");
    if (pthread_mutex_unlock(&monitor_mutex) != 0) die("monitor unlock failed");
    if (pthread_join(monitor_id, NULL) != 0) die("cannot join join monitor");
    free(workers);
    free(ids);
    return answer;
}

static U128 sequence_term(int n)
{
    memory_used = peak_memory = 0;
    const double started = now_seconds();
    PrimeClass classes[MAX_CLASSES];
    const unsigned class_count = generate_prime_classes(n, classes);
    const PrimeClass *left_classes[MAX_CLASSES], *right_classes[MAX_CLASSES];
    unsigned left_count, right_count, left_weight, right_weight;
    split_classes(classes, class_count, left_classes, &left_count,
                  right_classes, &right_count, &left_weight, &right_weight);

    BuildArgument left = {
        .n = n, .name = "L", .classes = left_classes,
        .class_count = left_count, .started = started
    };
    BuildArgument right = {
        .n = n, .name = "R", .classes = right_classes,
        .class_count = right_count, .started = started
    };
    int build_threads = requested_threads >= 2 ? 2 : 1;
    if (build_threads == 2) {
        pthread_t ids[2];
        if (pthread_create(&ids[0], NULL, build_side, &left) != 0 ||
            pthread_create(&ids[1], NULL, build_side, &right) != 0) {
            die("cannot create DP build workers");
        }
        if (pthread_join(ids[0], NULL) != 0 ||
            pthread_join(ids[1], NULL) != 0) {
            die("cannot join DP build workers");
        }
    } else {
        build_side(&left);
        build_side(&right);
    }

    const unsigned maximum = 4U * (unsigned)n;
    const uint64_t full = (UINT64_C(1) << maximum) - 1U;
    size_t lookups, matches;
    int join_threads;
    const U128 answer = join_maps(n, &left.result, &right.result, full,
                                  &lookups, &matches, &join_threads, started);
    U128 transitions = left.transitions;
    if (!add_u128(&transitions, right.transitions)) {
        die("total transition statistics overflowed U128");
    }
    const size_t left_states = left.result.count;
    const size_t right_states = right.result.count;
    map_destroy(&left.result);
    map_destroy(&right.result);
    size_t used, peak;
    memory_snapshot(&used, &peak);
    if (used != 0) die("internal DP memory leak accounting error");

    char transition_text[40];
    u128_text(transitions, transition_text);
    fprintf(stderr,
            "397767_03: n=%d, prime-class sparse DP+MITM, classes=%u "
            "(%u+%u; weights=%u+%u), states=%zu+%zu, transitions=%s, "
            "join-lookups=%zu, matches=%zu, build-threads=%d, "
            "join-threads=%d, peak-memory=%.2f GiB, limit=%.2f GiB, %.3f s\n",
            n, class_count, left_count, right_count, left_weight, right_weight,
            left_states, right_states, transition_text,
            lookups, matches, build_threads, join_threads,
            (double)peak / 1073741824.0,
            (double)memory_limit / 1073741824.0,
            now_seconds() - started);
    return answer;
}

static const char *const known[MAX_N + 1] = {
    NULL, "1", "2", "24", "204", "949", "11710", "481040",
    "5803706"
};

static void verify_known(int n, U128 value)
{
    if (n < MIN_N || n > MAX_N || known[n] == NULL) return;
    U128 expected;
    if (!parse_u128(known[n], &expected)) die("invalid built-in known term");
    if (value != expected) {
        fprintf(stderr, "error: known-term mismatch n=%d: got ", n);
        print_u128(stderr, value);
        fprintf(stderr, ", expected %s\n", known[n]);
        exit(EXIT_FAILURE);
    }
}

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s [N] [--threads T] [--memory-limit-mib M] "
            "[--output FILE|--no-bfile]\n"
            "       %s --term N [--threads T] [--memory-limit-mib M] "
            "[--output FILE|--no-bfile]\n"
            "       %s --check [--threads T] [--memory-limit-mib M] "
            "[--output FILE|--no-bfile]\n"
            "N prints a(1)..a(N); --term prints only a(N).\n",
            program, program, program);
}

int main(int argc, char **argv)
{
    int n = DEFAULT_N;
    size_t memory_mib = DEFAULT_MEMORY_MIB;
    bool positional = false, term_only = false, check = false;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--threads") == 0) {
            if (++i == argc) die("--threads needs an argument");
            requested_threads = parse_bounded(argv[i], 1, MAX_THREADS,
                                              "threads");
        } else if (strcmp(argv[i], "--memory-limit-mib") == 0) {
            if (++i == argc) die("--memory-limit-mib needs an argument");
            memory_mib = parse_memory_mib(argv[i]);
        } else if (strcmp(argv[i], "--output") == 0) {
            if (++i == argc || argv[i][0] == '\0') die("--output needs a file");
            output_path = argv[i];
            write_bfile = true;
        } else if (strcmp(argv[i], "--no-bfile") == 0) {
            write_bfile = false;
        } else if (strcmp(argv[i], "--term") == 0) {
            if (term_only || positional || check || ++i == argc) {
                die("invalid --term usage");
            }
            term_only = true;
            n = parse_bounded(argv[i], MIN_N, MAX_N, "N");
        } else if (strcmp(argv[i], "--check") == 0) {
            if (term_only || positional || check) die("invalid --check usage");
            check = true;
            n = 8;
        } else if (argv[i][0] == '-') {
            usage(argv[0]);
            return EXIT_FAILURE;
        } else {
            if (positional || term_only || check) die("multiple N arguments");
            positional = true;
            n = parse_bounded(argv[i], MIN_N, MAX_N, "N");
        }
    }

    if (memory_mib > SIZE_MAX / 1048576U) die("memory budget overflow");
    memory_limit = memory_mib * 1048576U;

    if (term_only) {
        require_recordable(n);
        const U128 value = sequence_term(n);
        verify_known(n, value);
        print_term(n, value);
        record_term(n, value);
        return EXIT_SUCCESS;
    }
    for (int k = MIN_N; k <= n; ++k) {
        require_recordable(k);
        const U128 value = sequence_term(k);
        verify_known(k, value);
        print_term(k, value);
        record_term(k, value);
    }
    if (check) fprintf(stderr, "397767_03: self-check passed\n");
    return EXIT_SUCCESS;
}
