/*
 * A320843 -- independent modular verifier.
 *
 * Count permutations sigma of {1,...,n} satisfying
 *
 *     sigma(i) divides i  or  i divides sigma(i)
 *
 * for every i.  This program deliberately uses a different algorithm from
 * 320843_03.c: a Johnson-Gentleman-style, row-by-row subset DP.  A state is
 * only the mask of columns already selected.  Rows and columns receive the
 * same lexicographic ordering as the reference Perl implementation.
 *
 * Normal term output is exact: primes are added until their product exceeds
 * the rigorous product-of-row-degrees bound, then CRT reconstructs the unique
 * value.  The intended verification is exact-to-exact: 02 never consumes
 * 03's result while computing.  --check compares independently reconstructed
 * exact values with the built-in terms.  --mod and --verify remain optional
 * quick residue diagnostics.
 *
 * To minimize peak memory, different primes are evaluated in separate DP
 * passes.  Only one uint32_t residue is stored per state.  This deliberately
 * trades additional running time for a substantially smaller verifier.
 *
 * The algorithm is exponential and intended only as an independent checker.
 * Its sparse hash tables are memory-limited.  The default limit is 2 GiB and
 * can be changed with A320843_02_MEMORY_MIB.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic \
 *       -I/opt/homebrew/opt/gmp/include -L/opt/homebrew/opt/gmp/lib \
 *       320843_02.c -lgmp -o 320843_02
 *
 * Examples:
 *   ./320843_02 --term 35
 *   ./320843_02 --crt 35
 *   ./320843_02 --mod 35
 *   ./320843_02 --verify 50 107833432035711440
 *   ./320843_02 --check 35
 *   ./320843_02 30 35
 *
 * Normal and --term result lines are exact b-file-style pairs:
 *
 *   n  a(n)
 *
 * A --mod result line is:
 *
 *   n  (a(n) mod 1000000007)  (a(n) mod 1000000009)
 *      (a(n) mod 998244353)
 */

#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <gmp.h>

#define MAX_SUPPORTED_N 60
#define KNOWN_MAX_N 50
#define DEFAULT_CHECK_N 30
#define FINGERPRINT_MODULUS_COUNT 3
#define MAX_MODULUS_COUNT 6
#define INITIAL_ENTRY_CAPACITY ((size_t)16)
#define INITIAL_TABLE_CAPACITY ((size_t)32)
#define DEFAULT_MEMORY_MIB UINT64_C(2048)
#define MIN_MEMORY_MIB UINT64_C(16)
#define MAX_MEMORY_MIB UINT64_C(65536)

static const uint32_t moduli[MAX_MODULUS_COUNT] = {
    UINT32_C(1000000007),
    UINT32_C(1000000009),
    UINT32_C(998244353),
    UINT32_C(996999989),
    UINT32_C(996999973),
    UINT32_C(996999953)
};

static const char *const known_terms[KNOWN_MAX_N + 1] = {
    "1",
    "1",
    "2",
    "3",
    "8",
    "10",
    "36",
    "41",
    "132",
    "250",
    "700",
    "750",
    "4010",
    "4237",
    "10680",
    "24679",
    "87328",
    "90478",
    "435812",
    "449586",
    "1939684",
    "3853278",
    "8650900",
    "8840110",
    "60035322",
    "80605209",
    "177211024",
    "368759752",
    "1380348224",
    "1401414640",
    "8892787136",
    "9014369784",
    "33923638848",
    "59455553072",
    "126536289568",
    "207587882368",
    "1495526775088",
    "1510769105288",
    "3187980614208",
    "5415462995568",
    "29811240618112",
    "30071845984896",
    "167426899579520",
    "168778036632608",
    "543720217208896",
    "1741288345700048",
    "3618889806595872",
    "3643985571635136",
    "28167109438114448",
    "33158989380172192",
    "107833432035711440"
};

typedef enum {
    DP_OK,
    DP_MEMORY_LIMIT,
    DP_ALLOCATION_FAILURE,
    DP_STATE_LIMIT,
    DP_INTERNAL_ERROR
} DpStatus;

typedef struct {
    uint64_t budget;
    uint64_t allocated;
    uint64_t peak;
    uint64_t required;
} MemoryTracker;

