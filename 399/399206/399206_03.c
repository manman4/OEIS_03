/*
 * A399206: Number of permutations p of [n] such that the n-1 products
 *          p(i)*p(i+1) are all distinct.
 *
 * Version 03 uses a prefix dynamic program, not the inclusion-exclusion over
 * linear forests used by 399206_02.c.
 *
 * Build:
 *   cc -O3 -std=c11 -Wall -Wextra -Wpedantic 399206_03.c -o 399206_03
 *
 * Examples:
 *   ./399206_03                 # n=0..15; updates b399206_03.txt
 *   ./399206_03 16 --stats
 *   ./399206_03 --target 15     # earlier b-file prefix required
 *   ./399206_03 15 --no-bfile
 *   ./399206_03 --self-test 10
 *
 * Prefix DP
 * ---------
 * An unordered edge {u,v} has color u*v.  Product equality is edge-color
 * equality, so the wanted permutations are exactly the oriented Hamilton
 * paths with no repeated edge color.
 *
 * A DP state consists of
 *
 *   (S, v, C),
 *
 * where S is the set of vertices already used, v is the last vertex, and C
 * is the set of repeated product colors already used.  Its value is the
 * number of prefixes represented by that exact state.  A color belonging to
 * only one edge is not stored: because vertices are never reused, that edge
 * itself cannot occur twice.
 *
 * The state is made substantially smaller by exact relevance forgetting.
 * From (S,v), a future edge is either {v,x} with x outside S, or has both
 * endpoints outside S.  A used color having no such edge can never conflict
 * later and is deleted from C.  After an extension the set of possible future
 * edges only shrinks: the old endpoint v becomes unavailable, while every
 * possible edge at the new endpoint was previously between two unused
 * vertices.  Hence a forgotten color can never become relevant again.
 *
 * For every unused x, append x unless color(v*x) is already in C; then add
 * that color, discard newly irrelevant colors, and merge equal full states.
 * This is an exhaustive, disjoint construction of every valid permutation.
 * Hash collisions cannot merge states because complete keys are compared.
 * For n<=18 all relevance masks are precomputed.  The practical limit n=20
 * also lets the vertex mask, endpoint, and at most 39 tracked colors share
 * one 64-bit key, while every state count is bounded by 20! < 2^64.
 *
 * --self-test uses a separate direct permutation recursion without DP,
 * relevance forgetting, or state hashing.
 *
 * Each completed term is checked against the existing contiguous b-file
 * prefix and saved under an exclusive lock through fsync plus atomic rename.
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
#error "399206_03.c requires unsigned __int128"
#endif

__extension__ typedef unsigned __int128 u128;

#define MAX_N 20U
#define DEFAULT_N 15U
#define DEFAULT_SELF_TEST_N 10U
#define MAX_SELF_TEST_N 11U
#define FUTURE_TABLE_MAX_N 18U
#define MAX_PRODUCT (MAX_N * MAX_N)
#define LAST_BITS 5U
#define VISITED_BITS MAX_N
#define COLOR_SHIFT (LAST_BITS + VISITED_BITS)
#define MAX_TRACKED_COLORS (64U - COLOR_SHIFT)
#define VISITED_MASK ((UINT32_C(1) << VISITED_BITS) - 1U)
#define U128_MAX_VALUE (~(u128)0)

_Static_assert(MAX_N < 32U, "vertex masks require fewer than 32 bits");
_Static_assert(MAX_N <= (UINT32_C(1) << LAST_BITS),
               "LAST_BITS cannot encode every vertex");
_Static_assert(COLOR_SHIFT < 64U, "the state fields do not fit uint64_t");

static const char *output_path = "b399206_03.txt";
static bool write_bfile = true;

typedef struct {
    uint64_t *keys;
    uint64_t *values;
    size_t capacity;
    size_t size;
} StateMap;

typedef struct {
    unsigned n;
    uint64_t used_vertices;
    bool used_product[MAX_PRODUCT + 1U];
    u128 count;
} DirectSearch;

typedef struct {
    uint64_t transitions;
    size_t peak_states;
    size_t peak_slots;
    size_t auxiliary_bytes;
    unsigned repeated_colors;
} Statistics;

static _Noreturn void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static void u128_text(u128 value, char text[40])
{
    char reverse[40];
    size_t length = 0U;
    do {
        reverse[length++] = (char)('0' + (unsigned)(value % 10U));
        value /= 10U;
    } while (value != 0U);
    for (size_t i = 0U; i < length; ++i)
        text[i] = reverse[length - 1U - i];
    text[length] = '\0';
}

static void print_u128(FILE *stream, u128 value)
{
    char text[40];
    u128_text(value, text);
    if (fputs(text, stream) == EOF)
        die("cannot write an integer");
}

static void *xcalloc(size_t count, size_t size)
{
    if (size != 0U && count > SIZE_MAX / size)
        die("allocation size overflow");
    void *memory = calloc(count, size);
    if (memory == NULL)
        die("out of memory");
    return memory;
}

static unsigned bit_index_u32(uint32_t bit)
{
#if defined(__clang__) || defined(__GNUC__)
    return (unsigned)__builtin_ctz(bit);
#else
    unsigned index = 0U;
    while ((bit & 1U) == 0U) {
        bit >>= 1U;
        ++index;
    }
    return index;
#endif
}

static size_t hash_u64(uint64_t key)
{
    key ^= key >> 30U;
    key *= UINT64_C(0xbf58476d1ce4e5b9);
    key ^= key >> 27U;
    key *= UINT64_C(0x94d049bb133111eb);
    key ^= key >> 31U;
    return (size_t)key;
}

static void map_insert_raw(StateMap *map, uint64_t key, uint64_t value)
{
    size_t position = hash_u64(key) & (map->capacity - 1U);
    while (map->keys[position] != 0U)
        position = (position + 1U) & (map->capacity - 1U);
    map->keys[position] = key;
    map->values[position] = value;
    ++map->size;
}

static void map_grow(StateMap *map)
{
    const size_t old_capacity = map->capacity;
    uint64_t *old_keys = map->keys;
    uint64_t *old_values = map->values;
    if (old_capacity != 0U && old_capacity > SIZE_MAX / 2U)
        die("state-table capacity overflow");
    map->capacity = old_capacity == 0U ? 16U : 2U * old_capacity;
    map->keys = xcalloc(map->capacity, sizeof(*map->keys));
    map->values = xcalloc(map->capacity, sizeof(*map->values));
    const size_t old_size = map->size;
    map->size = 0U;
    for (size_t i = 0U; i < old_capacity; ++i) {
        if (old_keys[i] != 0U)
            map_insert_raw(map, old_keys[i], old_values[i]);
    }
    free(old_values);
    free(old_keys);
    if (map->size != old_size)
        die("state-table rehash lost an entry");
}

static void map_add(StateMap *map, uint64_t key, uint64_t value)
{
    if (key == 0U)
        die("zero is reserved as the empty state key");
    if (map->size == SIZE_MAX)
        die("state-table size overflow");
    if (map->capacity == 0U)
        map_grow(map);

    size_t position = hash_u64(key) & (map->capacity - 1U);
    while (map->keys[position] != 0U) {
        if (map->keys[position] == key) {
            if (map->values[position] > UINT64_MAX - value)
                die("state count overflow");
            map->values[position] += value;
            return;
        }
        position = (position + 1U) & (map->capacity - 1U);
    }

    /* A duplicate does not need free space.  Grow only for a new state. */
    if (map->size + 1U > map->capacity - map->capacity / 4U) {
        map_grow(map);
        position = hash_u64(key) & (map->capacity - 1U);
        while (map->keys[position] != 0U)
            position = (position + 1U) & (map->capacity - 1U);
    }
    map->keys[position] = key;
    map->values[position] = value;
    ++map->size;
}

