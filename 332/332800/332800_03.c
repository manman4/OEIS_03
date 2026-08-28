/*
 * A332800 -- low-memory forward layer dynamic programming.
 *
 * Count permutations sigma of [n] for which
 *
 *   sigma(k) mod sigma(k+1) <= sigma(k+1) mod sigma(k+2).
 *
 * For |S| >= 2, D(S,x,r) is the number of valid orderings of S that end
 * in x and whose final remainder is exactly r.  Appending y outside S is
 * the exact transition
 *
 *   D(S union {y}, y, x mod y) += D(S,x,r),  if r <= x mod y.
 *
 * Unlike 332800_01 and 332800_02, this program works forward and keeps only
 * two adjacent |S|-layers.  Completed layers are freed, so memory is governed
 * by the largest adjacent pair rather than by all reached states.
 *
 * There is one proved shortcut.  Put h=floor(n/2).  If a remainder exceeds
 * h, then the corresponding pair, and every later pair, consists of values
 * greater than h in strictly increasing order.  Indeed, a mod b > h implies
 * a<b and a mod b=a>h; weak increase repeats the argument.  Consequently:
 *
 *   - a transition whose new remainder is >h has exactly one completion if
 *     every still-unused value is greater than its new last value, and none
 *     otherwise;
 *   - equivalently, when x>h and all unused values exceed x, their increasing
 *     order is the unique completion.
 *
 * Those completions are added directly and states with remainder >h are not
 * stored.  This is exact aggregation, not heuristic pruning.  Primality is
 * not used because it imposes no restriction on these remainder inequalities.
 *
 * Known OEIS values are verification data only.  Every printed value is
 * computed by the dynamic program (or, with --check, independently enumerated
 * as an additional check).
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic 332800_03.c -o 332800_03
 *
 * Usage:
 *   ./332800_03
 *   ./332800_03 --upto 22 --memory-mb 1024 --verbose
 *   ./332800_03 --term 22 --memory-mb 1024 --verbose
 *   ./332800_03 --check
 *
 * The default and --upto atomically replace b332800_03.txt.  --term and
 * --check do not modify it.
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
#define SHORTCUT_CHECK_MAX_N 14
#define DEFAULT_MEMORY_MB 1024
#define MIN_MEMORY_MB 32
#define MAX_MEMORY_MB 65536
#define INITIAL_CAPACITY 1024
#define LOAD_NUMERATOR 4
#define LOAD_DENOMINATOR 5
#define KEY_AUX_BITS 10
#define BFILE_NAME "b332800_03.txt"
#define BFILE_TEMP_TEMPLATE BFILE_NAME ".tmp.XXXXXX"
#define BFILE_LOCK_NAME BFILE_NAME ".lock"

typedef uint64_t mask_t;

_Static_assert(MAX_N <= 32, "the state mask reserves 32 bits");
_Static_assert(2 * 5 <= KEY_AUX_BITS,
               "the state key needs five bits each for last and remainder");
_Static_assert(MAX_N + KEY_AUX_BITS < 63,
               "packed state keys must fit below bit 63");

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
} LayerEntry;

_Static_assert(sizeof(LayerEntry) == 16, "unexpected layer entry size");

typedef struct {
    LayerEntry *entry;
    size_t capacity;
    size_t used;
} Layer;

typedef struct {
    size_t limit;
    size_t current;
    size_t peak;
} MemoryTracker;

typedef struct {
    uint64_t tested_transitions;
    uint64_t accepted_transitions;
    uint64_t shortcut_completions;
    uint64_t accumulated_paths;
    size_t total_states;
    size_t peak_layer_states;
    size_t peak_memory;
    size_t layer_states[MAX_N + 1];
    size_t layer_capacities[MAX_N + 1];
} Statistics;

typedef enum { MODE_UPTO, MODE_TERM } OutputMode;

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
    value ^= value >> 30;
    value *= UINT64_C(0xbf58476d1ce4e5b9);
    value ^= value >> 27;
    value *= UINT64_C(0x94d049bb133111eb);
    return value ^ (value >> 31);
}

static int first_value(mask_t mask)
{
#if defined(__clang__) || defined(__GNUC__)
    return (int)__builtin_ctzll(mask) + 1;
#else
    int value = 1;
    while ((mask & 1U) == 0U) { mask >>= 1U; ++value; }
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

static uint64_t pack_key(mask_t set, int last, unsigned remainder)
{
    if (last < 1 || last > MAX_N || remainder >= MAX_N)
        die("state key component out of range");
    if (set > UINT64_MAX >> KEY_AUX_BITS)
        die("state key mask overflow");
    uint64_t key = (set << KEY_AUX_BITS) |
                   ((uint64_t)(unsigned)(last - 1) << 5) | remainder;
    if (key == UINT64_MAX) die("state key overflow");
    return key;
}

static void unpack_key(uint64_t key, mask_t *set, int *last,
                       unsigned *remainder)
{
    *remainder = (unsigned)(key & UINT64_C(31));
    *last = (int)((key >> 5) & UINT64_C(31)) + 1;
    *set = key >> KEY_AUX_BITS;
}

static void checked_stat_increment(uint64_t *value, uint64_t add,
                                   const char *message)
{
    if (UINT64_MAX - *value < add) die(message);
    *value += add;
}

static void memory_reserve(MemoryTracker *tracker, size_t bytes)
{
    if (bytes > tracker->limit - tracker->current) {
        fprintf(stderr,
                "error: layer tables need more than --memory-mb limit "
                "(current=%.1f MiB, additional=%.1f MiB, limit=%.1f MiB)\n",
                (double)tracker->current / 1048576.0,
                (double)bytes / 1048576.0,
                (double)tracker->limit / 1048576.0);
        exit(EXIT_FAILURE);
    }
    tracker->current += bytes;
    if (tracker->peak < tracker->current) tracker->peak = tracker->current;
}

static void memory_release(MemoryTracker *tracker, size_t bytes)
{
    if (bytes > tracker->current) die("memory tracker underflow");
    tracker->current -= bytes;
}

static void layer_init(Layer *layer, size_t capacity,
                       MemoryTracker *tracker)
{
    if (capacity < INITIAL_CAPACITY ||
        (capacity & (capacity - 1U)) != 0U)
        die("invalid layer-table capacity");
    if (capacity > SIZE_MAX / sizeof(LayerEntry))
        die("layer-table allocation size overflow");
    size_t bytes = capacity * sizeof(LayerEntry);
    memory_reserve(tracker, bytes);
    layer->entry = calloc(capacity, sizeof(*layer->entry));
    if (layer->entry == NULL) {
        memory_release(tracker, bytes);
        fprintf(stderr, "error: could not allocate %.1f MiB layer table\n",
                (double)bytes / 1048576.0);
        exit(EXIT_FAILURE);
    }
    layer->capacity = capacity;
    layer->used = 0U;
}

static size_t capacity_for_entries(size_t entries)
{
    size_t capacity = INITIAL_CAPACITY;
    for (;;) {
        size_t limit = (capacity / LOAD_DENOMINATOR) * LOAD_NUMERATOR;
        if (entries + 1U < limit) return capacity;
        if (capacity > SIZE_MAX / 2U)
            die("layer-table capacity estimate overflow");
        capacity *= 2U;
    }
}

static void layer_destroy(Layer *layer, MemoryTracker *tracker)
{
    if (layer->entry == NULL) return;
    size_t bytes = layer->capacity * sizeof(*layer->entry);
    free(layer->entry);
    memory_release(tracker, bytes);
    layer->entry = NULL;
    layer->capacity = 0U;
    layer->used = 0U;
}

static void insert_rehashed(Layer *layer, LayerEntry source)
{
    uint64_t key = source.key_plus_one - 1U;
    size_t slot = (size_t)hash_u64(key) & (layer->capacity - 1U);
    while (layer->entry[slot].key_plus_one != 0U)
        slot = (slot + 1U) & (layer->capacity - 1U);
    layer->entry[slot] = source;
    ++layer->used;
}

static void layer_grow(Layer *layer, MemoryTracker *tracker)
{
    if (layer->capacity > SIZE_MAX / 2U)
        die("layer-table capacity overflow");
    Layer grown = {0};
    layer_init(&grown, layer->capacity * 2U, tracker);
    for (size_t i = 0; i < layer->capacity; ++i)
        if (layer->entry[i].key_plus_one != 0U)
            insert_rehashed(&grown, layer->entry[i]);
    if (grown.used != layer->used) die("layer rehash lost a state");
    layer_destroy(layer, tracker);
    *layer = grown;
}

static void layer_add(Layer *layer, MemoryTracker *tracker, uint64_t key,
                      uint64_t add)
{
    if (add == 0U) return;
    uint64_t stored = key + 1U;
    if (stored == 0U) die("stored state key overflow");

    for (;;) {
        size_t slot = (size_t)hash_u64(key) & (layer->capacity - 1U);
        for (;;) {
            LayerEntry *entry = &layer->entry[slot];
            if (entry->key_plus_one == stored) {
                if (UINT64_MAX - entry->count < add)
                    die("state count exceeds uint64_t");
                entry->count += add;
                return;
            }
            if (entry->key_plus_one == 0U) {
                size_t limit = (layer->capacity / LOAD_DENOMINATOR) *
                               LOAD_NUMERATOR;
                if (layer->used + 1U >= limit) break;
                entry->key_plus_one = stored;
                entry->count = add;
                ++layer->used;
                return;
            }
            slot = (slot + 1U) & (layer->capacity - 1U);
        }
        layer_grow(layer, tracker);
    }
}

static bool all_values_greater(mask_t values, int boundary)
{
    return (values & full_mask(boundary)) == 0U;
}

static uint64_t compute_term(int n, size_t memory_limit, bool use_shortcut,
                             Statistics *statistics)
{
    memset(statistics, 0, sizeof(*statistics));
    if (n == 0 || n == 1) return 1U;
    if (n == 2) return 2U;

    uint8_t remainder[MAX_N + 1][MAX_N + 1] = {{0}};
    for (int left = 1; left <= n; ++left)
        for (int right = 1; right <= n; ++right)
            remainder[left][right] = (uint8_t)(left % right);

    MemoryTracker tracker = {.limit = memory_limit};
    Layer current = {0};
    layer_init(&current, INITIAL_CAPACITY, &tracker);
    mask_t universe = full_mask(n);
    unsigned half = (unsigned)n / 2U;

    for (int first = 1; first <= n; ++first) {
        for (int last = 1; last <= n; ++last) {
            if (first == last) continue;
            unsigned r = remainder[first][last];
            checked_stat_increment(&statistics->tested_transitions, 1U,
                                   "transition statistics overflow");
            mask_t set = value_bit(first) | value_bit(last);
            if (use_shortcut && r > half) {
                mask_t unused = universe ^ set;
                if (all_values_greater(unused, last)) {
                    checked_stat_increment(&statistics->shortcut_completions,
                                           1U, "shortcut count overflow");
                    checked_stat_increment(&statistics->accumulated_paths, 1U,
                                           "answer exceeds uint64_t");
                }
                continue;
            }
            checked_stat_increment(&statistics->accepted_transitions, 1U,
                                   "transition statistics overflow");
            layer_add(&current, &tracker, pack_key(set, last, r), 1U);
        }
    }

    statistics->total_states = current.used;
    statistics->peak_layer_states = current.used;
    statistics->layer_states[2] = current.used;
    statistics->layer_capacities[2] = current.capacity;

    for (int size = 2; size < n; ++size) {
        Layer next = {0};
        layer_init(&next, capacity_for_entries(current.used), &tracker);

        for (size_t slot = 0; slot < current.capacity; ++slot) {
            LayerEntry *entry = &current.entry[slot];
            if (entry->key_plus_one == 0U) continue;

            mask_t set;
            int last;
            unsigned previous_remainder;
            unpack_key(entry->key_plus_one - 1U, &set, &last,
                       &previous_remainder);
            mask_t unused = universe ^ set;
            if (unused == 0U) die("full state appeared before final layer");

            if (use_shortcut && (unsigned)last > half &&
                all_values_greater(unused, last)) {
                checked_stat_increment(&statistics->shortcut_completions, 1U,
                                       "shortcut count overflow");
                checked_stat_increment(&statistics->accumulated_paths,
                                       entry->count,
                                       "answer exceeds uint64_t");
                continue;
            }

            mask_t candidates = unused;
            while (candidates != 0U) {
                int appended = first_value(candidates);
                candidates &= candidates - 1U;
                checked_stat_increment(&statistics->tested_transitions, 1U,
                                       "transition statistics overflow");
                unsigned new_remainder = remainder[last][appended];
                if (previous_remainder > new_remainder) continue;
                checked_stat_increment(&statistics->accepted_transitions, 1U,
                                       "transition statistics overflow");

                mask_t next_set = set | value_bit(appended);
                mask_t next_unused = universe ^ next_set;
                if (use_shortcut && new_remainder > half) {
                    if (all_values_greater(next_unused, appended)) {
                        checked_stat_increment(
                            &statistics->shortcut_completions, 1U,
                            "shortcut count overflow");
                        checked_stat_increment(&statistics->accumulated_paths,
                                               entry->count,
                                               "answer exceeds uint64_t");
                    }
                } else if (next_unused == 0U) {
                    checked_stat_increment(&statistics->accumulated_paths,
                                           entry->count,
                                           "answer exceeds uint64_t");
                } else {
                    layer_add(&next, &tracker,
                              pack_key(next_set, appended, new_remainder),
                              entry->count);
                }
            }
        }

        if (SIZE_MAX - statistics->total_states < next.used)
            die("state statistics overflow");
        statistics->total_states += next.used;
        if (statistics->peak_layer_states < next.used)
            statistics->peak_layer_states = next.used;
        statistics->layer_states[size + 1] = next.used;
        statistics->layer_capacities[size + 1] = next.capacity;
        layer_destroy(&current, &tracker);
        current = next;
    }

    layer_destroy(&current, &tracker);
    if (tracker.current != 0U) die("layer memory was not fully released");
    statistics->peak_memory = tracker.peak;
    return statistics->accumulated_paths;
}

static void verify_known(uint64_t value, int n)
{
    if (n <= KNOWN_MAX_N && value != known[n]) {
        fprintf(stderr, "error: A332800 mismatch at n=%d: got %" PRIu64
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
    int temporary = values[i]; values[i] = values[j]; values[j] = temporary;
    for (int left = i + 1, right = n - 1; left < right; ++left, --right) {
        temporary = values[left]; values[left] = values[right];
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
        for (int i = 0; i + 2 < n; ++i) {
            if (values[i] % values[i + 1] >
                values[i + 1] % values[i + 2]) {
                valid = false;
                break;
            }
        }
        if (valid) ++count;
    } while (next_permutation(values, n));
    return count;
}

static void release_bfile_lock(void)
{
    if (bfile_lock_descriptor < 0) return;
    const struct flock lock = {.l_type = F_UNLCK, .l_whence = SEEK_SET,
                               .l_start = 0, .l_len = 0};
    (void)fcntl(bfile_lock_descriptor, F_SETLK, &lock);
    (void)close(bfile_lock_descriptor);
    bfile_lock_descriptor = -1;
}

static void cleanup_bfile(void)
{
    if (bfile_temp_active) {
        (void)unlink(bfile_temp_name);
        bfile_temp_active = false;
    }
    release_bfile_lock();
}

static FILE *open_bfile(void)
{
    bfile_lock_descriptor = open(BFILE_LOCK_NAME, O_CREAT | O_RDWR, 0666);
    if (bfile_lock_descriptor < 0) die("cannot open b-file lock");
    const struct flock lock = {.l_type = F_WRLCK, .l_whence = SEEK_SET,
                               .l_start = 0, .l_len = 0};
    if (fcntl(bfile_lock_descriptor, F_SETLK, &lock) != 0)
        die("another writer holds the b-file lock");

    int descriptor = mkstemp(bfile_temp_name);
    if (descriptor < 0) die("cannot create temporary b-file");
    bfile_temp_active = true;

    struct stat existing;
    if (stat(BFILE_NAME, &existing) == 0) {
        if (fchmod(descriptor, existing.st_mode & 0777) != 0) {
            (void)close(descriptor);
            die("cannot preserve b-file permissions");
        }
    } else if (errno != ENOENT) {
        (void)close(descriptor);
        die("cannot inspect existing b-file");
    }
    FILE *stream = fdopen(descriptor, "w");
    if (stream == NULL) {
        (void)close(descriptor);
        die("cannot open temporary b-file stream");
    }
    return stream;
}

static void finish_bfile(FILE *stream)
{
    if (fflush(stream) != 0) die("cannot flush temporary b-file");
    int descriptor = fileno(stream);
    if (descriptor < 0 || fsync(descriptor) != 0)
        die("cannot sync temporary b-file");
    if (fclose(stream) != 0) die("cannot close temporary b-file");
    if (rename(bfile_temp_name, BFILE_NAME) != 0)
        die("cannot atomically replace b-file");
    bfile_temp_active = false;
    release_bfile_lock();
}

static void print_statistics(int n, uint64_t value,
                             const Statistics *statistics, double elapsed)
{
    fprintf(stderr,
            "332800_03: n=%d, a(n)=%" PRIu64
            ", states=%zu, peak_layer=%zu, tested=%" PRIu64
            ", accepted=%" PRIu64 ", shortcuts=%" PRIu64
            ", peak_memory=%.1f MiB, %.6f s\n",
            n, value, statistics->total_states,
            statistics->peak_layer_states, statistics->tested_transitions,
            statistics->accepted_transitions,
            statistics->shortcut_completions,
            (double)statistics->peak_memory / 1048576.0, elapsed);
    if (n >= 2) {
        fputs("332800_03: layers", stderr);
        for (int size = 2; size <= n; ++size) {
            fprintf(stderr, " %d:%zu/%zu", size,
                    statistics->layer_states[size],
                    statistics->layer_capacities[size]);
        }
        fputc('\n', stderr);
    }
}

static void usage(FILE *stream, const char *program)
{
    fprintf(stream,
            "Usage: %s [--upto N | --term N | --check] "
            "[--memory-mb MiB] [--no-shortcut] [--verbose]\n",
            program);
}

int main(int argc, char **argv)
{
    OutputMode mode = MODE_UPTO;
    int limit = DEFAULT_MAX_N;
    unsigned memory_mb = DEFAULT_MEMORY_MB;
    bool check = false, verbose = false, mode_seen = false;
    bool use_shortcut = true;

    if (atexit(cleanup_bfile) != 0) die("could not register cleanup");
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--upto") == 0 ||
            strcmp(argv[i], "--term") == 0) {
            if (mode_seen || i + 1 == argc) {
                usage(stderr, argv[0]); return EXIT_FAILURE;
            }
            mode = strcmp(argv[i], "--term") == 0 ? MODE_TERM : MODE_UPTO;
            limit = parse_integer(argv[++i], "n", 0, MAX_N);
            mode_seen = true;
        } else if (strcmp(argv[i], "--check") == 0) {
            if (mode_seen) { usage(stderr, argv[0]); return EXIT_FAILURE; }
            check = true; limit = CHECK_MAX_N; mode_seen = true;
        } else if (strcmp(argv[i], "--memory-mb") == 0) {
            if (i + 1 == argc) { usage(stderr, argv[0]); return EXIT_FAILURE; }
            memory_mb = (unsigned)parse_integer(argv[++i], "memory-mb",
                                                 MIN_MEMORY_MB,
                                                 MAX_MEMORY_MB);
        } else if (strcmp(argv[i], "--verbose") == 0) {
            verbose = true;
        } else if (strcmp(argv[i], "--no-shortcut") == 0) {
            use_shortcut = false;
        } else if (strcmp(argv[i], "--help") == 0 ||
                   strcmp(argv[i], "-h") == 0) {
            usage(stdout, argv[0]); return EXIT_SUCCESS;
        } else {
            fprintf(stderr, "error: unknown option: %s\n", argv[i]);
            usage(stderr, argv[0]); return EXIT_FAILURE;
        }
    }

    if ((size_t)memory_mb > SIZE_MAX / (1024U * 1024U))
        die("memory limit size overflow");
    size_t memory_limit = (size_t)memory_mb * 1024U * 1024U;

    uint64_t values[MAX_N + 1] = {0U};
    Statistics statistics[MAX_N + 1];
    double elapsed[MAX_N + 1] = {0.0};
    memset(statistics, 0, sizeof(statistics));

    int first_n = mode == MODE_TERM ? limit : 0;
    for (int n = first_n; n <= limit; ++n) {
        double started = monotonic_seconds();
        values[n] = compute_term(n, memory_limit, use_shortcut,
                                 &statistics[n]);
        elapsed[n] = monotonic_seconds() - started;
        verify_known(values[n], n);
    }

    if (mode == MODE_TERM) {
        if (verbose)
            print_statistics(limit, values[limit], &statistics[limit],
                             elapsed[limit]);
        printf("%" PRIu64 "\n", values[limit]);
        return EXIT_SUCCESS;
    }

    if (check) {
        for (int n = 0; n <= DIRECT_CHECK_MAX_N; ++n) {
            uint64_t direct = direct_term(n);
            if (direct != values[n]) {
                fprintf(stderr,
                        "error: direct enumeration mismatch at n=%d: "
                        "got %" PRIu64 ", expected %" PRIu64 "\n",
                        n, values[n], direct);
                return EXIT_FAILURE;
            }
        }
        for (int n = 0; n <= SHORTCUT_CHECK_MAX_N; ++n) {
            Statistics unshortened_statistics;
            uint64_t unshortened = compute_term(n, memory_limit, false,
                                                &unshortened_statistics);
            if (unshortened != values[n]) {
                fprintf(stderr,
                        "error: shortcut mismatch at n=%d: got %" PRIu64
                        ", unshortened DP gives %" PRIu64 "\n",
                        n, values[n], unshortened);
                return EXIT_FAILURE;
            }
        }
    }

    if (verbose)
        for (int n = 0; n <= limit; ++n)
            print_statistics(n, values[n], &statistics[n], elapsed[n]);

    FILE *bfile = check ? NULL : open_bfile();
    for (int n = 0; n <= limit; ++n) {
        if (bfile != NULL &&
            fprintf(bfile, "%d %" PRIu64 "\n", n, values[n]) < 0)
            die("cannot write temporary b-file");
        printf("%s%" PRIu64, n == 0 ? "" : ", ", values[n]);
    }
    putchar('\n');
    if (fflush(stdout) != 0) die("cannot flush stdout");
    if (bfile != NULL) finish_bfile(bfile);
    if (check)
        fprintf(stderr,
                "check passed: A332800(0..%d), direct permutations "
                "through n=%d, unshortened DP through n=%d\n",
                CHECK_MAX_N, DIRECT_CHECK_MAX_N, SHORTCUT_CHECK_MAX_N);
    return EXIT_SUCCESS;
}
