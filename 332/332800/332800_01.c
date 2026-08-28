/*
 * A332800 -- permutations sigma of [n] whose adjacent remainders
 *
 *       sigma(1) mod sigma(2), ..., sigma(n-1) mod sigma(n)
 *
 * are weakly increasing.
 *
 * For a nonempty set S, x in S, and t>=0, let F(S,x,t) count valid
 * orderings of S that end in x and whose final remainder is at most t.
 * Removing x exposes a unique penultimate value y and proves the exact
 * recurrence
 *
 *   F({x},x,t) = 1,
 *   F(S,x,t) = sum F(S\{x}, y, y mod x),
 *
 * where the sum is over y in S\{x} with y mod x <= t.  Therefore
 *
 *   a(0)=1,  a(n)=sum_{x=1..n} F([n],x,n-1).
 *
 * This program evaluates the recurrence top-down.  S is a bit mask and only
 * reached (S,x,t) states are stored in a sparse hash table.  Remainders and
 * eligibility masks are precomputed.  Thresholds that select the same set
 * of possible y values are canonicalized before memoization.
 *
 * For --upto N, a(N) is computed first.  This already reaches every root
 * state needed for every a(n), n<N: for each last value x<=n, follow the
 * valid increasing tail x,n+1,n+2,...,N backwards through the recurrence.
 * On reaching [n], the threshold is x and canonicalizes to x-1, exactly as
 * the root F([n],x,n-1) does.  Thus all smaller terms use the same memo table
 * and require only root lookups; no unproved pruning or stored answer is used.
 * Known OEIS terms are verification data only and are never returned as
 * computed terms.
 *
 * All count additions, packed keys, transitions, allocations, and configured
 * memory limits are checked.  A limit failure stops the program rather than
 * returning a partial or wrapped value.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic 332800_01.c -o 332800_01
 *
 * Usage:
 *   ./332800_01
 *   ./332800_01 --upto 22 --verbose
 *   ./332800_01 --term 22 --memory-mb 1024 --verbose
 *   ./332800_01 --check
 *
 * The default and --upto atomically replace b332800_01.txt while holding an
 * exclusive writer lock.  --term and --check do not modify the b-file.
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

#define MAX_N 32
#define DEFAULT_MAX_N 22
#define KNOWN_MAX_N 32
#define CHECK_MAX_N 17
#define DIRECT_CHECK_MAX_N 10
#define DEFAULT_MEMORY_MB 1024
#define MIN_MEMORY_MB 32
#define MAX_MEMORY_MB 65536
#define INITIAL_CAPACITY 1024
#define LOAD_NUMERATOR 7
#define LOAD_DENOMINATOR 10
#define KEY_AUX_BITS 10
#define MEMO_BUSY_BIT (UINT64_C(1) << 63)
#define BFILE_NAME "b332800_01.txt"
#define BFILE_TEMP_TEMPLATE BFILE_NAME ".tmp.XXXXXX"
#define BFILE_LOCK_NAME BFILE_NAME ".lock"

typedef uint64_t mask_t;

_Static_assert(MAX_N <= 32, "the packed state reserves 32 mask bits");
_Static_assert(2 * 5 <= KEY_AUX_BITS,
               "the packed state needs five bits each for x and t");
_Static_assert(MAX_N + KEY_AUX_BITS < 63,
               "packed memo keys must not overlap the busy bit");

static const uint64_t known[KNOWN_MAX_N + 1] = {
    UINT64_C(1),            UINT64_C(1),            UINT64_C(2),
    UINT64_C(4),            UINT64_C(9),            UINT64_C(21),
    UINT64_C(44),           UINT64_C(109),          UINT64_C(241),
    UINT64_C(530),          UINT64_C(1176),         UINT64_C(3180),
    UINT64_C(6456),         UINT64_C(14835),        UINT64_C(34672),
    UINT64_C(81877),        UINT64_C(179434),       UINT64_C(479275),
    UINT64_C(977224),       UINT64_C(2503363),      UINT64_C(5339049),
    UINT64_C(11207391),     UINT64_C(28379591),     UINT64_C(82473713),
    UINT64_C(166689486),    UINT64_C(370775384),    UINT64_C(877910547),
    UINT64_C(2150475950),   UINT64_C(4608590865),   UINT64_C(12146671367),
    UINT64_C(24620749285),  UINT64_C(64137229920),  UINT64_C(143062854926)
};

typedef struct {
    uint64_t key_plus_one;
    uint64_t count;
} MemoEntry;

/*
 * Even if the whole configured 64-GiB budget consisted of memo entries,
 * each entry can generate at most MAX_N transitions.  Root lookups for all
 * prefix terms add at most MAX_N*(MAX_N+1)/2 calls.  Hence the hot diagnostic
 * counters provably fit in uint64_t and need no per-transition branch.
 */