typedef struct {
    uint64_t *states;
    uint32_t *values;
    uint32_t *slots;
    size_t count;
    size_t entry_capacity;
    size_t state_capacity;
    size_t value_capacity;
    size_t table_capacity;
    size_t modulus_count;
    const uint32_t *active_moduli;
} StateMap;

typedef struct {
    size_t peak_states;
    uint64_t peak_bytes;
    double seconds;
} DpStats;

static void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static bool checked_add_u64(uint64_t left, uint64_t right,
                            uint64_t *result)
{
    if (left > UINT64_MAX - right) {
        return false;
    }
    *result = left + right;
    return true;
}

static bool checked_mul_size(size_t left, size_t right, size_t *result)
{
    if (left != 0 && right > SIZE_MAX / left) {
        return false;
    }
    *result = left * right;
    return true;
}

static void *tracked_malloc(MemoryTracker *memory, size_t bytes,
                            bool clear)
{
    uint64_t needed;
    if (!checked_add_u64(memory->allocated, (uint64_t)bytes, &needed)) {
        memory->required = UINT64_MAX;
        return NULL;
    }
    if (needed > memory->budget) {
        if (needed > memory->required) {
            memory->required = needed;
        }
        return NULL;
    }

    void *result = clear ? calloc(1, bytes) : malloc(bytes);
    if (result == NULL) {
        if (needed > memory->required) {
            memory->required = needed;
        }
        return NULL;
    }
    memory->allocated = needed;
    if (needed > memory->peak) {
        memory->peak = needed;
    }
    return result;
}

static void tracked_free(MemoryTracker *memory, void *pointer, size_t bytes)
{
    if (pointer != NULL) {
        free(pointer);
        if ((uint64_t)bytes > memory->allocated) {
            die("internal memory-accounting error");
        }
        memory->allocated -= (uint64_t)bytes;
    }
}

static uint64_t mix64(uint64_t value)
{
    value ^= value >> 30;
    value *= UINT64_C(0xbf58476d1ce4e5b9);
    value ^= value >> 27;
    value *= UINT64_C(0x94d049bb133111eb);
    value ^= value >> 31;
    return value;
}

static bool map_byte_sizes(size_t state_capacity, size_t value_capacity,
                           size_t table_capacity,
                           size_t modulus_count,
                           size_t *state_bytes, size_t *value_bytes,
                           size_t *slot_bytes)
{
    size_t values_per_map;
    return checked_mul_size(state_capacity, sizeof(uint64_t), state_bytes) &&
           checked_mul_size(value_capacity, modulus_count,
                            &values_per_map) &&
           checked_mul_size(values_per_map, sizeof(uint32_t), value_bytes) &&
           checked_mul_size(table_capacity, sizeof(uint32_t), slot_bytes);
}

static void map_destroy(StateMap *map, MemoryTracker *memory)
{
    size_t state_bytes = 0;
    size_t value_bytes = 0;
    size_t slot_bytes = 0;
    if (!map_byte_sizes(map->state_capacity, map->value_capacity,
                        map->table_capacity,
                        map->modulus_count,
                        &state_bytes, &value_bytes, &slot_bytes)) {
        die("internal map-size error");
    }
    tracked_free(memory, map->slots, slot_bytes);
    tracked_free(memory, map->values, value_bytes);
    tracked_free(memory, map->states, state_bytes);
    memset(map, 0, sizeof(*map));
}

static DpStatus map_init(StateMap *map, MemoryTracker *memory,
                         const uint32_t *active_moduli,
                         size_t modulus_count)
{
    memset(map, 0, sizeof(*map));
    map->entry_capacity = INITIAL_ENTRY_CAPACITY;
    map->state_capacity = INITIAL_ENTRY_CAPACITY;
    map->value_capacity = INITIAL_ENTRY_CAPACITY;
    map->table_capacity = INITIAL_TABLE_CAPACITY;
    map->modulus_count = modulus_count;
    map->active_moduli = active_moduli;

    size_t state_bytes;
    size_t value_bytes;
    size_t slot_bytes;
    if (!map_byte_sizes(map->state_capacity, map->value_capacity,
                        map->table_capacity,
                        map->modulus_count,
                        &state_bytes, &value_bytes, &slot_bytes)) {
        return DP_MEMORY_LIMIT;
    }

    map->states = tracked_malloc(memory, state_bytes, false);
    map->values = tracked_malloc(memory, value_bytes, false);
    map->slots = tracked_malloc(memory, slot_bytes, true);
    if (map->states == NULL || map->values == NULL || map->slots == NULL) {
        map_destroy(map, memory);
        return memory->required > memory->budget ? DP_MEMORY_LIMIT
                                                : DP_ALLOCATION_FAILURE;
    }
    return DP_OK;
}

