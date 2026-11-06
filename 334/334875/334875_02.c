/*
 * OEIS A334875: number of tribone tilings of an n-triangle.
 * https://oeis.org/A334875
 *
 * In row i, columns run from 0 through i. The two tile shapes with first
 * cell (i,j) also cover (i+1,j+1) and one of (i+1,j) or (i,j+1).
 *
 * Sweep from both ends to a common row. A mask records cells in that row
 * already covered by tiles extending from the processed side. To advance,
 * fill the first uncovered cell with each legal tile extending into the
 * next row. All tiles span two rows. Thus at the common row a top and a
 * bottom partial tiling fit exactly when their masks are complementary,
 * and each complete tiling has a unique such decomposition.
 *
 * Reflection j -> i-j swaps left and right and permutes the tiles. Store
 * one entry per orbit of masks, with the sum of the counts in the orbit.
 * Reflected masks have identical transition multiplicities to any output
 * orbit, so expanding just the representative with that sum is exact.
 * At the join, multiply orbit totals and divide by the orbit size: only
 * complementary masks match, and each mask has the same count as its
 * reflection. The division is exact.
 *
 * Build:
 *   cc -O3 -std=c11 -Wall -Wextra -Wpedantic 334875_02.c \
 *       -o 334875_02 $(pkg-config --cflags --libs gmp)
 * Usage:
 *   ./334875_02 [LAST [FIRST]]
 *   ./334875_02 38 33
 *
 * Prints FIRST..LAST as "n a(n)" on stdout. Defaults to 0..26. The
 * mask limit is 63. Runtime and memory can grow exponentially.
 * A334875_MEMORY_MIB limits live DP allocations, including old and new
 * tables during growth. The default is 1024 MiB. An allocation failure
 * or limit stops the run with an error on stderr and no line for that n.
 */

#include <errno.h>
#include <gmp.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

enum {
    MAX_N = 63,
    DEFAULT_LAST = 26,
    DEFAULT_MEMORY_MIB = 1024,
    MIB = 1024 * 1024,
    /* At most two choices per tile and at most n*(n+1)/6 tiles per path.
       There are at most 2^floor(n*(n+1)/6) partial tilings in any layer. */
    COUNT_WORDS = (MAX_N * (MAX_N + 1) / 6) / 64 + 1
};

#define WIDE_FLAG (UINT64_C(1) << 63)
#define KEY_MASK (WIDE_FLAG - 1)

typedef struct {
    uint64_t words[COUNT_WORDS];
} WideCount;

typedef struct {
    uint64_t key; /* Mask plus one in low bits. Zero denotes an empty slot. */
    union {
        uint64_t small;
        WideCount *wide;
    } count;
} Entry;

typedef struct {
    size_t used;
    size_t limit;
    int n;
} Workspace;

typedef struct {
    Entry *entries;
    size_t capacity;
    size_t size;
    unsigned width;
    Workspace *work;
} Layer;

static void *allocate(Workspace *work, size_t count, size_t bytes)
{
    if (count > (work->limit - work->used) / bytes) {
        fprintf(stderr, "n=%d: DP memory limit exceeded (A334875_MEMORY_MIB).\n",
                work->n);
        exit(EXIT_FAILURE);
    }
    void *data = calloc(count, bytes);
    if (data == NULL) {
        fprintf(stderr, "n=%d: cannot allocate DP memory.\n", work->n);
        exit(EXIT_FAILURE);
    }
    work->used += count * bytes;
    return data;
}

static uint64_t reverse_mask(uint64_t x, unsigned width)
{
#if defined(__clang__)
    x = __builtin_bitreverse64(x);
#else
    x = ((x >> 1) & UINT64_C(0x5555555555555555)) |
        ((x & UINT64_C(0x5555555555555555)) << 1);
    x = ((x >> 2) & UINT64_C(0x3333333333333333)) |
        ((x & UINT64_C(0x3333333333333333)) << 2);
    x = ((x >> 4) & UINT64_C(0x0f0f0f0f0f0f0f0f)) |
        ((x & UINT64_C(0x0f0f0f0f0f0f0f0f)) << 4);
    x = ((x >> 8) & UINT64_C(0x00ff00ff00ff00ff)) |
        ((x & UINT64_C(0x00ff00ff00ff00ff)) << 8);
    x = ((x >> 16) & UINT64_C(0x0000ffff0000ffff)) |
        ((x & UINT64_C(0x0000ffff0000ffff)) << 16);
    x = (x >> 32) | (x << 32);
#endif
    return x >> (64 - width);
}

static uint64_t full_mask(unsigned width)
{
    return (UINT64_C(1) << width) - 1;
}