_Static_assert(((uint64_t)MAX_MEMORY_MB * 1024U * 1024U /
                sizeof(MemoEntry)) * MAX_N +
                   MAX_N * (MAX_N + 1U) / 2U <
                   UINT64_MAX,
               "diagnostic counters must fit in uint64_t");

typedef struct {
    MemoEntry *entry;
    size_t capacity;
    size_t used;
} MemoTable;

typedef struct {
    MemoEntry *entry;
    MemoEntry *table_base;
    uint64_t stored_key;
} MemoReservation;

typedef struct {
    int universe_n;
    int current_n;
    uint8_t remainder[MAX_N + 1][MAX_N + 1];
    mask_t equal_mask[MAX_N + 1][MAX_N];
    mask_t at_most_mask[MAX_N + 1][MAX_N];
    MemoTable memo;
    size_t memory_limit;
    size_t live_memory;
    size_t peak_memory;
    size_t peak_states;
    uint64_t calls;
    uint64_t memo_hits;
    uint64_t transitions;
} Search;

typedef struct {
    size_t new_states;
    size_t total_states;
    size_t peak_memory;
    uint64_t calls;
    uint64_t memo_hits;
    uint64_t transitions;
} Statistics;

typedef enum {
    MODE_UPTO,
    MODE_TERM
} OutputMode;

static char bfile_temp_name[] = BFILE_TEMP_TEMPLATE;
static int bfile_lock_descriptor = -1;
static bool bfile_temp_active = false;

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

static uint64_t hash_u64(uint64_t value)
{
    /* All 42 packed-key bits are folded before the low-bit table index. */
    value ^= value >> 23;
    value *= UINT64_C(0x2127599bf4325c37);
    return value ^ (value >> 47);
}

static int first_value(mask_t mask)
{
#if defined(__clang__) || defined(__GNUC__)
    return (int)__builtin_ctzll(mask) + 1;
#else
    int value = 1;
    while ((mask & 1U) == 0U) {
        mask >>= 1U;
        ++value;
    }
    return value;
#endif
}

static mask_t value_bit(int value)
{
    return UINT64_C(1) << (value - 1);
}

static mask_t full_mask(int n)
{
    return n == 0 ? 0U : (UINT64_C(1) << n) - 1U;
}

static uint64_t pack_key(mask_t set, int last, unsigned threshold)
{
    if (last < 1 || last > MAX_N || threshold >= MAX_N)
        die("memo key component out of range");
    if (set > UINT64_MAX >> KEY_AUX_BITS)
        die("memo key mask overflow");
    return (set << KEY_AUX_BITS) |
           ((uint64_t)(unsigned)(last - 1) << 5) |
           (uint64_t)threshold;
}

static void memory_error(const Search *search, size_t requested)
{
    fprintf(stderr,
            "error: memo memory limit exceeded at n=%d "
            "(live %.1f MiB + request %.1f MiB > limit %.1f MiB); "
            "states=%zu, transitions=%" PRIu64 "; increase --memory-mb\n",
            search->current_n, (double)search->live_memory / 1048576.0,
            (double)requested / 1048576.0,
            (double)search->memory_limit / 1048576.0,
            search->memo.used, search->transitions);
    exit(EXIT_FAILURE);
}

static MemoEntry *allocate_entries(Search *search, size_t capacity)
{
    if (capacity > SIZE_MAX / sizeof(MemoEntry))
        die("memo allocation size overflow");
    size_t bytes = capacity * sizeof(MemoEntry);
    if (search->live_memory > search->memory_limit ||
        bytes > search->memory_limit - search->live_memory)
        memory_error(search, bytes);
    MemoEntry *entry = calloc(capacity, sizeof(*entry));
    if (entry == NULL) {
        fprintf(stderr,
                "error: could not allocate %.1f MiB for memo table at n=%d\n",
                (double)bytes / 1048576.0, search->current_n);
        exit(EXIT_FAILURE);
    }
    search->live_memory += bytes;
    if (search->live_memory > search->peak_memory)
        search->peak_memory = search->live_memory;
    return entry;
}