static void map_clear(StateMap *map)
{
    free(map->values);
    free(map->keys);
    memset(map, 0, sizeof(*map));
}

static uint64_t encode_state(uint32_t visited, unsigned last,
                             uint64_t used_colors)
{
    return (used_colors << COLOR_SHIFT) |
           ((uint64_t)visited << LAST_BITS) | (uint64_t)last;
}

static void decode_state(uint64_t key, uint32_t *visited, unsigned *last,
                         uint64_t *used_colors)
{
    *last = (unsigned)(key & ((UINT32_C(1) << LAST_BITS) - 1U));
    *visited = (uint32_t)((key >> LAST_BITS) & VISITED_MASK);
    *used_colors = key >> COLOR_SHIFT;
}

static uint64_t future_color_mask(uint32_t remaining, unsigned last,
                                  const uint64_t *inside_colors,
                                  uint64_t edge_color[MAX_N][MAX_N])
{
    uint64_t result = inside_colors[remaining];
    uint32_t scan = remaining;
    while (scan != 0U) {
        const uint32_t bit = scan & (0U - scan);
        scan ^= bit;
        result |= edge_color[last][bit_index_u32(bit)];
    }
    return result;
}

static unsigned make_edge_colors(unsigned n,
                                 uint64_t edge_color[MAX_N][MAX_N])
{
    unsigned multiplicity[MAX_PRODUCT + 1U] = {0};
    for (unsigned u = 0U; u < n; ++u)
        for (unsigned v = u + 1U; v < n; ++v)
            ++multiplicity[(u + 1U) * (v + 1U)];

    int color_index[MAX_PRODUCT + 1U];
    for (unsigned product = 0U; product <= MAX_PRODUCT; ++product)
        color_index[product] = -1;
    unsigned count = 0U;
    for (unsigned product = 1U; product <= n * n; ++product) {
        if (multiplicity[product] > 1U) {
            if (count == MAX_TRACKED_COLORS)
                die("repeated product colors do not fit the state key");
            color_index[product] = (int)count++;
        }
    }

    memset(edge_color, 0, MAX_N * MAX_N * sizeof(edge_color[0][0]));
    for (unsigned u = 0U; u < n; ++u) {
        for (unsigned v = u + 1U; v < n; ++v) {
            const int index = color_index[(u + 1U) * (v + 1U)];
            if (index >= 0)
                edge_color[u][v] = edge_color[v][u] =
                    UINT64_C(1) << (unsigned)index;
        }
    }
    return count;
}

