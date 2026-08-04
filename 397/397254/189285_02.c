/*
 * A189285 and directed a_{d,d} -- Tauraso-style q convolution.
 *
 * Count permutations p of {1,...,n} satisfying
 *
 *                 p(i+d) - p(i) != d,   1 <= i <= n-d.
 *
 * The command line accepts 2<=d<=64.  Its default d=6 is A189285.
 *
 * George Spahn and Doron Zeilberger give the inclusion-exclusion formula
 * for the directed condition p(i+r)-p(i) != s in
 *
 *   "Counting Permutations Where The Difference Between Entries Located
 *    r Places Apart Can never be s",
 *   Enumerative Combinatorics and Applications 3:2 (2023), Article S2R10,
 *   https://doi.org/10.54550/ECA2023V3S2R10
 *
 * Roberto Tauraso's q construction in
 *
 *   "The Dinner Table Problem: The Rectangular Case",
 *   INTEGERS 6 (2006), #A11,
 *   https://math.colgate.edu/~integers/g11/g11.pdf
 *
 * provides the convolution organization retained by this program.  His
 * absolute-difference problem counts two orientations of each nonempty
 * component and consequently contains 2^c.  The directed a_{d,d} formula
 * counts only the +d orientation, so that factor is absent here.
 *
 * Put n_k = |{i in [n] : i == k (mod d)}|.  For a selected subset of the
 * n_k-1 edges in the k-th path, let h_l count all components of edge-length
 * l, over all d paths.  Define
 *
 *       r = sum_l l*h_l,             c = sum_l h_l.
 *
 * Let W(H) be the number of source edge subsets with component histogram H.
 * For a path of m vertices and a fixed histogram, its multiplicity is
 *
 *       binomial(m-r,c) c! / product_l h_l!.
 *
 * Generating these histograms and multiplying their polynomials computes W
 * exactly, without enumerating 2^(m-1) edge subsets.  This simultaneously
 * sums Tauraso's factors
 *
 *       product_k binomial(n_k-r_k,c_k)
 *
 * and all compositions l_{1,k}+...+l_{c_k,k}=r_k.
 *
 * The corresponding q_{n,d}(L) depends only on H.  If x_l components of
 * length l are sent to a target path of size m, its unlabeled contribution
 * is
 *
 *       (m-s)_{t} / product_l x_l!,
 *       s = sum_l l*x_l,  t = sum_l x_l,
 *
 * where (u)_t is a falling factorial and sum_l (l+1)*x_l <= m.  Therefore
 * q(H) is product_l h_l! times the coefficient of z^H in the product of
 * these d small target-path polynomials.  The default evaluator multiplies
 * those polynomials once and extracts every required coefficient together;
 * this only interchanges a finite sum and coefficient extraction in
 * the inclusion-exclusion formula.  It does not replace it by a recurrence.
 * The older per-H bounded mixed-radix
 * convolution remains available as --direct-q for an independent check.
 * For the directed condition, the theorem becomes
 *
 *   a_{d,d}(n) = sum_H (-1)^r (n-r-c)! W(H) q(H).
 *
 * Histogram maps use only the active ceil(n/d)-1 components.  Each slot
 * stores a linear 64-bit hash, so a componentwise-sum hash is obtained by
 * one unsigned addition during convolution.  Collisions are still resolved
 * by exact byte comparison; this changes only lookup cost, never equality.
 *
 * GMP is used for every quantity that can grow like n!.  The command line
 * supports 2<=d<=64 and n<=128.  A residue path can therefore have at most
 * 64 vertices.  Source multiplicities fit unsigned 128-bit arithmetic
 * because their sum is at most 2^126.  A range run writes
 * b189285_02_dD.txt.  A readable part file is flushed after every term and
 * renamed atomically after success.
 *
 * Checks:
 *   - every available OEIS term a(0)..a(24) is built in and verified;
 *   - --check also uses an independent permutation DP through n=16;
 *   - general d=2..8 is independently checked through n=12.
 *
 * Build:
 *
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic \
 *     -I/opt/homebrew/include -L/opt/homebrew/lib \
 *     189285_02.c -lgmp -o 189285_02
 *
 * Usage:
 *
 *   ./189285_02                 # write a(0)..a(20)
 *   ./189285_02 32              # write a(0)..a(32)
 *   ./189285_02 --term 32
 *   ./189285_02 --upto 32
 *   ./189285_02 --d 2 --term 30
 *   ./189285_02 --direct-q --term 30
 *   ./189285_02 --check
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
#error "189285_02 requires a platform with 64-bit unsigned long"
#endif

#if !defined(__SIZEOF_INT128__)
#error "189285_02 requires compiler support for unsigned __int128"
#endif

__extension__ typedef unsigned __int128 uint128_t;

#define A189285_D 6
#define SEQUENCE_OFFSET 0
#define DEFAULT_MAX_N 20
#define DEFAULT_CHECK_N 24
#define MAX_SUPPORTED_N 128
#define MAX_SUPPORTED_D 64
#define KNOWN_MAX_N 24
#define BRUTE_CHECK_MAX_N 16
#define GENERAL_CHECK_MAX_D 8
#define GENERAL_CHECK_MAX_N 12
#define DIRECT_Q_CHECK_MAX_N 20
#define MAX_COMPONENT_LENGTH (MAX_SUPPORTED_N / 2 - 1)
#define INITIAL_HIST_CAPACITY ((size_t)16)
#define LOAD_NUMERATOR ((size_t)7)
#define LOAD_DENOMINATOR ((size_t)10)

static const char *const known_terms[KNOWN_MAX_N + 1] = {
    "1",
    "1",
    "2",
    "6",
    "24",
    "120",
    "720",
    "4920",
    "37488",
    "319644",
    "3033264",
    "31784280",
    "364902480",
    "4538652840",
    "61102571376",
    "885045657564",
    "13722397569072",
    "226742901078120",
    "3977354871110160",
    "73816786920489720",
    "1444940702597713008",
    "29750236302549282948",
    "642693417769786746864",
    "14535097378893765189240",
    "343437047642562028552080"
};

typedef struct {
    bool occupied;
    uint64_t hash;
    uint8_t count[MAX_COMPONENT_LENGTH];
    uint128_t multiplicity;
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
    uint64_t multiplicity;
} LocalPattern;

typedef struct {
    LocalPattern *item;
    size_t count;
    size_t capacity;
} LocalPatternSet;

typedef struct {
    uint8_t count[MAX_COMPONENT_LENGTH];
    size_t delta;
    unsigned long weight;
} TargetConfig;

typedef struct {
    TargetConfig *item;
    size_t count;
    size_t capacity;
} TargetConfigSet;

typedef struct {
    mpz_t *left;
    mpz_t *right;
    uint32_t *left_used;
    uint32_t *right_used;
    uint8_t *digits;
    size_t capacity;
} QWorkspace;

typedef enum {
    Q_GLOBAL,
    Q_DIRECT
} QMode;

typedef struct {
    size_t source_histograms;
    size_t target_histograms;
    uint64_t histogram_convolution_pairs;
    size_t maximum_q_states;
    uint64_t q_transitions;
    QMode q_mode;
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

static size_t checked_product(size_t a, size_t b)
{
    if (a != 0U && b > SIZE_MAX / a) {
        die("allocation size overflow");
    }
    return a * b;
}

static void *xmalloc_array(size_t count, size_t size)
{
    size_t bytes = checked_product(count, size);
    void *pointer = malloc(bytes == 0U ? 1U : bytes);
    if (pointer == NULL) {
        die("out of memory");
    }
    return pointer;
}

static void *xcalloc_array(size_t count, size_t size)
{
    (void)checked_product(count, size);
    void *pointer = calloc(count == 0U ? 1U : count,
                           size == 0U ? 1U : size);
    if (pointer == NULL) {
        die("out of memory");
    }
    return pointer;
}

static void *xrealloc_array(void *old, size_t count, size_t size)
{
    size_t bytes = checked_product(count, size);
    void *pointer = realloc(old, bytes == 0U ? 1U : bytes);
    if (pointer == NULL) {
        die("out of memory while growing an array");
    }
    return pointer;
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

static void validate_n_d(int n, int d)
{
    if (n < 0 || n > MAX_SUPPORTED_N ||
        d < 2 || d > MAX_SUPPORTED_D) {
        die("unsupported n or d");
    }
}

static uint64_t checked_add_u64(uint64_t a, uint64_t b)
{
    if (b > UINT64_MAX - a) {
        die("source multiplicity addition overflow");
    }
    return a + b;
}

static uint64_t checked_mul_u64(uint64_t a, uint64_t b)
{
    if (a != 0U && b > UINT64_MAX / a) {
        die("source multiplicity multiplication overflow");
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

static bool same_histogram(const uint8_t left[MAX_COMPONENT_LENGTH],
                           const uint8_t right[MAX_COMPONENT_LENGTH],
                           int length)
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
    map->slots = NULL;
    map->capacity = 0U;
    map->size = 0U;
    map->key_length = 0;
}

static HistSlot *hist_map_find_slot(HistMap *map,
                                    const uint8_t count[MAX_COMPONENT_LENGTH],
                                    uint64_t hash)
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

static void hist_map_add(HistMap *map,
                         const uint8_t count[MAX_COMPONENT_LENGTH],
                         uint64_t hash, uint128_t multiplicity)
{
    if ((map->size + 1U) * LOAD_DENOMINATOR >
        map->capacity * LOAD_NUMERATOR) {
        hist_map_rehash(map);
    }
    HistSlot *slot = hist_map_find_slot(map, count, hash);
    if (slot->occupied) {
        slot->multiplicity = checked_add_u128(
            slot->multiplicity, multiplicity);
        return;
    }
    slot->occupied = true;
    slot->hash = hash;
    memcpy(slot->count, count, (size_t)map->key_length);
    slot->multiplicity = multiplicity;
    ++map->size;
}

static const HistSlot *hist_map_lookup(
    const HistMap *map,
    const uint8_t count[MAX_COMPONENT_LENGTH], uint64_t hash)
{
    size_t mask = map->capacity - 1U;
    size_t index = (size_t)hash & mask;
    while (map->slots[index].occupied) {
        if (map->slots[index].hash == hash &&
            same_histogram(map->slots[index].count, count,
                           map->key_length)) {
            return &map->slots[index];
        }
        index = (index + 1U) & mask;
    }
    return NULL;
}

static uint128_t hist_map_multiplicity_sum(const HistMap *map)
{
    uint128_t sum = 0U;
    for (size_t i = 0; i < map->capacity; ++i) {
        if (map->slots[i].occupied) {
            sum = checked_add_u128(sum, map->slots[i].multiplicity);
        }
    }
    return sum;
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

static void local_pattern_set_init(LocalPatternSet *set)
{
    memset(set, 0, sizeof(*set));
}

static void local_pattern_set_append(
    LocalPatternSet *set,
    const uint8_t histogram[MAX_COMPONENT_LENGTH],
    int length_limit, uint64_t multiplicity)
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
    set->item[set->count].multiplicity = multiplicity;
    ++set->count;
}

static void local_pattern_set_destroy(LocalPatternSet *set)
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

static void generate_local_patterns_recursive(LocalGeneration *generation,
                                              int length_index,
                                              int edge_count,
                                              int component_count)
{
    if (length_index == generation->length_limit) {
        uint64_t gaps = binomial_u64(
            (unsigned)(generation->path_size - edge_count),
            (unsigned)component_count);
        uint64_t orderings = histogram_orderings(
            generation->histogram, generation->length_limit);
        local_pattern_set_append(
            generation->output, generation->histogram,
            generation->length_limit,
            checked_mul_u64(gaps, orderings));
        return;
    }

    int length = length_index + 1;
    int remaining_vertices = generation->path_size -
                             edge_count - component_count;
    unsigned maximum =
        (unsigned)(remaining_vertices / (length + 1));
    for (unsigned count = 0U; count <= maximum; ++count) {
        generation->histogram[length_index] = (uint8_t)count;
        generate_local_patterns_recursive(
            generation, length_index + 1,
            edge_count + length * (int)count,
            component_count + (int)count);
    }
    generation->histogram[length_index] = 0U;
}

static void local_patterns_for_path(int path_size,
                                    LocalPatternSet *output)
{
    if (path_size < 1 || path_size > MAX_COMPONENT_LENGTH + 1) {
        die("internal path size is outside the supported range");
    }
    local_pattern_set_init(output);
    LocalGeneration generation;
    generation.path_size = path_size;
    generation.length_limit = path_size - 1;
    memset(generation.histogram, 0, sizeof(generation.histogram));
    generation.output = output;
    generate_local_patterns_recursive(&generation, 0, 0, 0);
}

/*
 * Multiply the d local path polynomials.  There are only two possible path
 * sizes, floor(n/d) and ceil(n/d), so cache both local pattern sets.
 *
 * This routine is used once for W(H), and once more by the fast q evaluator
 * for the product of Tauraso's target-path polynomials.  For r=s=d their
 * local coefficients have the same numerical expression, but the two maps
 * are deliberately built separately.  --direct-q does not use the second
 * map at all.
 */