static void memo_init(Search *search, size_t hint)
{
    size_t capacity = INITIAL_CAPACITY;
    while (capacity < hint) {
        if (capacity > SIZE_MAX / 2U) die("memo capacity overflow");
        capacity *= 2U;
    }
    search->memo.entry = allocate_entries(search, capacity);
    search->memo.capacity = capacity;
    search->memo.used = 0U;
}

/*
 * Empirical capacity planning only: this changes allocation timing, never
 * the recurrence or its result.  For the current canonical state encoding,
 * measured n=10..14 fit below load 0.7 in 2^(n+3) entries, and measured
 * n=15..22 fit in 2^(n+4).  If the requested memory budget cannot hold it,
 * start small and retain the checked grow-or-fail behavior.
 */
static size_t memo_capacity_hint(int n, size_t memory_limit)
{
    if (n < 10 || n > 22) return INITIAL_CAPACITY;
    unsigned shift = (unsigned)n + (n < 15 ? 3U : 4U);
    if (shift >= (unsigned)(sizeof(size_t) * 8U)) return INITIAL_CAPACITY;
    size_t capacity = (size_t)1U << shift;
    if (capacity > SIZE_MAX / sizeof(MemoEntry)) return INITIAL_CAPACITY;
    size_t bytes = capacity * sizeof(MemoEntry);
    return bytes <= memory_limit ? capacity : INITIAL_CAPACITY;
}

static void memo_free(Search *search)
{
    if (search->memo.entry == NULL) return;
    size_t bytes = search->memo.capacity * sizeof(*search->memo.entry);
    if (bytes > search->live_memory) die("memo memory accounting error");
    free(search->memo.entry);
    search->live_memory -= bytes;
    memset(&search->memo, 0, sizeof(search->memo));
}

static void memo_grow(Search *search)
{
    MemoTable old = search->memo;
    if (old.capacity > SIZE_MAX / 2U) die("memo capacity overflow");
    search->memo.entry = allocate_entries(search, old.capacity * 2U);
    search->memo.capacity = old.capacity * 2U;
    search->memo.used = 0U;

    for (size_t i = 0; i < old.capacity; ++i) {
        MemoEntry item = old.entry[i];
        if (item.key_plus_one == 0U) continue;
        uint64_t stored = item.key_plus_one & ~MEMO_BUSY_BIT;
        if (stored == 0U) die("invalid memo key during growth");
        size_t slot = (size_t)hash_u64(stored - 1U) &
                      (search->memo.capacity - 1U);
        while (search->memo.entry[slot].key_plus_one != 0U)
            slot = (slot + 1U) & (search->memo.capacity - 1U);
        search->memo.entry[slot] = item;
        ++search->memo.used;
    }

    size_t old_bytes = old.capacity * sizeof(*old.entry);
    free(old.entry);
    if (old_bytes > search->live_memory) die("memo accounting underflow");
    search->live_memory -= old_bytes;
}

/*
 * Look up a state or reserve its empty slot before recursive evaluation.
 * Every recurrence edge removes one set bit, so encountering the same busy
 * key would prove an internal cycle and is an error.  A reservation remains
 * valid across table growth because memo_publish can locate the moved key.
 */
static bool memo_get_or_reserve(Search *search, uint64_t key, uint64_t hash,
                                uint64_t *count,
                                MemoReservation *reservation)
{
    uint64_t stored = key + 1U;
    if (stored == 0U || (stored & MEMO_BUSY_BIT) != 0U)
        die("memo stored-key overflow");

    for (;;) {
        size_t slot = (size_t)hash & (search->memo.capacity - 1U);
        while (search->memo.entry[slot].key_plus_one != 0U) {
            uint64_t present = search->memo.entry[slot].key_plus_one;
            if ((present & ~MEMO_BUSY_BIT) == stored) {
                if ((present & MEMO_BUSY_BIT) != 0U)
                    die("memo recurrence cycle detected");
                *count = search->memo.entry[slot].count;
                ++search->memo_hits;
                return true;
            }
            slot = (slot + 1U) & (search->memo.capacity - 1U);
        }
        if ((search->memo.used + 1U) * LOAD_DENOMINATOR >=
            search->memo.capacity * LOAD_NUMERATOR) {
            memo_grow(search);
            continue;
        }

        MemoEntry *entry = &search->memo.entry[slot];
        entry->count = 0U;
        entry->key_plus_one = stored | MEMO_BUSY_BIT;
        ++search->memo.used;
        if (search->memo.used > search->peak_states)
            search->peak_states = search->memo.used;
        reservation->entry = entry;
        reservation->table_base = search->memo.entry;
        reservation->stored_key = stored;
        return false;
    }
}