static uint64_t *make_inside_colors(unsigned n,
                                    uint64_t edge_color[MAX_N][MAX_N])
{
    const size_t subset_count = (size_t)1U << n;
    uint64_t *inside = xcalloc(subset_count, sizeof(*inside));
    for (uint32_t mask = 1U; mask < (uint32_t)subset_count; ++mask) {
        const uint32_t bit = mask & (0U - mask);
        const unsigned vertex = bit_index_u32(bit);
        const uint32_t rest = mask ^ bit;
        uint64_t colors = inside[rest];
        uint32_t scan = rest;
        while (scan != 0U) {
            const uint32_t other_bit = scan & (0U - scan);
            scan ^= other_bit;
            colors |= edge_color[vertex][bit_index_u32(other_bit)];
        }
        inside[mask] = colors;
    }
    return inside;
}

/*
 * future[last,mask] contains every repeated color on an edge that can still
 * be used when last is the current endpoint and mask is the unused set.
 * For the practical range this turns relevance forgetting into one lookup.
 */
static uint64_t *make_future_colors(
    unsigned n, const uint64_t *inside_colors,
    uint64_t edge_color[MAX_N][MAX_N])
{
    if (n > FUTURE_TABLE_MAX_N)
        return NULL;
    const size_t subset_count = (size_t)1U << n;
    if ((size_t)n > SIZE_MAX / subset_count)
        die("future-color table size overflow");
    const size_t entries = (size_t)n * subset_count;
    uint64_t *future = xcalloc(entries, sizeof(*future));
    for (unsigned last = 0U; last < n; ++last) {
        uint64_t *row = future + (size_t)last * subset_count;
        for (uint32_t mask = 1U; mask < (uint32_t)subset_count; ++mask) {
            const uint32_t bit = mask & (0U - mask);
            const uint32_t rest = mask ^ bit;
            row[mask] = inside_colors[mask] | row[rest] |
                        edge_color[last][bit_index_u32(bit)];
        }
    }
    return future;
}

static u128 factorial(unsigned n)
{
    u128 value = 1U;
    for (unsigned i = 2U; i <= n; ++i) {
        if (value > U128_MAX_VALUE / i)
            die("factorial overflow");
        value *= i;
    }
    return value;
}

