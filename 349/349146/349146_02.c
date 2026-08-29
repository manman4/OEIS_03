/*
 * A349146 -- ordered n-tuples with integral reciprocal sum (algorithm 02).
 *
 * An ordered n-tuple with entries in [n] has multiplicities
 * c_1,...,c_n.  We count
 *
 *   c_j >= 0,  sum_j c_j = n,  and  sum_j c_j/j is an integer.
 *
 * Let L=lcm(1,...,n).  The last condition is exactly
 *
 *   sum_j c_j*(L/j) == 0 (mod L).                         (1)
 *
 * A dynamic program stores (number of entries used, residue).  Unlike
 * 349146_01, this implementation has no hash table and never inserts generated
 * states one by one.  Its state vector is kept in increasing packed-key order.
 * For a fixed multiplicity c, adding c*(M/j) rotates the residue-sorted vector
 * cyclically, so it is still a sorted stream after at most one split.  The
 * streams for c=0,...,n are merged by a min-heap; equal keys are summed while
 * they leave the heap.  This gives an independent sort/merge implementation
 * of the transition and deterministic O(number of states) storage.
 *
 * A state count is the number u!/Product c_j! of ordered words on the u entries
 * whose denominators have been processed.  A stream for multiplicity c
 * multiplies it by binomial(u+c,c).  The omitted value c_1=n-u is inserted at
 * the end with factor binomial(n,u), giving exactly n!/Product c_j!.
 *
 * To avoid ever storing the possibly enormous L, first consider a prime
 * p>n/2.  The only denominator divisible by p is p itself.  Its p-primary
 * condition is c_p == 0 (mod p), and 0<=c_p<=n<2p, so exactly c_p=0 or p is
 * possible.  The latter contributes p/p=1 and hence changes only the number
 * of entries used.  These choices are seeded directly.
 *
 * The remaining modulus M is the product, over primes p<=n/2, of the largest
 * power P=p^e<=n.  It fits in uint64_t for every supported n<=66.  Remaining
 * denominators are grouped by decreasing largest prime factor.  While p is
 * current, every such denominator j divides M and has residue weight M/j.
 * The denominator p is processed last in its group.  At that point no future
 * denominator is divisible by p, so a state survives exactly when its residue
 * is 0 modulo P.  Write M=P*M'.  A surviving representative r is divisible
 * by P; replacing (M,r) by (M',r/P) is exact because
 *
 *   Sum c_j/j == r/M (mod 1) == (r/P)/M' (mod 1).
 *
 * Thus each completed prime-power constraint is removed without losing or
 * merging solutions.  After all groups, M=1 and every remaining state is an
 * integral tuple.
 *
 * This is an independent state-processing algorithm for the definition.
 * Known OEIS terms
 * are verification data only and are never returned as computed answers.
 * Counts use an internal sixteen-limb unsigned 512-bit type.  For supported
 * n<=66, every partial or final count is at most n^n, and
 *
 *   66^66
 *     = 1229984803535237425357460579824952453848609953896821302286319065669207712270213276022808840210306942692366529569453244416
 *     < 2^512.
 *
 * The largest initial M*(n+1) in this range occurs at n=66 and is
 * 135450532614196800 < 2^64, so packed state keys also fit.  Runtime checks
 * remain in place for all arithmetic and allocations.
 *
 * Count additions and multiplications, binomial additions, LCM products,
 * packed keys, counters, allocation sizes, and configured memory limits are
 * checked at runtime.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic -Werror \
 *       349146_02.c -o 349146_02
 *
 * Usage:
 *   ./349146_02
 *   ./349146_02 --upto 25 --verbose
 *   ./349146_02 --term 25 --memory-mb 1024 --verbose
 *   ./349146_02 --check
 *
 * The default and --upto modes print completed terms and atomically replace
 * b349146_02.txt.  --term and --check do not modify the b-file.
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
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define MAX_N 66
#define DEFAULT_UPTO 66
#define KNOWN_MAX_N 23
#define DIRECT_CHECK_MAX_N 12
#define DEFAULT_MEMORY_MB 1024
#define MIN_MEMORY_MB 16
#define MAX_MEMORY_MB 65536
#define INITIAL_CAPACITY 1024
#define BFILE_NAME "b349146_02.txt"
#define BFILE_TEMP_TEMPLATE BFILE_NAME ".tmp.XXXXXX"
#define BFILE_LOCK_NAME BFILE_NAME ".lock"
#define COUNT_LIMBS 16
#define DECIMAL_BASE UINT32_C(1000000000)
#define MAX_DECIMAL_CHUNKS 18

static const char *const known[KNOWN_MAX_N + 1] = {
    "1", "1", "2", "5", "25", "82", "1310", "6757",
    "73204", "612534", "12021898", "100648935", "3293923530",
    "30781757528", "543076024093", "22444907405573",
    "490532466616585", "6321096033756031", "293288707966712654",
    "4209069624596495601", "231798923882314673793",
    "15160706809349856453181", "265850457583646602080422",
    "4542630089978045405518910"
};

typedef struct {
    uint32_t limb[COUNT_LIMBS];
} Count;

typedef struct {
    uint64_t key;
    Count count;
} VectorEntry;

typedef struct {
    VectorEntry *entry;
    size_t size;
    size_t capacity;
} StateVector;

typedef struct {
    unsigned multiplicity;
    size_t split;
    size_t scanned;
    uint64_t delta;
    uint64_t key;
    Count count;
} MergeStream;

typedef struct {
    int n;
    unsigned radix;
    uint64_t modulus;
    const uint64_t (*binomial)[MAX_N + 1];
    size_t memory_limit;
    size_t live_memory;
    size_t peak_memory;
    size_t peak_states;
    uint64_t transitions;
    uint64_t outputs;
    uint64_t merges;
} Search;

typedef struct {
    uint64_t initial_modulus;
    size_t peak_states;
    size_t peak_memory;
    uint64_t transitions;
    uint64_t outputs;
    uint64_t merges;
    double seconds;
} Statistics;

typedef enum {
    MODE_UPTO,
    MODE_TERM,
    MODE_CHECK
} OutputMode;

static _Noreturn void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static double monotonic_seconds(void)
{
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) != 0)
        die("clock_gettime failed");
    return (double)now.tv_sec + (double)now.tv_nsec / 1000000000.0;
}

static int parse_integer(const char *text, const char *label,
                         int minimum, int maximum)
{
    errno = 0;
    char *end = NULL;
    intmax_t value = strtoimax(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' ||
        value < minimum || value > maximum) {
        fprintf(stderr, "error: %s must be in %d..%d: %s\n",
                label, minimum, maximum, text);
        exit(EXIT_FAILURE);
    }
    return (int)value;
}

static Count count_from_u64(uint64_t value)
{
    Count result;
    memset(&result, 0, sizeof(result));
    result.limb[0] = (uint32_t)value;
    result.limb[1] = (uint32_t)(value >> 32);
    return result;
}

static bool count_is_zero(const Count *value)
{
    for (unsigned i = 0; i < COUNT_LIMBS; ++i)
        if (value->limb[i] != 0U)
            return false;
    return true;
}

static bool count_equal(const Count *left, const Count *right)
{
    return memcmp(left, right, sizeof(*left)) == 0;
}

static void count_add(Count *left, const Count *right)
{
    uint64_t carry = 0;
    for (unsigned i = 0; i < COUNT_LIMBS; ++i) {
        const uint64_t sum = (uint64_t)left->limb[i] +
            right->limb[i] + carry;
        left->limb[i] = (uint32_t)sum;
        carry = sum >> 32;
    }
    if (carry != 0U)
        die("count exceeds the internal 512-bit type");
}

static Count count_multiply_u32(const Count *value, uint32_t factor)
{
    Count result;
    memset(&result, 0, sizeof(result));
    uint64_t carry = 0U;
    for (unsigned i = 0U; i < COUNT_LIMBS; ++i) {
        const uint64_t product =
            (uint64_t)value->limb[i] * factor + carry;
        result.limb[i] = (uint32_t)product;
        carry = product >> 32;
    }
    if (carry != 0U)
        die("count multiplication exceeds the internal 512-bit type");
    return result;
}

static Count count_multiply_u64(const Count *value, uint64_t factor)
{
    const uint32_t low_factor = (uint32_t)factor;
    const uint32_t high_factor = (uint32_t)(factor >> 32);
    Count result = count_multiply_u32(value, low_factor);
    if (high_factor == 0U)
        return result;

    Count high = count_multiply_u32(value, high_factor);
    if (high.limb[COUNT_LIMBS - 1U] != 0U)
        die("count multiplication exceeds the internal 512-bit type");
    for (unsigned i = COUNT_LIMBS - 1U; i != 0U; --i)
        high.limb[i] = high.limb[i - 1U];
    high.limb[0] = 0U;
    count_add(&result, &high);
    return result;
}

static void count_add_u32(Count *value, uint32_t amount)
{
    uint64_t carry = amount;
    for (unsigned i = 0U; i < COUNT_LIMBS && carry != 0U; ++i) {
        const uint64_t sum = (uint64_t)value->limb[i] + carry;
        value->limb[i] = (uint32_t)sum;
        carry = sum >> 32;
    }
    if (carry != 0U)
        die("decimal count exceeds the internal 512-bit type");
}

static Count count_from_decimal(const char *text)
{
    Count result = count_from_u64(UINT64_C(0));
    if (*text == '\0')
        die("empty decimal verification value");
    for (const unsigned char *cursor = (const unsigned char *)text;
         *cursor != '\0'; ++cursor) {
        if (*cursor < (unsigned char)'0' || *cursor > (unsigned char)'9')
            die("invalid decimal verification value");
        result = count_multiply_u32(&result, UINT32_C(10));
        count_add_u32(&result, (uint32_t)(*cursor - (unsigned char)'0'));
    }
    return result;
}

static uint32_t count_divide_decimal_base(Count *value)
{
    uint64_t remainder = 0;
    for (int i = COUNT_LIMBS - 1; i >= 0; --i) {
        const uint64_t current = remainder * UINT64_C(4294967296) +
            value->limb[i];
        value->limb[i] = (uint32_t)(current / DECIMAL_BASE);
        remainder = current % DECIMAL_BASE;
    }
    return (uint32_t)remainder;
}

static int print_count(FILE *stream, const Count *value)
{
    if (count_is_zero(value))
        return fputc('0', stream) == EOF ? -1 : 0;

    Count work = *value;
    uint32_t chunk[MAX_DECIMAL_CHUNKS];
    memset(chunk, 0, sizeof(chunk));
    unsigned length = 0;
    while (!count_is_zero(&work)) {
        if (length >= MAX_DECIMAL_CHUNKS)
            die("decimal count buffer is too small");
        chunk[length++] = count_divide_decimal_base(&work);
    }
    if (fprintf(stream, "%" PRIu32, chunk[length - 1U]) < 0)
        return -1;
    while (--length != 0U)
        if (fprintf(stream, "%09" PRIu32, chunk[length - 1U]) < 0)
            return -1;
    return 0;
}

static void checked_increment(uint64_t *value, const char *label)
{
    if (*value == UINT64_MAX) {
        fprintf(stderr, "error: %s counter overflow\n", label);
        exit(EXIT_FAILURE);
    }
    ++*value;
}

static uint64_t gcd_u64(uint64_t left, uint64_t right)
{
    while (right != 0U) {
        const uint64_t remainder = left % right;
        left = right;
        right = remainder;
    }
    return left;
}

static uint64_t make_lcm(int n)
{
    uint64_t result = 1;
    for (uint64_t value = 2; value <= (uint64_t)n; ++value) {
        const uint64_t factor = value / gcd_u64(result, value);
        if (factor != 0U && result > UINT64_MAX / factor)
            die("lcm(1..n) exceeds uint64_t");
        result *= factor;
    }
    return result;
}

static bool is_prime(unsigned value)
{
    if (value < 2U)
        return false;
    if ((value & 1U) == 0U)
        return value == 2U;
    for (unsigned divisor = 3U;
         divisor <= value / divisor; divisor += 2U)
        if (value % divisor == 0U)
            return false;
    return true;
}

static uint64_t largest_prime_power(unsigned prime, unsigned n)
{
    uint64_t power = prime;
    while (power <= (uint64_t)n / prime)
        power *= prime;
    return power;
}

static uint64_t make_initial_modulus(int n)
{
    uint64_t result = 1;
    for (unsigned prime = 2U; prime <= (unsigned)n / 2U; ++prime) {
        if (!is_prime(prime))
            continue;
        const uint64_t power = largest_prime_power(prime, (unsigned)n);
        if (result > UINT64_MAX / power)
            die("reduced initial modulus exceeds uint64_t");
        result *= power;
    }
    return result;
}

static void make_binomial_table(uint64_t table[MAX_N + 1][MAX_N + 1],
                                int n)
{
    memset(table, 0,
           (size_t)(MAX_N + 1) * (size_t)(MAX_N + 1) * sizeof(uint64_t));
    table[0][0] = UINT64_C(1);
    for (int row = 1; row <= n; ++row) {
        table[row][0] = UINT64_C(1);
        table[row][row] = UINT64_C(1);
        for (int column = 1; column < row; ++column) {
            const uint64_t left = table[row - 1][column - 1];
            const uint64_t right = table[row - 1][column];
            if (left > UINT64_MAX - right)
                die("binomial coefficient exceeds uint64_t");
            table[row][column] = left + right;
        }
    }
}

static uint64_t add_mod(uint64_t left, uint64_t right, uint64_t modulus)
{
    if (modulus == 0U || left >= modulus || right >= modulus)
        die("modular-addition invariant violated");
    if (right == 0U)
        return left;
    return left >= modulus - right ? left - (modulus - right) : left + right;
}

static uint64_t pack_key_with_modulus(const Search *search,
                                      unsigned used, uint64_t residue,
                                      uint64_t modulus)
{
    if (modulus == 0U || used > (unsigned)search->n || residue >= modulus)
        die("state key component out of range");
    if (residue > (UINT64_MAX - used) / search->radix)
        die("state key overflow");
    return residue * search->radix + used;
}

static uint64_t pack_key(const Search *search,
                         unsigned used, uint64_t residue)
{
    return pack_key_with_modulus(
        search, used, residue, search->modulus);
}

static void unpack_key(const Search *search, uint64_t key,
                       unsigned *used, uint64_t *residue)
{
    *used = (unsigned)(key % search->radix);
    *residue = key / search->radix;
    if (*used > (unsigned)search->n || *residue >= search->modulus)
        die("stored state key is invalid");
}

static void memory_error(const Search *search, size_t requested)
{
    fprintf(stderr,
            "error: state memory limit exceeded at n=%d "
            "(live %.1f MiB + request %.1f MiB > limit %.1f MiB); "
            "transitions=%" PRIu64 "; increase --memory-mb\n",
            search->n, (double)search->live_memory / 1048576.0,
            (double)requested / 1048576.0,
            (double)search->memory_limit / 1048576.0,
            search->transitions);
    exit(EXIT_FAILURE);
}

static VectorEntry *allocate_entries(Search *search, size_t capacity)
{
    if (capacity == 0U || capacity > SIZE_MAX / sizeof(VectorEntry))
        die("state allocation size overflow");
    const size_t bytes = capacity * sizeof(VectorEntry);
    if (search->live_memory > search->memory_limit ||
        bytes > search->memory_limit - search->live_memory)
        memory_error(search, bytes);
    VectorEntry *entry = malloc(bytes);
    if (entry == NULL)
        die("cannot allocate state vector");
    search->live_memory += bytes;
    if (search->live_memory > search->peak_memory)
        search->peak_memory = search->live_memory;
    return entry;
}

static void grow_vector(Search *search, StateVector *vector)
{
    const size_t old_capacity = vector->capacity;
    if (old_capacity > SIZE_MAX / 2U)
        die("state capacity overflow");
    const size_t new_capacity = old_capacity == 0U ?
        INITIAL_CAPACITY : old_capacity * 2U;
    VectorEntry *new_entry = allocate_entries(search, new_capacity);
    if (vector->size != 0U)
        memcpy(new_entry, vector->entry,
               vector->size * sizeof(*new_entry));

    if (old_capacity != 0U) {
        const size_t old_bytes = old_capacity * sizeof(VectorEntry);
        free(vector->entry);
        search->live_memory -= old_bytes;
    }
    vector->entry = new_entry;
    vector->capacity = new_capacity;
}

static void append_sorted(Search *search, StateVector *vector,
                          uint64_t key, const Count *amount)
{
    if (vector->size != 0U) {
        VectorEntry *last = &vector->entry[vector->size - 1U];
        if (key < last->key)
            die("merge produced unsorted state keys");
        if (key == last->key) {
            count_add(&last->count, amount);
            checked_increment(&search->merges, "state-merge");
            return;
        }
    }
    if (vector->size == vector->capacity)
        grow_vector(search, vector);
    vector->entry[vector->size].key = key;
    vector->entry[vector->size].count = *amount;
    ++vector->size;
    checked_increment(&search->outputs, "state-output");
    if (vector->size > search->peak_states)
        search->peak_states = vector->size;
}

static void free_vector(Search *search, StateVector *vector)
{
    if (vector->capacity != 0U) {
        const size_t bytes = vector->capacity * sizeof(VectorEntry);
        free(vector->entry);
        search->live_memory -= bytes;
    }
    memset(vector, 0, sizeof(*vector));
}

static size_t lower_bound_residue(const StateVector *vector,
                                  unsigned radix, uint64_t residue)
{
    size_t low = 0U;
    size_t high = vector->size;
    while (low < high) {
        const size_t middle = low + (high - low) / 2U;
        if (vector->entry[middle].key / radix < residue)
            low = middle + 1U;
        else
            high = middle;
    }
    return low;
}

static bool advance_stream(Search *search, const StateVector *current,
                           MergeStream *stream, bool closes_prime,
                           uint64_t prime_power, uint64_t new_modulus)
{
    while (stream->scanned < current->size) {
        size_t index = stream->split + stream->scanned;
        if (index >= current->size)
            index -= current->size;
        ++stream->scanned;

        unsigned used = 0U;
        uint64_t residue = 0U;
        unpack_key(search, current->entry[index].key, &used, &residue);
        if (stream->multiplicity > (unsigned)search->n - used)
            continue;
        checked_increment(&search->transitions, "transition");

        const uint64_t shifted = add_mod(
            residue, stream->delta, search->modulus);
        if (closes_prime && shifted % prime_power != 0U)
            continue;
        const uint64_t reduced = closes_prime ?
            shifted / prime_power : shifted;
        if (reduced >= new_modulus)
            die("reduced residue is out of range");
        stream->key = pack_key_with_modulus(
            search, used + stream->multiplicity, reduced, new_modulus);
        stream->count = count_multiply_u64(
            &current->entry[index].count,
            search->binomial[used + stream->multiplicity]
                            [stream->multiplicity]);
        return true;
    }
    return false;
}

static void heap_push(MergeStream *heap, size_t *size,
                      const MergeStream *stream)
{
    if (*size >= (size_t)MAX_N + 1U)
        die("merge heap overflow");
    size_t child = (*size)++;
    while (child != 0U) {
        const size_t parent = (child - 1U) / 2U;
        if (heap[parent].key <= stream->key)
            break;
        heap[child] = heap[parent];
        child = parent;
    }
    heap[child] = *stream;
}

static void heap_sift_down(MergeStream *heap, size_t size)
{
    if (size == 0U)
        return;
    const MergeStream root = heap[0];
    size_t parent = 0U;
    for (;;) {
        const size_t left = parent * 2U + 1U;
        if (left >= size)
            break;
        const size_t right = left + 1U;
        size_t child = left;
        if (right < size && heap[right].key < heap[left].key)
            child = right;
        if (root.key <= heap[child].key)
            break;
        heap[parent] = heap[child];
        parent = child;
    }
    heap[parent] = root;
}

static StateVector merge_transition(Search *search,
                                    const StateVector *current,
                                    uint64_t weight, bool closes_prime,
                                    uint64_t prime_power,
                                    uint64_t new_modulus)
{
    StateVector next;
    memset(&next, 0, sizeof(next));
    MergeStream heap[MAX_N + 1];
    size_t heap_size = 0U;
    uint64_t delta = 0U;

    for (unsigned multiplicity = 0U;
         multiplicity <= (unsigned)search->n; ++multiplicity) {
        MergeStream stream;
        memset(&stream, 0, sizeof(stream));
        stream.multiplicity = multiplicity;
        stream.delta = delta;
        /* Residues at least M-delta wrap to the beginning after rotation;
           starting there makes this multiplicity's stream sorted. */
        const uint64_t threshold = delta == 0U ?
            0U : search->modulus - delta;
        stream.split = lower_bound_residue(
            current, search->radix, threshold);
        if (advance_stream(search, current, &stream, closes_prime,
                           prime_power, new_modulus))
            heap_push(heap, &heap_size, &stream);
        if (multiplicity != (unsigned)search->n)
            delta = add_mod(delta, weight, search->modulus);
    }

    while (heap_size != 0U) {
        MergeStream root = heap[0];
        append_sorted(search, &next, root.key, &root.count);
        if (advance_stream(search, current, &root, closes_prime,
                           prime_power, new_modulus)) {
            heap[0] = root;
        } else {
            --heap_size;
            if (heap_size != 0U)
                heap[0] = heap[heap_size];
        }
        heap_sift_down(heap, heap_size);
    }
    return next;
}

