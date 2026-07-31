/*
 * A354756 -- exact sparse permanent implementation.
 *
 * Count permutations sigma of {1,...,n} satisfying
 *
 *     lcm(i, sigma(i)) <= n
 *
 * for every i.  The corresponding 0-1 matrix is symmetric and sparse:
 *
 *     A[i,j] = 1  iff  lcm(i,j) <= n.
 *
 * The matrix remains sparse and symmetric.  In particular, after the indices
 * 1..floor(n/2) and floor(n/2)+1..n are grouped, the lower-right block is an
 * identity matrix.  Instead of a dense 2^n array, the primary
 * algorithm recursively counts perfect matchings of the sparse bipartite
 * graph.  It eliminates degree-one vertices, factors disconnected balanced
 * components, branches on a minimum-degree vertex, and memoizes only reached
 * pairs of row/column masks.  A separate sparse layer DP is retained for an
 * independent check through n=40.
 *
 * The graph algorithm is evaluated modulo several fixed 31-bit primes, one
 * pass at a time, and CRT reconstructs the exact permanent.  The primes are
 * deliberately different from those used by 354756_03.c.  A rigorous product
 * of row degrees bounds the answer; CRT reconstruction is accepted only when
 * the selected prime product exceeds that bound and every residue rechecks.
 * Storing one uint32_t residue instead of a multi-limb integer cuts the
 * dominant memo storage while keeping this graph algorithm independent of
 * the row-layer DP in 354756_03.c.
 *
 * Safety policy:
 *   - n is limited to 60 because column subsets use uint64_t masks;
 *   - active and transient DP hash-table storage is limited to 2 GiB by
 *     default (override with A354756_MEMORY_MIB);
 *   - allocation failure and bound overflow stop with an error;
 *   - the b-file is written to a .part file beside the executable and is
 *     atomically renamed only after successful completion.
 *
 * Useful invocations:
 *
 *   ./354756_02 --check
 *   ./354756_02 --term 38
 *   ./354756_02 35
 *   ./354756_02 39 39   # use built-in known terms 0..38; compute only 39
 *
 * The normal output file is b354756_1.txt beside the executable, regardless
 * of the current working directory.
 */

#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <gmp.h>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#define DEFAULT_MAX_N 30
#define DEFAULT_CHECK_N 30
#define INDEPENDENT_LAYER_CHECK_MAX_N 40
#define MAX_SUPPORTED_N 60
#define KNOWN_MAX_N 38
#define DEFAULT_MEMORY_MIB UINT64_C(2048)
#define MIN_MEMORY_MIB UINT64_C(16)
#define MAX_MEMORY_MIB UINT64_C(65536)
#define INITIAL_ENTRY_CAPACITY ((size_t)16)
#define INITIAL_TABLE_CAPACITY ((size_t)32)
#define ROW_SELECTION_SAMPLES ((size_t)65536)
#define GRAPH_MAX_MODULUS_COUNT ((size_t)6)

static const uint32_t graph_moduli[GRAPH_MAX_MODULUS_COUNT] = {
    UINT32_C(2147483647),
    UINT32_C(2147483629),
    UINT32_C(2147483587),
    UINT32_C(2147483579),
    UINT32_C(2147483563),
    UINT32_C(2147483549)
};

static const char *const known_terms[KNOWN_MAX_N + 1] = {
    "1",
    "1",
    "2",
    "3",
    "8",
    "10",
    "56",
    "64",
    "192",
    "332",
    "1184",
    "1264",
    "12192",
    "12872",
    "37568",
    "100836",
    "311760",
    "322320",
    "2338368",
    "2408848",
    "14433408",
    "32058912",
    "76931008",
    "78528704",
    "919469408",
    "1158792224",
    "2689828672",
    "4675217824",
    "21679173184",
    "21984820864",
    "381078324992",
    "386159441600",
    "1202247415040",
    "2207841138624",
    "4860086689536",
    "8681783534848",
    "112777175188224",
    "113878087417856",
    "247857779387904"
};

typedef struct {
    uint64_t mask;
    unsigned degree;
    unsigned value;
} SparseRow;

static unsigned gcd_positive(unsigned left, unsigned right)
{
    while (right != 0) {
        unsigned remainder = left % right;
        left = right;
        right = remainder;
    }
    return left;
}

static bool compatible_pair(int left, int right, int n)
{
    unsigned divisor = gcd_positive((unsigned)left, (unsigned)right);
    uint64_t lcm = ((uint64_t)(unsigned)left / divisor) *
                   (uint64_t)(unsigned)right;
    return lcm <= (uint64_t)(unsigned)n;
}

typedef struct {
    uint64_t *states;    /* dense list of used-column masks */
    uint64_t *values;    /* dense consecutive fixed-limb values */
    uint32_t *slots;     /* dense index+1; zero means empty */
    size_t entry_capacity;
    size_t table_capacity;
    size_t count;
    size_t limbs;
} StateMap;

typedef struct {
    uint64_t total_states;
    uint64_t transitions;
    size_t peak_states;
    uint64_t peak_bytes;
    uint64_t required_bytes;
    size_t limbs;
    size_t bound_bits;
    double seconds;
} DpStats;

typedef enum {
    DP_OK = 0,
    DP_MEMORY_LIMIT,
    DP_ALLOCATION_FAILURE,
    DP_ARITHMETIC_OVERFLOW,
    DP_INTERNAL_ERROR
} DpStatus;

static void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static int parse_n(const char *text, const char *label)
{
    char *end = NULL;
    long value;

    errno = 0;
    value = strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' ||
        value < 0 || value > MAX_SUPPORTED_N) {
        fprintf(stderr, "error: %s must be in 0..%d: %s\n",
                label, MAX_SUPPORTED_N, text);
        exit(EXIT_FAILURE);
    }
    return (int)value;
}

static uint64_t memory_budget_bytes(void)
{
    const char *text = getenv("A354756_MEMORY_MIB");
    uint64_t mib = DEFAULT_MEMORY_MIB;

    if (text != NULL && *text != '\0') {
        char *end = NULL;
        unsigned long long parsed;

        errno = 0;
        parsed = strtoull(text, &end, 10);
        if (errno != 0 || end == text || *end != '\0' ||
            parsed < MIN_MEMORY_MIB || parsed > MAX_MEMORY_MIB) {
            fprintf(stderr,
                    "error: A354756_MEMORY_MIB must be in %" PRIu64
                    "..%" PRIu64 ": %s\n",
                    MIN_MEMORY_MIB, MAX_MEMORY_MIB, text);
            exit(EXIT_FAILURE);
        }
        mib = (uint64_t)parsed;
    }
    return mib << 20;
}

static char *path_beside_executable(const char *argv0,
                                    const char *filename)
{
    char executable[PATH_MAX];
    char resolved[PATH_MAX];
    bool have_path = false;

#ifdef __APPLE__
    uint32_t size = (uint32_t)sizeof(executable);
    if (_NSGetExecutablePath(executable, &size) == 0) {
        have_path = true;
    }
#elif defined(__linux__)
    ssize_t length = readlink("/proc/self/exe", executable,
                              sizeof(executable) - 1);
    if (length >= 0) {
        executable[length] = '\0';
        have_path = true;
    }
#endif

    if (!have_path) {
        size_t length = strlen(argv0);
        if (length >= sizeof(executable)) {
            die("executable path is too long");
        }
        memcpy(executable, argv0, length + 1);
    }

    const char *base = realpath(executable, resolved);
    if (base == NULL) {
        base = executable;
    }
    const char *slash = strrchr(base, '/');
    size_t directory_length =
        slash == NULL ? 1 : (size_t)(slash - base);
    size_t filename_length = strlen(filename);
    char *path = malloc(directory_length + 1 + filename_length + 1);
    if (path == NULL) {
        die("could not allocate the output path");
    }

    if (slash == NULL) {
        path[0] = '.';
    } else if (directory_length == 0) {
        path[0] = '/';
        directory_length = 1;
    } else {
        memcpy(path, base, directory_length);
    }
    path[directory_length] = '/';
    memcpy(path + directory_length + 1, filename,
           filename_length + 1);
    return path;
}