static u128 calculate(unsigned n, Statistics *statistics, bool show_layers)
{
    memset(statistics, 0, sizeof(*statistics));
    if (n == 0U)
        return 1U;
    const u128 permutation_bound = factorial(n);
    if (permutation_bound > UINT64_MAX)
        die("state counts do not fit uint64_t");

    uint64_t edge_color[MAX_N][MAX_N];
    statistics->repeated_colors = make_edge_colors(n, edge_color);
    const uint32_t full = (UINT32_C(1) << n) - 1U;
    const size_t subset_count = (size_t)1U << n;
    uint64_t *inside_colors = make_inside_colors(n, edge_color);
    uint64_t *future_colors =
        make_future_colors(n, inside_colors, edge_color);
    statistics->auxiliary_bytes = subset_count * sizeof(*inside_colors);
    if (future_colors != NULL) {
        if ((size_t)n >
            (SIZE_MAX - statistics->auxiliary_bytes) /
                (subset_count * sizeof(*future_colors)))
            die("auxiliary-memory statistic overflow");
        statistics->auxiliary_bytes +=
            (size_t)n * subset_count * sizeof(*future_colors);
    }

    StateMap current = {0}, next = {0};
    for (unsigned vertex = 0U; vertex < n; ++vertex)
        map_add(&current,
                encode_state(UINT32_C(1) << vertex, vertex, 0U), 1U);
    statistics->peak_states = current.size;
    statistics->peak_slots = current.capacity;

    for (unsigned length = 1U; length < n; ++length) {
        for (size_t slot = 0U; slot < current.capacity; ++slot) {
            const uint64_t key = current.keys[slot];
            if (key == 0U)
                continue;
            uint32_t visited;
            unsigned last;
            uint64_t used_colors;
            decode_state(key, &visited, &last, &used_colors);
            const uint32_t unused = full ^ visited;
            uint32_t choices = unused;
            while (choices != 0U) {
                const uint32_t bit = choices & (0U - choices);
                choices ^= bit;
                const unsigned vertex = bit_index_u32(bit);
                const uint64_t color = edge_color[last][vertex];
                if (color != 0U && (used_colors & color) != 0U)
                    continue;
                if (statistics->transitions == UINT64_MAX)
                    die("transition statistic overflow");
                ++statistics->transitions;
                const uint32_t new_visited = visited | bit;
                const uint32_t remaining = full ^ new_visited;
                const uint64_t relevant = future_colors != NULL
                    ? future_colors[(size_t)vertex * subset_count + remaining]
                    : future_color_mask(remaining, vertex, inside_colors,
                                        edge_color);
                const uint64_t new_colors =
                    (used_colors | color) & relevant;
                map_add(&next,
                        encode_state(new_visited, vertex, new_colors),
                        current.values[slot]);
            }
        }

        if (current.capacity > SIZE_MAX - next.capacity)
            die("state-table slot statistic overflow");
        const size_t simultaneous_slots = current.capacity + next.capacity;
        if (next.size > statistics->peak_states)
            statistics->peak_states = next.size;
        if (simultaneous_slots > statistics->peak_slots)
            statistics->peak_slots = simultaneous_slots;
        if (show_layers) {
            const double memory_mib =
                ((double)simultaneous_slots *
                     (double)(sizeof(uint64_t) + sizeof(uint64_t)) +
                 (double)statistics->auxiliary_bytes) /
                (1024.0 * 1024.0);
            if (fprintf(stderr,
                        "  length=%u states=%zu slots=%zu, approx %.1f MiB\n",
                        length + 1U, next.size, simultaneous_slots,
                        memory_mib) < 0)
                die("cannot write layer statistics");
        }
        map_clear(&current);
        current = next;
        memset(&next, 0, sizeof(next));
    }

    u128 result = 0U;
    for (size_t slot = 0U; slot < current.capacity; ++slot) {
        if (current.keys[slot] == 0U)
            continue;
        if (result > U128_MAX_VALUE - current.values[slot])
            die("answer overflow");
        result += current.values[slot];
    }
    if (result > permutation_bound)
        die("DP result exceeds n!");
    map_clear(&current);
    free(future_colors);
    free(inside_colors);
    return result;
}