static void map_clear(StateMap *map)
{
    memset(map->slots, 0,
           map->table_capacity * sizeof(*map->slots));
    map->count = 0;
}

static DpStatus map_rehash(StateMap *map, MemoryTracker *memory,
                           size_t new_capacity)
{
    if (new_capacity == 0 ||
        (new_capacity & (new_capacity - 1)) != 0) {
        memory->required = UINT64_MAX;
        return DP_MEMORY_LIMIT;
    }
    size_t new_bytes;
    if (!checked_mul_size(new_capacity, sizeof(uint32_t), &new_bytes)) {
        memory->required = UINT64_MAX;
        return DP_MEMORY_LIMIT;
    }
    uint32_t *new_slots = tracked_malloc(memory, new_bytes, true);
    if (new_slots == NULL) {
        return memory->required > memory->budget ? DP_MEMORY_LIMIT
                                                : DP_ALLOCATION_FAILURE;
    }

    size_t new_mask = new_capacity - 1;
    for (size_t entry = 0; entry < map->count; ++entry) {
        size_t slot = (size_t)mix64(map->states[entry]) & new_mask;
        while (new_slots[slot] != 0) {
            slot = (slot + 1) & new_mask;
        }
        new_slots[slot] = (uint32_t)(entry + 1);
    }

    size_t old_bytes = map->table_capacity * sizeof(*map->slots);
    tracked_free(memory, map->slots, old_bytes);
    map->slots = new_slots;
    map->table_capacity = new_capacity;
    return DP_OK;
}

static DpStatus map_grow_entries(StateMap *map, MemoryTracker *memory)
{
    size_t increment = map->entry_capacity / 2;
    if (increment == 0 || map->entry_capacity > SIZE_MAX - increment) {
        memory->required = UINT64_MAX;
        return DP_MEMORY_LIMIT;
    }
    size_t new_capacity = map->entry_capacity + increment;
    size_t new_state_bytes;
    size_t new_value_bytes;
    size_t ignored_slot_bytes;
    if (!map_byte_sizes(new_capacity, new_capacity,
                        map->table_capacity,
                        map->modulus_count,
                        &new_state_bytes, &new_value_bytes,
                        &ignored_slot_bytes)) {
        memory->required = UINT64_MAX;
        return DP_MEMORY_LIMIT;
    }

    /*
     * Grow and release the two dense arrays separately.  This keeps the
     * transient allocation below the old+new size of both arrays together.
     * The physical capacities are tracked independently so cleanup remains
     * correct if the second allocation fails.
     */
    uint64_t *new_states = tracked_malloc(memory, new_state_bytes, false);
    if (new_states == NULL) {
        return memory->required > memory->budget ? DP_MEMORY_LIMIT
                                                : DP_ALLOCATION_FAILURE;
    }
    memcpy(new_states, map->states,
           map->count * sizeof(*new_states));
    size_t old_state_bytes =
        map->state_capacity * sizeof(*map->states);
    tracked_free(memory, map->states, old_state_bytes);
    map->states = new_states;
    map->state_capacity = new_capacity;

    uint32_t *new_values =
        tracked_malloc(memory, new_value_bytes, false);
    if (new_values == NULL) {
        return memory->required > memory->budget ? DP_MEMORY_LIMIT
                                                : DP_ALLOCATION_FAILURE;
    }
    memcpy(new_values, map->values,
           map->count * map->modulus_count * sizeof(*new_values));
    size_t old_value_bytes =
        map->value_capacity * map->modulus_count * sizeof(*map->values);
    tracked_free(memory, map->values, old_value_bytes);
    map->values = new_values;
    map->value_capacity = new_capacity;
    map->entry_capacity = new_capacity;
    return DP_OK;
}

