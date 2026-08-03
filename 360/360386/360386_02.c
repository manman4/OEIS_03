/*
 * A360386 -- exact tiling-partition formula of Spahn and Zeilberger.
 *
 * Count permutations p of {1,...,n} satisfying
 *
 *              |p(i+8) - p(i)| != 8,   1 <= i <= n-8.
 *
 * More generally, this program evaluates
 *
 *   b_{d,d}(n) = #{p in S_n : |p(i+d)-p(i)| != d for 1<=i<=n-d}
 *
 * for a user-selected d>=2.  The default d=8 is OEIS A360386.
 *
 * Reference:
 *   G. Spahn and D. Zeilberger,
 *   "Counting Permutations Where The Difference Between Entries Located
 *    r Places Apart Can never be s",
 *   arXiv:2211.02550, 2022.
 *
 * The paper tiles {1,...,n} by chains whose successive entries differ by d.
 * Write a tile-size partition in frequency notation as
 *
 *              alpha = 1^a_1 2^a_2 ... n^a_n,
 *
 * and let C_alpha(n,d) be the number of such d-step tilings.  The absolute
 * value formula in the paper, specialized to r=s=d, is
 *
 *   b_{d,d}(n) = sum_alpha C_alpha(n,d)^2
 *                  * (-1)^(a_1+...+a_n-n)
 *                  * product_j a_j! * 2^(a_2+...+a_n).
 *
 * The implementation stores only the non-singleton tile histogram H:
 * h_l is the number of tiles of size l+1.  Put
 *
 *       r = sum_l l*h_l,   c = sum_l h_l,   a_1 = n-r-c.
 *
 * For one residue path of m vertices, a fixed H occurs
 *
 *       binomial(m-r,c) * c! / product_l h_l!
 *
 * times.  Multiplying these small histogram polynomials over the d residue
 * paths gives W(H)=C_alpha(n,d).  Thus the final exact sum is
 *
 *   b_{d,d}(n) = sum_H (-1)^r * W(H)^2 * 2^c
 *                  * (n-r-c)! * product_l h_l!.
 *
 * This is independent of 360386_01.c's evaluation of Tauraso's q_{n,d}(L):
 * there is no target-assignment convolution here.  The sum of all W(H) is
 * checked to be 2^(n-d), the number of subsets of the d-step edges.
 *
 * For speed, histogram keys use only the active ceil(n/d)-1 components,
 * rather than the full compiled 63-byte capacity.  Each slot also stores a
 * linear 64-bit hash, so the hash of a componentwise sum is obtained by one
 * unsigned addition during polynomial convolution.  Hash collisions are
 * always resolved by an exact byte comparison; neither optimization changes
 * the tiling formula or relies on probabilistic equality.
 *
 * GMP holds the final signed sum.  For 2<=d<=64 and n<=128, every W(H)
 * fits unsigned 128-bit arithmetic because all of them sum to at most
 * 2^126.  Per-residue-path coefficients still fit uint64_t because a path
 * has at most 64 vertices.
 *
 * Checks:
 *   - OEIS b-files for b_{d,d}, d=2..10: A110128, A117574,
 *     A189255, A189256, A189271, A360384, A360386, A360462,
 *     and A360463 (all terms present in those b-files as of 2026-08-03);
 *   - an independent permutation DP through n=16;
 *   - general d=2..8 independently through n=12.
 *
 * Build:
 *
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic \
 *     -I/opt/homebrew/include -L/opt/homebrew/lib \
 *     360386_02.c -lgmp -o 360386_02
 *
 * Usage:
 *
 *   ./360386_02
 *   ./360386_02 40
 *   ./360386_02 --term 40
 *   ./360386_02 --d 2 --term 48
 *   ./360386_02 --d 5 --upto 40
 *   ./360386_02 --check
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

#if ULONG_MAX < UINT64_MAX
#error "360386_02 requires a platform with 64-bit unsigned long"
#endif

#if !defined(__SIZEOF_INT128__)
#error "360386_02 requires compiler support for unsigned __int128"
#endif

__extension__ typedef unsigned __int128 uint128_t;

#define A360386_D 8
#define SEQUENCE_OFFSET 0
#define DEFAULT_MAX_N 20
#define DEFAULT_CHECK_N 20
#define MAX_SUPPORTED_N 128
#define MAX_SUPPORTED_D 64
#define MAX_COMPONENT_LENGTH (MAX_SUPPORTED_N / 2 - 1)
#define BRUTE_CHECK_MAX_N 16
#define GENERAL_CHECK_MAX_D 8
#define GENERAL_CHECK_MAX_N 12
#define INITIAL_HIST_CAPACITY ((size_t)16)
#define LOAD_NUMERATOR ((size_t)7)
#define LOAD_DENOMINATOR ((size_t)10)
#define ARRAY_LENGTH(array) (sizeof(array) / sizeof((array)[0]))

/* OEIS A110128 = b_{2,2}, offset 0. */
static const char *const known_d2[] = {
    "1", "1", "2", "4", "16", "44", "200", "1288", "9512",
    "78652", "744360", "7867148", "91310696", "1154292796",
    "15784573160", "232050062524", "3648471927912",
    "61080818510972", "1084657970877416", "20361216987032284",
    "402839381030339816", "8377409956454452732",
    "182678001773062774632", "4167828820253946799580",
    "99292137525499869150056"
};