static Count count_ordered_tuples(int n, size_t memory_limit,
                                  Statistics *statistics)
{
    memset(statistics, 0, sizeof(*statistics));
    const double start = monotonic_seconds();

    uint64_t binomial[MAX_N + 1][MAX_N + 1];
    make_binomial_table(binomial, n);

    Search search;
    memset(&search, 0, sizeof(search));
    search.n = n;
    search.radix = (unsigned)n + 1U;
    search.modulus = make_initial_modulus(n);
    search.binomial = binomial;
    search.memory_limit = memory_limit;
    const uint64_t initial_modulus = search.modulus;

    StateVector current;
    memset(&current, 0, sizeof(current));
    const Count one = count_from_u64(UINT64_C(1));
    append_sorted(&search, &current, pack_key(&search, 0U, 0U), &one);

    /* For p>n/2, the exact choices are c_p=0 or c_p=p.  Two positive
       choices cannot coexist because their sum would exceed n. */
    int singleton_prime_count = 0;
    for (int prime = n / 2 + 1; prime <= n; ++prime) {
        if (!is_prime((unsigned)prime))
            continue;
        append_sorted(&search, &current,
                      pack_key(&search, (unsigned)prime, 0U), &one);
        ++singleton_prime_count;
    }

    int order[MAX_N - 1];
    int order_count = 0;
    bool scheduled[MAX_N + 1];
    memset(scheduled, 0, sizeof(scheduled));
    for (int prime = n / 2; prime >= 2; --prime) {
        if (!is_prime((unsigned)prime))
            continue;
        for (int denominator = n; denominator >= 2; --denominator) {
            if (!scheduled[denominator] && denominator % prime == 0) {
                if (order_count >= MAX_N - 1)
                    die("denominator order overflow");
                order[order_count++] = denominator;
                scheduled[denominator] = true;
            }
        }
    }
    const int expected_order_count =
        (n == 0 ? 0 : n - 1) - singleton_prime_count;
    if (order_count != expected_order_count)
        die("denominator order is incomplete");

    for (int order_index = 0; order_index < order_count; ++order_index) {
        const int denominator = order[order_index];
        const uint64_t old_modulus = search.modulus;
        if (old_modulus % (uint64_t)denominator != 0U)
            die("denominator does not divide the current modulus");
        const uint64_t weight = old_modulus / (uint64_t)denominator;
        const bool closes_prime = is_prime((unsigned)denominator);
        uint64_t prime_power = 1;
        uint64_t new_modulus = old_modulus;
        if (closes_prime) {
            prime_power = largest_prime_power(
                (unsigned)denominator, (unsigned)n);
            if (old_modulus % prime_power != 0U)
                die("prime-power modulus invariant violated");
            new_modulus = old_modulus / prime_power;
        }

        StateVector next = merge_transition(
            &search, &current, weight, closes_prime,
            prime_power, new_modulus);
        free_vector(&search, &current);
        current = next;
        search.modulus = new_modulus;
    }

    if (search.modulus != 1U)
        die("not all prime-power constraints were eliminated");
    Count total = count_from_u64(UINT64_C(0));
    for (size_t index = 0; index < current.size; ++index) {
        unsigned used = 0U;
        uint64_t residue = 0U;
        unpack_key(&search, current.entry[index].key, &used, &residue);
        if (residue != 0U)
            die("nonzero residue remains at final modulus");
        const Count completed = count_multiply_u64(
            &current.entry[index].count, search.binomial[n][used]);
        count_add(&total, &completed);
    }

    statistics->initial_modulus = initial_modulus;
    statistics->peak_states = search.peak_states;
    statistics->peak_memory = search.peak_memory;
    statistics->transitions = search.transitions;
    statistics->outputs = search.outputs;
    statistics->merges = search.merges;
    statistics->seconds = monotonic_seconds() - start;
    free_vector(&search, &current);
    if (search.live_memory != 0U)
        die("state memory accounting invariant violated");
    return total;
}