/* Independent definition-level oracle used only by --self-test. */
static void direct_recursion(DirectSearch *search, unsigned depth,
                             unsigned last)
{
    if (depth == search->n) {
        if (search->count == U128_MAX_VALUE)
            die("direct-search count overflow");
        ++search->count;
        return;
    }
    for (unsigned vertex = 1U; vertex <= search->n; ++vertex) {
        const uint64_t bit = UINT64_C(1) << (vertex - 1U);
        if ((search->used_vertices & bit) != 0U)
            continue;
        const unsigned product = depth == 0U ? 0U : last * vertex;
        if (depth != 0U && search->used_product[product])
            continue;
        search->used_vertices |= bit;
        if (depth != 0U)
            search->used_product[product] = true;
        direct_recursion(search, depth + 1U, vertex);
        if (depth != 0U)
            search->used_product[product] = false;
        search->used_vertices ^= bit;
    }
}

static u128 calculate_direct(unsigned n)
{
    DirectSearch search;
    memset(&search, 0, sizeof(search));
    search.n = n;
    direct_recursion(&search, 0U, 0U);
    return search.count;
}

/* ---------------- Atomic, resumable b-file output ---------------- */

static bool parse_u128_token(const char *text, const char **end, u128 *value)
{
    if (*text < '0' || *text > '9')
        return false;
    u128 result = 0U;
    do {
        const unsigned digit = (unsigned)(*text - '0');
        if (result > (U128_MAX_VALUE - digit) / 10U)
            return false;
        result = 10U * result + digit;
        ++text;
    } while (*text >= '0' && *text <= '9');
    *end = text;
    *value = result;
    return true;
}

static char *path_with_suffix(const char *suffix)
{
    const size_t path_length = strlen(output_path);
    const size_t suffix_length = strlen(suffix);
    if (path_length > SIZE_MAX - suffix_length - 1U)
        die("output path is too long");
    char *path = malloc(path_length + suffix_length + 1U);
    if (path == NULL)
        die("cannot allocate a derived output path");
    memcpy(path, output_path, path_length);
    memcpy(path + path_length, suffix, suffix_length + 1U);
    return path;
}

static int lock_bfile(void)
{
    char *lock_path = path_with_suffix(".lock");
    const int descriptor = open(lock_path, O_RDWR | O_CREAT, 0666);
    free(lock_path);
    if (descriptor < 0)
        die("cannot open the b-file lock");
    struct flock lock = {.l_type = F_WRLCK, .l_whence = SEEK_SET};
    while (fcntl(descriptor, F_SETLKW, &lock) != 0) {
        if (errno != EINTR) {
            close(descriptor);
            die("cannot lock the b-file");
        }
    }
    return descriptor;
}

static void unlock_bfile(int descriptor)
{
    struct flock lock = {.l_type = F_UNLCK, .l_whence = SEEK_SET};
    if (fcntl(descriptor, F_SETLK, &lock) != 0 || close(descriptor) != 0)
        die("cannot unlock the b-file");
}

static unsigned read_bfile(u128 values[MAX_N + 1U])
{
    FILE *input = fopen(output_path, "r");
    if (input == NULL) {
        if (errno == ENOENT)
            return 0U;
        die("cannot read the b-file");
    }
    char line[256];
    unsigned next = 0U;
    while (fgets(line, sizeof(line), input) != NULL) {
        const size_t length = strlen(line);
        if (length == sizeof(line) - 1U && line[length - 1U] != '\n') {
            fclose(input);
            die("the b-file contains an overlong line");
        }
        char *cursor = line;
        while (*cursor == ' ' || *cursor == '\t')
            ++cursor;
        if (*cursor == '\0' || *cursor == '\n' || *cursor == '#')
            continue;
        if (*cursor < '0' || *cursor > '9') {
            fclose(input);
            die("the b-file contains an invalid index");
        }
        errno = 0;
        char *index_end = NULL;
        const uintmax_t index = strtoumax(cursor, &index_end, 10);
        if (errno == ERANGE || index_end == cursor || index > MAX_N ||
            index != next) {
            fclose(input);
            die("the b-file is malformed or has a gap");
        }
        cursor = index_end;
        if (*cursor != ' ' && *cursor != '\t') {
            fclose(input);
            die("the b-file is malformed or has a gap");
        }
        while (*cursor == ' ' || *cursor == '\t')
            ++cursor;
        const char *value_end = NULL;
        u128 value;
        if (!parse_u128_token(cursor, &value_end, &value)) {
            fclose(input);
            die("the b-file contains an invalid value");
        }
        cursor = (char *)value_end;
        while (*cursor == ' ' || *cursor == '\t')
            ++cursor;
        if (*cursor == '\r')
            ++cursor;
        if (*cursor == '\n')
            ++cursor;
        if (*cursor != '\0') {
            fclose(input);
            die("the b-file contains trailing data");
        }
        values[next++] = value;
    }
    if (ferror(input) || fclose(input) != 0)
        die("cannot finish reading the b-file");
    return next;
}