/* OEIS A117574 = b_{3,3}, offset 0; official b-file through n=30. */
static const char *const known_d3[] = {
    "1", "1", "2", "6", "20", "80", "384", "2240", "15424",
    "123456", "1110928", "11287232", "127016304", "1565107248",
    "20935873872", "301974271248", "4669727780624",
    "77046043259824", "1350585114106416", "25062108668100208",
    "490725684463001488", "10109820295907492304",
    "218584873235926445712", "4948433337202579395696",
    "117052159699521707890992", "2887549945416110999938672",
    "74158795581965682470686352", "1979635478643025761775312848",
    "54847606543679146894975981328",
    "1575028667838499959325329451376",
    "46819703912445220921650905524656"
};

/* OEIS A189255 = b_{4,4}, offset 1; official b-file through n=27. */
static const char *const known_d4[] = {
    "1", "2", "6", "24", "108", "544", "3264", "23040",
    "176832", "1563392", "15536160", "171172224", "2066033472",
    "27146652480", "385447394880", "5878028516736",
    "95776238793504", "1660164417866304", "30496085473606944",
    "591661117634375040", "12087628978334638752",
    "259353732110769122368", "5830260198084688642464",
    "137021386015707177170304", "3360010743290968081122336",
    "85815593755614101982027328", "2279036042450233133952884256"
};

/* OEIS A189256 = b_{5,5}, offset 1; official b-file through n=26. */
static const char *const known_d5[] = {
    "1", "2", "6", "24", "120", "672", "4128", "28992",
    "231936", "2088960", "20434944", "221871360", "2645370624",
    "34344038400", "482103767040", "7269498483456",
    "117240911729664", "2013265377314688", "36665783917283328",
    "705762463906133760", "14313891805008665856",
    "305058200866133496192", "6815052066896539474560",
    "159241382359638761724672", "3883921082001847040221440",
    "98699868661535185749114624"
};

/* OEIS A189271 = b_{6,6}, offset 1; official b-file through n=24. */
static const char *const known_d6[] = {
    "1", "2", "6", "24", "120", "720", "4800", "34752",
    "280512", "2528256", "25282560", "278323200", "3289036800",
    "42336448512", "589351062528", "8820501301248",
    "141215147788800", "2407845089203200", "43543159894318080",
    "832618225074748416", "16782891792284791296",
    "355619038157213568000", "7901759178346667289600",
    "183700312404587478328320"
};

/* OEIS A360384 = b_{7,7}, offset 0; official b-file through n=20. */
static const char *const known_d7[] = {
    "1", "1", "2", "6", "24", "120", "720", "5040", "38880",
    "323520", "2953728", "29666304", "326584320", "3919011840",
    "50969640960", "705722941440", "10504707686400",
    "167258901086208", "2836455721721856", "51038126752727040",
    "971132188892405760"
};

/* OEIS A360386 = b_{8,8}, offset 0; official b-file through n=20. */
static const char *const known_d8[] = {
    "1", "1", "2", "6", "24", "120", "720", "5040", "40320",
    "352800", "3312000", "33742080", "373948416", "4498421760",
    "58506854400", "819095961600", "12290021130240",
    "194999390208000", "3294723692298240", "59059379675381760",
    "1119453588069875712"
};

/* OEIS A360462 = b_{9,9}, offset 0; official b-file through n=21. */
static const char *const known_d9[] = {
    "1", "1", "2", "6", "24", "120", "720", "5040", "40320",
    "362880", "3548160", "37054080", "416551680", "5050368000",
    "65944535040", "924630958080", "13873661706240",
    "221978587299840", "3774394191052800", "67494538767237120",
    "1276009602375352320", "25429720417085030400"
};

/* OEIS A360463 = b_{10,10}, offset 0; official b-file through n=21. */
static const char *const known_d10[] = {
    "1", "1", "2", "6", "24", "120", "720", "5040", "40320",
    "362880", "3628800", "39191040", "450293760", "5534403840",
    "72864645120", "1026562867200", "15441868062720",
    "247319592468480", "4205303137566720", "75695456476200960",
    "1438421245702963200", "28616864514850160640"
};

typedef struct {
    const char *oeis_id;
    int d;
    int first_n;
    size_t term_count;
    const char *const *term;
} KnownSequence;

static const KnownSequence known_sequences[] = {
    {"A110128", 2, 0, ARRAY_LENGTH(known_d2), known_d2},
    {"A117574", 3, 0, ARRAY_LENGTH(known_d3), known_d3},
    {"A189255", 4, 1, ARRAY_LENGTH(known_d4), known_d4},
    {"A189256", 5, 1, ARRAY_LENGTH(known_d5), known_d5},
    {"A189271", 6, 1, ARRAY_LENGTH(known_d6), known_d6},
    {"A360384", 7, 0, ARRAY_LENGTH(known_d7), known_d7},
    {"A360386", 8, 0, ARRAY_LENGTH(known_d8), known_d8},
    {"A360462", 9, 0, ARRAY_LENGTH(known_d9), known_d9},
    {"A360463", 10, 0, ARRAY_LENGTH(known_d10), known_d10}
};