typedef struct {
    int n;
    uint64_t lcm;
    const uint64_t (*binomial)[MAX_N + 1];
    Count count;
} BruteSearch;

static void brute_visit(BruteSearch *search, int denominator,
                        unsigned remaining, unsigned used,
                        uint64_t residue, const Count *ways)
{
    if (denominator > search->n) {
        if (residue == 0U) {
            const Count completed = count_multiply_u64(
                ways, search->binomial[search->n][used]);
            count_add(&search->count, &completed);
        }
        return;
    }
    const uint64_t weight = search->lcm / (uint64_t)denominator;
    uint64_t next_residue = residue;
    for (unsigned multiplicity = 0;
         multiplicity <= remaining; ++multiplicity) {
        const Count weighted = count_multiply_u64(
            ways, search->binomial[used + multiplicity][multiplicity]);
        brute_visit(search, denominator + 1,
                    remaining - multiplicity, used + multiplicity,
                    next_residue, &weighted);
        if (multiplicity != remaining)
            next_residue = add_mod(next_residue, weight, search->lcm);
    }
}

static Count brute_count(int n)
{
    uint64_t binomial[MAX_N + 1][MAX_N + 1];
    make_binomial_table(binomial, n);
    BruteSearch search;
    memset(&search, 0, sizeof(search));
    search.n = n;
    search.lcm = make_lcm(n);
    search.binomial = binomial;
    const Count one = count_from_u64(UINT64_C(1));
    brute_visit(&search, 2, (unsigned)n, 0U, 0U, &one);
    return search.count;
}