static void memo_publish(Search *search, uint64_t hash, uint64_t count,
                         const MemoReservation *reservation)
{
    MemoEntry *entry = reservation->entry;
    uint64_t busy_key = reservation->stored_key | MEMO_BUSY_BIT;

    if (reservation->table_base != search->memo.entry ||
        entry->key_plus_one != busy_key) {
        size_t slot = (size_t)hash & (search->memo.capacity - 1U);
        while (search->memo.entry[slot].key_plus_one != 0U &&
               (search->memo.entry[slot].key_plus_one & ~MEMO_BUSY_BIT) !=
                   reservation->stored_key)
            slot = (slot + 1U) & (search->memo.capacity - 1U);
        entry = &search->memo.entry[slot];
    }

    if (entry->key_plus_one != busy_key)
        die("lost or prematurely published memo reservation");
    entry->count = count;
    entry->key_plus_one = reservation->stored_key;
}

static void search_init(Search *search, int n, size_t memory_limit)
{
    memset(search, 0, sizeof(*search));
    search->universe_n = n;
    search->current_n = n;
    search->memory_limit = memory_limit;

    for (int last = 1; last <= n; ++last) {
        for (int value = 1; value <= n; ++value) {
            unsigned r = (unsigned)(value % last);
            search->remainder[value][last] = (uint8_t)r;
            search->equal_mask[last][r] |= value_bit(value);
        }
        mask_t accumulated = 0U;
        for (unsigned r = 0; r < (unsigned)n; ++r) {
            accumulated |= search->equal_mask[last][r];
            search->at_most_mask[last][r] = accumulated;
        }
    }
    memo_init(search, memo_capacity_hint(n, memory_limit));
}

/*
 * F(set,last,threshold).  For fixed set and last, only which remainders occur
 * at or below threshold matters.  Replacing threshold by the largest such
 * occurring remainder leaves the recurrence unchanged and merges states.
 */
static uint64_t count_state(Search *search, mask_t set, int last,
                            unsigned threshold)
{
    ++search->calls;

    mask_t bit = value_bit(last);
    if ((set & bit) == 0U) die("memo state does not contain its last value");
    mask_t rest = set ^ bit;
    if (rest == 0U) return 1U;

    if (threshold >= (unsigned)last) threshold = (unsigned)last - 1U;
    while ((rest & search->equal_mask[last][threshold]) == 0U) {
        if (threshold == 0U) return 0U;
        --threshold;
    }

    uint64_t key = pack_key(set, last, threshold);
    uint64_t hash = hash_u64(key);
    uint64_t cached;
    MemoReservation reservation;
    if (memo_get_or_reserve(search, key, hash, &cached, &reservation))
        return cached;

    mask_t candidates = rest & search->at_most_mask[last][threshold];
    uint64_t total = 0U;
    while (candidates != 0U) {
        int penultimate = first_value(candidates);
        candidates &= candidates - 1U;
        ++search->transitions;
        unsigned next_threshold = search->remainder[penultimate][last];
        uint64_t add = count_state(search, rest, penultimate,
                                   next_threshold);
        if (UINT64_MAX - total < add) die("state count exceeds uint64_t");
        total += add;
    }
    memo_publish(search, hash, total, &reservation);
    return total;
}

static uint64_t compute_term(Search *search, int n,
                             Statistics *statistics)
{
    memset(statistics, 0, sizeof(*statistics));
    if (n == 0) return 1U;

    if (n > search->universe_n)
        die("term exceeds initialized search universe");
    search->current_n = n;
    size_t states_before = search->memo.used;
    uint64_t calls_before = search->calls;
    uint64_t hits_before = search->memo_hits;
    uint64_t transitions_before = search->transitions;

    mask_t all = full_mask(n);
    uint64_t answer = 0U;
    for (int last = 1; last <= n; ++last) {
        uint64_t add = count_state(search, all, last, (unsigned)n - 1U);
        if (UINT64_MAX - answer < add) die("A332800 term exceeds uint64_t");
        answer += add;
    }

    if (search->memo.used < states_before || search->calls < calls_before ||
        search->memo_hits < hits_before ||
        search->transitions < transitions_before)
        die("statistics counter underflow");
    statistics->new_states = search->memo.used - states_before;
    statistics->total_states = search->memo.used;
    statistics->peak_memory = search->peak_memory;
    statistics->calls = search->calls - calls_before;
    statistics->memo_hits = search->memo_hits - hits_before;
    statistics->transitions = search->transitions - transitions_before;
    return answer;
}

