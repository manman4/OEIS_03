/*
 * OEIS A334875: number of tribone tilings of an n-triangle.
 * https://oeis.org/A334875
 *
 * Use the coordinates and two tile shapes in the OEIS entry. Row i has
 * columns 0..i. A tile whose first cell is (i,j) also covers (i+1,j+1)
 * and exactly one of (i+1,j) and (i,j+1).
 *
 * At the start of a row, the occupied cells inherited from the previous
 * row completely determine the possible continuations. Fill the first
 * uncovered cell using each legal tile. Its tile must start there since
 * any earlier cell is already covered. Once the row is full, add the
 * incoming count to the resulting mask for the next row. This gives
 * each tiling one path and counts different paths to a mask separately.
 *
 * Build:
 *   cc -O3 -std=c11 -Wall -Wextra -Wpedantic 334875_01.c \
 *       -o 334875_01 $(pkg-config --cflags --libs gmp)
 * Usage:
 *   ./334875_01 [LAST [FIRST]]
 *   ./334875_01 26 21
 * Prints FIRST..LAST as "n a(n)" on stdout. Defaults to 0..26.
 * The mask representation supports n <= 63. Time and memory can grow
 * exponentially, so the mask limit is not a practical running limit.
 */

#include <errno.h>
#include <gmp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

enum { MAX_N = 63, DEFAULT_LAST = 26 };

typedef struct {
    uint64_t key; /* Occupied mask plus one. Zero denotes an empty slot. */
    mpz_t count;
} Entry;

typedef struct {
    Entry *entries;
    size_t capacity;
    size_t size;
} Layer;

static size_t slot_for(const Entry *entries, size_t capacity, uint64_t key)
{
    uint64_t hash = key;
    hash ^= hash >> 30;
    hash *= UINT64_C(0xbf58476d1ce4e5b9);
    hash ^= hash >> 27;
    hash *= UINT64_C(0x94d049bb133111eb);
    hash ^= hash >> 31;

    size_t slot = (size_t)hash & (capacity - 1);
    while (entries[slot].key != 0 && entries[slot].key != key)
        slot = (slot + 1) & (capacity - 1);
    return slot;
}

static void grow_layer(Layer *layer)
{
    if (layer->capacity > SIZE_MAX / sizeof(Entry) / 2) {
        fputs("Too many frontier states.\n", stderr);
        exit(EXIT_FAILURE);
    }
    size_t capacity = layer->capacity ? 2 * layer->capacity : 16;
    Entry *entries = calloc(capacity, sizeof(*entries));
    if (entries == NULL) {
        fputs("Cannot allocate frontier states.\n", stderr);
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < layer->capacity; ++i) {
        Entry *entry = &layer->entries[i];
        if (entry->key != 0)
            entries[slot_for(entries, capacity, entry->key)] = *entry;
    }
    free(layer->entries);
    layer->entries = entries;
    layer->capacity = capacity;
}

static void add_count(Layer *layer, uint64_t occupied, mpz_srcptr count)
{
    if (layer->size >= layer->capacity / 2)
        grow_layer(layer);

    uint64_t key = occupied + 1;
    Entry *entry = &layer->entries[slot_for(layer->entries, layer->capacity, key)];
    if (entry->key == 0) {
        entry->key = key;
        mpz_init_set(entry->count, count);
        ++layer->size;
    } else {
        mpz_add(entry->count, entry->count, count);
    }
}

static void clear_layer(Layer *layer)
{
    for (size_t i = 0; i < layer->capacity; ++i) {
        if (layer->entries[i].key != 0)
            mpz_clear(layer->entries[i].count);
    }
    free(layer->entries);
}

/* When both remaining rows must be filled, their leftmost holes force
 * the next tile. Aligned holes force the tile with two lower cells.
 * A top hole one column to the left forces the tile with two upper cells.
 */