static double monotonic_seconds(void)
{
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) != 0) {
        return 0.0;
    }
    return (double)now.tv_sec + (double)now.tv_nsec * 1.0e-9;
}

static uint64_t mix64(uint64_t value)
{
    value += UINT64_C(0x9e3779b97f4a7c15);
    value = (value ^ (value >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    value = (value ^ (value >> 27)) * UINT64_C(0x94d049bb133111eb);
    return value ^ (value >> 31);
}

static bool checked_add_u64(uint64_t a, uint64_t b, uint64_t *result)
{
    if (UINT64_MAX - a < b) {
        return false;
    }
    *result = a + b;
    return true;
}

static bool map_storage_bytes(size_t entry_capacity,
                              size_t table_capacity, size_t limbs,
                              uint64_t *bytes)
{
    if (limbs == 0 ||
        entry_capacity > SIZE_MAX / sizeof(uint64_t) ||
        entry_capacity > SIZE_MAX / limbs ||
        entry_capacity * limbs > SIZE_MAX / sizeof(uint64_t) ||
        table_capacity > SIZE_MAX / sizeof(uint32_t)) {
        return false;
    }

    uint64_t state_bytes =
        (uint64_t)entry_capacity * sizeof(uint64_t);
    uint64_t value_bytes =
        (uint64_t)entry_capacity * (uint64_t)limbs *
        sizeof(uint64_t);
    uint64_t slot_bytes =
        (uint64_t)table_capacity * sizeof(uint32_t);
    uint64_t partial;
    return checked_add_u64(state_bytes, value_bytes, &partial) &&
           checked_add_u64(partial, slot_bytes, bytes);
}

static uint64_t map_bytes(const StateMap *map)
{
    uint64_t bytes = 0;
    if (!map_storage_bytes(map->entry_capacity,
                           map->table_capacity, map->limbs,
                           &bytes)) {
        return UINT64_MAX;
    }
    return bytes;
}

static bool map_allocate_empty(StateMap *map, size_t entry_capacity,
                               size_t table_capacity, size_t limbs)
{
    uint64_t ignored;
    if (entry_capacity < INITIAL_ENTRY_CAPACITY ||
        table_capacity < INITIAL_TABLE_CAPACITY ||
        (table_capacity & (table_capacity - 1)) != 0 ||
        !map_storage_bytes(entry_capacity, table_capacity, limbs,
                           &ignored)) {
        return false;
    }

    uint64_t *states = malloc(entry_capacity * sizeof(*states));
    uint64_t *values =
        malloc(entry_capacity * limbs * sizeof(*values));
    uint32_t *slots = calloc(table_capacity, sizeof(*slots));
    if (states == NULL || values == NULL || slots == NULL) {
        free(slots);
        free(values);
        free(states);
        return false;
    }

    map->states = states;
    map->values = values;
    map->slots = slots;
    map->entry_capacity = entry_capacity;
    map->table_capacity = table_capacity;
    map->count = 0;
    map->limbs = limbs;
    return true;
}

static void map_destroy(StateMap *map)
{
    free(map->slots);
    free(map->values);
    free(map->states);
    memset(map, 0, sizeof(*map));
}

static void map_clear(StateMap *map)
{
    memset(map->slots, 0,
           map->table_capacity * sizeof(*map->slots));
    map->count = 0;
}

static DpStatus map_rehash_slots(StateMap *map,
                                 size_t new_table_capacity,
                                 uint64_t other_bytes,
                                 uint64_t memory_budget,
                                 DpStats *stats)
{
    uint64_t old_bytes = map_bytes(map);
    uint64_t new_slot_bytes =
        (uint64_t)new_table_capacity * sizeof(uint32_t);
    uint64_t partial;
    uint64_t transient;

    if (new_table_capacity < map->table_capacity ||
        (new_table_capacity & (new_table_capacity - 1)) != 0 ||
        new_table_capacity > SIZE_MAX / sizeof(uint32_t) ||
        !checked_add_u64(other_bytes, old_bytes, &partial) ||
        !checked_add_u64(partial, new_slot_bytes, &transient)) {
        stats->required_bytes = UINT64_MAX;
        return DP_MEMORY_LIMIT;
    }
    if (transient > memory_budget) {
        stats->required_bytes = transient;
        return DP_MEMORY_LIMIT;
    }
    if (transient > stats->peak_bytes) {
        stats->peak_bytes = transient;
    }

    uint32_t *new_slots =
        calloc(new_table_capacity, sizeof(*new_slots));
    if (new_slots == NULL) {
        stats->required_bytes = transient;
        return DP_ALLOCATION_FAILURE;
    }

    size_t table_mask = new_table_capacity - 1;
    for (size_t entry = 0; entry < map->count; ++entry) {
        size_t slot =
            (size_t)mix64(map->states[entry]) & table_mask;
        while (new_slots[slot] != 0) {
            slot = (slot + 1) & table_mask;
        }
        new_slots[slot] = (uint32_t)(entry + 1);
    }

    free(map->slots);
    map->slots = new_slots;
    map->table_capacity = new_table_capacity;
    return DP_OK;
}

static DpStatus map_grow_entries(StateMap *map,
                                 uint64_t other_bytes,
                                 uint64_t memory_budget,
                                 DpStats *stats)
{
    /* A 25% growth step limits transient copy memory on the 8-GB target. */
    size_t increase = map->entry_capacity / 4;
    if (increase < 1) {
        increase = 1;
    }
    if (map->entry_capacity > SIZE_MAX - increase) {
        stats->required_bytes = UINT64_MAX;
        return DP_MEMORY_LIMIT;
    }
    size_t new_capacity = map->entry_capacity + increase;
    if (new_capacity >= (size_t)UINT32_MAX) {
        stats->required_bytes = UINT64_MAX;
        return DP_MEMORY_LIMIT;
    }

    uint64_t old_bytes = map_bytes(map);
    uint64_t new_state_bytes =
        (uint64_t)new_capacity * sizeof(uint64_t);
    uint64_t new_value_bytes =
        (uint64_t)new_capacity * (uint64_t)map->limbs *
        sizeof(uint64_t);
    uint64_t partial;
    uint64_t transient;
    if (!checked_add_u64(other_bytes, old_bytes, &partial) ||
        !checked_add_u64(partial, new_state_bytes, &transient) ||
        !checked_add_u64(transient, new_value_bytes, &transient)) {
        stats->required_bytes = UINT64_MAX;
        return DP_MEMORY_LIMIT;
    }
    if (transient > memory_budget) {
        stats->required_bytes = transient;
        return DP_MEMORY_LIMIT;
    }
    if (transient > stats->peak_bytes) {
        stats->peak_bytes = transient;
    }

    uint64_t *new_states =
        malloc(new_capacity * sizeof(*new_states));
    uint64_t *new_values =
        malloc(new_capacity * map->limbs * sizeof(*new_values));
    if (new_states == NULL || new_values == NULL) {
        free(new_values);
        free(new_states);
        stats->required_bytes = transient;
        return DP_ALLOCATION_FAILURE;
    }
    memcpy(new_states, map->states,
           map->count * sizeof(*new_states));
    memcpy(new_values, map->values,
           map->count * map->limbs * sizeof(*new_values));
    free(map->values);
    free(map->states);
    map->states = new_states;
    map->values = new_values;
    map->entry_capacity = new_capacity;
    return DP_OK;
}

static DpStatus map_find_or_insert(StateMap *map, uint64_t state,
                                   const uint64_t *source,
                                   uint64_t other_bytes,
                                   uint64_t memory_budget,
                                   DpStats *stats)
{
    for (;;) {
        size_t table_mask = map->table_capacity - 1;
        size_t slot = (size_t)mix64(state) & table_mask;

        while (map->slots[slot] != 0) {
            size_t entry = (size_t)map->slots[slot] - 1;
            if (map->states[entry] == state) {
                uint64_t carry = 0;
                uint64_t *destination =
                    map->values + entry * map->limbs;
                for (size_t limb = 0; limb < map->limbs; ++limb) {
                    __uint128_t sum =
                        (__uint128_t)destination[limb] +
                        source[limb] + carry;
                    destination[limb] = (uint64_t)sum;
                    carry = (uint64_t)(sum >> 64);
                }
                return carry == 0 ? DP_OK
                                  : DP_ARITHMETIC_OVERFLOW;
            }
            slot = (slot + 1) & table_mask;
        }

        size_t maximum_load =
            map->table_capacity - map->table_capacity / 5;
        if (map->count + 1 > maximum_load) {
            if (map->table_capacity > SIZE_MAX / 2) {
                stats->required_bytes = UINT64_MAX;
                return DP_MEMORY_LIMIT;
            }
            DpStatus status = map_rehash_slots(
                map, map->table_capacity * 2, other_bytes,
                memory_budget, stats);
            if (status != DP_OK) {
                return status;
            }
            continue;
        }
        if (map->count == map->entry_capacity) {
            DpStatus status = map_grow_entries(
                map, other_bytes, memory_budget, stats);
            if (status != DP_OK) {
                return status;
            }
        }
        if (map->count >= (size_t)UINT32_MAX - 1) {
            stats->required_bytes = UINT64_MAX;
            return DP_MEMORY_LIMIT;
        }

        size_t entry = map->count++;
        map->states[entry] = state;
        memcpy(map->values + entry * map->limbs, source,
               map->limbs * sizeof(*source));
        map->slots[slot] = (uint32_t)(entry + 1);
        return DP_OK;
    }
}

static int compare_rows(const void *left, const void *right)
{
    const SparseRow *a = left;
    const SparseRow *b = right;
    if (a->degree != b->degree) {
        return a->degree < b->degree ? -1 : 1;
    }
    if (a->mask != b->mask) {
        /* Descending mask is a useful deterministic tie-break heuristic. */
        return a->mask > b->mask ? -1 : 1;
    }
    if (a->value != b->value) {
        return a->value < b->value ? -1 : 1;
    }
    return 0;
}

static SparseRow *build_rows(int n)
{
    SparseRow *rows = calloc((size_t)n, sizeof(*rows));
    if (rows == NULL) {
        return NULL;
    }

    for (int row = 1; row <= n; ++row) {
        uint64_t mask = 0;
        for (int column = 1; column <= n; ++column) {
            if (compatible_pair(row, column, n)) {
                mask |= UINT64_C(1) << (column - 1);
            }
        }
        rows[row - 1].mask = mask;
        rows[row - 1].degree =
            (unsigned)__builtin_popcountll(mask);
        rows[row - 1].value = (unsigned)row;
    }
    qsort(rows, (size_t)n, sizeof(*rows), compare_rows);
    return rows;
}

static void make_path_bound(mpz_t bound, const SparseRow *rows, int n)
{
    mpz_set_ui(bound, 1);
    for (int position = 0; position < n; ++position) {
        if (rows[position].degree == 0) {
            mpz_set_ui(bound, 0);
            return;
        }
        mpz_mul_ui(bound, bound,
                  (unsigned long)rows[position].degree);
    }
}

/*
 * Select one row for the whole next DP layer.  Using a single row for every
 * state keeps the processed-row set implicit in the layer number, so the DP
 * key remains only the used-column mask.  The score is the number of outgoing
 * transitions over a deterministic hash-uniform sample of current states.
 */
static int choose_next_row(const SparseRow *rows, const bool *processed,
                           int n, const StateMap *current)
{
    uint64_t scores[MAX_SUPPORTED_N] = { 0 };
    size_t samples = current->count < ROW_SELECTION_SAMPLES
                         ? current->count
                         : ROW_SELECTION_SAMPLES;

    for (size_t sample = 0; sample < samples; ++sample) {
        size_t entry = samples == current->count
                           ? sample
                           : (sample * current->count) / samples;
        uint64_t used = current->states[entry];
        for (int row = 0; row < n; ++row) {
            if (!processed[row]) {
                scores[row] += (uint64_t)__builtin_popcountll(
                    rows[row].mask & ~used);
            }
        }
    }

    int best = -1;
    for (int row = 0; row < n; ++row) {
        if (processed[row]) {
            continue;
        }
        /* rows[] is already sorted by the deterministic tie-break rule. */
        if (best < 0 || scores[row] < scores[best]) {
            best = row;
        }
    }
    return best;
}

static void limbs_to_mpz(mpz_t result, const uint64_t *limbs,
                         size_t count)
{
    mpz_import(result, count, -1, sizeof(*limbs), 0, 0, limbs);
}

static const char *dp_status_text(DpStatus status)
{
    switch (status) {
    case DP_OK:
        return "ok";
    case DP_MEMORY_LIMIT:
        return "DP memory limit exceeded";
    case DP_ALLOCATION_FAILURE:
        return "DP allocation failed";
    case DP_ARITHMETIC_OVERFLOW:
        return "fixed-limb DP overflowed its rigorous bound";
    case DP_INTERNAL_ERROR:
        return "internal DP consistency check failed";
    }
    return "unknown DP error";
}

static DpStatus permanent_layer_exact(mpz_t result, int n,
                                      uint64_t memory_budget,
                                      bool report, DpStats *stats)
{
    memset(stats, 0, sizeof(*stats));
    double start_time = monotonic_seconds();

    if (n == 0) {
        mpz_set_ui(result, 1);
        stats->total_states = 1;
        stats->peak_states = 1;
        stats->limbs = 1;
        stats->bound_bits = 1;
        stats->seconds = monotonic_seconds() - start_time;
        return DP_OK;
    }

    SparseRow *rows = build_rows(n);
    if (rows == NULL) {
        return DP_ALLOCATION_FAILURE;
    }

    mpz_t upper_bound;
    mpz_init(upper_bound);
    make_path_bound(upper_bound, rows, n);
    stats->bound_bits = mpz_sizeinbase(upper_bound, 2);
    stats->limbs = (stats->bound_bits + 63) / 64;
    if (stats->limbs == 0) {
        stats->limbs = 1;
    }

    StateMap current = { 0 };
    StateMap next = { 0 };
    if (!map_allocate_empty(&current, INITIAL_ENTRY_CAPACITY,
                            INITIAL_TABLE_CAPACITY, stats->limbs) ||
        !map_allocate_empty(&next, INITIAL_ENTRY_CAPACITY,
                            INITIAL_TABLE_CAPACITY, stats->limbs)) {
        map_destroy(&next);
        map_destroy(&current);
        mpz_clear(upper_bound);
        free(rows);
        return DP_ALLOCATION_FAILURE;
    }

    uint64_t initial_bytes = UINT64_MAX;
    if (!checked_add_u64(map_bytes(&current), map_bytes(&next),
                         &initial_bytes) ||
        initial_bytes > memory_budget) {
        stats->required_bytes = initial_bytes;
        map_destroy(&next);
        map_destroy(&current);
        mpz_clear(upper_bound);
        free(rows);
        return DP_MEMORY_LIMIT;
    }
    stats->peak_bytes = initial_bytes;

    current.states[0] = 0;
    memset(current.values, 0,
           current.limbs * sizeof(*current.values));
    current.values[0] = 1;
    size_t initial_slot = (size_t)mix64(0) &
                          (current.table_capacity - 1);
    current.slots[initial_slot] = 1;
    current.count = 1;
    stats->total_states = 1;
    stats->peak_states = 1;

    bool processed[MAX_SUPPORTED_N] = { false };

    if (report) {
        fprintf(stderr,
                "354756_02: n=%d, bound=%zu bits, %zu limb%s, "
                "memory limit=%.2f GiB\n",
                n, stats->bound_bits, stats->limbs,
                stats->limbs == 1 ? "" : "s",
                (double)memory_budget /
                    (double)(UINT64_C(1) << 30));
    }

    DpStatus status = DP_OK;
    for (int position = 0; position < n; ++position) {
        int row_index = choose_next_row(rows, processed, n, &current);
        if (row_index < 0) {
            status = DP_INTERNAL_ERROR;
            goto cleanup;
        }
        processed[row_index] = true;
        const SparseRow *active_row = &rows[row_index];
        map_clear(&next);
        uint64_t other_bytes = map_bytes(&current);

        for (size_t entry = 0; entry < current.count; ++entry) {
            uint64_t used = current.states[entry];
            uint64_t available = active_row->mask & ~used;
            const uint64_t *source =
                current.values + entry * current.limbs;

            while (available != 0) {
                uint64_t bit = available & (UINT64_C(0) - available);
                available ^= bit;
                if (stats->transitions != UINT64_MAX) {
                    ++stats->transitions;
                }
                status = map_find_or_insert(
                    &next, used | bit, source, other_bytes,
                    memory_budget, stats);
                if (status != DP_OK) {
                    goto cleanup;
                }
            }
        }

        StateMap temporary = current;
        current = next;
        next = temporary;

        if (current.count > stats->peak_states) {
            stats->peak_states = current.count;
        }
        if (UINT64_MAX - stats->total_states < current.count) {
            stats->total_states = UINT64_MAX;
        } else {
            stats->total_states += (uint64_t)current.count;
        }

        uint64_t active_bytes;
        if (checked_add_u64(map_bytes(&current), map_bytes(&next),
                            &active_bytes) &&
            active_bytes > stats->peak_bytes) {
            stats->peak_bytes = active_bytes;
        }

        if (report && (n < 40 || position == 0 ||
                       position + 1 == n ||
                       (position + 1) % 2 == 0)) {
            fprintf(stderr,
                    "354756_02: n=%d layer=%d/%d row=%u "
                    "states=%zu allocated=%.2f GiB\n",
                    n, position + 1, n, active_row->value,
                    current.count,
                    (double)active_bytes /
                        (double)(UINT64_C(1) << 30));
        }

        if (current.count == 0) {
            mpz_set_ui(result, 0);
            status = DP_OK;
            goto verify;
        }
    }

    {
        uint64_t full_mask = (UINT64_C(1) << n) - 1;
        if (current.count != 1 || current.states[0] != full_mask) {
            status = DP_INTERNAL_ERROR;
            goto cleanup;
        }
        limbs_to_mpz(result, current.values, current.limbs);
    }

verify:
    if (mpz_sgn(result) < 0 || mpz_cmp(result, upper_bound) > 0) {
        status = DP_INTERNAL_ERROR;
        goto cleanup;
    }

cleanup:
    stats->seconds = monotonic_seconds() - start_time;
    map_destroy(&next);
    map_destroy(&current);
    mpz_clear(upper_bound);
    free(rows);
    return status;
}

/*
 * Primary algorithm: recursive sparse perfect-matching DP.
 *
 * A state is a pair (remaining row mask, remaining column mask).  Degree-one
 * vertices are eliminated, disconnected balanced bipartite components are
 * multiplied independently, and a minimum-degree row or column is used for
 * branching.  Symmetry of the lcm-compatibility matrix identifies (R,C) with
 * (C,R).  This uses substantially less memory than the layer DP at n>=40.
 */

#define MEMO_REPORT_STEP ((size_t)1000000)

typedef struct {
    uint64_t *row_masks;
    uint64_t *column_masks;
    uint32_t *values;
    uint32_t *slots;
    size_t entry_capacity;
    size_t table_capacity;
    size_t count;
} PairMemo;

typedef struct {
    uint64_t adjacency[MAX_SUPPORTED_N];
    int n;
    uint32_t modulus;
    uint64_t memory_budget;
    PairMemo memo;
    DpStats *stats;
    bool report;
    size_t next_report;
} GraphSolver;

static uint64_t pair_hash(uint64_t rows, uint64_t columns)
{
    uint64_t right = mix64(columns + UINT64_C(0xd6e8feb86659fd93));
    return mix64(rows) ^ ((right << 29) | (right >> 35));
}

static bool pair_storage_bytes(size_t entry_capacity,
                               size_t table_capacity,
                               uint64_t *bytes)
{
    if (entry_capacity > SIZE_MAX / sizeof(uint64_t) ||
        entry_capacity > SIZE_MAX / sizeof(uint32_t) ||
        table_capacity > SIZE_MAX / sizeof(uint32_t)) {
        return false;
    }
    uint64_t key_bytes =
        (uint64_t)entry_capacity * 2 * sizeof(uint64_t);
    uint64_t value_bytes =
        (uint64_t)entry_capacity * sizeof(uint32_t);
    uint64_t slot_bytes =
        (uint64_t)table_capacity * sizeof(uint32_t);
    uint64_t partial;
    return checked_add_u64(key_bytes, value_bytes, &partial) &&
           checked_add_u64(partial, slot_bytes, bytes);
}

static uint64_t pair_memo_bytes(const PairMemo *memo)
{
    uint64_t bytes = 0;
    if (!pair_storage_bytes(memo->entry_capacity,
                            memo->table_capacity, &bytes)) {
        return UINT64_MAX;
    }
    return bytes;
}

static bool pair_memo_init(PairMemo *memo)
{
    memset(memo, 0, sizeof(*memo));
    memo->entry_capacity = INITIAL_ENTRY_CAPACITY;
    memo->table_capacity = INITIAL_TABLE_CAPACITY;
    memo->row_masks =
        malloc(memo->entry_capacity * sizeof(*memo->row_masks));
    memo->column_masks =
        malloc(memo->entry_capacity * sizeof(*memo->column_masks));
    memo->values = malloc(memo->entry_capacity *
                          sizeof(*memo->values));
    memo->slots =
        calloc(memo->table_capacity, sizeof(*memo->slots));
    if (memo->row_masks == NULL || memo->column_masks == NULL ||
        memo->values == NULL || memo->slots == NULL) {
        free(memo->slots);
        free(memo->values);
        free(memo->column_masks);
        free(memo->row_masks);
        memset(memo, 0, sizeof(*memo));
        return false;
    }
    return true;
}

static void pair_memo_destroy(PairMemo *memo)
{
    free(memo->slots);
    free(memo->values);
    free(memo->column_masks);
    free(memo->row_masks);
    memset(memo, 0, sizeof(*memo));
}

static DpStatus pair_memo_rehash(GraphSolver *solver,
                                 size_t new_table_capacity)
{
    PairMemo *memo = &solver->memo;
    uint64_t old_bytes = pair_memo_bytes(memo);
    uint64_t new_slot_bytes =
        (uint64_t)new_table_capacity * sizeof(uint32_t);
    uint64_t transient;

    if (new_table_capacity < memo->table_capacity ||
        (new_table_capacity & (new_table_capacity - 1)) != 0 ||
        new_table_capacity > SIZE_MAX / sizeof(uint32_t) ||
        !checked_add_u64(old_bytes, new_slot_bytes, &transient)) {
        solver->stats->required_bytes = UINT64_MAX;
        return DP_MEMORY_LIMIT;
    }
    if (transient > solver->memory_budget) {
        solver->stats->required_bytes = transient;
        return DP_MEMORY_LIMIT;
    }
    if (transient > solver->stats->peak_bytes) {
        solver->stats->peak_bytes = transient;
    }

    uint32_t *new_slots =
        calloc(new_table_capacity, sizeof(*new_slots));
    if (new_slots == NULL) {
        solver->stats->required_bytes = transient;
        return DP_ALLOCATION_FAILURE;
    }
    size_t table_mask = new_table_capacity - 1;
    for (size_t entry = 0; entry < memo->count; ++entry) {
        size_t slot = (size_t)pair_hash(memo->row_masks[entry],
                                        memo->column_masks[entry]) &
                      table_mask;
        while (new_slots[slot] != 0) {
            slot = (slot + 1) & table_mask;
        }
        new_slots[slot] = (uint32_t)(entry + 1);
    }
    free(memo->slots);
    memo->slots = new_slots;
    memo->table_capacity = new_table_capacity;
    return DP_OK;
}

static DpStatus pair_memo_grow_entries(GraphSolver *solver)
{
    PairMemo *memo = &solver->memo;
    size_t increase = memo->entry_capacity / 4;
    if (increase < 1) {
        increase = 1;
    }
    if (memo->entry_capacity > SIZE_MAX - increase) {
        solver->stats->required_bytes = UINT64_MAX;
        return DP_MEMORY_LIMIT;
    }
    size_t new_capacity = memo->entry_capacity + increase;
    if (new_capacity >= (size_t)UINT32_MAX) {
        solver->stats->required_bytes = UINT64_MAX;
        return DP_MEMORY_LIMIT;
    }

    uint64_t old_bytes = pair_memo_bytes(memo);
    uint64_t old_row_bytes =
        (uint64_t)memo->entry_capacity * sizeof(*memo->row_masks);
    uint64_t old_column_bytes =
        (uint64_t)memo->entry_capacity * sizeof(*memo->column_masks);
    uint64_t old_value_bytes =
        (uint64_t)memo->entry_capacity * sizeof(*memo->values);
    uint64_t new_key_bytes =
        (uint64_t)new_capacity * 2 * sizeof(uint64_t);
    uint64_t new_row_bytes = new_key_bytes / 2;
    uint64_t new_column_bytes = new_key_bytes / 2;
    uint64_t new_value_bytes =
        (uint64_t)new_capacity * sizeof(*memo->values);

    /*
     * Grow the three dense arrays separately.  A realloc that has to move an
     * array may briefly retain that old array while allocating its
     * replacement, but it does not require copies of all three replacements
     * at once.  The conservative peaks below account for that behavior.
     */
    uint64_t after_rows;
    uint64_t after_columns;
    uint64_t row_peak;
    uint64_t column_peak;
    uint64_t value_peak;
    if (!checked_add_u64(old_bytes, new_row_bytes, &row_peak) ||
        old_bytes < old_row_bytes ||
        !checked_add_u64(old_bytes - old_row_bytes,
                         new_row_bytes, &after_rows) ||
        !checked_add_u64(after_rows, new_column_bytes, &column_peak) ||
        after_rows < old_column_bytes ||
        !checked_add_u64(after_rows - old_column_bytes,
                         new_column_bytes, &after_columns) ||
        !checked_add_u64(after_columns, new_value_bytes, &value_peak) ||
        after_columns < old_value_bytes) {
        solver->stats->required_bytes = UINT64_MAX;
        return DP_MEMORY_LIMIT;
    }
    uint64_t transient = row_peak;
    if (column_peak > transient) {
        transient = column_peak;
    }
    if (value_peak > transient) {
        transient = value_peak;
    }
    if (transient > solver->memory_budget) {
        solver->stats->required_bytes = transient;
        return DP_MEMORY_LIMIT;
    }
    if (transient > solver->stats->peak_bytes) {
        solver->stats->peak_bytes = transient;
    }

    uint64_t *new_rows = realloc(memo->row_masks,
                                 new_capacity * sizeof(*new_rows));
    if (new_rows == NULL) {
        solver->stats->required_bytes = transient;
        return DP_ALLOCATION_FAILURE;
    }
    memo->row_masks = new_rows;

    uint64_t *new_columns = realloc(
        memo->column_masks, new_capacity * sizeof(*new_columns));
    if (new_columns == NULL) {
        solver->stats->required_bytes = transient;
        return DP_ALLOCATION_FAILURE;
    }
    memo->column_masks = new_columns;

    uint32_t *new_values = realloc(
        memo->values, new_capacity * sizeof(*new_values));
    if (new_values == NULL) {
        solver->stats->required_bytes = transient;
        return DP_ALLOCATION_FAILURE;
    }
    memo->values = new_values;
    memo->entry_capacity = new_capacity;
    return DP_OK;
}

static bool pair_memo_lookup(const PairMemo *memo, uint64_t rows,
                             uint64_t columns, uint32_t *result)
{
    size_t table_mask = memo->table_capacity - 1;
    size_t slot =
        (size_t)pair_hash(rows, columns) & table_mask;
    while (memo->slots[slot] != 0) {
        size_t entry = (size_t)memo->slots[slot] - 1;
        if (memo->row_masks[entry] == rows &&
            memo->column_masks[entry] == columns) {
            *result = memo->values[entry];
            return true;
        }
        slot = (slot + 1) & table_mask;
    }
    return false;
}

static DpStatus pair_memo_store(GraphSolver *solver, uint64_t rows,
                                uint64_t columns,
                                uint32_t value)
{
    PairMemo *memo = &solver->memo;
    for (;;) {
        size_t table_mask = memo->table_capacity - 1;
        size_t slot =
            (size_t)pair_hash(rows, columns) & table_mask;
        while (memo->slots[slot] != 0) {
            size_t entry = (size_t)memo->slots[slot] - 1;
            if (memo->row_masks[entry] == rows &&
                memo->column_masks[entry] == columns) {
                memo->values[entry] = value;
                return DP_OK;
            }
            slot = (slot + 1) & table_mask;
        }

        size_t maximum_load =
            memo->table_capacity - memo->table_capacity / 5;
        if (memo->count + 1 > maximum_load) {
            if (memo->table_capacity > SIZE_MAX / 2) {
                solver->stats->required_bytes = UINT64_MAX;
                return DP_MEMORY_LIMIT;
            }
            DpStatus status = pair_memo_rehash(
                solver, memo->table_capacity * 2);
            if (status != DP_OK) {
                return status;
            }
            continue;
        }
        if (memo->count == memo->entry_capacity) {
            DpStatus status = pair_memo_grow_entries(solver);
            if (status != DP_OK) {
                return status;
            }
        }
        if (memo->count >= (size_t)UINT32_MAX - 1) {
            solver->stats->required_bytes = UINT64_MAX;
            return DP_MEMORY_LIMIT;
        }

        size_t entry = memo->count++;
        memo->row_masks[entry] = rows;
        memo->column_masks[entry] = columns;
        memo->values[entry] = value;
        memo->slots[slot] = (uint32_t)(entry + 1);
        if (memo->count > solver->stats->peak_states) {
            solver->stats->peak_states = memo->count;
        }

        if (solver->report && memo->count >= solver->next_report) {
            fprintf(stderr,
                    "354756_02: n=%d memo=%zu calls=%" PRIu64
                    " allocated=%.2f GiB\n",
                    solver->n, memo->count,
                    solver->stats->total_states,
                    (double)pair_memo_bytes(memo) /
                        (double)(UINT64_C(1) << 30));
            solver->next_report += MEMO_REPORT_STEP;
        }
        return DP_OK;
    }
}

static uint32_t residue_add(uint32_t left, uint32_t right,
                            uint32_t modulus)
{
    uint64_t sum = (uint64_t)left + right;
    if (sum >= modulus) {
        sum -= modulus;
    }
    return (uint32_t)sum;
}

static uint32_t residue_multiply(uint32_t left, uint32_t right,
                                 uint32_t modulus)
{
    return (uint32_t)(((uint64_t)left * right) % modulus);
}

static uint64_t neighbor_union(const GraphSolver *solver,
                               uint64_t vertices,
                               uint64_t allowed_targets)
{
    uint64_t neighbors = 0;
    while (vertices != 0) {
        uint64_t bit = vertices & (UINT64_C(0) - vertices);
        vertices ^= bit;
        unsigned index = (unsigned)__builtin_ctzll(bit);
        neighbors |= solver->adjacency[index];
    }
    return neighbors & allowed_targets;
}

static DpStatus solve_graph_state(GraphSolver *solver, uint64_t rows,
                                  uint64_t columns, uint32_t *result)
{
    if (solver->stats->total_states != UINT64_MAX) {
        ++solver->stats->total_states;
    }

    /* Repeatedly eliminate forced degree-one rows and columns. */
    for (;;) {
        bool changed = false;
        uint64_t scan = rows;
        while (scan != 0) {
            uint64_t row_bit = scan & (UINT64_C(0) - scan);
            scan ^= row_bit;
            unsigned row = (unsigned)__builtin_ctzll(row_bit);
            uint64_t choices =
                solver->adjacency[row] & columns;
            unsigned degree =
                (unsigned)__builtin_popcountll(choices);
            if (degree == 0) {
                *result = 0;
                return DP_OK;
            }
            if (degree == 1) {
                rows ^= row_bit;
                columns ^= choices;
                changed = true;
                break;
            }
        }
        if (changed) {
            if (rows == 0) {
                *result = 1;
                return DP_OK;
            }
            continue;
        }

        scan = columns;
        while (scan != 0) {
            uint64_t column_bit = scan & (UINT64_C(0) - scan);
            scan ^= column_bit;
            unsigned column =
                (unsigned)__builtin_ctzll(column_bit);
            uint64_t choices =
                solver->adjacency[column] & rows;
            unsigned degree =
                (unsigned)__builtin_popcountll(choices);
            if (degree == 0) {
                *result = 0;
                return DP_OK;
            }
            if (degree == 1) {
                columns ^= column_bit;
                rows ^= choices;
                changed = true;
                break;
            }
        }
        if (!changed) {
            break;
        }
        if (rows == 0) {
            *result = 1;
            return DP_OK;
        }
    }

    if (__builtin_popcountll(rows) !=
        __builtin_popcountll(columns)) {
        *result = 0;
        return DP_OK;
    }
    if (rows > columns) {
        uint64_t temporary = rows;
        rows = columns;
        columns = temporary;
    }
    if (pair_memo_lookup(&solver->memo, rows, columns, result)) {
        return DP_OK;
    }

    /* Find the bipartite connected component of the first row. */
    uint64_t component_rows = rows & (UINT64_C(0) - rows);
    uint64_t component_columns = 0;
    uint64_t frontier_rows = component_rows;
    while (frontier_rows != 0) {
        uint64_t new_columns =
            neighbor_union(solver, frontier_rows, columns) &
            ~component_columns;
        if (new_columns == 0) {
            break;
        }
        component_columns |= new_columns;
        uint64_t new_rows =
            neighbor_union(solver, new_columns, rows) &
            ~component_rows;
        component_rows |= new_rows;
        frontier_rows = new_rows;
    }

    DpStatus status;
    if (component_rows != rows || component_columns != columns) {
        if (__builtin_popcountll(component_rows) !=
            __builtin_popcountll(component_columns)) {
            *result = 0;
        } else {
            uint32_t left;
            uint32_t right;
            status = solve_graph_state(solver, component_rows,
                                       component_columns, &left);
            if (status != DP_OK) {
                return status;
            }
            status = solve_graph_state(solver,
                                       rows ^ component_rows,
                                       columns ^ component_columns,
                                       &right);
            if (status != DP_OK) {
                return status;
            }
            *result = residue_multiply(left, right, solver->modulus);
        }
        return pair_memo_store(solver, rows, columns, *result);
    }

    /* Branch on a minimum-degree row or column. */
    unsigned best_degree = UINT_MAX;
    bool use_row = true;
    uint64_t selected_bit = 0;
    uint64_t choices = 0;
    uint64_t scan = rows;
    while (scan != 0) {
        uint64_t bit = scan & (UINT64_C(0) - scan);
        scan ^= bit;
        unsigned index = (unsigned)__builtin_ctzll(bit);
        uint64_t available = solver->adjacency[index] & columns;
        unsigned degree =
            (unsigned)__builtin_popcountll(available);
        if (degree < best_degree) {
            best_degree = degree;
            use_row = true;
            selected_bit = bit;
            choices = available;
        }
    }
    scan = columns;
    while (scan != 0) {
        uint64_t bit = scan & (UINT64_C(0) - scan);
        scan ^= bit;
        unsigned index = (unsigned)__builtin_ctzll(bit);
        uint64_t available = solver->adjacency[index] & rows;
        unsigned degree =
            (unsigned)__builtin_popcountll(available);
        if (degree < best_degree) {
            best_degree = degree;
            use_row = false;
            selected_bit = bit;
            choices = available;
        }
    }

    *result = 0;
    while (choices != 0) {
        uint64_t choice = choices & (UINT64_C(0) - choices);
        choices ^= choice;
        if (solver->stats->transitions != UINT64_MAX) {
            ++solver->stats->transitions;
        }
        uint32_t term;
        status = use_row
                     ? solve_graph_state(solver,
                                         rows ^ selected_bit,
                                         columns ^ choice, &term)
                     : solve_graph_state(solver, rows ^ choice,
                                         columns ^ selected_bit, &term);
        if (status != DP_OK) {
            return status;
        }
        *result = residue_add(*result, term, solver->modulus);
    }
    return pair_memo_store(solver, rows, columns, *result);
}

static DpStatus permanent_modular(uint32_t *residue, int n,
                                  uint32_t modulus,
                                  uint64_t memory_budget, bool report,
                                  DpStats *stats)
{
    memset(stats, 0, sizeof(*stats));
    double start_time = monotonic_seconds();
    if (n == 0) {
        *residue = 1;
        stats->total_states = 1;
        stats->peak_states = 1;
        stats->limbs = 1;
        stats->seconds = monotonic_seconds() - start_time;
        return DP_OK;
    }

    stats->limbs = 1;

    GraphSolver solver;
    memset(&solver, 0, sizeof(solver));
    solver.n = n;
    solver.modulus = modulus;
    solver.memory_budget = memory_budget;
    solver.stats = stats;
    solver.report = report;
    solver.next_report = MEMO_REPORT_STEP;
    for (int i = 0; i < n; ++i) {
        uint64_t mask = 0;
        for (int j = 0; j < n; ++j) {
            int left = i + 1;
            int right = j + 1;
            if (compatible_pair(left, right, n)) {
                mask |= UINT64_C(1) << j;
            }
        }
        solver.adjacency[i] = mask;
    }
    if (!pair_memo_init(&solver.memo)) {
        return DP_ALLOCATION_FAILURE;
    }
    stats->peak_bytes = pair_memo_bytes(&solver.memo);
    if (stats->peak_bytes > memory_budget) {
        stats->required_bytes = stats->peak_bytes;
        pair_memo_destroy(&solver.memo);
        return DP_MEMORY_LIMIT;
    }

    if (report) {
        fprintf(stderr,
                "354756_02: n=%d graph modular DP, p=%" PRIu32
                ", memory limit=%.2f GiB\n",
                n, modulus,
                (double)memory_budget /
                    (double)(UINT64_C(1) << 30));
    }

    uint32_t value = 0;
    uint64_t full_mask = (UINT64_C(1) << n) - 1;
    DpStatus status =
        solve_graph_state(&solver, full_mask, full_mask, &value);
    if (status == DP_OK) {
        *residue = value;
    }

    stats->peak_states = solver.memo.count > stats->peak_states
                             ? solver.memo.count
                             : stats->peak_states;
    uint64_t active_bytes = pair_memo_bytes(&solver.memo);
    if (active_bytes > stats->peak_bytes) {
        stats->peak_bytes = active_bytes;
    }
    stats->seconds = monotonic_seconds() - start_time;
    pair_memo_destroy(&solver.memo);
    return status;
}

static size_t choose_graph_moduli(const mpz_t bound, mpz_t product)
{
    mpz_set_ui(product, 1);
    for (size_t count = 1; count <= GRAPH_MAX_MODULUS_COUNT; ++count) {
        mpz_mul_ui(product, product,
                   (unsigned long)graph_moduli[count - 1]);
        if (mpz_cmp(product, bound) > 0) {
            return count;
        }
    }
    return 0;
}

static uint32_t residue_power(uint32_t base, uint32_t exponent,
                              uint32_t modulus)
{
    uint64_t result = 1;
    uint64_t factor = base;
    while (exponent != 0) {
        if ((exponent & 1U) != 0) {
            result = (result * factor) % modulus;
        }
        factor = (factor * factor) % modulus;
        exponent >>= 1;
    }
    return (uint32_t)result;
}

static bool reconstruct_graph_crt(mpz_t result, mpz_t product,
                                  const uint32_t *residues,
                                  size_t modulus_count)
{
    mpz_set_ui(result, 0);
    mpz_set_ui(product, 1);
    for (size_t index = 0; index < modulus_count; ++index) {
        uint32_t modulus = graph_moduli[index];
        uint32_t product_mod =
            (uint32_t)mpz_fdiv_ui(product, (unsigned long)modulus);
        if (product_mod == 0) {
            return false;
        }
        uint32_t inverse =
            residue_power(product_mod, modulus - 2, modulus);
        uint32_t result_mod =
            (uint32_t)mpz_fdiv_ui(result, (unsigned long)modulus);
        uint64_t difference =
            ((uint64_t)residues[index] + modulus - result_mod) % modulus;
        unsigned long multiplier =
            (unsigned long)((difference * inverse) % modulus);
        mpz_addmul_ui(result, product, multiplier);
        mpz_mul_ui(product, product, (unsigned long)modulus);
    }

    for (size_t index = 0; index < modulus_count; ++index) {
        if (mpz_fdiv_ui(result,
                        (unsigned long)graph_moduli[index]) !=
            residues[index]) {
            return false;
        }
    }
    return true;
}

static uint64_t saturating_add_u64(uint64_t left, uint64_t right)
{
    return UINT64_MAX - left < right ? UINT64_MAX : left + right;
}

static DpStatus permanent_exact(mpz_t result, int n,
                                uint64_t memory_budget, bool report,
                                DpStats *stats)
{
    memset(stats, 0, sizeof(*stats));

    SparseRow *rows = build_rows(n);
    if (rows == NULL && n != 0) {
        return DP_ALLOCATION_FAILURE;
    }
    mpz_t upper_bound;
    mpz_t selected_product;
    mpz_t reconstructed_product;
    mpz_inits(upper_bound, selected_product, reconstructed_product, NULL);
    make_path_bound(upper_bound, rows, n);
    free(rows);

    stats->bound_bits = mpz_sizeinbase(upper_bound, 2);
    stats->limbs = 1;
    size_t modulus_count =
        choose_graph_moduli(upper_bound, selected_product);
    if (modulus_count == 0) {
        mpz_clears(reconstructed_product, selected_product,
                   upper_bound, NULL);
        return DP_INTERNAL_ERROR;
    }

    if (report) {
        fprintf(stderr,
                "354756_02: n=%d exact graph CRT, bound=%zu bits, "
                "%zu sequential prime passes (%zu-bit product), "
                "memory limit=%.2f GiB\n",
                n, stats->bound_bits, modulus_count,
                mpz_sizeinbase(selected_product, 2),
                (double)memory_budget /
                    (double)(UINT64_C(1) << 30));
    }

    uint32_t residues[GRAPH_MAX_MODULUS_COUNT] = { 0 };
    for (size_t index = 0; index < modulus_count; ++index) {
        if (report) {
            fprintf(stderr,
                    "354756_02: n=%d CRT pass %zu/%zu, p=%" PRIu32
                    "\n",
                    n, index + 1, modulus_count,
                    graph_moduli[index]);
        }
        DpStats pass_stats;
        DpStatus status = permanent_modular(
            &residues[index], n, graph_moduli[index], memory_budget,
            report && index == 0, &pass_stats);
        stats->total_states = saturating_add_u64(
            stats->total_states, pass_stats.total_states);
        stats->transitions = saturating_add_u64(
            stats->transitions, pass_stats.transitions);
        if (pass_stats.peak_states > stats->peak_states) {
            stats->peak_states = pass_stats.peak_states;
        }
        if (pass_stats.peak_bytes > stats->peak_bytes) {
            stats->peak_bytes = pass_stats.peak_bytes;
        }
        if (pass_stats.required_bytes > stats->required_bytes) {
            stats->required_bytes = pass_stats.required_bytes;
        }
        stats->seconds += pass_stats.seconds;
        if (status != DP_OK) {
            mpz_clears(reconstructed_product, selected_product,
                       upper_bound, NULL);
            return status;
        }
        if (report) {
            fprintf(stderr,
                    "354756_02: n=%d pass %zu/%zu ok, residue=%" PRIu32
                    ", states=%zu, peak=%.2f GiB, %.3f s\n",
                    n, index + 1, modulus_count, residues[index],
                    pass_stats.peak_states,
                    (double)pass_stats.peak_bytes /
                        (double)(UINT64_C(1) << 30),
                    pass_stats.seconds);
        }
    }

    bool valid = reconstruct_graph_crt(result, reconstructed_product,
                                       residues, modulus_count) &&
                 mpz_cmp(reconstructed_product, upper_bound) > 0 &&
                 mpz_sgn(result) >= 0 &&
                 mpz_cmp(result, upper_bound) <= 0;
    mpz_clears(reconstructed_product, selected_product,
               upper_bound, NULL);
    return valid ? DP_OK : DP_INTERNAL_ERROR;
}

static void report_failure(int n, DpStatus status,
                           uint64_t memory_budget,
                           const DpStats *stats)
{
    fprintf(stderr, "error: A354756 n=%d: %s\n",
            n, dp_status_text(status));
    if (status == DP_MEMORY_LIMIT ||
        status == DP_ALLOCATION_FAILURE) {
        if (stats->required_bytes != 0 &&
            stats->required_bytes != UINT64_MAX) {
            fprintf(stderr,
                    "required at the failed growth: %.2f GiB; "
                    "configured limit: %.2f GiB\n",
                    (double)stats->required_bytes /
                        (double)(UINT64_C(1) << 30),
                    (double)memory_budget /
                        (double)(UINT64_C(1) << 30));
        }
        fprintf(stderr,
                "Increase A354756_MEMORY_MIB only if the machine has "
                "enough physical RAM.\n");
    }
}

static void verify_known_value(const mpz_t value, int n)
{
    if (n < 0 || n > KNOWN_MAX_N) {
        return;
    }
    mpz_t expected;
    mpz_init(expected);
    if (mpz_set_str(expected, known_terms[n], 10) != 0) {
        mpz_clear(expected);
        die("invalid built-in A354756 term");
    }
    if (mpz_cmp(value, expected) != 0) {
        gmp_fprintf(stderr,
                    "error: known-term check failed at n=%d: "
                    "got %Zd, expected %Zd\n",
                    n, value, expected);
        mpz_clear(expected);
        exit(EXIT_FAILURE);
    }
    mpz_clear(expected);
}

static void compute_checked(mpz_t value, int n,
                            uint64_t memory_budget, bool report)
{
    DpStats stats;
    DpStatus status =
        permanent_exact(value, n, memory_budget, report, &stats);
    if (status != DP_OK) {
        report_failure(n, status, memory_budget, &stats);
        exit(EXIT_FAILURE);
    }
    verify_known_value(value, n);
    if (report) {
        fprintf(stderr,
                "354756_02: n=%d done, peak states=%zu, "
                "total states=%" PRIu64 ", transitions=%" PRIu64
                ", peak allocation=%.2f GiB, %.3f s\n",
                n, stats.peak_states, stats.total_states,
                stats.transitions,
                (double)stats.peak_bytes /
                    (double)(UINT64_C(1) << 30),
                stats.seconds);
    }
}

static int check_known_terms(int max_n, uint64_t memory_budget)
{
    if (max_n > KNOWN_MAX_N) {
        fprintf(stderr,
                "error: --check maximum is %d because later terms "
                "are not built in\n",
                KNOWN_MAX_N);
        return EXIT_FAILURE;
    }

    mpz_t value;
    mpz_t layer_value;
    mpz_init(value);
    mpz_init(layer_value);
    for (int n = 0; n <= max_n; ++n) {
        compute_checked(value, n, memory_budget, false);
        if (n <= INDEPENDENT_LAYER_CHECK_MAX_N) {
            DpStats layer_stats;
            DpStatus layer_status = permanent_layer_exact(
                layer_value, n, memory_budget, false, &layer_stats);
            if (layer_status != DP_OK) {
                report_failure(n, layer_status, memory_budget,
                               &layer_stats);
                mpz_clear(layer_value);
                mpz_clear(value);
                return EXIT_FAILURE;
            }
            if (mpz_cmp(value, layer_value) != 0) {
                gmp_fprintf(
                    stderr,
                    "error: graph and layer DP disagree at n=%d: "
                    "%Zd versus %Zd\n",
                    n, value, layer_value);
                mpz_clear(layer_value);
                mpz_clear(value);
                return EXIT_FAILURE;
            }
        }
    }
    mpz_clear(layer_value);
    mpz_clear(value);
    printf("ok: graph DP agrees with the known A354756 prefix "
           "for n=0..%d; layer DP independently agrees for n=0..%d\n",
           max_n,
           max_n < INDEPENDENT_LAYER_CHECK_MAX_N
               ? max_n : INDEPENDENT_LAYER_CHECK_MAX_N);
    return EXIT_SUCCESS;
}

static void write_known(FILE *stream, int n)
{
    if (n < 0 || n > KNOWN_MAX_N) {
        die("requested built-in term is unavailable");
    }
    if (fprintf(stream, "%d %s\n", n, known_terms[n]) < 0) {
        die("could not write a built-in b-file term");
    }
}

static void produce_b_file(const char *argv0, int max_n, int start_n,
                           uint64_t memory_budget)
{
    if (start_n > max_n + 1 || start_n > KNOWN_MAX_N + 1) {
        fprintf(stderr,
                "error: START_N must be at most min(MAX_N+1,%d)\n",
                KNOWN_MAX_N + 1);
        exit(EXIT_FAILURE);
    }

    char *path = path_beside_executable(argv0, "b354756_1.txt");
    size_t part_size = strlen(path) + strlen(".part") + 1;
    char *part = malloc(part_size);
    if (part == NULL) {
        free(path);
        die("could not allocate the temporary output path");
    }
    snprintf(part, part_size, "%s.part", path);

    FILE *stream = fopen(part, "w");
    if (stream == NULL) {
        fprintf(stderr, "error: cannot open %s: %s\n",
                part, strerror(errno));
        free(part);
        free(path);
        exit(EXIT_FAILURE);
    }

    for (int n = 0; n < start_n && n <= max_n; ++n) {
        write_known(stream, n);
    }
    if (start_n > 0) {
        fprintf(stderr,
                "354756_02: using built-in verified prefix n=0..%d\n",
                start_n - 1);
    }

    mpz_t value;
    mpz_init(value);
    for (int n = start_n; n <= max_n; ++n) {
        compute_checked(value, n, memory_budget, true);
        if (gmp_fprintf(stream, "%d %Zd\n", n, value) < 0 ||
            fflush(stream) != 0) {
            mpz_clear(value);
            fclose(stream);
            free(part);
            free(path);
            die("could not write the A354756 b-file");
        }
    }
    mpz_clear(value);

    if (fclose(stream) != 0) {
        free(part);
        free(path);
        die("could not close the A354756 b-file");
    }
    if (rename(part, path) != 0) {
        fprintf(stderr, "error: cannot rename %s to %s: %s\n",
                part, path, strerror(errno));
        free(part);
        free(path);
        exit(EXIT_FAILURE);
    }
    printf("wrote %s (n=0..%d)\n", path, max_n);
    free(part);
    free(path);
}

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s [MAX_N [START_N]]\n"
            "       %s --term N\n"
            "       %s --check [MAX_N]\n"
            "\n"
            "MAX_N defaults to %d and may be at most %d.\n"
            "START_N defaults to 0.  Terms below START_N are read from\n"
            "the built-in verified prefix, which currently ends at n=%d.\n"
            "The default memory limit is %" PRIu64 " MiB; override it with\n"
            "A354756_MEMORY_MIB.\n",
            program, program, program, DEFAULT_MAX_N,
            MAX_SUPPORTED_N, KNOWN_MAX_N, DEFAULT_MEMORY_MIB);
}

int main(int argc, char **argv)
{
    uint64_t memory_budget = memory_budget_bytes();

    if (argc >= 2 && strcmp(argv[1], "--check") == 0) {
        if (argc > 3) {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
        int max_n = argc == 3
                        ? parse_n(argv[2], "check MAX_N")
                        : DEFAULT_CHECK_N;
        return check_known_terms(max_n, memory_budget);
    }

    if (argc >= 2 && strcmp(argv[1], "--term") == 0) {
        if (argc != 3) {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
        int n = parse_n(argv[2], "N");
        mpz_t value;
        mpz_init(value);
        compute_checked(value, n, memory_budget, true);
        gmp_printf("%d %Zd\n", n, value);
        mpz_clear(value);
        return EXIT_SUCCESS;
    }

    if (argc > 3) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    int max_n = argc >= 2 ? parse_n(argv[1], "MAX_N")
                          : DEFAULT_MAX_N;
    int start_n = argc == 3 ? parse_n(argv[2], "START_N") : 0;
    produce_b_file(argv[0], max_n, start_n, memory_budget);
    return EXIT_SUCCESS;
}