static Count compute_term(int n, size_t memory_limit,
                          Statistics *statistics)
{
    const Count value = count_ordered_tuples(
        n, memory_limit, statistics);
    const Count expected = n <= KNOWN_MAX_N ?
        count_from_decimal(known[n]) : count_from_u64(UINT64_C(0));
    if (n <= KNOWN_MAX_N && !count_equal(&value, &expected)) {
        fprintf(stderr,
                "error: verification failed at A349146(%d): computed=", n);
        (void)print_count(stderr, &value);
        fprintf(stderr, ", expected=%s\n", known[n]);
        exit(EXIT_FAILURE);
    }
    return value;
}

static void report_statistics(int n, const Statistics *statistics)
{
    fprintf(stderr,
            "349146_02: n=%d, initial_modulus=%" PRIu64
            ", peak_states=%zu, "
            "transitions=%" PRIu64 ", outputs=%" PRIu64 ", "
            "merges=%" PRIu64 ", peak=%.1f MiB, elapsed=%.3f s\n",
            n, statistics->initial_modulus, statistics->peak_states,
            statistics->transitions, statistics->outputs,
            statistics->merges,
            (double)statistics->peak_memory / 1048576.0,
            statistics->seconds);
}

static int acquire_bfile_lock(void)
{
    const int descriptor = open(BFILE_LOCK_NAME, O_CREAT | O_RDWR, 0666);
    if (descriptor < 0)
        die("cannot open b-file lock");
    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    if (fcntl(descriptor, F_SETLK, &lock) != 0) {
        close(descriptor);
        die("another writer holds the b-file lock");
    }
    return descriptor;
}

