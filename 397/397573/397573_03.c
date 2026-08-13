/*
 * A397573 -- exact pair-sum-class sparse DP with a reflection join.
 *
 * Count partitions of {1,...,4*n} into unordered four-element blocks whose
 * elements can be divided into two pairs of equal sum.  For a<b<c<d, the only
 * possible equal pairing is {a,d},{b,c}, since a+b<c+d and a+c<b+d.
 * Thus every valid block has one well-defined pair sum.
 *
 * For a fixed sum s, let
 *
 *     E_s = {{i,s-i}: 1 <= i < s-i <= 4*n}.
 *
 * The edges in E_s are mutually disjoint.  If 2*r of them are selected, they
 * can be grouped into r unordered pairs of edges in (2*r-1)!! ways; each such
 * grouping is exactly r valid four-element blocks of pair sum s.
 *
 * The sparse DP processes the low sums s=3,...,4*n.  A state mask records all
 * vertices already used, and its exact U128 value counts the block partitions
 * represented by that mask.  For every even subset T of the currently unused
 * edges in E_s, the transition is
 *
 *   next[M union V(T)] += current[M] * (|T|-1)!!.
 *
 * Induction over the processed sums proves this invariant; grouping a final
 * partition by its unique pair sums proves that nothing is missed or counted
 * twice.
 *
 * Put m=4*n.  Reflection i -> m+1-i maps pair sum s to 2*(m+1)-s.  Hence the
 * high-sum DP is the reflection of the low-sum DP and need not be built.  The
 * fixed central sum m+1 is joined explicitly: for each low state A and every
 * even subset C of unused central pairs, the required reflected high state is
 *
 *     full_mask XOR reflect(A) XOR C.
 *
 * A hash lookup supplies its count.  This is an exact meet-in-the-middle join,
 * not a hash-probability argument: open-addressing compares the full mask and
 * grows instead of evicting entries.  Hash collisions therefore affect only
 * probe length.  No state is discarded and no heuristic cutoff is used.
 *
 * MAX_N=12 uses 48 mask bits.  Any partial or complete value is bounded by the
 * unrestricted number (4*n)!/(24^n*n!) of four-set partitions, whose n=12
 * value has 120 bits.  U128 is therefore sufficient; every addition and
 * multiplication is checked.  A configurable memory ceiling causes a clean
 * failure before a hash-table allocation exceeds the requested budget.
 *
 * Build:
 *   clang -O3 -march=native -std=c11 -Wall -Wextra -Wpedantic \
 *       397573_03.c -o 397573_03
 *
 * Usage:
 *   ./397573_03 7
 *   ./397573_03 --term 7 --no-bfile
 *   ./397573_03 --term 8 --memory-limit-mib 8192 --no-bfile
 *   ./397573_03 --check --no-bfile
 *
 * A positional N computes a(0),...,a(N).  Completed terms are atomically
 * recorded in b397573_03.txt by default.  --term N requires an existing
 * contiguous prefix unless --no-bfile is used.  An interrupted or failed
 * calculation never records its unfinished term.
 */

#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#if !defined(__SIZEOF_INT128__)
#error "397573_03.c requires unsigned __int128"
#endif
__extension__ typedef unsigned __int128 U128;

#define MAX_N 12
#define MAX_VALUES (4 * MAX_N)
#define DEFAULT_N 7
#define DEFAULT_MEMORY_MIB 4096
#define MIN_MEMORY_MIB 64
#define MAX_MEMORY_MIB 32768

_Static_assert(MAX_VALUES < 64, "unused-element set must fit in uint64_t");

typedef struct {
    uint64_t *keys;
    U128 *values;
    size_t capacity;
    size_t count;
    size_t grow_at;
} StateMap;

static const char *output_path = "b397573_03.txt";
static bool write_bfile = true;
static size_t memory_limit;
static size_t memory_used;
static size_t peak_memory;