static void add_residues(uint32_t *destination, const uint32_t *source,
                         const uint32_t *active_moduli,
                         size_t modulus_count)
{
    for (size_t index = 0; index < modulus_count; ++index) {
        uint32_t sum = destination[index] + source[index];
        if (sum >= active_moduli[index]) {
            sum -= active_moduli[index];
        }
        destination[index] = sum;
    }
}

static DpStatus map_add(StateMap *map, MemoryTracker *memory,
                        uint64_t state, const uint32_t *value)
{
    for (;;) {
        size_t table_mask = map->table_capacity - 1;
        size_t slot = (size_t)mix64(state) & table_mask;
        while (map->slots[slot] != 0) {
            size_t entry = (size_t)map->slots[slot] - 1;
            if (map->states[entry] == state) {
                add_residues(map->values + entry * map->modulus_count,
                             value, map->active_moduli,
                             map->modulus_count);
                return DP_OK;
            }
            slot = (slot + 1) & table_mask;
        }

        size_t maximum_load =
            map->table_capacity - map->table_capacity / 5;
        if (map->count + 1 > maximum_load) {
            if (map->table_capacity > SIZE_MAX / 2) {
                memory->required = UINT64_MAX;
                return DP_MEMORY_LIMIT;
            }
            DpStatus status =
                map_rehash(map, memory, map->table_capacity * 2);
            if (status != DP_OK) {
                return status;
            }
            continue;
        }
        if (map->count == map->entry_capacity) {
            DpStatus status = map_grow_entries(map, memory);
            if (status != DP_OK) {
                return status;
            }
        }
        if (map->count >= (size_t)UINT32_MAX - 1) {
            return DP_STATE_LIMIT;
        }

        size_t entry = map->count++;
        map->states[entry] = state;
        memcpy(map->values + entry * map->modulus_count, value,
               map->modulus_count * sizeof(*value));
        map->slots[slot] = (uint32_t)(entry + 1);
        return DP_OK;
    }
}

static bool map_lookup(const StateMap *map, uint64_t state,
                       uint32_t *result)
{
    size_t table_mask = map->table_capacity - 1;
    size_t slot = (size_t)mix64(state) & table_mask;
    while (map->slots[slot] != 0) {
        size_t entry = (size_t)map->slots[slot] - 1;
        if (map->states[entry] == state) {
            memcpy(result, map->values + entry * map->modulus_count,
                   map->modulus_count * sizeof(*result));
            return true;
        }
        slot = (slot + 1) & table_mask;
    }
    return false;
}

/* Lexicographic order of the binary string at column positions 0..n-1. */
static int compare_masks_lex(uint64_t left, uint64_t right, int n)
{
    for (int column = 0; column < n; ++column) {
        unsigned left_bit = (unsigned)((left >> column) & 1);
        unsigned right_bit = (unsigned)((right >> column) & 1);
        if (left_bit != right_bit) {
            return left_bit < right_bit ? -1 : 1;
        }
    }
    return 0;
}

static void sort_masks_lex(uint64_t *masks, int n)
{
    /* n <= 60, so stable insertion sort is sufficient and context-free. */
    for (int index = 1; index < n; ++index) {
        uint64_t selected = masks[index];
        int position = index;
        while (position > 0 &&
               compare_masks_lex(selected, masks[position - 1], n) < 0) {
            masks[position] = masks[position - 1];
            --position;
        }
        masks[position] = selected;
    }
}

static void build_ordered_rows(uint64_t *rows, int n)
{
    for (int row = 0; row < n; ++row) {
        uint64_t mask = 0;
        for (int column = 0; column < n; ++column) {
            int left = row + 1;
            int right = column + 1;
            if (left % right == 0 || right % left == 0) {
                mask |= UINT64_C(1) << column;
            }
        }
        rows[row] = mask;
    }

    /* Equivalent to Perl's: slm tm slm @a. */
    sort_masks_lex(rows, n);
    uint64_t transposed[MAX_SUPPORTED_N] = {0};
    for (int column = 0; column < n; ++column) {
        uint64_t mask = 0;
        for (int row = 0; row < n; ++row) {
            if (((rows[row] >> column) & 1) != 0) {
                mask |= UINT64_C(1) << row;
            }
        }
        transposed[column] = mask;
    }
    sort_masks_lex(transposed, n);
    memcpy(rows, transposed, (size_t)n * sizeof(*rows));
}