static void validate_bfile(void)
{
    if (!write_bfile)
        return;
    u128 values[MAX_N + 1U];
    const int descriptor = lock_bfile();
    (void)read_bfile(values);
    unlock_bfile(descriptor);
}

static void record_term(unsigned n, u128 value)
{
    if (!write_bfile)
        return;
    u128 values[MAX_N + 1U];
    const int lock_descriptor = lock_bfile();
    unsigned prefix = read_bfile(values);
    if (n < prefix) {
        if (values[n] != value) {
            unlock_bfile(lock_descriptor);
            die("a computed term disagrees with the b-file");
        }
        unlock_bfile(lock_descriptor);
        return;
    }
    if (n != prefix) {
        unlock_bfile(lock_descriptor);
        die("cannot record a term after a b-file gap");
    }
    values[prefix++] = value;

    char *temporary = path_with_suffix(".tmp.XXXXXX");
    const int temporary_descriptor = mkstemp(temporary);
    if (temporary_descriptor < 0) {
        unlock_bfile(lock_descriptor);
        free(temporary);
        die("cannot create a temporary b-file");
    }
    FILE *output = fdopen(temporary_descriptor, "w");
    if (output == NULL) {
        close(temporary_descriptor);
        unlink(temporary);
        unlock_bfile(lock_descriptor);
        free(temporary);
        die("cannot open the temporary b-file stream");
    }
    bool failed = false;
    for (unsigned index = 0U; index < prefix; ++index) {
        char number[40];
        u128_text(values[index], number);
        if (fprintf(output, "%u %s\n", index, number) < 0)
            failed = true;
    }
    if (!failed && fflush(output) != 0)
        failed = true;
    if (!failed && fsync(fileno(output)) != 0)
        failed = true;
    if (fclose(output) != 0)
        failed = true;
    if (failed || rename(temporary, output_path) != 0) {
        unlink(temporary);
        unlock_bfile(lock_descriptor);
        free(temporary);
        die("cannot atomically update the b-file");
    }
    free(temporary);
    unlock_bfile(lock_descriptor);
}

static unsigned parse_n(const char *text, unsigned maximum,
                        const char *description)
{
    if (text == NULL || *text == '\0')
        die("empty integer argument");
    errno = 0;
    char *end = NULL;
    const uintmax_t value = strtoumax(text, &end, 10);
    if (errno == ERANGE || end == text || *end != '\0' || value > maximum) {
        fprintf(stderr, "error: %s must be in 0..%u: %s\n",
                description, maximum, text);
        exit(EXIT_FAILURE);
    }
    return (unsigned)value;
}

static double monotonic_seconds(void)
{
    struct timespec value;
    if (clock_gettime(CLOCK_MONOTONIC, &value) != 0)
        die("clock_gettime failed");
    return (double)value.tv_sec + 1.0e-9 * (double)value.tv_nsec;
}

static void print_term(unsigned n, u128 value)
{
    if (fprintf(stdout, "%u ", n) < 0)
        die("cannot write the result index");
    print_u128(stdout, value);
    if (fputc('\n', stdout) == EOF)
        die("cannot finish the result line");
}