static void build_histogram_product(
    HistMap *result, const int path_sizes[MAX_SUPPORTED_D], int d,
    uint64_t *convolution_pairs)
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

    LocalPatternSet cache[2];
    int cache_path_size[2] = {-1, -1};
    int cache_count = 0;
    local_pattern_set_init(&cache[0]);
    local_pattern_set_init(&cache[1]);

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
            local_patterns_for_path(path_sizes[path], &cache[cache_index]);
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
                for (int length = 0;
                     length < length_limit; ++length) {
                    unsigned value = (unsigned)slot->count[length] +
                                     patterns->item[pattern_index].count[length];
                    if (value > UINT8_MAX) {
                        die("component histogram overflow");
                    }
                    combined[length] = (uint8_t)value;
                }
                uint128_t multiplicity = checked_mul_u128_u64(
                    slot->multiplicity,
                    patterns->item[pattern_index].multiplicity);
                uint64_t combined_hash =
                    slot->hash + patterns->item[pattern_index].hash;
                hist_map_add(&next, combined, combined_hash, multiplicity);
                *convolution_pairs = checked_add_u64(
                    *convolution_pairs, 1U);
            }
        }
        hist_map_destroy(&current);
        current = next;
    }
    for (int i = 0; i < cache_count; ++i) {
        local_pattern_set_destroy(&cache[i]);
    }
    *result = current;
}