static double monotonic_seconds(void)
{
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) != 0) {
        die("clock_gettime failed");
    }
    return (double)now.tv_sec + (double)now.tv_nsec / 1.0e9;
}

static DpStatus modular_permanent(int n, uint64_t memory_budget,
                                  bool report,
                                  const uint32_t *active_moduli,
                                  size_t modulus_count,
                                  uint32_t *result, DpStats *stats)
{
    memset(stats, 0, sizeof(*stats));
    double start_time = monotonic_seconds();
    if (modulus_count == 0 || modulus_count > MAX_MODULUS_COUNT) {
        return DP_INTERNAL_ERROR;
    }

    MemoryTracker memory = {
        .budget = memory_budget,
        .allocated = 0,
        .peak = 0,
        .required = 0
    };
    StateMap current;
    StateMap next;
    DpStatus status = map_init(&current, &memory, active_moduli,
                               modulus_count);
    if (status != DP_OK) {
        return status;
    }
    status = map_init(&next, &memory, active_moduli, modulus_count);
    if (status != DP_OK) {
        map_destroy(&current, &memory);
        return status;
    }

    const uint32_t one[MAX_MODULUS_COUNT] = {1, 1, 1, 1, 1, 1};
    status = map_add(&current, &memory, 0, one);
    if (status != DP_OK) {
        goto finished;
    }
    stats->peak_states = current.count;

    uint64_t rows[MAX_SUPPORTED_N] = {0};
    build_ordered_rows(rows, n);
    for (int layer = 0; layer < n; ++layer) {
        map_clear(&next);
        for (size_t entry = 0; entry < current.count; ++entry) {
            uint64_t used = current.states[entry];
            uint64_t available = rows[layer] & ~used;
            const uint32_t *value =
                current.values + entry * modulus_count;
            while (available != 0) {
                uint64_t bit = available & (~available + 1);
                available ^= bit;
                status = map_add(&next, &memory, used | bit, value);
                if (status != DP_OK) {
                    goto finished;
                }
            }
        }

        StateMap temporary = current;
        current = next;
        next = temporary;
        if (current.count > stats->peak_states) {
            stats->peak_states = current.count;
        }
        if (report) {
            fprintf(stderr,
                    "320843_02: n=%d layer=%d/%d states=%zu "
                    "memory=%.3f GiB\n",
                    n, layer + 1, n, current.count,
                    (double)memory.allocated /
                        (double)(UINT64_C(1) << 30));
        }
        if (current.count == 0) {
            status = DP_INTERNAL_ERROR;
            goto finished;
        }
    }

    {
        uint64_t full_mask = n == 0
                                 ? 0
                                 : (UINT64_C(1) << n) - 1;
        if (!map_lookup(&current, full_mask, result)) {
            status = DP_INTERNAL_ERROR;
        }
    }

finished:
    stats->peak_bytes = memory.peak;
    stats->seconds = monotonic_seconds() - start_time;
    if (status == DP_MEMORY_LIMIT && memory.required > stats->peak_bytes) {
        stats->peak_bytes = memory.required;
    }
    map_destroy(&next, &memory);
    map_destroy(&current, &memory);
    return status;
}

static uint32_t decimal_mod(const char *decimal, uint32_t modulus,
                            bool *valid)
{
    if (*decimal == '\0') {
        *valid = false;
        return 0;
    }
    uint64_t residue = 0;
    for (const unsigned char *cursor = (const unsigned char *)decimal;
         *cursor != '\0'; ++cursor) {
        if (*cursor < '0' || *cursor > '9') {
            *valid = false;
            return 0;
        }
        residue = (residue * 10 + (uint64_t)(*cursor - '0')) % modulus;
    }
    *valid = true;
    return (uint32_t)residue;
}

static bool residues_of_decimal(const char *decimal, uint32_t *result)
{
    for (size_t index = 0; index < FINGERPRINT_MODULUS_COUNT; ++index) {
        bool valid = false;
        result[index] = decimal_mod(decimal, moduli[index], &valid);
        if (!valid) {
            return false;
        }
    }
    return true;
}