static Entry *find_slot(const Layer *layer, uint64_t key)
{
    uint64_t hash = key;
    hash = (hash ^ (hash >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    hash = (hash ^ (hash >> 27)) * UINT64_C(0x94d049bb133111eb);
    hash ^= hash >> 31;

    size_t mask = layer->capacity - 1;
    size_t i = (size_t)hash & mask;
    while (layer->entries[i].key != 0 &&
           (layer->entries[i].key & KEY_MASK) != key)
        i = (i + 1) & mask;
    return &layer->entries[i];
}

static void grow_layer(Layer *layer)
{
    if (layer->capacity > SIZE_MAX / 2) {
        fputs("DP table size overflow.\n", stderr);
        exit(EXIT_FAILURE);
    }
    Layer old = *layer;
    layer->capacity = old.capacity ? 2 * old.capacity : 16;
    layer->entries = allocate(layer->work, layer->capacity, sizeof(Entry));
    for (size_t i = 0; i < old.capacity; ++i) {
        Entry *entry = &old.entries[i];
        if (entry->key != 0)
            *find_slot(layer, entry->key & KEY_MASK) = *entry;
    }
    free(old.entries);
    layer->work->used -= old.capacity * sizeof(Entry);
}

static void add_wide(Entry *dest, const Entry *source, Workspace *work)
{
    if ((dest->key & WIDE_FLAG) == 0) {
        WideCount *wide = allocate(work, 1, sizeof(*wide));
        wide->words[0] = dest->count.small;
        dest->count.wide = wide;
        dest->key |= WIDE_FLAG;
    }

    const uint64_t *words = &source->count.small;
    size_t length = 1;
    if (source->key & WIDE_FLAG) {
        words = source->count.wide->words;
        length = COUNT_WORDS;
    }
    uint64_t carry = 0;
    for (size_t i = 0; i < length || (carry != 0 && i < COUNT_WORDS); ++i) {
        uint64_t old = dest->count.wide->words[i];
        uint64_t part = i < length ? words[i] : 0;
        uint64_t sum = old + part;
        uint64_t with_carry = sum + carry;
        dest->count.wide->words[i] = with_carry;
        carry = (sum < old) || (with_carry < sum);
    }
    if (carry != 0) {
        fputs("DP count overflow.\n", stderr);
        exit(EXIT_FAILURE);
    }
}

static void add_count(Layer *layer, uint64_t mask, const Entry *source)
{
    uint64_t reflected = reverse_mask(mask, layer->width);
    uint64_t key = 1 + (mask < reflected ? mask : reflected);

    if (layer->capacity == 0)
        grow_layer(layer);
    Entry *entry = find_slot(layer, key);
    if (entry->key == 0) {
        if (layer->size >= layer->capacity - layer->capacity / 4) {
            grow_layer(layer);
            entry = find_slot(layer, key);
        }
        entry->key = key;
        ++layer->size;
    }

    if (((entry->key | source->key) & WIDE_FLAG) == 0 &&
        UINT64_MAX - entry->count.small >= source->count.small) {
        entry->count.small += source->count.small;
    } else {
        add_wide(entry, source, layer->work);
    }
}

static void clear_layer(Layer *layer)
{
    for (size_t i = 0; i < layer->capacity; ++i) {
        Entry *entry = &layer->entries[i];
        if (entry->key & WIDE_FLAG) {
            free(entry->count.wide);
            layer->work->used -= sizeof(WideCount);
        }
    }
    free(layer->entries);
    layer->work->used -= layer->capacity * sizeof(Entry);
}

static void sweep_down(uint64_t holes, uint64_t below,
                       const Entry *source, Layer *next)
{
    while (holes != 0) {
        uint64_t first = holes & -holes;
        uint64_t right = first << 1;
        if (below & right)
            return;
        holes ^= first;
        if ((below & first) == 0) {
            if (holes & right)
                sweep_down(holes ^ right, below | right, source, next);
            below |= first | right;
        } else {
            if ((holes & right) == 0)
                return;
            holes ^= right;
            below |= right;
        }
    }
    add_count(next, below, source);
}

static void sweep_up(uint64_t holes, uint64_t above, uint64_t above_full,
                     const Entry *source, Layer *next)
{
    while (holes != 0) {
        uint64_t first = holes & -holes;
        uint64_t left = first >> 1;
        uint64_t right = first << 1;
        /* Either tile uses column j above. If the tile uses two lower
           cells they must be j and j+1, since j is the first hole. */
        if ((above & first) != 0 || (above_full & first) == 0)
            return;
        holes ^= first;
        if (holes & right) {
            if (left != 0 && (above & left) == 0)
                sweep_up(holes, above | left | first, above_full, source, next);
            holes ^= right;
            above |= first;
        } else {
            if (left == 0 || (above & left) != 0)
                return;
            above |= left | first;
        }
    }
    add_count(next, above, source);
}

static Layer build_half(Workspace *work, unsigned start, unsigned cut)
{
    int down = start < cut;
    Layer current = {.width = start, .work = work};
    const Entry seed = {.key = 1, .count.small = 1};
    add_count(&current, 0, &seed);

    while (current.width != cut && current.size != 0) {
        unsigned width = current.width;
        Layer next = {.width = down ? width + 1 : width - 1, .work = work};
        uint64_t full = full_mask(width);
        for (size_t i = 0; i < current.capacity; ++i) {
            const Entry *entry = &current.entries[i];
            if (entry->key == 0)
                continue;
            uint64_t holes = full ^ ((entry->key & KEY_MASK) - 1);
            if (down)
                sweep_down(holes, 0, entry, &next);
            else
                sweep_up(holes, 0, full >> 1, entry, &next);
        }
        clear_layer(&current);
        current = next;
    }
    return current;
}

static void import_count(mpz_t result, const Entry *entry)
{
    if (entry->key & WIDE_FLAG) {
        mpz_import(result, COUNT_WORDS, -1, sizeof(uint64_t), 0, 0,
                   entry->count.wide->words);
    } else {
#if ULONG_MAX >= UINT64_MAX
        mpz_set_ui(result, entry->count.small);
#else
        mpz_import(result, 1, -1, sizeof(uint64_t), 0, 0, &entry->count.small);
#endif
    }
}

static void join_halves(mpz_t result, const Layer *a, const Layer *b)
{
    if (a->size == 0 || b->size == 0)
        return;
    uint64_t full = full_mask(a->width);
    mpz_t fixed, pairs, x, y;
    mpz_inits(fixed, pairs, x, y, NULL);
    for (size_t i = 0; i < a->capacity; ++i) {
        const Entry *entry = &a->entries[i];
        if (entry->key == 0)
            continue;
        uint64_t mask = (entry->key & KEY_MASK) - 1;
        uint64_t reflected = reverse_mask(mask, a->width);
        /* Complement reverses order, so complement the larger mask. */
        const Entry *match = find_slot(b, (full ^ reflected) + 1);
        if (match->key != 0) {
            import_count(x, entry);
            import_count(y, match);
            mpz_addmul(mask == reflected ? fixed : pairs, x, y);
        }
    }
    mpz_fdiv_q_2exp(pairs, pairs, 1);
    mpz_add(result, fixed, pairs);
    mpz_clears(fixed, pairs, x, y, NULL);
}

static void count_tilings(unsigned n, size_t limit, mpz_t result)
{
    mpz_set_ui(result, n == 0);
    if (n == 0)
        return;

    /* Conway-Lagarias tileability criterion, as recorded by OEIS.
       It also rules out n=63, so DP masks have at most 62 bits and
       adding one to a mask cannot set the WIDE_FLAG bit. */
    unsigned residue = n % 12;
    if (residue != 0 && residue != 2 && residue != 9 && residue != 11)
        return;

    /* The bottom sweep starts at the widest row, so give it fewer rows. */
    unsigned cut = n > 14 ? n - 7 : (n + 1) / 2;
    Workspace work = {.limit = limit, .n = (int)n};
    Layer bottom = build_half(&work, n, cut);
    Layer top = build_half(&work, 1, cut);
    if (top.capacity <= bottom.capacity)
        join_halves(result, &top, &bottom);
    else
        join_halves(result, &bottom, &top);
    clear_layer(&top);
    clear_layer(&bottom);
}

static unsigned long long parse_number(const char *arg)
{
    if (*arg < '0' || *arg > '9')
        return ULLONG_MAX;
    char *end;
    errno = 0;
    unsigned long long value = strtoull(arg, &end, 10);
    return errno == 0 && *end == '\0' ? value : ULLONG_MAX;
}

int main(int argc, char **argv)
{
    unsigned long long last = argc >= 2 ? parse_number(argv[1]) : DEFAULT_LAST;
    unsigned long long first = argc >= 3 ? parse_number(argv[2]) : 0;
    if (argc > 3 || last > MAX_N || first > last) {
        fprintf(stderr, "Usage: %s [LAST [FIRST]], 0 <= FIRST <= LAST <= %d\n",
                argv[0], MAX_N);
        return EXIT_FAILURE;
    }

    const char *memory = getenv("A334875_MEMORY_MIB");
    unsigned long long mib = memory ? parse_number(memory) : DEFAULT_MEMORY_MIB;
    if (mib == 0 || mib > SIZE_MAX / MIB) {
        fputs("A334875_MEMORY_MIB must be a positive memory limit in MiB.\n", stderr);
        return EXIT_FAILURE;
    }

    mpz_t result;
    mpz_init(result);
    int status = EXIT_SUCCESS;
    for (unsigned n = (unsigned)first; n <= (unsigned)last; ++n) {
        count_tilings(n, (size_t)mib * MIB, result);
        if (gmp_printf("%u %Zd\n", n, result) < 0 || fflush(stdout) == EOF) {
            fputs("Cannot write result.\n", stderr);
            status = EXIT_FAILURE;
            break;
        }
    }
    mpz_clear(result);
    return status;
}