typedef struct {
    bool occupied;
    uint64_t hash;
    uint8_t count[MAX_COMPONENT_LENGTH];
    uint128_t coefficient;
} HistSlot;

typedef struct {
    HistSlot *slots;
    size_t capacity;
    size_t size;
    int key_length;
} HistMap;

typedef struct {
    uint64_t hash;
    uint8_t count[MAX_COMPONENT_LENGTH];
    uint64_t coefficient;
} LocalPattern;

typedef struct {
    LocalPattern *item;
    size_t count;
    size_t capacity;
} LocalPatternSet;

typedef struct {
    size_t histogram_count;
    size_t peak_histogram_count;
    uint64_t convolution_pairs;
    uint128_t coefficient_sum;
    double seconds;
} FormulaStats;

typedef struct {
    int n;
    int d;
    bool starts_chain[BRUTE_CHECK_MAX_N];
    uint64_t *memo;
} BruteContext;

static void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static size_t checked_product_size(size_t a, size_t b)
{
    if (a != 0U && b > SIZE_MAX / a) {
        die("allocation size overflow");
    }
    return a * b;
}

static void *xmalloc_array(size_t count, size_t size)
{
    size_t bytes = checked_product_size(count, size);
    void *pointer = malloc(bytes == 0U ? 1U : bytes);
    if (pointer == NULL) {
        die("out of memory");
    }
    return pointer;
}

static void *xcalloc_array(size_t count, size_t size)
{
    (void)checked_product_size(count, size);
    void *pointer = calloc(count == 0U ? 1U : count,
                           size == 0U ? 1U : size);
    if (pointer == NULL) {
        die("out of memory");
    }
    return pointer;
}

static void *xrealloc_array(void *old, size_t count, size_t size)
{
    size_t bytes = checked_product_size(count, size);
    void *pointer = realloc(old, bytes == 0U ? 1U : bytes);
    if (pointer == NULL) {
        die("out of memory while growing an array");
    }
    return pointer;
}

static uint64_t checked_add_u64(uint64_t a, uint64_t b)
{
    if (b > UINT64_MAX - a) {
        die("uint64_t addition overflow");
    }
    return a + b;
}

static uint64_t checked_mul_u64(uint64_t a, uint64_t b)
{
    if (a != 0U && b > UINT64_MAX / a) {
        die("uint64_t multiplication overflow");
    }
    return a * b;
}

static uint128_t checked_add_u128(uint128_t a, uint128_t b)
{
    uint128_t sum = a + b;
    if (sum < a) {
        die("unsigned 128-bit addition overflow");
    }
    return sum;
}

static uint128_t checked_mul_u128_u64(uint128_t a, uint64_t b)
{
    const uint128_t maximum = (uint128_t)-1;
    if (b != 0U && a > maximum / b) {
        die("unsigned 128-bit multiplication overflow");
    }
    return a * b;
}

static void mpz_set_u128(mpz_t result, uint128_t value)
{
    uint64_t words[2] = {
        (uint64_t)value,
        (uint64_t)(value >> 64)
    };
    size_t word_count = words[1] == 0U ? 1U : 2U;
    mpz_import(result, word_count, -1, sizeof(words[0]), 0, 0, words);
}

static double monotonic_seconds(void)
{
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) != 0) {
        die("clock_gettime failed");
    }
    return (double)now.tv_sec + (double)now.tv_nsec / 1000000000.0;
}

static int parse_n(const char *text, const char *label)
{
    char *end = NULL;
    errno = 0;
    long value = strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' ||
        value < SEQUENCE_OFFSET || value > MAX_SUPPORTED_N) {
        fprintf(stderr, "error: %s must be in %d..%d: %s\n",
                label, SEQUENCE_OFFSET, MAX_SUPPORTED_N, text);
        exit(EXIT_FAILURE);
    }
    return (int)value;
}

static int parse_d(const char *text)
{
    char *end = NULL;
    errno = 0;
    long value = strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' ||
        value < 2 || value > MAX_SUPPORTED_D) {
        fprintf(stderr, "error: D must be in 2..%d: %s\n",
                MAX_SUPPORTED_D, text);
        exit(EXIT_FAILURE);
    }
    return (int)value;
}