static void verify_known(uint64_t value, int n)
{
    if (n <= KNOWN_MAX_N && value != known[n]) {
        fprintf(stderr,
                "error: A332800 mismatch at n=%d: got %" PRIu64
                ", expected %" PRIu64 "\n",
                n, value, known[n]);
        exit(EXIT_FAILURE);
    }
}

static bool next_permutation(int values[MAX_N], int n)
{
    int i = n - 2;
    while (i >= 0 && values[i] >= values[i + 1]) --i;
    if (i < 0) return false;
    int j = n - 1;
    while (values[j] <= values[i]) --j;
    int temporary = values[i];
    values[i] = values[j];
    values[j] = temporary;
    for (int left = i + 1, right = n - 1; left < right; ++left, --right) {
        temporary = values[left];
        values[left] = values[right];
        values[right] = temporary;
    }
    return true;
}

static uint64_t direct_term(int n)
{
    if (n == 0) return 1U;
    int values[MAX_N];
    for (int i = 0; i < n; ++i) values[i] = i + 1;
    uint64_t count = 0U;
    do {
        bool valid = true;
        if (n >= 3) {
            int previous = values[0] % values[1];
            for (int i = 1; i < n - 1; ++i) {
                int current = values[i] % values[i + 1];
                if (previous > current) {
                    valid = false;
                    break;
                }
                previous = current;
            }
        }
        if (valid) {
            if (count == UINT64_MAX) die("direct count overflow");
            ++count;
        }
    } while (next_permutation(values, n));
    return count;
}

static void release_bfile_lock(void)
{
    if (bfile_lock_descriptor < 0) return;
    int saved_error = errno;
    const struct flock lock = {
        .l_type = F_UNLCK,
        .l_whence = SEEK_SET,
        .l_start = 0,
        .l_len = 0
    };
    (void)fcntl(bfile_lock_descriptor, F_SETLK, &lock);
    (void)close(bfile_lock_descriptor);
    bfile_lock_descriptor = -1;
    errno = saved_error;
}

static void cleanup_bfile(void)
{
    int saved_error = errno;
    if (bfile_temp_active) {
        (void)unlink(bfile_temp_name);
        bfile_temp_active = false;
    }
    release_bfile_lock();
    errno = saved_error;
}

static FILE *open_bfile(void)
{
    bfile_lock_descriptor = open(BFILE_LOCK_NAME, O_CREAT | O_RDWR, 0666);
    if (bfile_lock_descriptor < 0) {
        fprintf(stderr, "error: cannot open %s: %s\n",
                BFILE_LOCK_NAME, strerror(errno));
        exit(EXIT_FAILURE);
    }
    const struct flock lock = {
        .l_type = F_WRLCK,
        .l_whence = SEEK_SET,
        .l_start = 0,
        .l_len = 0
    };
    if (fcntl(bfile_lock_descriptor, F_SETLK, &lock) != 0) {
        int saved_error = errno;
        cleanup_bfile();
        if (saved_error == EACCES || saved_error == EAGAIN)
            fprintf(stderr, "error: another writer holds %s\n",
                    BFILE_LOCK_NAME);
        else
            fprintf(stderr, "error: cannot lock %s: %s\n",
                    BFILE_LOCK_NAME, strerror(saved_error));
        exit(EXIT_FAILURE);
    }

    int descriptor = mkstemp(bfile_temp_name);
    if (descriptor < 0) {
        int saved_error = errno;
        cleanup_bfile();
        fprintf(stderr, "error: cannot create %s: %s\n",
                BFILE_TEMP_TEMPLATE, strerror(saved_error));
        exit(EXIT_FAILURE);
    }
    bfile_temp_active = true;

    struct stat existing;
    mode_t mode;
    if (stat(BFILE_NAME, &existing) == 0) {
        mode = existing.st_mode & 0777;
    } else if (errno == ENOENT) {
        mode_t mask = umask(0);
        (void)umask(mask);
        mode = 0666 & ~mask;
    } else {
        int saved_error = errno;
        (void)close(descriptor);
        errno = saved_error;
        cleanup_bfile();
        fprintf(stderr, "error: cannot inspect %s: %s\n",
                BFILE_NAME, strerror(saved_error));
        exit(EXIT_FAILURE);
    }
    if (fchmod(descriptor, mode) != 0) {
        int saved_error = errno;
        (void)close(descriptor);
        errno = saved_error;
        cleanup_bfile();
        fprintf(stderr, "error: cannot set permissions on %s: %s\n",
                bfile_temp_name, strerror(saved_error));
        exit(EXIT_FAILURE);
    }
    FILE *stream = fdopen(descriptor, "w");
    if (stream == NULL) {
        int saved_error = errno;
        (void)close(descriptor);
        errno = saved_error;
        cleanup_bfile();
        fprintf(stderr, "error: cannot open %s: %s\n",
                bfile_temp_name, strerror(saved_error));
        exit(EXIT_FAILURE);
    }
    return stream;
}