static void q_workspace_init(QWorkspace *workspace)
{
    memset(workspace, 0, sizeof(*workspace));
}

static void q_workspace_clear_arrays(QWorkspace *workspace)
{
    if (workspace->left != NULL) {
        for (size_t i = 0; i < workspace->capacity; ++i) {
            mpz_clear(workspace->left[i]);
            mpz_clear(workspace->right[i]);
        }
    }
    free(workspace->left);
    free(workspace->right);
    free(workspace->left_used);
    free(workspace->right_used);
    free(workspace->digits);
    workspace->left = NULL;
    workspace->right = NULL;
    workspace->left_used = NULL;
    workspace->right_used = NULL;
    workspace->digits = NULL;
    workspace->capacity = 0U;
}

static void q_workspace_destroy(QWorkspace *workspace)
{
    q_workspace_clear_arrays(workspace);
}

static void q_workspace_ensure(QWorkspace *workspace, size_t capacity)
{
    if (capacity <= workspace->capacity) {
        return;
    }
    q_workspace_clear_arrays(workspace);
    workspace->left = xmalloc_array(capacity, sizeof(*workspace->left));
    workspace->right = xmalloc_array(capacity, sizeof(*workspace->right));
    workspace->left_used =
        xmalloc_array(capacity, sizeof(*workspace->left_used));
    workspace->right_used =
        xmalloc_array(capacity, sizeof(*workspace->right_used));
    workspace->digits = xmalloc_array(
        checked_product(capacity, MAX_COMPONENT_LENGTH),
        sizeof(*workspace->digits));
    for (size_t i = 0; i < capacity; ++i) {
        mpz_init(workspace->left[i]);
        mpz_init(workspace->right[i]);
    }
    workspace->capacity = capacity;
}