static _Noreturn void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static int parse_n(const char *text)
{
    char *end = NULL;
    errno = 0;
    const long value = strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' ||
        value < 0 || value > MAX_N) {
        fprintf(stderr, "error: N must be in 0..%d: %s\n", MAX_N, text);
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
        int index;
        char number[40], extra;
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
    if (ferror(input) || fclose(input) != 0) die("cannot read complete b-file");
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
                "error: %s ends before n=%d; compute the missing prefix "
                "first, or use --no-bfile\n", output_path, n);
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
        if (strcmp(values[n], text) != 0) die("value disagrees with b-file");
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
    fprintf(stderr, "397573_03: recorded computed term n=%d in %s\n",
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

static bool multiply_u128(U128 a, U128 b, U128 *product)
{
    const U128 maximum = ~(U128)0;
    if (a != 0 && b > maximum / a) return false;
    *product = a * b;
    return true;
}

static void account_allocation(size_t bytes)
{
    if (bytes > memory_limit - memory_used) {
        fprintf(stderr,
                "error: DP needs another %.2f MiB but the --memory-limit-mib "
                "budget is exhausted (used %.2f MiB of %.2f MiB)\n",
                (double)bytes / 1048576.0,
                (double)memory_used / 1048576.0,
                (double)memory_limit / 1048576.0);
        exit(EXIT_FAILURE);
    }
    memory_used += bytes;
    if (memory_used > peak_memory) peak_memory = memory_used;
}

static void *budget_malloc(size_t bytes)
{
    account_allocation(bytes);
    void *memory = malloc(bytes);
    if (memory == NULL) {
        memory_used -= bytes;
        die("memory allocation failed");
    }
    return memory;
}

static void budget_free(void *memory, size_t bytes)
{
    free(memory);
    if (bytes > memory_used) die("internal memory accounting error");
    memory_used -= bytes;
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

static uint64_t reverse64(uint64_t x)
{
    x = ((x & UINT64_C(0x5555555555555555)) << 1) |
        ((x >> 1) & UINT64_C(0x5555555555555555));
    x = ((x & UINT64_C(0x3333333333333333)) << 2) |
        ((x >> 2) & UINT64_C(0x3333333333333333));
    x = ((x & UINT64_C(0x0f0f0f0f0f0f0f0f)) << 4) |
        ((x >> 4) & UINT64_C(0x0f0f0f0f0f0f0f));
    x = ((x & UINT64_C(0x00ff00ff00ff00ff)) << 8) |
        ((x >> 8) & UINT64_C(0x00ff00ff00ff00ff));
    x = ((x & UINT64_C(0x0000ffff0000ffff)) << 16) |
        ((x >> 16) & UINT64_C(0x0000ffff0000ffff));
    return (x << 32) | (x >> 32);
}

static uint64_t reflect_mask(uint64_t mask, unsigned width)
{
    return reverse64(mask) >> (64U - width);
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

static uint64_t *make_subset_vertices(const uint64_t *edges, unsigned count,
                                      size_t *bytes)
{
    if (count >= sizeof(size_t) * 8U) die("subset table is too large");
    const size_t length = (size_t)1U << count;
    if (length > SIZE_MAX / sizeof(uint64_t)) die("subset table overflow");
    *bytes = length * sizeof(uint64_t);
    uint64_t *vertices = budget_malloc(*bytes);
    vertices[0] = 0;
    for (size_t subset = 1; subset < length; ++subset) {
        const size_t bit = subset & (~subset + 1U);
        vertices[subset] = vertices[subset ^ bit] |
                           edges[(unsigned)__builtin_ctzll((uint64_t)bit)];
    }
    return vertices;
}

static void make_double_factorials(uint64_t values[2 * MAX_N + 1])
{
    values[0] = values[1] = 1;
    for (unsigned k = 2; k <= 2U * MAX_N; ++k) {
        if ((k & 1U) != 0) {
            if (values[k - 2U] > UINT64_MAX / k) {
                die("double factorial overflow");
            }
            values[k] = values[k - 2U] * k;
        } else {
            values[k] = values[k - 1U];
        }
    }
}

static StateMap build_low_dp(int n, uint64_t *transition_total,
                             double started)
{
    const unsigned maximum = 4U * (unsigned)n;
    uint64_t weights[2 * MAX_N + 1];
    make_double_factorials(weights);

    StateMap current;
    map_init(&current, 1);
    map_insert_new(&current, 0, 1);
    *transition_total = 0;

    for (unsigned sum = 3; sum <= maximum; ++sum) {
        uint64_t edges[2 * MAX_N];
        const unsigned edge_count = make_sum_edges(maximum, sum, edges);
        if (edge_count < 2) continue;
        size_t subset_bytes;
        uint64_t *subset_vertices = make_subset_vertices(
            edges, edge_count, &subset_bytes);

        if (current.count > (SIZE_MAX - 16U) / 2U) {
            die("expected state count overflow");
        }
        StateMap next;
        map_init(&next, 2U * current.count + 16U);
        uint64_t sum_transitions = 0;
        size_t processed = 0;
        double next_heartbeat = now_seconds() + 10.0;

        for (size_t slot = 0; slot < current.capacity; ++slot) {
            const uint64_t state = current.keys[slot];
            if (state == UINT64_MAX) continue;
            uint32_t available = 0;
            for (unsigned edge = 0; edge < edge_count; ++edge) {
                if ((state & edges[edge]) == 0) available |= UINT32_C(1) << edge;
            }
            uint32_t subset = available;
            for (;;) {
                const unsigned selected = (unsigned)__builtin_popcount(subset);
                if ((selected & 1U) == 0) {
                    U128 addend;
                    if (!multiply_u128(current.values[slot],
                                       (U128)weights[selected], &addend)) {
                        die("DP transition overflowed U128");
                    }
                    map_add(&next, state | subset_vertices[subset], addend);
                    if (sum_transitions == UINT64_MAX ||
                        *transition_total == UINT64_MAX) {
                        die("transition counter overflow");
                    }
                    ++sum_transitions;
                    ++*transition_total;
                }
                if (subset == 0) break;
                subset = (subset - 1U) & available;
            }

            ++processed;
            if ((processed & 65535U) == 0 && now_seconds() >= next_heartbeat) {
                fprintf(stderr,
                        "397573_03: heartbeat n=%d, sum=%u, states=%zu/%zu, "
                        "next=%zu, transitions=%" PRIu64 ", %.1f s\n",
                        n, sum, processed, current.count, next.count,
                        sum_transitions, now_seconds() - started);
                next_heartbeat = now_seconds() + 10.0;
            }
        }

        budget_free(subset_vertices, subset_bytes);
        map_destroy(&current);
        current = next;
        fprintf(stderr,
                "397573_03: n=%d, sum=%u, pairs=%u, states=%zu, "
                "transitions=%" PRIu64 ", memory=%.2f MiB, %.3f s\n",
                n, sum, edge_count, current.count, sum_transitions,
                (double)memory_used / 1048576.0, now_seconds() - started);
    }
    return current;
}

static U128 join_high_and_central(int n, const StateMap *low,
                                  uint64_t *lookups, double started)
{
    const unsigned maximum = 4U * (unsigned)n;
    const uint64_t full = (UINT64_C(1) << maximum) - 1U;
    uint64_t central_edges[2 * MAX_N];
    const unsigned central_count = make_sum_edges(
        maximum, maximum + 1U, central_edges);
    if (central_count != 2U * (unsigned)n) {
        die("internal central-pair count mismatch");
    }
    size_t subset_bytes;
    uint64_t *subset_vertices = make_subset_vertices(
        central_edges, central_count, &subset_bytes);
    uint64_t weights[2 * MAX_N + 1];
    make_double_factorials(weights);

    U128 answer = 0;
    *lookups = 0;
    size_t processed = 0;
    double next_heartbeat = now_seconds() + 10.0;
    for (size_t slot = 0; slot < low->capacity; ++slot) {
        const uint64_t state = low->keys[slot];
        if (state == UINT64_MAX) continue;
        uint32_t available = 0;
        for (unsigned edge = 0; edge < central_count; ++edge) {
            if ((state & central_edges[edge]) == 0) {
                available |= UINT32_C(1) << edge;
            }
        }
        const uint64_t base = full ^ reflect_mask(state, maximum);
        uint32_t subset = available;
        for (;;) {
            const unsigned selected = (unsigned)__builtin_popcount(subset);
            if ((selected & 1U) == 0) {
                const uint64_t wanted = base ^ subset_vertices[subset];
                U128 high;
                if (map_get(low, wanted, &high)) {
                    U128 product, weighted;
                    if (!multiply_u128(low->values[slot], high, &product) ||
                        !multiply_u128(product, (U128)weights[selected],
                                       &weighted) ||
                        !add_u128(&answer, weighted)) {
                        die("final join overflowed U128");
                    }
                }
                if (*lookups == UINT64_MAX) die("lookup counter overflow");
                ++*lookups;
            }
            if (subset == 0) break;
            subset = (subset - 1U) & available;
        }

        ++processed;
        if ((processed & 65535U) == 0 && now_seconds() >= next_heartbeat) {
            fprintf(stderr,
                    "397573_03: heartbeat n=%d, join=%zu/%zu, "
                    "lookups=%" PRIu64 ", %.1f s\n",
                    n, processed, low->count, *lookups,
                    now_seconds() - started);
            next_heartbeat = now_seconds() + 10.0;
        }
    }
    budget_free(subset_vertices, subset_bytes);
    return answer;
}

static U128 sequence_term(int n)
{
    if (n == 0) return 1;
    memory_used = peak_memory = 0;
    const double started = now_seconds();
    uint64_t transitions, lookups;
    StateMap low = build_low_dp(n, &transitions, started);
    const U128 answer = join_high_and_central(
        n, &low, &lookups, started);
    const size_t low_states = low.count;
    map_destroy(&low);
    if (memory_used != 0) die("internal memory leak accounting error");
    fprintf(stderr,
            "397573_03: n=%d, pair-sum sparse DP, low-states=%zu, "
            "transitions=%" PRIu64 ", join-lookups=%" PRIu64
            ", peak-memory=%.2f MiB, %.3f s\n",
            n, low_states, transitions, lookups,
            (double)peak_memory / 1048576.0, now_seconds() - started);
    return answer;
}

static const char *const known[] = {
    "1", "1", "6", "86", "1990", "74323", "4226026", "332352318"
};

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

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s [N] [--memory-limit-mib M] "
            "[--output FILE|--no-bfile]\n"
            "       %s --term N [--memory-limit-mib M] "
            "[--output FILE|--no-bfile]\n"
            "       %s --check [--memory-limit-mib M] "
            "[--output FILE|--no-bfile]\n",
            program, program, program);
}

int main(int argc, char **argv)
{
    int n = DEFAULT_N;
    size_t memory_mib = DEFAULT_MEMORY_MIB;
    bool positional = false, term_only = false, check = false;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--memory-limit-mib") == 0) {
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
            n = parse_n(argv[i]);
        } else if (strcmp(argv[i], "--check") == 0) {
            if (term_only || positional || check) die("invalid --check usage");
            check = true;
            n = 7;
        } else if (argv[i][0] == '-') {
            usage(argv[0]);
            return EXIT_FAILURE;
        } else {
            if (positional || term_only || check) die("multiple N arguments");
            positional = true;
            n = parse_n(argv[i]);
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
    for (int k = 0; k <= n; ++k) {
        require_recordable(k);
        const U128 value = sequence_term(k);
        verify_known(k, value);
        print_term(k, value);
        record_term(k, value);
    }
    if (check) fprintf(stderr, "397573_03: self-check passed\n");
    return EXIT_SUCCESS;
}