static const char *status_text(DpStatus status)
{
    switch (status) {
    case DP_OK:
        return "ok";
    case DP_MEMORY_LIMIT:
        return "memory limit exceeded";
    case DP_ALLOCATION_FAILURE:
        return "allocation failed";
    case DP_STATE_LIMIT:
        return "more than UINT32_MAX states";
    case DP_INTERNAL_ERROR:
        return "internal consistency check failed";
    }
    return "unknown error";
}

static void print_result(int n, const uint32_t *residues)
{
    printf("%d", n);
    for (size_t index = 0; index < FINGERPRINT_MODULUS_COUNT; ++index) {
        printf(" %" PRIu32, residues[index]);
    }
    putchar('\n');
}

static bool same_residues(const uint32_t *left, const uint32_t *right)
{
    return memcmp(left, right,
                  FINGERPRINT_MODULUS_COUNT * sizeof(*left)) == 0;
}

static bool compute_with_moduli(int n, uint64_t memory_budget, bool report,
                                const uint32_t *active_moduli,
                                size_t modulus_count, uint32_t *residues)
{
    DpStats stats;
    DpStatus status = modular_permanent(n, memory_budget, report,
                                        active_moduli, modulus_count,
                                        residues, &stats);
    if (status != DP_OK) {
        fprintf(stderr,
                "320843_02: n=%d failed: %s; peak/required %.3f GiB\n",
                n, status_text(status),
                (double)stats.peak_bytes /
                    (double)(UINT64_C(1) << 30));
        return false;
    }
    fprintf(stderr,
            "320843_02: n=%d ok, moduli=%zu, peak states=%zu, "
            "peak memory=%.3f GiB, %.3f s\n",
            n, modulus_count, stats.peak_states,
            (double)stats.peak_bytes /
                (double)(UINT64_C(1) << 30),
            stats.seconds);
    return true;
}

static bool compute_one(int n, uint64_t memory_budget, bool report,
                        uint32_t *residues)
{
    for (size_t index = 0; index < FINGERPRINT_MODULUS_COUNT; ++index) {
        fprintf(stderr,
                "320843_02: n=%d modular pass %zu/%d, p=%" PRIu32 "\n",
                n, index + 1, FINGERPRINT_MODULUS_COUNT, moduli[index]);
        if (!compute_with_moduli(n, memory_budget,
                                 report && index == 0,
                                 &moduli[index], 1,
                                 &residues[index])) {
            return false;
        }
    }
    return true;
}

static int parse_n(const char *text, const char *label)
{
    errno = 0;
    char *end = NULL;
    long value = strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' || value < 0 ||
        value > MAX_SUPPORTED_N) {
        fprintf(stderr, "error: %s must be an integer from 0 to %d\n",
                label, MAX_SUPPORTED_N);
        exit(EXIT_FAILURE);
    }
    return (int)value;
}

static uint64_t memory_budget_bytes(void)
{
    const char *setting = getenv("A320843_02_MEMORY_MIB");
    uint64_t mib = DEFAULT_MEMORY_MIB;
    if (setting != NULL && *setting != '\0') {
        errno = 0;
        char *end = NULL;
        unsigned long long parsed = strtoull(setting, &end, 10);
        if (errno != 0 || end == setting || *end != '\0' ||
            parsed < MIN_MEMORY_MIB || parsed > MAX_MEMORY_MIB) {
            fprintf(stderr,
                    "error: A320843_02_MEMORY_MIB must be from %" PRIu64
                    " to %" PRIu64 "\n",
                    MIN_MEMORY_MIB, MAX_MEMORY_MIB);
            exit(EXIT_FAILURE);
        }
        mib = (uint64_t)parsed;
    }
    return mib * (UINT64_C(1) << 20);
}

static int compute_crt_value(mpz_t exact, int n,
                             uint64_t memory_budget, bool report);

