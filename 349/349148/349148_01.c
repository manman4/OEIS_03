/*
 * A349148 -- unordered n-tuples with integral reciprocal sum.
 *
 * An unordered n-tuple with entries in [n] is represented uniquely by
 * multiplicities c_1,...,c_n, where c_j is the number of occurrences of j.
 * We count
 *
 *   c_j >= 0,  sum_j c_j = n,  and  sum_j c_j/j is an integer.
 *
 * Let L=lcm(1,...,n).  The last condition is exactly
 *
 *   sum_j c_j*(L/j) == 0 (mod L).                         (1)
 *
 * A sparse dynamic program stores (number of entries used, residue).  The
 * omitted value c_1 is determined uniquely as n minus the number used and does
 * not affect integrality, because 1/1 is integral.
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
 * This is an independent implementation of the definition.  Known OEIS terms
 * are verification data only and are never returned as computed answers.
 * Counts use an internal four-limb unsigned 128-bit type.  For the supported
 * n<=66, every partial multiplicity count is at most
 *
 *   binomial(2*66-1,66)
 *     = 188694833082770476622296176145946360850 < 2^128.
 *
 * The largest initial M*(n+1) in this range occurs at n=66 and is
 * 135450532614196800 < 2^64, so packed state keys also fit.  Runtime checks
 * remain in place for all arithmetic and allocations.
 *
 * All additions, LCM products, packed keys, counters, allocation sizes, and
 * configured memory limits are nevertheless checked at runtime.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic -Werror \
 *       349148_01.c -o 349148_01
 *
 * Usage:
 *   ./349148_01
 *   ./349148_01 --upto 25 --verbose
 *   ./349148_01 --term 25 --memory-mb 1024 --verbose
 *   ./349148_01 --check
 *
 * The default and --upto modes print completed terms and atomically replace
 * b349148_01.txt.  --term and --check do not modify the b-file.
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
#define KNOWN_MAX_N 25
#define DIRECT_CHECK_MAX_N 10
#define DEFAULT_MEMORY_MB 1024
#define MIN_MEMORY_MB 16
#define MAX_MEMORY_MB 65536
#define INITIAL_CAPACITY 1024
#define LOAD_NUMERATOR 7
#define LOAD_DENOMINATOR 10
#define BFILE_NAME "b349148_01.txt"
#define BFILE_TEMP_TEMPLATE BFILE_NAME ".tmp.XXXXXX"
#define BFILE_LOCK_NAME BFILE_NAME ".lock"
#define COUNT_LIMBS 4
#define DECIMAL_BASE UINT32_C(1000000000)
#define MAX_DECIMAL_CHUNKS 5

static const uint64_t known[KNOWN_MAX_N + 1] = {
    UINT64_C(1),         UINT64_C(1),         UINT64_C(2),
    UINT64_C(3),         UINT64_C(6),         UINT64_C(9),
    UINT64_C(25),        UINT64_C(39),        UINT64_C(84),
    UINT64_C(158),       UINT64_C(381),       UINT64_C(610),
    UINT64_C(2175),      UINT64_C(3489),      UINT64_C(7252),
    UINT64_C(24744),     UINT64_C(54658),     UINT64_C(89031),
    UINT64_C(273604),    UINT64_C(443746),    UINT64_C(1690517),
    UINT64_C(5261990),   UINT64_C(9399018),   UINT64_C(15470605),
    UINT64_C(58261863),  UINT64_C(102574465)
};

typedef struct {
    uint32_t limb[COUNT_LIMBS];
} Count;

typedef struct {
    uint64_t key_plus_one;
    Count count;
} StateEntry;

typedef struct {
    StateEntry *entry;
    size_t capacity;
    size_t used;
} StateTable;

typedef struct {
    int n;
    unsigned radix;
    uint64_t modulus;
    size_t memory_limit;
    size_t live_memory;
    size_t peak_memory;
    size_t peak_states;
    uint64_t transitions;
    uint64_t insertions;
    uint64_t merges;
} Search;

typedef struct {
    uint64_t initial_modulus;
    size_t peak_states;
    size_t peak_memory;
    uint64_t transitions;
    uint64_t insertions;
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
        die("count exceeds the internal 128-bit type");
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

static uint64_t add_mod(uint64_t left, uint64_t right, uint64_t modulus)
{
    if (modulus == 0U || left >= modulus || right >= modulus)
        die("modular-addition invariant violated");
    if (right == 0U)
        return left;
    return left >= modulus - right ? left - (modulus - right) : left + right;
}

static uint64_t hash_u64(uint64_t value)
{
    /* Masks make the intended modulo-2^64 left shifts sanitizer-clean. */
    value ^= value >> 31;
    value ^= (value & (UINT64_MAX >> 11)) << 11;
    value ^= value >> 17;
    value ^= (value & (UINT64_MAX >> 23)) << 23;
    return value ^ (value >> 13);
}