static int finishes_triangle(uint64_t top, uint64_t bottom)
{
    while (top != 0 && bottom != 0) {
        uint64_t first = top & -top;
        uint64_t right = first << 1;
        uint64_t lower_first = bottom & -bottom;
        if (lower_first == first && (bottom & right) != 0) {
            top ^= first;
            bottom ^= first | right;
        } else if (lower_first == right && (top & right) != 0) {
            top ^= first | right;
            bottom ^= right;
        } else {
            return 0;
        }
    }
    return top == 0 && bottom == 0;
}

static void fill_row(uint64_t uncovered, uint64_t below, uint64_t last_mask,
                     mpz_srcptr count, Layer *next)
{
    if (uncovered == 0) {
        if (last_mask == 0) {
            add_count(next, below, count);
        } else if (finishes_triangle((last_mask >> 1) ^ below, last_mask)) {
            add_count(next, last_mask, count);
        }
        return;
    }

    uint64_t first = uncovered & -uncovered;
    uint64_t right = first << 1;
    if ((below & right) != 0)
        return;
    uncovered ^= first;
    if ((below & first) == 0)
        fill_row(uncovered, below | first | right, last_mask, count, next);
    if ((uncovered & right) != 0)
        fill_row(uncovered ^ right, below | right, last_mask, count, next);
}

static void count_tilings(int n, mpz_t result)
{
    mpz_set_ui(result, n == 0);
    if (n == 0)
        return;

    /* Conway-Lagarias tileability criterion, as recorded by OEIS. */
    int residue = n % 12;
    if (residue != 0 && residue != 2 && residue != 9 && residue != 11)
        return;

    /* Finish the final two rows together when their top mask is known. */
    uint64_t last_mask = (UINT64_C(1) << n) - 1;
    if (n == 2) {
        mpz_set_ui(result, finishes_triangle(1, last_mask));
        return;
    }

    Layer current = {0};
    mpz_set_ui(result, 1);
    add_count(&current, 0, result);
    mpz_set_ui(result, 0);

    for (int width = 1; width <= n - 2 && current.size != 0; ++width) {
        uint64_t full = (UINT64_C(1) << width) - 1;
        uint64_t finish_mask = width == n - 2 ? last_mask : 0;
        Layer next = {0};
        for (size_t i = 0; i < current.capacity; ++i) {
            const Entry *entry = &current.entries[i];
            if (entry->key != 0)
                fill_row(full ^ (entry->key - 1), 0, finish_mask, entry->count, &next);
        }
        clear_layer(&current);
        current = next;
    }

    if (current.size != 0) {
        size_t slot = slot_for(current.entries, current.capacity, last_mask + 1);
        if (current.entries[slot].key != 0)
            mpz_set(result, current.entries[slot].count);
    }
    clear_layer(&current);
}

static int parse_index(const char *arg)
{
    if (*arg < '0' || *arg > '9')
        return -1;
    char *end;
    errno = 0;
    long n = strtol(arg, &end, 10);
    return errno == 0 && *end == '\0' && n <= MAX_N ? (int)n : -1;
}

int main(int argc, char **argv)
{
    int last = argc >= 2 ? parse_index(argv[1]) : DEFAULT_LAST;
    int first = argc >= 3 ? parse_index(argv[2]) : 0;
    if (argc > 3 || first < 0 || last < first) {
        fprintf(stderr, "Usage: %s [LAST [FIRST]], 0 <= FIRST <= LAST <= %d\n",
                argv[0], MAX_N);
        return EXIT_FAILURE;
    }

    mpz_t result;
    mpz_init(result);
    int status = EXIT_SUCCESS;
    for (int n = first; n <= last; ++n) {
        count_tilings(n, result);
        if (gmp_printf("%d %Zd\n", n, result) < 0 || fflush(stdout) == EOF) {
            fputs("Cannot write result.\n", stderr);
            status = EXIT_FAILURE;
            break;
        }
    }
    mpz_clear(result);
    return status;
}