static void run_self_test(unsigned maximum)
{
    for (unsigned n = 0U; n <= maximum; ++n) {
        Statistics statistics;
        const u128 dynamic = calculate(n, &statistics, false);
        const u128 direct = calculate_direct(n);
        if (dynamic != direct) {
            fprintf(stderr, "self-test mismatch at n=%u: DP=", n);
            print_u128(stderr, dynamic);
            fprintf(stderr, ", direct=");
            print_u128(stderr, direct);
            fputc('\n', stderr);
            exit(EXIT_FAILURE);
        }
        if (n <= 5U && dynamic != factorial(n))
            die("small-n factorial regression failed");
    }
    printf("self-test passed through n=%u\n", maximum);
}

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s [N] [--stats] [--output FILE|--no-bfile]\n"
            "       %s --target N [--stats] [--output FILE|--no-bfile]\n"
            "       %s --self-test [N]\n"
            "N defaults to %u and must be in 0..%u.\n"
            "Each completed term is recorded in %s by default.\n",
            program, program, program, DEFAULT_N, MAX_N, output_path);
}

int main(int argc, char **argv)
{
    unsigned maximum = DEFAULT_N;
    bool target_only = false;
    bool stats = false;
    bool self_test = false;
    bool have_n = false;
    bool output_option = false;
    bool no_bfile_option = false;

    for (int argument = 1; argument < argc; ++argument) {
        if (strcmp(argv[argument], "--help") == 0 ||
            strcmp(argv[argument], "-h") == 0) {
            usage(argv[0]);
            return EXIT_SUCCESS;
        }
        if (strcmp(argv[argument], "--stats") == 0) {
            if (stats)
                die("duplicate --stats");
            stats = true;
            continue;
        }
        if (strcmp(argv[argument], "--output") == 0) {
            if (output_option || argument + 1 >= argc ||
                argv[argument + 1][0] == '\0')
                die("invalid --output");
            output_option = true;
            output_path = argv[++argument];
            continue;
        }
        if (strcmp(argv[argument], "--no-bfile") == 0) {
            if (no_bfile_option)
                die("duplicate --no-bfile");
            no_bfile_option = true;
            continue;
        }
        if (strcmp(argv[argument], "--target") == 0) {
            if (target_only || self_test || have_n || argument + 1 >= argc)
                die("invalid --target");
            target_only = true;
            maximum = parse_n(argv[++argument], MAX_N, "target");
            have_n = true;
            continue;
        }
        if (strcmp(argv[argument], "--self-test") == 0) {
            if (self_test || target_only || have_n)
                die("invalid --self-test");
            self_test = true;
            maximum = DEFAULT_SELF_TEST_N;
            if (argument + 1 < argc && argv[argument + 1][0] != '-') {
                maximum = parse_n(argv[++argument], MAX_SELF_TEST_N,
                                  "self-test limit");
                have_n = true;
            }
            continue;
        }
        if (argv[argument][0] == '-' || have_n || target_only || self_test) {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
        maximum = parse_n(argv[argument], MAX_N, "N");
        have_n = true;
    }

    if (output_option && no_bfile_option)
        die("--output and --no-bfile are mutually exclusive");
    if (self_test) {
        if (output_option)
            die("--output is not used with --self-test");
        if (stats)
            die("--stats is not used with --self-test");
        run_self_test(maximum);
        return EXIT_SUCCESS;
    }
    write_bfile = !no_bfile_option;
    validate_bfile();

    const unsigned first = target_only ? maximum : 0U;
    for (unsigned n = first; n <= maximum; ++n) {
        const double started = monotonic_seconds();
        Statistics statistics;
        const u128 value = calculate(n, &statistics, stats);
        record_term(n, value);
        print_term(n, value);
        if (fflush(stdout) != 0)
            die("cannot flush result output");
        if (stats) {
            const double memory_mib =
                (double)statistics.peak_slots *
                (double)(sizeof(uint64_t) + sizeof(uint64_t)) /
                (1024.0 * 1024.0);
            if (fprintf(stderr,
                        "n=%u: %.3f s, repeated colors=%u, transitions=%" PRIu64
                        ", peak states=%zu, table slots %.1f MiB\n",
                        n, monotonic_seconds() - started,
                        statistics.repeated_colors, statistics.transitions,
                        statistics.peak_states, memory_mib) < 0)
                die("cannot write statistics");
        }
        if (n == maximum)
            break;
    }
    if (write_bfile &&
        fprintf(stderr, "b-file verified/updated: %s\n", output_path) < 0)
        die("cannot write the b-file completion message");
    return EXIT_SUCCESS;
}