static int check_known(int max_n, uint64_t memory_budget)
{
    if (max_n > KNOWN_MAX_N) {
        fprintf(stderr, "error: built-in exact terms end at n=%d\n",
                KNOWN_MAX_N);
        return EXIT_FAILURE;
    }
    mpz_t actual;
    mpz_t expected;
    mpz_inits(actual, expected, NULL);
    for (int n = 0; n <= max_n; ++n) {
        if (compute_crt_value(actual, n, memory_budget, false) !=
            EXIT_SUCCESS) {
            mpz_clears(actual, expected, NULL);
            return EXIT_FAILURE;
        }
        if (mpz_set_str(expected, known_terms[n], 10) != 0) {
            die("invalid built-in decimal term");
        }
        if (mpz_cmp(actual, expected) != 0) {
            fprintf(stderr, "320843_02: mismatch at n=%d\n", n);
            gmp_fprintf(stderr, "  actual:   %Zd\n", actual);
            gmp_fprintf(stderr, "  expected: %Zd\n", expected);
            mpz_clears(actual, expected, NULL);
            return EXIT_FAILURE;
        }
    }
    mpz_clears(actual, expected, NULL);
    printf("verified independently reconstructed exact A320843 terms "
           "for n=0..%d\n", max_n);
    return EXIT_SUCCESS;
}

static int verify_decimal(int n, const char *decimal,
                          uint64_t memory_budget)
{
    uint32_t actual[FINGERPRINT_MODULUS_COUNT];
    uint32_t expected[FINGERPRINT_MODULUS_COUNT];
    if (!residues_of_decimal(decimal, expected)) {
        fprintf(stderr, "error: VALUE must be a nonnegative decimal integer\n");
        return EXIT_FAILURE;
    }
    if (!compute_one(n, memory_budget, true, actual)) {
        return EXIT_FAILURE;
    }
    print_result(n, actual);
    if (!same_residues(actual, expected)) {
        fprintf(stderr, "320843_02: VALUE does not match at n=%d\n", n);
        return EXIT_FAILURE;
    }
    printf("verified n=%d against VALUE modulo all three primes\n", n);
    return EXIT_SUCCESS;
}

static void make_degree_bound(mpz_t bound, int n)
{
    mpz_set_ui(bound, 1);
    for (int row = 1; row <= n; ++row) {
        unsigned degree = 0;
        for (int column = 1; column <= n; ++column) {
            if (row % column == 0 || column % row == 0) {
                ++degree;
            }
        }
        mpz_mul_ui(bound, bound, (unsigned long)degree);
    }
}

static size_t choose_crt_moduli(const mpz_t bound, mpz_t product)
{
    mpz_set_ui(product, 1);
    for (size_t count = 1; count <= MAX_MODULUS_COUNT; ++count) {
        mpz_mul_ui(product, product,
                  (unsigned long)moduli[count - 1]);
        if (mpz_cmp(product, bound) > 0) {
            return count;
        }
    }
    return 0;
}

static uint32_t modular_power(uint32_t base, uint32_t exponent,
                              uint32_t modulus)
{
    uint64_t result = 1;
    uint64_t factor = base;
    while (exponent != 0) {
        if ((exponent & 1) != 0) {
            result = (result * factor) % modulus;
        }
        factor = (factor * factor) % modulus;
        exponent >>= 1;
    }
    return (uint32_t)result;
}

static bool reconstruct_crt(mpz_t result, mpz_t product,
                            const uint32_t *residues,
                            size_t modulus_count)
{
    mpz_set_ui(result, 0);
    mpz_set_ui(product, 1);
    for (size_t index = 0; index < modulus_count; ++index) {
        uint32_t modulus = moduli[index];
        uint32_t product_mod =
            (uint32_t)mpz_fdiv_ui(product, (unsigned long)modulus);
        if (product_mod == 0) {
            return false;
        }
        uint32_t inverse =
            modular_power(product_mod, modulus - 2, modulus);
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
        if (mpz_fdiv_ui(result, (unsigned long)moduli[index]) !=
            residues[index]) {
            return false;
        }
    }
    return true;
}