static void target_config_set_init(TargetConfigSet *set)
{
    memset(set, 0, sizeof(*set));
}

static void target_config_set_append(
    TargetConfigSet *set,
    const uint8_t count[MAX_COMPONENT_LENGTH],
    size_t delta, unsigned long weight)
{
    if (set->count == set->capacity) {
        size_t new_capacity = set->capacity == 0U
                                  ? 32U : set->capacity * 2U;
        if (new_capacity < set->capacity) {
            die("target-configuration capacity overflow");
        }
        set->item = xrealloc_array(set->item, new_capacity,
                                   sizeof(*set->item));
        set->capacity = new_capacity;
    }
    TargetConfig *configuration = &set->item[set->count++];
    memcpy(configuration->count, count, MAX_COMPONENT_LENGTH);
    configuration->delta = delta;
    configuration->weight = weight;
}

static void target_config_set_destroy(TargetConfigSet *set)
{
    free(set->item);
    memset(set, 0, sizeof(*set));
}

typedef struct {
    const uint8_t *histogram;
    const size_t *stride;
    int path_size;
    int length_limit;
    uint8_t current[MAX_COMPONENT_LENGTH];
    TargetConfigSet *output;
} TargetGeneration;

static void generate_target_configs_recursive(TargetGeneration *generation,
                                              int length_index,
                                              int used_lengths,
                                              int component_count)
{
    if (length_index == generation->length_limit) {
        int available = generation->path_size - used_lengths;
        if (available < component_count) {
            die("invalid target configuration");
        }
        size_t delta = 0U;
        for (int i = 0; i < generation->length_limit; ++i) {
            delta += (size_t)generation->current[i] *
                     generation->stride[i];
        }
        uint64_t gaps = binomial_u64((unsigned)available,
                                     (unsigned)component_count);
        uint64_t orderings = histogram_orderings(
            generation->current, generation->length_limit);
        uint64_t weight = checked_mul_u64(gaps, orderings);
        target_config_set_append(generation->output,
                                 generation->current, delta,
                                 (unsigned long)weight);
        return;
    }

    int length = length_index + 1;
    int occupied_per_component = length + 1;
    int remaining_slots = generation->path_size -
                          used_lengths - component_count;
    unsigned maximum = generation->histogram[length_index];
    unsigned capacity_maximum =
        remaining_slots <= 0
            ? 0U
            : (unsigned)(remaining_slots / occupied_per_component);
    if (maximum > capacity_maximum) {
        maximum = capacity_maximum;
    }
    for (unsigned count = 0U; count <= maximum; ++count) {
        generation->current[length_index] = (uint8_t)count;
        generate_target_configs_recursive(
            generation, length_index + 1,
            used_lengths + length * (int)count,
            component_count + (int)count);
    }
    generation->current[length_index] = 0U;
}