static uint64_t hash_weight(unsigned index)
{
    uint64_t value = (uint64_t)index + UINT64_C(0x9e3779b97f4a7c15);
    value = (value ^ (value >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    value = (value ^ (value >> 27)) * UINT64_C(0x94d049bb133111eb);
    return value ^ (value >> 31);
}

/* A linear hash makes hash(A+B) == hash(A)+hash(B), modulo 2^64. */
static uint64_t histogram_hash(
    const uint8_t count[MAX_COMPONENT_LENGTH], int length)
{
    uint64_t hash = 0U;
    for (int i = 0; i < length; ++i) {
        hash += (uint64_t)count[i] * hash_weight((unsigned)i);
    }
    return hash;
}

static bool same_histogram(
    const uint8_t left[MAX_COMPONENT_LENGTH],
    const uint8_t right[MAX_COMPONENT_LENGTH], int length)
{
    return memcmp(left, right, (size_t)length) == 0;
}

static void hist_map_init(HistMap *map, int key_length)
{
    if (key_length < 0 || key_length > MAX_COMPONENT_LENGTH) {
        die("invalid histogram key length");
    }
    map->capacity = INITIAL_HIST_CAPACITY;
    map->size = 0U;
    map->key_length = key_length;
    map->slots = xcalloc_array(map->capacity, sizeof(*map->slots));
}

static void hist_map_destroy(HistMap *map)
{
    free(map->slots);
    memset(map, 0, sizeof(*map));
}

static HistSlot *hist_map_find_slot(
    HistMap *map, const uint8_t count[MAX_COMPONENT_LENGTH], uint64_t hash)
{
    size_t mask = map->capacity - 1U;
    size_t index = (size_t)hash & mask;
    while (map->slots[index].occupied &&
           (map->slots[index].hash != hash ||
            !same_histogram(map->slots[index].count, count,
                            map->key_length))) {
        index = (index + 1U) & mask;
    }
    return &map->slots[index];
}

static void hist_map_rehash(HistMap *map)
{
    if (map->capacity > SIZE_MAX / 2U) {
        die("histogram table capacity overflow");
    }
    HistSlot *old_slots = map->slots;
    size_t old_capacity = map->capacity;
    map->capacity *= 2U;
    map->size = 0U;
    map->slots = xcalloc_array(map->capacity, sizeof(*map->slots));
    for (size_t i = 0; i < old_capacity; ++i) {
        if (!old_slots[i].occupied) {
            continue;
        }
        HistSlot *destination =
            hist_map_find_slot(map, old_slots[i].count,
                               old_slots[i].hash);
        *destination = old_slots[i];
        ++map->size;
    }
    free(old_slots);
}

static void hist_map_add(
    HistMap *map, const uint8_t count[MAX_COMPONENT_LENGTH],
    uint64_t hash, uint128_t coefficient)
{
    if ((map->size + 1U) * LOAD_DENOMINATOR >
        map->capacity * LOAD_NUMERATOR) {
        hist_map_rehash(map);
    }
    HistSlot *slot = hist_map_find_slot(map, count, hash);
    if (slot->occupied) {
        slot->coefficient = checked_add_u128(
            slot->coefficient, coefficient);
        return;
    }
    slot->occupied = true;
    slot->hash = hash;
    memcpy(slot->count, count, (size_t)map->key_length);
    slot->coefficient = coefficient;
    ++map->size;
}

static uint64_t binomial_u64(unsigned n, unsigned k)
{
    if (k > n) {
        return 0U;
    }
    if (k > n - k) {
        k = n - k;
    }
    uint64_t value = 1U;
    for (unsigned i = 1U; i <= k; ++i) {
        uint128_t product = (uint128_t)value * (n - k + i);
        if (product % i != 0U) {
            die("nonintegral binomial intermediate");
        }
        product /= i;
        if (product > UINT64_MAX) {
            die("binomial coefficient exceeds uint64_t");
        }
        value = (uint64_t)product;
    }
    return value;
}

static uint64_t histogram_orderings(
    const uint8_t histogram[MAX_COMPONENT_LENGTH], int length_limit)
{
    unsigned placed = 0U;
    uint64_t value = 1U;
    for (int length = 0; length < length_limit; ++length) {
        unsigned count = histogram[length];
        value = checked_mul_u64(
            value, binomial_u64(placed + count, count));
        placed += count;
    }
    return value;
}

static void local_set_init(LocalPatternSet *set)
{
    memset(set, 0, sizeof(*set));
}

static void local_set_append(
    LocalPatternSet *set,
    const uint8_t histogram[MAX_COMPONENT_LENGTH],
    int length_limit, uint64_t coefficient)
{
    if (set->count == set->capacity) {
        size_t new_capacity = set->capacity == 0U
                                  ? 32U : set->capacity * 2U;
        if (new_capacity < set->capacity) {
            die("local-pattern capacity overflow");
        }
        set->item = xrealloc_array(set->item, new_capacity,
                                   sizeof(*set->item));
        set->capacity = new_capacity;
    }
    memcpy(set->item[set->count].count, histogram,
           MAX_COMPONENT_LENGTH);
    set->item[set->count].hash =
        histogram_hash(histogram, length_limit);
    set->item[set->count].coefficient = coefficient;
    ++set->count;
}

static void local_set_destroy(LocalPatternSet *set)
{
    free(set->item);
    memset(set, 0, sizeof(*set));
}

typedef struct {
    int path_size;
    int length_limit;
    uint8_t histogram[MAX_COMPONENT_LENGTH];
    LocalPatternSet *output;
} LocalGeneration;

static void generate_local_recursive(LocalGeneration *generation,
                                     int length_index, int edge_count,
                                     int component_count)
{
    if (length_index == generation->length_limit) {
        uint64_t gap_choices = binomial_u64(
            (unsigned)(generation->path_size - edge_count),
            (unsigned)component_count);
        uint64_t component_orders = histogram_orderings(
            generation->histogram, generation->length_limit);
        local_set_append(
            generation->output, generation->histogram,
            generation->length_limit,
            checked_mul_u64(gap_choices, component_orders));
        return;
    }

    int length = length_index + 1;
    int remaining_vertices = generation->path_size -
                             edge_count - component_count;
    unsigned maximum =
        (unsigned)(remaining_vertices / (length + 1));
    for (unsigned count = 0U; count <= maximum; ++count) {
        generation->histogram[length_index] = (uint8_t)count;
        generate_local_recursive(
            generation, length_index + 1,
            edge_count + length * (int)count,
            component_count + (int)count);
    }
    generation->histogram[length_index] = 0U;
}

static void generate_local_patterns(int path_size,
                                    LocalPatternSet *output)
{
    if (path_size < 1 || path_size > MAX_COMPONENT_LENGTH + 1) {
        die("residue path exceeds the compiled component limit");
    }
    local_set_init(output);
    LocalGeneration generation;
    generation.path_size = path_size;
    generation.length_limit = path_size - 1;
    memset(generation.histogram, 0, sizeof(generation.histogram));
    generation.output = output;
    generate_local_recursive(&generation, 0, 0, 0);
}

static void build_tiling_coefficients(
    HistMap *result, const int path_sizes[MAX_SUPPORTED_D], int d,
    FormulaStats *stats)
{
    int length_limit = 0;
    for (int path = 0; path < d; ++path) {
        if (path_sizes[path] - 1 > length_limit) {
            length_limit = path_sizes[path] - 1;
        }
    }

    HistMap current;
    hist_map_init(&current, length_limit);
    uint8_t zero[MAX_COMPONENT_LENGTH] = {0};
    hist_map_add(&current, zero, 0U, 1U);
    stats->peak_histogram_count = 1U;

    LocalPatternSet cache[2];
    int cache_path_size[2] = {-1, -1};
    int cache_count = 0;
    local_set_init(&cache[0]);
    local_set_init(&cache[1]);

    for (int path = 0; path < d; ++path) {
        int cache_index = -1;
        for (int i = 0; i < cache_count; ++i) {
            if (cache_path_size[i] == path_sizes[path]) {
                cache_index = i;
                break;
            }
        }
        if (cache_index < 0) {
            if (cache_count == 2) {
                die("more than two residue-path sizes found");
            }
            cache_index = cache_count++;
            cache_path_size[cache_index] = path_sizes[path];
            generate_local_patterns(path_sizes[path], &cache[cache_index]);
        }
        const LocalPatternSet *patterns = &cache[cache_index];
        HistMap next;
        hist_map_init(&next, length_limit);
        for (size_t slot_index = 0;
             slot_index < current.capacity; ++slot_index) {
            const HistSlot *slot = &current.slots[slot_index];
            if (!slot->occupied) {
                continue;
            }
            for (size_t pattern_index = 0;
                 pattern_index < patterns->count; ++pattern_index) {
                uint8_t combined[MAX_COMPONENT_LENGTH];
                for (int length = 0; length < length_limit; ++length) {
                    unsigned value = (unsigned)slot->count[length] +
                        patterns->item[pattern_index].count[length];
                    if (value > UINT8_MAX) {
                        die("component histogram overflow");
                    }
                    combined[length] = (uint8_t)value;
                }
                uint128_t coefficient = checked_mul_u128_u64(
                    slot->coefficient,
                    patterns->item[pattern_index].coefficient);
                uint64_t combined_hash =
                    slot->hash + patterns->item[pattern_index].hash;
                hist_map_add(&next, combined, combined_hash, coefficient);
                stats->convolution_pairs = checked_add_u64(
                    stats->convolution_pairs, 1U);
            }
        }
        hist_map_destroy(&current);
        current = next;
        if (current.size > stats->peak_histogram_count) {
            stats->peak_histogram_count = current.size;
        }
    }

    for (int i = 0; i < cache_count; ++i) {
        local_set_destroy(&cache[i]);
    }
    *result = current;
}

static void verify_decimal(const mpz_t value, const char *expected,
                           const char *label, int n)
{
    mpz_t reference;
    mpz_init(reference);
    if (mpz_set_str(reference, expected, 10) != 0) {
        mpz_clear(reference);
        die("invalid built-in decimal term");
    }
    if (mpz_cmp(value, reference) != 0) {
        gmp_fprintf(stderr,
                    "error: %s mismatch at n=%d: got %Zd, expected %Zd\n",
                    label, n, value, reference);
        mpz_clear(reference);
        exit(EXIT_FAILURE);
    }
    mpz_clear(reference);
}

static const KnownSequence *find_known_sequence(int d)
{
    for (size_t i = 0; i < ARRAY_LENGTH(known_sequences); ++i) {
        if (known_sequences[i].d == d) {
            return &known_sequences[i];
        }
    }
    return NULL;
}

static void verify_known_value(const mpz_t value, int n, int d)
{
    const KnownSequence *sequence = find_known_sequence(d);
    if (sequence == NULL || n < sequence->first_n) {
        return;
    }
    size_t index = (size_t)(n - sequence->first_n);
    if (index < sequence->term_count) {
        verify_decimal(value, sequence->term[index],
                       sequence->oeis_id, n);
    }
}

static void compute_formula(mpz_t result, int n, int d,
                            FormulaStats *stats)
{
    if (n < 0 || n > MAX_SUPPORTED_N ||
        d < 2 || d > MAX_SUPPORTED_D) {
        die("unsupported n or d");
    }
    memset(stats, 0, sizeof(*stats));
    double started = monotonic_seconds();

    mpz_t factorial[MAX_SUPPORTED_N + 1];
    for (int i = 0; i <= n; ++i) {
        mpz_init(factorial[i]);
    }
    mpz_set_ui(factorial[0], 1UL);
    for (int i = 1; i <= n; ++i) {
        mpz_mul_ui(factorial[i], factorial[i - 1], (unsigned long)i);
    }

    if (n <= d) {
        mpz_set(result, factorial[n]);
        stats->histogram_count = 1U;
        stats->peak_histogram_count = 1U;
        stats->coefficient_sum = 1U;
        stats->seconds = monotonic_seconds() - started;
        for (int i = 0; i <= n; ++i) {
            mpz_clear(factorial[i]);
        }
        return;
    }

    int path_sizes[MAX_SUPPORTED_D] = {0};
    int length_limit = 0;
    for (int k = 1; k <= d; ++k) {
        path_sizes[k - 1] = (n - k) / d + 1;
        if (path_sizes[k - 1] > MAX_COMPONENT_LENGTH + 1) {
            die("residue path exceeds the compiled component limit");
        }
        if (path_sizes[k - 1] - 1 > length_limit) {
            length_limit = path_sizes[k - 1] - 1;
        }
    }

    HistMap coefficients;
    build_tiling_coefficients(&coefficients, path_sizes, d, stats);
    stats->histogram_count = coefficients.size;

    uint128_t coefficient_sum = 0U;
    for (size_t i = 0; i < coefficients.capacity; ++i) {
        if (coefficients.slots[i].occupied) {
            coefficient_sum = checked_add_u128(
                coefficient_sum, coefficients.slots[i].coefficient);
        }
    }
    uint128_t expected_sum = (uint128_t)1U << (unsigned)(n - d);
    if (coefficient_sum != expected_sum) {
        hist_map_destroy(&coefficients);
        die("tiling coefficients do not sum to 2^(n-d)");
    }
    stats->coefficient_sum = coefficient_sum;

    mpz_t term;
    mpz_init(term);
    mpz_set_ui(result, 0UL);
    for (size_t slot_index = 0;
         slot_index < coefficients.capacity; ++slot_index) {
        const HistSlot *slot = &coefficients.slots[slot_index];
        if (!slot->occupied) {
            continue;
        }
        int r = 0;
        int c = 0;
        for (int length = 0; length < length_limit; ++length) {
            r += (length + 1) * slot->count[length];
            c += slot->count[length];
        }
        int singleton_count = n - r - c;
        if (singleton_count < 0) {
            mpz_clear(term);
            hist_map_destroy(&coefficients);
            die("invalid tile histogram");
        }

        mpz_set_u128(term, slot->coefficient);
        mpz_mul(term, term, term);
        mpz_mul(term, term, factorial[singleton_count]);
        for (int length = 0; length < length_limit; ++length) {
            if (slot->count[length] > 1U) {
                mpz_mul(term, term, factorial[slot->count[length]]);
            }
        }
        mpz_mul_2exp(term, term, (mp_bitcnt_t)c);
        if ((r & 1) == 0) {
            mpz_add(result, result, term);
        } else {
            mpz_sub(result, result, term);
        }
    }

    if (mpz_sgn(result) < 0 || mpz_cmp(result, factorial[n]) > 0) {
        mpz_clear(term);
        hist_map_destroy(&coefficients);
        die("formula result is outside 0..n!");
    }
    mpz_clear(term);
    hist_map_destroy(&coefficients);
    stats->seconds = monotonic_seconds() - started;
    for (int i = 0; i <= n; ++i) {
        mpz_clear(factorial[i]);
    }
}

static uint64_t brute_recursive(BruteContext *context, uint64_t used,
                                int last_value)
{
    int assigned = __builtin_popcountll(used);
    if (assigned == context->n) {
        return 1U;
    }
    if (context->starts_chain[assigned]) {
        last_value = context->n;
    }
    size_t index = checked_product_size(
                       (size_t)used, (size_t)context->n + 1U) +
                   (size_t)last_value;
    if (context->memo[index] != UINT64_MAX) {
        return context->memo[index];
    }
    uint64_t total = 0U;
    for (int value = 0; value < context->n; ++value) {
        uint64_t bit = UINT64_C(1) << (unsigned)value;
        if ((used & bit) != 0U) {
            continue;
        }
        if (last_value < context->n) {
            int difference = value - last_value;
            if (difference == context->d || difference == -context->d) {
                continue;
            }
        }
        total = checked_add_u64(
            total, brute_recursive(context, used | bit, value));
    }
    context->memo[index] = total;
    return total;
}

static uint64_t brute_count(int n, int d)
{
    if (n < 0 || n > BRUTE_CHECK_MAX_N) {
        die("independent check n is outside its supported range");
    }
    BruteContext context;
    context.n = n;
    context.d = d;
    memset(context.starts_chain, 0, sizeof(context.starts_chain));
    int assigned = 0;
    for (int residue = 0; residue < d && residue < n; ++residue) {
        bool first = true;
        for (int position = residue; position < n; position += d) {
            context.starts_chain[assigned++] = first;
            first = false;
        }
    }
    if (assigned != n) {
        die("independent-check position ordering failed");
    }
    size_t mask_count = (size_t)1U << (unsigned)n;
    size_t memo_count = checked_product_size(
        mask_count, (size_t)n + 1U);
    context.memo = xmalloc_array(memo_count, sizeof(*context.memo));
    memset(context.memo, 0xff,
           checked_product_size(memo_count, sizeof(*context.memo)));
    uint64_t result = brute_recursive(&context, 0U, n);
    free(context.memo);
    return result;
}

static void report_formula(int n, int d, const FormulaStats *stats)
{
    fprintf(stderr,
            "360386_02: d=%d, n=%d, histograms=%zu, peak=%zu, "
            "convolution pairs=%" PRIu64 ", %.3f s\n",
            d, n, stats->histogram_count,
            stats->peak_histogram_count,
            stats->convolution_pairs, stats->seconds);
}

static void compute_value(mpz_t result, int n, int d, bool verbose)
{
    FormulaStats stats;
    compute_formula(result, n, d, &stats);
    verify_known_value(result, n, d);
    if (verbose) {
        report_formula(n, d, &stats);
    }
}

static int check_implementation(int maximum_n)
{
    mpz_t value;
    mpz_init(value);
    for (int n = 0; n <= maximum_n; ++n) {
        compute_value(value, n, A360386_D, false);
        if (n <= BRUTE_CHECK_MAX_N) {
            uint64_t independent = brute_count(n, A360386_D);
            if (mpz_cmp_ui(value, (unsigned long)independent) != 0) {
                gmp_fprintf(stderr,
                            "error: A360386 DP mismatch at n=%d: "
                            "formula=%Zd, DP=%" PRIu64 "\n",
                            n, value, independent);
                mpz_clear(value);
                return EXIT_FAILURE;
            }
        }
    }
    for (size_t sequence_index = 0;
         sequence_index < ARRAY_LENGTH(known_sequences);
         ++sequence_index) {
        const KnownSequence *sequence =
            &known_sequences[sequence_index];
        for (size_t term_index = 0;
             term_index < sequence->term_count; ++term_index) {
            int n = sequence->first_n + (int)term_index;
            FormulaStats stats;
            compute_formula(value, n, sequence->d, &stats);
            verify_decimal(value, sequence->term[term_index],
                           sequence->oeis_id, n);
        }
    }
    for (int d = 2; d <= GENERAL_CHECK_MAX_D; ++d) {
        for (int n = 0; n <= GENERAL_CHECK_MAX_N; ++n) {
            FormulaStats stats;
            compute_formula(value, n, d, &stats);
            uint64_t independent = brute_count(n, d);
            if (mpz_cmp_ui(value, (unsigned long)independent) != 0) {
                gmp_fprintf(stderr,
                            "error: general-d DP mismatch at d=%d, n=%d: "
                            "formula=%Zd, DP=%" PRIu64 "\n",
                            d, n, value, independent);
                mpz_clear(value);
                return EXIT_FAILURE;
            }
        }
    }
    mpz_clear(value);
    printf("ok: OEIS b-files verified:");
    for (size_t i = 0; i < ARRAY_LENGTH(known_sequences); ++i) {
        const KnownSequence *sequence = &known_sequences[i];
        int last_n = sequence->first_n +
                     (int)sequence->term_count - 1;
        printf("%s %s(d=%d,n=%d..%d)",
               i == 0U ? "" : ",", sequence->oeis_id,
               sequence->d, sequence->first_n, last_n);
    }
    printf("; independent DP for d=2..%d through n=%d "
           "(and d=%d through n=%d)\n",
           GENERAL_CHECK_MAX_D, GENERAL_CHECK_MAX_N,
           A360386_D,
           maximum_n < BRUTE_CHECK_MAX_N
               ? maximum_n : BRUTE_CHECK_MAX_N);
    return EXIT_SUCCESS;
}

static char *path_beside_executable(const char *argv0,
                                    const char *filename)
{
    char executable[PATH_MAX];
    char resolved[PATH_MAX];
    bool found = false;
#ifdef __APPLE__
    uint32_t size = (uint32_t)sizeof(executable);
    if (_NSGetExecutablePath(executable, &size) == 0) {
        found = true;
    }
#elif defined(__linux__)
    ssize_t length = readlink("/proc/self/exe", executable,
                              sizeof(executable) - 1U);
    if (length >= 0) {
        executable[length] = '\0';
        found = true;
    }
#endif
    if (!found) {
        size_t length = strlen(argv0);
        if (length >= sizeof(executable)) {
            die("executable path is too long");
        }
        memcpy(executable, argv0, length + 1U);
    }
    const char *base = realpath(executable, resolved);
    if (base == NULL) {
        base = executable;
    }
    const char *slash = strrchr(base, '/');
    size_t directory_length =
        slash == NULL ? 1U : (size_t)(slash - base);
    if (slash != NULL && directory_length == 0U) {
        directory_length = 1U;
    }
    size_t filename_length = strlen(filename);
    char *path = xmalloc_array(directory_length + 1U +
                               filename_length + 1U, 1U);
    if (slash == NULL) {
        path[0] = '.';
    } else if (slash == base) {
        path[0] = '/';
    } else {
        memcpy(path, base, directory_length);
    }
    path[directory_length] = '/';
    memcpy(path + directory_length + 1U, filename,
           filename_length + 1U);
    return path;
}

static void produce_file(const char *argv0, int maximum_n, int d)
{
    char final_name[64];
    char part_name[64];
    int final_length = snprintf(final_name, sizeof(final_name),
                                "b360386_02_d%d.txt", d);
    int part_length = snprintf(part_name, sizeof(part_name),
                               "b360386_02_d%d_part.txt", d);
    if (final_length < 0 || (size_t)final_length >= sizeof(final_name) ||
        part_length < 0 || (size_t)part_length >= sizeof(part_name)) {
        die("output filename is too long");
    }

    char *final_path = path_beside_executable(argv0, final_name);
    char *part_path = path_beside_executable(argv0, part_name);
    FILE *stream = fopen(part_path, "w");
    if (stream == NULL) {
        fprintf(stderr, "error: cannot open %s: %s\n",
                part_path, strerror(errno));
        free(final_path);
        free(part_path);
        exit(EXIT_FAILURE);
    }
    mpz_t value;
    mpz_init(value);
    for (int n = 0; n <= maximum_n; ++n) {
        compute_value(value, n, d, true);
        if (gmp_fprintf(stream, "%d %Zd\n", n, value) < 0 ||
            fflush(stream) != 0) {
            mpz_clear(value);
            fclose(stream);
            free(final_path);
            free(part_path);
            die("could not write the output file");
        }
    }
    mpz_clear(value);
    if (fclose(stream) != 0) {
        free(final_path);
        free(part_path);
        die("could not close the output file");
    }
    if (rename(part_path, final_path) != 0) {
        fprintf(stderr, "error: cannot rename %s to %s: %s\n",
                part_path, final_path, strerror(errno));
        free(final_path);
        free(part_path);
        exit(EXIT_FAILURE);
    }
    printf("wrote %s (d=%d, n=0..%d)\n",
           final_path, d, maximum_n);
    free(final_path);
    free(part_path);
}

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s [MAX_N]\n"
            "       %s --term N\n"
            "       %s --d D [MAX_N]\n"
            "       %s --d D --term N\n"
            "       %s --d D --upto MAX_N\n"
            "       %s --check [MAX_N]\n"
            "\n"
            "D defaults to %d (A360386) and may be 2..%d.\n"
            "MAX_N defaults to %d and may be at most %d.\n"
            "A range run writes b360386_02_dD.txt.\n"
            "--check verifies built-in OEIS b-file data for D=2..10.\n",
            program, program, program, program, program, program,
            A360386_D, MAX_SUPPORTED_D,
            DEFAULT_MAX_N, MAX_SUPPORTED_N);
}