static int compute_crt_value(mpz_t exact, int n,
                             uint64_t memory_budget, bool report)
{
    mpz_t bound;
    mpz_t selected_product;
    mpz_t reconstructed_product;
    mpz_inits(bound, selected_product, reconstructed_product, NULL);
    make_degree_bound(bound, n);
    size_t modulus_count = choose_crt_moduli(bound, selected_product);
    if (modulus_count == 0) {
        fprintf(stderr,
                "error: the available CRT product does not exceed the "
                "rigorous bound\n");
        mpz_clears(bound, selected_product, reconstructed_product, NULL);
        return EXIT_FAILURE;
    }

    fprintf(stderr,
            "320843_02: n=%d CRT exact mode, bound=%zu bits, "
            "using %zu sequential prime passes (%zu-bit product)\n",
            n, mpz_sizeinbase(bound, 2), modulus_count,
            mpz_sizeinbase(selected_product, 2));
    uint32_t residues[MAX_MODULUS_COUNT];
    for (size_t index = 0; index < modulus_count; ++index) {
        fprintf(stderr,
                "320843_02: n=%d CRT pass %zu/%zu, p=%" PRIu32 "\n",
                n, index + 1, modulus_count, moduli[index]);
        if (!compute_with_moduli(n, memory_budget,
                                 report && index == 0,
                                 &moduli[index], 1,
                                 &residues[index])) {
            mpz_clears(bound, selected_product,
                       reconstructed_product, NULL);
            return EXIT_FAILURE;
        }
    }
    if (!reconstruct_crt(exact, reconstructed_product, residues,
                         modulus_count) ||
        mpz_cmp(reconstructed_product, bound) <= 0 ||
        mpz_cmp(exact, bound) > 0) {
        fprintf(stderr,
                "error: CRT reconstruction failed its rigorous bound check\n");
        mpz_clears(bound, selected_product, reconstructed_product, NULL);
        return EXIT_FAILURE;
    }
    mpz_clears(bound, selected_product, reconstructed_product, NULL);
    return EXIT_SUCCESS;
}

static int compute_crt_exact(int n, uint64_t memory_budget, bool report)
{
    mpz_t exact;
    mpz_init(exact);
    int status = compute_crt_value(exact, n, memory_budget, report);
    if (status == EXIT_SUCCESS && gmp_printf("%d %Zd\n", n, exact) < 0) {
        status = EXIT_FAILURE;
    }
    mpz_clear(exact);
    return status;
}

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s FROM [TO]\n"
            "       %s --term N\n"
            "       %s --crt N\n"
            "       %s --mod N\n"
            "       %s --verify N VALUE\n"
            "       %s --check [MAX_N]\n"
            "\n"
            "N, FROM, and TO must be from 0 to %d.\n"
            "--check defaults to n=0..%d and has built-in values through "
            "n=%d.\n"
            "The default memory limit is %" PRIu64 " MiB; override it with\n"
            "A320843_02_MEMORY_MIB.\n",
            program, program, program, program, program, program,
            MAX_SUPPORTED_N,
            DEFAULT_CHECK_N, KNOWN_MAX_N, DEFAULT_MEMORY_MIB);
}

int main(int argc, char **argv)
{
    uint64_t memory_budget = memory_budget_bytes();

    if (argc >= 2 && strcmp(argv[1], "--term") == 0) {
        if (argc != 3) {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
        int n = parse_n(argv[2], "N");
        return compute_crt_exact(n, memory_budget, true);
    }

    if (argc >= 2 && strcmp(argv[1], "--crt") == 0) {
        if (argc != 3) {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
        int n = parse_n(argv[2], "N");
        return compute_crt_exact(n, memory_budget, true);
    }

    if (argc >= 2 && strcmp(argv[1], "--mod") == 0) {
        if (argc != 3) {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
        int n = parse_n(argv[2], "N");
        uint32_t residues[FINGERPRINT_MODULUS_COUNT];
        if (!compute_one(n, memory_budget, true, residues)) {
            return EXIT_FAILURE;
        }
        print_result(n, residues);
        return EXIT_SUCCESS;
    }

    if (argc >= 2 && strcmp(argv[1], "--verify") == 0) {
        if (argc != 4) {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
        int n = parse_n(argv[2], "N");
        return verify_decimal(n, argv[3], memory_budget);
    }

    if (argc >= 2 && strcmp(argv[1], "--check") == 0) {
        if (argc > 3) {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
        int max_n = argc == 3 ? parse_n(argv[2], "MAX_N")
                              : DEFAULT_CHECK_N;
        return check_known(max_n, memory_budget);
    }

    if (argc < 2 || argc > 3) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    int from = parse_n(argv[1], "FROM");
    int to = argc == 3 ? parse_n(argv[2], "TO") : from;
    if (from > to) {
        die("FROM must not exceed TO");
    }
    for (int n = from; n <= to; ++n) {
        if (compute_crt_exact(n, memory_budget, false) != EXIT_SUCCESS) {
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}