static uint64_t pack_key_with_modulus(const Search *search,
                                      unsigned used, uint64_t residue,
                                      uint64_t modulus)
{
    if (modulus == 0U || used > (unsigned)search->n || residue >= modulus)
        die("state key component out of range");
    if (residue > (UINT64_MAX - used) / search->radix)
        die("state key overflow");
    const uint64_t key = residue * search->radix + used;
    if (key == UINT64_MAX)
        die("state key sentinel overflow");
    return key;
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

static StateEntry *allocate_entries(Search *search, size_t capacity)
{
    if (capacity == 0U || capacity > SIZE_MAX / sizeof(StateEntry))
        die("state allocation size overflow");
    const size_t bytes = capacity * sizeof(StateEntry);
    if (search->live_memory > search->memory_limit ||
        bytes > search->memory_limit - search->live_memory)
        memory_error(search, bytes);
    StateEntry *entry = calloc(capacity, sizeof(*entry));
    if (entry == NULL)
        die("cannot allocate state table");
    search->live_memory += bytes;
    if (search->live_memory > search->peak_memory)
        search->peak_memory = search->live_memory;
    return entry;
}

static void insert_without_growth(StateEntry *entry, size_t capacity,
                                  uint64_t key_plus_one, Count count)
{
    size_t slot = (size_t)hash_u64(key_plus_one - 1U) & (capacity - 1U);
    while (entry[slot].key_plus_one != 0U)
        slot = (slot + 1U) & (capacity - 1U);
    entry[slot].key_plus_one = key_plus_one;
    entry[slot].count = count;
}

static void grow_table(Search *search, StateTable *table)
{
    const size_t old_capacity = table->capacity;
    if (old_capacity > SIZE_MAX / 2U)
        die("state capacity overflow");
    const size_t new_capacity = old_capacity == 0U ?
        INITIAL_CAPACITY : old_capacity * 2U;
    if ((new_capacity & (new_capacity - 1U)) != 0U)
        die("state capacity is not a power of two");

    StateEntry *new_entry = allocate_entries(search, new_capacity);
    for (size_t i = 0; i < old_capacity; ++i) {
        const StateEntry old = table->entry[i];
        if (old.key_plus_one != 0U)
            insert_without_growth(new_entry, new_capacity,
                                  old.key_plus_one, old.count);
    }

    if (old_capacity != 0U) {
        const size_t old_bytes = old_capacity * sizeof(StateEntry);
        free(table->entry);
        search->live_memory -= old_bytes;
    }
    table->entry = new_entry;
    table->capacity = new_capacity;
}

static void table_add(Search *search, StateTable *table,
                      uint64_t key, const Count *amount)
{
    if (table->capacity == 0U ||
        table->used + 1U >
            table->capacity * LOAD_NUMERATOR / LOAD_DENOMINATOR)
        grow_table(search, table);

    const uint64_t stored_key = key + 1U;
    size_t slot = (size_t)hash_u64(key) & (table->capacity - 1U);
    for (;;) {
        StateEntry *entry = &table->entry[slot];
        if (entry->key_plus_one == 0U) {
            entry->key_plus_one = stored_key;
            entry->count = *amount;
            ++table->used;
            checked_increment(&search->insertions, "state-insertion");
            if (table->used > search->peak_states)
                search->peak_states = table->used;
            return;
        }
        if (entry->key_plus_one == stored_key) {
            count_add(&entry->count, amount);
            checked_increment(&search->merges, "state-merge");
            return;
        }
        slot = (slot + 1U) & (table->capacity - 1U);
    }
}

static void free_table(Search *search, StateTable *table)
{
    if (table->capacity != 0U) {
        const size_t bytes = table->capacity * sizeof(StateEntry);
        free(table->entry);
        search->live_memory -= bytes;
    }
    memset(table, 0, sizeof(*table));
}

static Count count_unordered_tuples(int n, size_t memory_limit,
                                    Statistics *statistics)
{
    memset(statistics, 0, sizeof(*statistics));
    const double start = monotonic_seconds();

    Search search;
    memset(&search, 0, sizeof(search));
    search.n = n;
    search.radix = (unsigned)n + 1U;
    search.modulus = make_initial_modulus(n);
    search.memory_limit = memory_limit;
    const uint64_t initial_modulus = search.modulus;

    StateTable current;
    memset(&current, 0, sizeof(current));
    const Count one = count_from_u64(UINT64_C(1));
    table_add(&search, &current, pack_key(&search, 0U, 0U), &one);

    /* For p>n/2, the exact choices are c_p=0 or c_p=p.  Two positive
       choices cannot coexist because their sum would exceed n. */
    int singleton_prime_count = 0;
    for (int prime = n; prime > n / 2; --prime) {
        if (!is_prime((unsigned)prime))
            continue;
        table_add(&search, &current,
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

        StateTable next;
        memset(&next, 0, sizeof(next));
        for (size_t slot = 0; slot < current.capacity; ++slot) {
            const StateEntry state = current.entry[slot];
            if (state.key_plus_one == 0U)
                continue;
            unsigned used = 0;
            uint64_t residue = 0;
            unpack_key(&search, state.key_plus_one - 1U,
                       &used, &residue);
            uint64_t next_residue = residue;
            const unsigned maximum = (unsigned)n - used;
            for (unsigned multiplicity = 0;
                 multiplicity <= maximum; ++multiplicity) {
                checked_increment(&search.transitions, "transition");
                if (!closes_prime || next_residue % prime_power == 0U) {
                    const uint64_t reduced = closes_prime ?
                        next_residue / prime_power : next_residue;
                    if (reduced >= new_modulus)
                        die("reduced residue is out of range");
                    const uint64_t key = pack_key_with_modulus(
                        &search, used + multiplicity,
                        reduced, new_modulus);
                    table_add(&search, &next, key, &state.count);
                }
                if (multiplicity != maximum)
                    next_residue = add_mod(
                        next_residue, weight, old_modulus);
            }
        }

        free_table(&search, &current);
        current = next;
        search.modulus = new_modulus;
    }

    if (search.modulus != 1U)
        die("not all prime-power constraints were eliminated");
    Count total = count_from_u64(UINT64_C(0));
    for (size_t slot = 0; slot < current.capacity; ++slot)
        if (current.entry[slot].key_plus_one != 0U)
            count_add(&total, &current.entry[slot].count);

    statistics->initial_modulus = initial_modulus;
    statistics->peak_states = search.peak_states;
    statistics->peak_memory = search.peak_memory;
    statistics->transitions = search.transitions;
    statistics->insertions = search.insertions;
    statistics->merges = search.merges;
    statistics->seconds = monotonic_seconds() - start;
    free_table(&search, &current);
    if (search.live_memory != 0U)
        die("state memory accounting invariant violated");
    return total;
}

typedef struct {
    int n;
    uint64_t lcm;
    Count count;
} BruteSearch;

static void brute_visit(BruteSearch *search, int denominator,
                        unsigned remaining, uint64_t residue)
{
    if (denominator > search->n) {
        if (residue == 0U)
            count_add(&search->count,
                      &(Count){{UINT32_C(1), 0U, 0U, 0U}});
        return;
    }
    const uint64_t weight = search->lcm / (uint64_t)denominator;
    uint64_t next_residue = residue;
    for (unsigned multiplicity = 0;
         multiplicity <= remaining; ++multiplicity) {
        brute_visit(search, denominator + 1,
                    remaining - multiplicity, next_residue);
        if (multiplicity != remaining)
            next_residue = add_mod(next_residue, weight, search->lcm);
    }
}

static Count brute_count(int n)
{
    BruteSearch search;
    memset(&search, 0, sizeof(search));
    search.n = n;
    search.lcm = make_lcm(n);
    brute_visit(&search, 2, (unsigned)n, 0U);
    return search.count;
}

static Count compute_term(int n, size_t memory_limit,
                          Statistics *statistics)
{
    const Count value = count_unordered_tuples(
        n, memory_limit, statistics);
    const Count expected = n <= KNOWN_MAX_N ?
        count_from_u64(known[n]) : count_from_u64(UINT64_C(0));
    if (n <= KNOWN_MAX_N && !count_equal(&value, &expected)) {
        fprintf(stderr,
                "error: verification failed at A349148(%d): computed=", n);
        (void)print_count(stderr, &value);
        fprintf(stderr, ", expected=%" PRIu64 "\n", known[n]);
        exit(EXIT_FAILURE);
    }
    return value;
}

static void report_statistics(int n, const Statistics *statistics)
{
    fprintf(stderr,
            "349148_01: n=%d, initial_modulus=%" PRIu64
            ", peak_states=%zu, "
            "transitions=%" PRIu64 ", insertions=%" PRIu64 ", "
            "merges=%" PRIu64 ", peak=%.1f MiB, elapsed=%.3f s\n",
            n, statistics->initial_modulus, statistics->peak_states,
            statistics->transitions, statistics->insertions,
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
        printf("%d ", requested);
        if (print_count(stdout, &value) < 0 || fputc('\n', stdout) == EOF)
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
        printf("%d ", n);
        if (print_count(stdout, &terms[n]) < 0 || fputc('\n', stdout) == EOF)
            die("cannot write standard output");
        fflush(stdout);
        if (verbose)
            report_statistics(n, &statistics);
    }
    write_bfile(terms, requested);
    return EXIT_SUCCESS;
}