static void write_bfile(const Count *terms, int upto)
{
    const int lock_descriptor = acquire_bfile_lock();
    char temporary[] = BFILE_TEMP_TEMPLATE;
    const int descriptor = mkstemp(temporary);
    if (descriptor < 0) {
        close(lock_descriptor);
        die("cannot create temporary b-file");
    }
    const mode_t old_mask = umask(0);
    (void)umask(old_mask);
    if (fchmod(descriptor, (mode_t)(0666 & ~old_mask)) != 0) {
        close(descriptor);
        unlink(temporary);
        close(lock_descriptor);
        die("cannot set temporary b-file permissions");
    }
    FILE *file = fdopen(descriptor, "w");
    if (file == NULL) {
        close(descriptor);
        unlink(temporary);
        close(lock_descriptor);
        die("cannot open temporary b-file stream");
    }

    bool failed = false;
    for (int n = 0; n <= upto; ++n) {
        if (fprintf(file, "%d ", n) < 0 ||
            print_count(file, &terms[n]) < 0 ||
            fputc('\n', file) == EOF)
            failed = true;
    }
    if (fflush(file) != 0 || fsync(descriptor) != 0)
        failed = true;
    if (fclose(file) != 0)
        failed = true;
    if (failed) {
        unlink(temporary);
        close(lock_descriptor);
        die("cannot write temporary b-file");
    }
    if (rename(temporary, BFILE_NAME) != 0) {
        unlink(temporary);
        close(lock_descriptor);
        die("cannot replace b-file");
    }
    close(lock_descriptor);
}