static void generate_target_configs(
    const uint8_t histogram[MAX_COMPONENT_LENGTH],
    const size_t stride[MAX_COMPONENT_LENGTH], int path_size,
    int length_limit, TargetConfigSet *output)
{
    target_config_set_init(output);
    TargetGeneration generation;
    generation.histogram = histogram;
    generation.stride = stride;
    generation.path_size = path_size;
    generation.length_limit = length_limit;
    memset(generation.current, 0, sizeof(generation.current));
    generation.output = output;
    generate_target_configs_recursive(&generation, 0, 0, 0);
}

static void compute_q(mpz_t result,
                      const uint8_t histogram[MAX_COMPONENT_LENGTH],
                      const int path_sizes[MAX_SUPPORTED_D], int d,
                      mpz_t factorial[MAX_SUPPORTED_N + 1],
                      QWorkspace *workspace, FormulaStats *stats)
{
    int length_limit = 0;
    for (int path = 0; path < d; ++path) {
        if (path_sizes[path] - 1 > length_limit) {
            length_limit = path_sizes[path] - 1;
        }
    }
    size_t stride[MAX_COMPONENT_LENGTH];
    size_t state_count = 1U;
    for (int length = 0; length < length_limit; ++length) {
        stride[length] = state_count;
        size_t radix = (size_t)histogram[length] + 1U;
        state_count = checked_product(state_count, radix);
    }
    if (state_count > UINT32_MAX) {
        die("q-polynomial state index exceeds uint32_t");
    }
    q_workspace_ensure(workspace, state_count);
    if (state_count > stats->maximum_q_states) {
        stats->maximum_q_states = state_count;
    }

    for (size_t index = 0; index < state_count; ++index) {
        size_t quotient = index;
        for (int length = length_limit - 1;
             length >= 0; --length) {
            size_t digit = quotient / stride[length];
            quotient %= stride[length];
            workspace->digits[index * MAX_COMPONENT_LENGTH +
                              (size_t)length] = (uint8_t)digit;
        }
    }

    TargetConfigSet *configuration_sets =
        xcalloc_array((size_t)d, sizeof(*configuration_sets));
    for (int path = 0; path < d; ++path) {
        generate_target_configs(histogram, stride, path_sizes[path],
                                length_limit,
                                &configuration_sets[path]);
    }

    mpz_t *left = workspace->left;
    mpz_t *right = workspace->right;
    uint32_t *left_used = workspace->left_used;
    uint32_t *right_used = workspace->right_used;
    size_t left_count = 1U;
    left_used[0] = 0U;
    mpz_set_ui(left[0], 1UL);

    for (int path = 0; path < d; ++path) {
        size_t right_count = 0U;
        const TargetConfigSet *set = &configuration_sets[path];
        for (size_t active = 0; active < left_count; ++active) {
            size_t source = left_used[active];
            const uint8_t *source_digits =
                &workspace->digits[source * MAX_COMPONENT_LENGTH];
            for (size_t configuration_index = 0;
                 configuration_index < set->count;
                 ++configuration_index) {
                const TargetConfig *configuration =
                    &set->item[configuration_index];
                bool fits = true;
                for (int length = 0;
                     length < length_limit; ++length) {
                    if ((unsigned)source_digits[length] +
                        configuration->count[length] >
                        histogram[length]) {
                        fits = false;
                        break;
                    }
                }
                if (!fits) {
                    continue;
                }
                size_t destination = source + configuration->delta;
                if (mpz_sgn(right[destination]) == 0) {
                    right_used[right_count++] = (uint32_t)destination;
                }
                mpz_addmul_ui(right[destination], left[source],
                              configuration->weight);
                if (stats->q_transitions == UINT64_MAX) {
                    die("q-transition counter overflow");
                }
                ++stats->q_transitions;
            }
        }
        for (size_t active = 0; active < left_count; ++active) {
            mpz_set_ui(left[left_used[active]], 0UL);
        }
        mpz_t *number_swap = left;
        left = right;
        right = number_swap;
        uint32_t *index_swap = left_used;
        left_used = right_used;
        right_used = index_swap;
        left_count = right_count;
    }

    mpz_set(result, left[state_count - 1U]);
    for (int length = 0; length < length_limit; ++length) {
        if (histogram[length] > 1U) {
            mpz_mul(result, result, factorial[histogram[length]]);
        }
    }
    for (size_t active = 0; active < left_count; ++active) {
        mpz_set_ui(left[left_used[active]], 0UL);
    }
    for (int path = 0; path < d; ++path) {
        target_config_set_destroy(&configuration_sets[path]);
    }
    free(configuration_sets);
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

static void compute_formula(mpz_t result, int n, int d, QMode q_mode,
                            FormulaStats *stats)
{
    if (d < 2 || d > MAX_SUPPORTED_D ||
        n < 0 || n > MAX_SUPPORTED_N) {
        die("unsupported parameters to Tauraso's formula");
    }
    validate_n_d(n, d);
    memset(stats, 0, sizeof(*stats));
    stats->q_mode = q_mode;
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
        stats->source_histograms = 1U;
        stats->target_histograms = q_mode == Q_GLOBAL ? 1U : 0U;
        stats->maximum_q_states = q_mode == Q_DIRECT ? 1U : 0U;
        stats->seconds = monotonic_seconds() - started;
        for (int i = 0; i <= n; ++i) {
            mpz_clear(factorial[i]);
        }
        return;
    }

    int path_sizes[MAX_SUPPORTED_D] = {0};
    for (int k = 1; k <= d; ++k) {
        path_sizes[k - 1] = (n - k) / d + 1;
        if (path_sizes[k - 1] > MAX_COMPONENT_LENGTH + 1) {
            die("path size exceeds the compiled component limit");
        }
    }

    HistMap source;
    build_histogram_product(&source, path_sizes, d,
                            &stats->histogram_convolution_pairs);
    stats->source_histograms = source.size;
    uint128_t expected_sum = (uint128_t)1U << (unsigned)(n - d);
    if (hist_map_multiplicity_sum(&source) != expected_sum) {
        hist_map_destroy(&source);
        die("source multiplicities do not sum to 2^(n-d)");
    }

    HistMap target = {0};
    if (q_mode == Q_GLOBAL) {
        build_histogram_product(&target, path_sizes, d,
                                &stats->histogram_convolution_pairs);
        stats->target_histograms = target.size;
        if (target.size != source.size ||
            hist_map_multiplicity_sum(&target) != expected_sum) {
            hist_map_destroy(&target);
            hist_map_destroy(&source);
            die("target coefficients failed their global invariants");
        }
    }

    QWorkspace workspace;
    q_workspace_init(&workspace);
    mpz_t q;
    mpz_t term;
    mpz_inits(q, term, NULL);
    mpz_set_ui(result, 0UL);

    for (size_t slot_index = 0;
         slot_index < source.capacity; ++slot_index) {
        const HistSlot *slot = &source.slots[slot_index];
        if (!slot->occupied) {
            continue;
        }
        int r = 0;
        int c = 0;
        for (int length = 0;
             length < source.key_length; ++length) {
            r += (length + 1) * slot->count[length];
            c += slot->count[length];
        }
        if (r + c > n) {
            die("invalid source component histogram");
        }
        if (q_mode == Q_DIRECT) {
            compute_q(q, slot->count, path_sizes, d, factorial,
                      &workspace, stats);
        } else {
            /*
             * q(H) = product_l h_l! times [z^H] product_k T_{n_k}(z).
             * target holds all coefficients of that product, so this is
             * Tauraso's original q, evaluated for every H in one pass.
             */
            const HistSlot *target_slot =
                hist_map_lookup(&target, slot->count, slot->hash);
            if (target_slot == NULL) {
                hist_map_destroy(&target);
                hist_map_destroy(&source);
                die("source histogram is absent from target polynomial");
            }
            mpz_set_u128(q, target_slot->multiplicity);
            for (int length = 0;
                 length < target.key_length; ++length) {
                if (slot->count[length] > 1U) {
                    mpz_mul(q, q, factorial[slot->count[length]]);
                }
            }
        }
        mpz_set_u128(term, slot->multiplicity);
        mpz_mul(term, term, q);
        mpz_mul(term, term, factorial[n - r - c]);
        if ((r & 1) == 0) {
            mpz_add(result, result, term);
        } else {
            mpz_sub(result, result, term);
        }
    }

    if (mpz_sgn(result) < 0 || mpz_cmp(result, factorial[n]) > 0) {
        die("formula result is outside 0..n!");
    }
    mpz_clears(q, term, NULL);
    q_workspace_destroy(&workspace);
    if (q_mode == Q_GLOBAL) {
        hist_map_destroy(&target);
    }
    hist_map_destroy(&source);
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
    size_t index = checked_product((size_t)used,
                                   (size_t)context->n + 1U) +
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
            if (difference == context->d) {
                continue;
            }
        }
        uint64_t addend =
            brute_recursive(context, used | bit, value);
        total = checked_add_u64(total, addend);
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
    size_t memo_count = checked_product(mask_count, (size_t)n + 1U);
    context.memo = xmalloc_array(memo_count, sizeof(*context.memo));
    memset(context.memo, 0xff,
           checked_product(memo_count, sizeof(*context.memo)));
    uint64_t result = brute_recursive(&context, 0U, n);
    free(context.memo);
    return result;
}