static void write_bfile_term(FILE *stream, int n, uint64_t value)
{
    if (fprintf(stream, "%d %" PRIu64 "\n", n, value) < 0 ||
        fflush(stream) != 0) {
        int saved_error = errno == 0 ? EIO : errno;
        (void)fclose(stream);
        errno = saved_error;
        cleanup_bfile();
        fprintf(stderr, "error: cannot write %s: %s\n",
                bfile_temp_name, strerror(saved_error));
        exit(EXIT_FAILURE);
    }
}

static void finish_bfile(FILE *stream)
{
    bool failed = false;
    int saved_error = 0;
    if (fflush(stream) != 0) {
        failed = true;
        saved_error = errno;
    }
    int descriptor = fileno(stream);
    if (descriptor < 0) {
        failed = true;
        if (saved_error == 0) saved_error = errno;
    } else if (!failed && fsync(descriptor) != 0) {
        failed = true;
        saved_error = errno;
    }
    if (fclose(stream) != 0) {
        failed = true;
        if (saved_error == 0) saved_error = errno;
    }
    if (failed) {
        if (saved_error == 0) saved_error = EIO;
        errno = saved_error;
        cleanup_bfile();
        fprintf(stderr, "error: cannot finalize %s: %s\n",
                bfile_temp_name, strerror(saved_error));
        exit(EXIT_FAILURE);
    }
    if (rename(bfile_temp_name, BFILE_NAME) != 0) {
        int rename_error = errno;
        errno = rename_error;
        cleanup_bfile();
        fprintf(stderr, "error: cannot replace %s: %s\n",
                BFILE_NAME, strerror(rename_error));
        exit(EXIT_FAILURE);
    }
    bfile_temp_active = false;
    release_bfile_lock();
}

static void usage(FILE *stream, const char *program)
{
    fprintf(stream,
            "Usage: %s [--upto N | --term N | --check] "
            "[--memory-mb MiB] [--verbose]\n",
            program);
}

static void print_statistics(int n, uint64_t value,
                             const Statistics *statistics, double elapsed)
{
    double hit_rate = statistics->calls == 0U
                          ? 0.0
                          : 100.0 * (double)statistics->memo_hits /
                                (double)statistics->calls;
    fprintf(stderr,
            "332800_01: n=%d, a(n)=%" PRIu64
            ", new_states=%zu, total_states=%zu, calls=%" PRIu64
            ", memo_hits=%" PRIu64 " (%.1f%%), transitions=%" PRIu64
            ", peak_memory=%.1f MiB, %.6f s\n",
            n, value, statistics->new_states, statistics->total_states,
            statistics->calls, statistics->memo_hits, hit_rate,
            statistics->transitions,
            (double)statistics->peak_memory / 1048576.0, elapsed);
}