int main(int argc, char **argv)
{
    if (argc == 2 &&
        (strcmp(argv[1], "--help") == 0 ||
         strcmp(argv[1], "-h") == 0)) {
        usage(argv[0]);
        return EXIT_SUCCESS;
    }
    if (argc >= 2 && strcmp(argv[1], "--check") == 0) {
        if (argc > 3) {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
        int maximum_n = argc == 3
                            ? parse_n(argv[2], "MAX_N")
                            : DEFAULT_CHECK_N;
        return check_implementation(maximum_n);
    }

    typedef enum {
        RUN_UPTO,
        RUN_TERM
    } RunMode;
    RunMode mode = RUN_UPTO;
    int d = A360386_D;
    int n = -1;
    bool have_d = false;
    bool have_n = false;

    for (int argument = 1; argument < argc; ++argument) {
        const char *text = argv[argument];
        if (strcmp(text, "--d") == 0) {
            if (have_d || argument + 1 >= argc) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            d = parse_d(argv[++argument]);
            have_d = true;
        } else if (strcmp(text, "--term") == 0 ||
                   strcmp(text, "--upto") == 0) {
            if (have_n || argument + 1 >= argc) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            mode = strcmp(text, "--term") == 0 ? RUN_TERM : RUN_UPTO;
            n = parse_n(argv[++argument],
                        mode == RUN_TERM ? "N" : "MAX_N");
            have_n = true;
        } else if (text[0] == '-' || have_n) {
            usage(argv[0]);
            return EXIT_FAILURE;
        } else {
            n = parse_n(text, "MAX_N");
            mode = RUN_UPTO;
            have_n = true;
        }
    }

    if (!have_n) {
        n = DEFAULT_MAX_N;
    }
    if (mode == RUN_TERM) {
        mpz_t value;
        mpz_init(value);
        compute_value(value, n, d, true);
        gmp_printf("%d %Zd\n", n, value);
        mpz_clear(value);
        return EXIT_SUCCESS;
    }
    produce_file(argv[0], n, d);
    return EXIT_SUCCESS;
}