static void report_formula(int n, int d, const FormulaStats *stats)
{
    if (stats->q_mode == Q_DIRECT) {
        fprintf(stderr,
                "189285_02: d=%d, n=%d, direct q, "
                "source histograms=%zu, convolution pairs=%" PRIu64
                ", max q states=%zu, q transitions=%" PRIu64
                ", %.3f s\n",
                d, n, stats->source_histograms,
                stats->histogram_convolution_pairs,
                stats->maximum_q_states, stats->q_transitions,
                stats->seconds);
    } else {
        fprintf(stderr,
                "189285_02: d=%d, n=%d, global q, "
                "source histograms=%zu, target histograms=%zu, "
                "convolution pairs=%" PRIu64 ", %.3f s\n",
                d, n, stats->source_histograms,
                stats->target_histograms,
                stats->histogram_convolution_pairs, stats->seconds);
    }
}

static void compute_value(mpz_t result, int n, int d, QMode q_mode,
                          bool verbose)
{
    FormulaStats stats;
    compute_formula(result, n, d, q_mode, &stats);
    if (d == A189285_D && n <= KNOWN_MAX_N) {
        verify_decimal(result, known_terms[n], "A189285", n);
    }
    if (verbose) {
        report_formula(n, d, &stats);
    }
}

static int check_implementation(int maximum_n)
{
    mpz_t value;
    mpz_t direct;
    mpz_inits(value, direct, NULL);
    for (int n = SEQUENCE_OFFSET; n <= maximum_n; ++n) {
        compute_value(value, n, A189285_D, Q_GLOBAL, false);
        if (n <= DIRECT_Q_CHECK_MAX_N) {
            FormulaStats direct_stats;
            compute_formula(direct, n, A189285_D, Q_DIRECT,
                            &direct_stats);
            if (mpz_cmp(value, direct) != 0) {
                gmp_fprintf(stderr,
                            "error: global/direct q mismatch at d=%d, "
                            "n=%d: global=%Zd, direct=%Zd\n",
                            A189285_D, n, value, direct);
                mpz_clears(value, direct, NULL);
                return EXIT_FAILURE;
            }
        }
        if (n <= BRUTE_CHECK_MAX_N) {
            uint64_t independent = brute_count(n, A189285_D);
            if (mpz_cmp_ui(value, (unsigned long)independent) != 0) {
                gmp_fprintf(stderr,
                            "error: independent DP mismatch at n=%d: "
                            "formula=%Zd, DP=%" PRIu64 "\n",
                            n, value, independent);
                mpz_clears(value, direct, NULL);
                return EXIT_FAILURE;
            }
        }
    }
    for (int d = 2; d <= GENERAL_CHECK_MAX_D; ++d) {
        for (int n = 0; n <= GENERAL_CHECK_MAX_N; ++n) {
            FormulaStats stats;
            FormulaStats direct_stats;
            compute_formula(value, n, d, Q_GLOBAL, &stats);
            compute_formula(direct, n, d, Q_DIRECT, &direct_stats);
            if (mpz_cmp(value, direct) != 0) {
                gmp_fprintf(stderr,
                            "error: global/direct q mismatch at d=%d, "
                            "n=%d: global=%Zd, direct=%Zd\n",
                            d, n, value, direct);
                mpz_clears(value, direct, NULL);
                return EXIT_FAILURE;
            }
            uint64_t independent = brute_count(n, d);
            if (mpz_cmp_ui(value, (unsigned long)independent) != 0) {
                gmp_fprintf(stderr,
                            "error: general-d DP mismatch at d=%d, n=%d: "
                            "formula=%Zd, DP=%" PRIu64 "\n",
                            d, n, value, independent);
                mpz_clears(value, direct, NULL);
                return EXIT_FAILURE;
            }
        }
    }
    mpz_clears(value, direct, NULL);
    printf("ok: global and direct-q evaluations agree through n=%d "
           "for d=6; A189285 b-file agrees through n=%d, with the independent "
           "DP through n=%d; directed a_{d,d} for d=2..%d was "
           "independently checked through n=%d\n",
           maximum_n < DIRECT_Q_CHECK_MAX_N
               ? maximum_n : DIRECT_Q_CHECK_MAX_N,
           maximum_n < KNOWN_MAX_N ? maximum_n : KNOWN_MAX_N,
           maximum_n < BRUTE_CHECK_MAX_N
               ? maximum_n : BRUTE_CHECK_MAX_N,
           GENERAL_CHECK_MAX_D, GENERAL_CHECK_MAX_N);
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

static void produce_file(const char *argv0, int maximum_n, int d,
                         QMode q_mode)
{
    char final_name[64];
    char part_name[64];
    int final_length = snprintf(final_name, sizeof(final_name),
                                "b189285_02_d%d.txt", d);
    int part_length = snprintf(part_name, sizeof(part_name),
                               "b189285_02_d%d_part.txt", d);
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
    for (int n = SEQUENCE_OFFSET; n <= maximum_n; ++n) {
        compute_value(value, n, d, q_mode, true);
        if (gmp_fprintf(stream, "%d %Zd\n", n, value) < 0 ||
            fflush(stream) != 0) {
            mpz_clear(value);
            fclose(stream);
            free(final_path);
            free(part_path);
            die("could not write the A189285 output file");
        }
    }
    mpz_clear(value);
    if (fclose(stream) != 0) {
        free(final_path);
        free(part_path);
        die("could not close the A189285 output file");
    }
    if (rename(part_path, final_path) != 0) {
        fprintf(stderr, "error: cannot rename %s to %s: %s\n",
                part_path, final_path, strerror(errno));
        free(final_path);
        free(part_path);
        exit(EXIT_FAILURE);
    }
    printf("wrote %s (d=%d, n=%d..%d)\n",
           final_path, d, SEQUENCE_OFFSET, maximum_n);
    free(final_path);
    free(part_path);
}

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s [MAX_N]\n"
            "       %s --term N\n"
            "       %s --upto MAX_N\n"
            "       %s --d D [MAX_N]\n"
            "       %s --d D --term N\n"
            "       %s --d D --upto MAX_N\n"
            "       %s --direct-q --d D --term N\n"
            "       %s --check [MAX_N]\n"
            "\n"
            "D defaults to %d (A189285) and may be 2..%d.\n"
            "MAX_N defaults to %d and may be at most %d.\n"
            "A range run writes b189285_02_dD.txt.\n"
            "--direct-q uses the slower per-histogram q convolution.\n",
            program, program, program, program, program, program, program,
            program, A189285_D, MAX_SUPPORTED_D, DEFAULT_MAX_N,
            MAX_SUPPORTED_N);
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
        validate_n_d(maximum_n, A189285_D);
        return check_implementation(maximum_n);
    }

    typedef enum {
        RUN_UPTO,
        RUN_TERM
    } RunMode;
    RunMode mode = RUN_UPTO;
    int d = A189285_D;
    int n = -1;
    bool have_d = false;
    bool have_n = false;
    bool have_q_mode = false;
    QMode q_mode = Q_GLOBAL;

    for (int argument = 1; argument < argc; ++argument) {
        const char *text = argv[argument];
        if (strcmp(text, "--direct-q") == 0) {
            if (have_q_mode) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            q_mode = Q_DIRECT;
            have_q_mode = true;
        } else if (strcmp(text, "--d") == 0) {
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
    validate_n_d(n, d);
    if (mode == RUN_TERM) {
        mpz_t value;
        mpz_init(value);
        compute_value(value, n, d, q_mode, true);
        gmp_printf("%d %Zd\n", n, value);
        mpz_clear(value);
        return EXIT_SUCCESS;
    }
    produce_file(argv[0], n, d, q_mode);
    return EXIT_SUCCESS;
}