int main(int argc, char **argv)
{
    OutputMode mode = MODE_UPTO;
    int limit = DEFAULT_MAX_N;
    unsigned memory_mb = DEFAULT_MEMORY_MB;
    bool check = false;
    bool verbose = false;
    bool mode_seen = false;

    if (atexit(cleanup_bfile) != 0) die("could not register cleanup");
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--upto") == 0 ||
            strcmp(argv[i], "--term") == 0) {
            if (mode_seen || i + 1 == argc) {
                usage(stderr, argv[0]);
                return EXIT_FAILURE;
            }
            mode = strcmp(argv[i], "--term") == 0 ? MODE_TERM : MODE_UPTO;
            limit = parse_integer(argv[++i], "n", 0, MAX_N);
            mode_seen = true;
        } else if (strcmp(argv[i], "--check") == 0) {
            if (mode_seen) {
                usage(stderr, argv[0]);
                return EXIT_FAILURE;
            }
            check = true;
            limit = CHECK_MAX_N;
            mode_seen = true;
        } else if (strcmp(argv[i], "--memory-mb") == 0) {
            if (i + 1 == argc) {
                usage(stderr, argv[0]);
                return EXIT_FAILURE;
            }
            memory_mb = (unsigned)parse_integer(argv[++i], "memory-mb",
                                                 MIN_MEMORY_MB,
                                                 MAX_MEMORY_MB);
        } else if (strcmp(argv[i], "--verbose") == 0) {
            verbose = true;
        } else if (strcmp(argv[i], "--help") == 0 ||
                   strcmp(argv[i], "-h") == 0) {
            usage(stdout, argv[0]);
            return EXIT_SUCCESS;
        } else {
            fprintf(stderr, "error: unknown option: %s\n", argv[i]);
            usage(stderr, argv[0]);
            return EXIT_FAILURE;
        }
    }

    if ((size_t)memory_mb > SIZE_MAX / (1024U * 1024U))
        die("memory limit size overflow");
    size_t memory_limit = (size_t)memory_mb * 1024U * 1024U;

    if (mode == MODE_TERM) {
        Statistics statistics;
        uint64_t value;
        double started = monotonic_seconds();
        if (limit == 0) {
            memset(&statistics, 0, sizeof(statistics));
            value = 1U;
        } else {
            Search search;
            search_init(&search, limit, memory_limit);
            value = compute_term(&search, limit, &statistics);
            memo_free(&search);
            if (search.live_memory != 0U)
                die("memo memory accounting leak");
        }
        verify_known(value, limit);
        if (verbose)
            print_statistics(limit, value, &statistics,
                             monotonic_seconds() - started);
        if (printf("%" PRIu64 "\n", value) < 0 || fflush(stdout) != 0)
            die("could not finish stdout");
        return EXIT_SUCCESS;
    }

    uint64_t values[MAX_N + 1] = {0U};
    Statistics all_statistics[MAX_N + 1];
    double elapsed[MAX_N + 1] = {0.0};
    memset(all_statistics, 0, sizeof(all_statistics));
    values[0] = 1U;
    verify_known(values[0], 0);

    if (limit > 0) {
        Search search;
        search_init(&search, limit, memory_limit);

        /*
         * Descending order is essential for the reuse proof in the file
         * header: a(limit) populates every smaller prefix root first.
         */
        for (int n = limit; n >= 1; --n) {
            double started = monotonic_seconds();
            values[n] = compute_term(&search, n, &all_statistics[n]);
            elapsed[n] = monotonic_seconds() - started;
            verify_known(values[n], n);
        }

        memo_free(&search);
        if (search.live_memory != 0U) die("memo memory accounting leak");
    }

    if (check) {
        int direct_limit = limit < DIRECT_CHECK_MAX_N
                               ? limit
                               : DIRECT_CHECK_MAX_N;
        for (int n = 0; n <= direct_limit; ++n) {
            uint64_t direct = direct_term(n);
            if (direct != values[n]) {
                fprintf(stderr,
                        "error: direct mismatch at n=%d: DP=%" PRIu64
                        ", direct=%" PRIu64 "\n",
                        n, values[n], direct);
                return EXIT_FAILURE;
            }
        }
    }

    if (verbose) {
        for (int n = 0; n <= limit; ++n)
            print_statistics(n, values[n], &all_statistics[n], elapsed[n]);
    }

    FILE *bfile = check ? NULL : open_bfile();
    for (int n = 0; n <= limit; ++n) {
        if (bfile != NULL) write_bfile_term(bfile, n, values[n]);
        if (printf("%s%" PRIu64, n == 0 ? "" : ", ", values[n]) < 0)
            die("could not write stdout");
    }
    if (putchar('\n') == EOF || fflush(stdout) != 0)
        die("could not finish stdout");
    if (bfile != NULL) finish_bfile(bfile);
    if (check)
        fprintf(stderr,
                "check passed: A332800(0..%d), direct permutations "
                "through n=%d\n",
                CHECK_MAX_N, DIRECT_CHECK_MAX_N);
    return EXIT_SUCCESS;
}