static void run_check(size_t memory_limit, bool verbose)
{
    for (int n = 0; n <= DIRECT_CHECK_MAX_N; ++n) {
        Statistics statistics;
        const Count dynamic = compute_term(n, memory_limit, &statistics);
        const Count brute = brute_count(n);
        if (!count_equal(&dynamic, &brute)) {
            fprintf(stderr,
                    "error: direct check failed at n=%d: dynamic=", n);
            (void)print_count(stderr, &dynamic);
            fprintf(stderr, ", brute=");
            (void)print_count(stderr, &brute);
            fputc('\n', stderr);
            exit(EXIT_FAILURE);
        }
        if (verbose)
            report_statistics(n, &statistics);
    }
    printf("check passed for n=0..%d\n", DIRECT_CHECK_MAX_N);
}

static void usage(const char *program)
{
    printf("Usage:\n"
           "  %s [--upto N] [--memory-mb M] [--verbose]\n"
           "  %s --term N [--memory-mb M] [--verbose]\n"
           "  %s --check [--memory-mb M] [--verbose]\n"
           "\nValid N range: 0..%d. Default --upto: %d.\n",
           program, program, program, MAX_N, DEFAULT_UPTO);
}

int main(int argc, char **argv)
{
    OutputMode mode = MODE_UPTO;
    int requested = DEFAULT_UPTO;
    int memory_mb = DEFAULT_MEMORY_MB;
    bool mode_was_set = false;
    bool verbose = false;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--upto") == 0 ||
            strcmp(argv[i], "--term") == 0) {
            if (mode_was_set)
                die("specify only one of --upto, --term, and --check");
            if (++i >= argc)
                die("missing value after mode option");
            mode = strcmp(argv[i - 1], "--upto") == 0 ?
                MODE_UPTO : MODE_TERM;
            requested = parse_integer(argv[i], "N", 0, MAX_N);
            mode_was_set = true;
        } else if (strcmp(argv[i], "--check") == 0) {
            if (mode_was_set)
                die("specify only one of --upto, --term, and --check");
            mode = MODE_CHECK;
            mode_was_set = true;
        } else if (strcmp(argv[i], "--memory-mb") == 0) {
            if (++i >= argc)
                die("missing value after --memory-mb");
            memory_mb = parse_integer(argv[i], "memory-mb",
                                      MIN_MEMORY_MB, MAX_MEMORY_MB);
        } else if (strcmp(argv[i], "--verbose") == 0) {
            verbose = true;
        } else if (strcmp(argv[i], "--help") == 0 ||
                   strcmp(argv[i], "-h") == 0) {
            usage(argv[0]);
            return EXIT_SUCCESS;
        } else {
            fprintf(stderr, "error: unknown option: %s\n", argv[i]);
            usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    if ((size_t)memory_mb > SIZE_MAX / (size_t)1048576)
        die("memory limit size overflow");
    const size_t memory_limit = (size_t)memory_mb * (size_t)1048576;

    if (mode == MODE_CHECK) {
        run_check(memory_limit, verbose);
        return EXIT_SUCCESS;
    }

    if (mode == MODE_TERM) {
        Statistics statistics;
        const Count value = compute_term(
            requested, memory_limit, &statistics);
        if (fprintf(stdout, "%d ", requested) < 0 ||
            print_count(stdout, &value) < 0 ||
            fputc('\n', stdout) == EOF || fflush(stdout) != 0)
            die("cannot write standard output");
        if (verbose)
            report_statistics(requested, &statistics);
        return EXIT_SUCCESS;
    }

    Count terms[MAX_N + 1];
    memset(terms, 0, sizeof(terms));
    for (int n = 0; n <= requested; ++n) {
        Statistics statistics;
        terms[n] = compute_term(n, memory_limit, &statistics);
        if (fprintf(stdout, "%d ", n) < 0 ||
            print_count(stdout, &terms[n]) < 0 ||
            fputc('\n', stdout) == EOF || fflush(stdout) != 0)
            die("cannot write standard output");
        if (verbose)
            report_statistics(n, &statistics);
    }
    write_bfile(terms, requested);
    return EXIT_SUCCESS;
}
