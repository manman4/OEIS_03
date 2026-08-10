/*
 * A322180 -- out-of-core ranked-layer DP.
 *
 * Number of permutations of the multiset
 *
 *     {1, 2, ..., n, 1, 2, ..., n}
 *
 * such that at most k entries occur between the two copies of k, for
 * every k=1,...,n.
 *
 * Put the labels down in the fixed order 1,2,...,n (the most constrained
 * pairs first).  A DP state is the bit mask of the occupied positions
 * among the 2*n positions.  When label
 * k is placed at positions i<j, the condition is simply
 *
 *     j - i <= k + 1.
 *
 * Since the label being placed is determined by popcount(mask)/2, the mask
 * is the complete state: no label information has to be stored.  Only two
 * adjacent population-count layers are retained.  Masks in a layer are
 * addressed by their combinatorial-number-system (colexicographic) rank;
 * a split lookup table evaluates that rank in O(1).  Every final labelled
 * placement is reached exactly once, and it is the same thing as one
 * multiset permutation counted by A322180.
 *
 * Unlike 322180_01.c, layer arrays are temporary file-backed mmap regions.
 * The operating system can evict their pages to SSD, so the calculation is
 * not limited by physical RAM.  Masks are scanned in colexicographic rank
 * order; writes to the following layer use an O(1) split rank table.
 * Temporary files are unlinked immediately after mmap, ensuring cleanup on
 * normal exit and after a killed process.
 *
 * Layers through 11 placed labels use uint64_t: the number of labelled
 * pairings of 2*r positions is at most (2*r)!/2^r, which is below 2^64 for
 * r<=11.  Later layers use unsigned __int128.  At n=16 the peak pair of
 * temporary layers occupies about 7.99 GiB of disk.  A free-space preflight
 * is performed before computation.  The default computes n=0..9.
 *
 * Operational safety:
 *   - all sizes, input ranges, and coefficient additions are checked;
 *   - mkstemp creates private files and no existing path is overwritten;
 *   - temporary files are unlinked immediately and are reclaimed by the OS
 *     after normal exit, an error, SIGINT, or forced termination;
 *   - an existing b-file is validated, rewritten to a private temporary,
 *     fsync'ed, and atomically replaced only after a completed term;
 *   - the final term is printed only after every layer completes;
 *   - no network access, subprocess, or persistent external state is used.
 * A 1-GiB reserve is required beyond the calculated peak.  Concurrent disk
 * consumption can still make an mmap write fail, but cannot damage an
 * existing file because all writes target private unlinked temporary files.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic -pthread \
 *       322180_03.c -o 322180_03
 *
 * Usage:
 *   ./322180_03 --term 16
 *   ./322180_03 16
 *   ./322180_03 --check
 *   ./322180_03 --term 16 --work-dir /path/with/free/space
 * Results are atomically recorded in b322180_03.txt by default.  Use
 * --output FILE to select another b-file or --no-bfile to disable writing.
 */

#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <limits.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <time.h>
#include <unistd.h>

#if !defined(__SIZEOF_INT128__)
#error "322180_03.c requires unsigned __int128"
#endif

__extension__ typedef unsigned __int128 U128;

#define MAX_N 16
#define DEFAULT_N 9
#define MAX_THREADS 16
#define DEFAULT_CHECK_N 10
#define SPACE_RESERVE (UINT64_C(1024) * 1024 * 1024)

typedef struct {
    void *values;
    uint64_t length;
    size_t bytes;
    int fd;
    bool wide;
} Layer;

static const char *work_directory = ".";
static const char *output_path = "b322180_03.txt";
static int requested_threads = 4;
static bool write_bfile = true;

typedef struct {
    unsigned low_bits;
    uint64_t low_mask;
    uint64_t *low_rank;
    uint64_t *high_rank;
    uint8_t *low_count;
    uint64_t high_states;
} RankTable;

static const char *const known[] = {
    "1", "1", "5", "36", "466", "8942", "240366", "8576860",
    "392952468", "22470271108", "1568892808268",
    "131334875308512", "12984315462058432",
    "1496760634095487312", "198986563111871018384",
    "30220934753050481089120", "5199918150481843712777536"
};

static void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static int parse_n(const char *text)
{
    char *end = NULL;
    errno = 0;
    const long value = strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' ||
        value < 0 || value > MAX_N) {
        fprintf(stderr, "error: N must be in 0..%d: %s\n", MAX_N, text);
        exit(EXIT_FAILURE);
    }
    return (int)value;
}

static int parse_threads(const char *text)
{
    char *end = NULL;
    errno = 0;
    const long value = strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' ||
        value < 1 || value > MAX_THREADS) {
        fprintf(stderr, "error: threads must be in 1..%d: %s\n",
                MAX_THREADS, text);
        exit(EXIT_FAILURE);
    }
    return (int)value;
}

static int print_u128(FILE *stream, U128 value)
{
    char digits[40];
    size_t length = 0;

    do {
        digits[length++] = (char)('0' + (unsigned)(value % 10));
        value /= 10;
    } while (value != 0);

    while (length != 0) {
        if (fputc(digits[--length], stream) == EOF) {
            return -1;
        }
    }
    return 0;
}

static bool parse_u128(const char *text, U128 *result)
{
    const U128 maximum = ~(U128)0;
    U128 value = 0;

    if (*text == '\0') {
        return false;
    }
    for (; *text != '\0'; ++text) {
        if (*text < '0' || *text > '9') {
            return false;
        }
        const unsigned digit = (unsigned)(*text - '0');
        if (value > (maximum - digit) / 10) {
            return false;
        }
        value = value * 10 + digit;
    }
    *result = value;
    return true;
}

static void store_bfile_term(int n, U128 value)
{
    U128 values[MAX_N + 1];
    int count = 0;
    mode_t output_mode = 0644;
    struct stat metadata;
    if (stat(output_path, &metadata) == 0) {
        output_mode = metadata.st_mode & 0777;
    } else if (errno != ENOENT) {
        die("cannot inspect b-file");
    }

    FILE *input = fopen(output_path, "r");
    if (input == NULL && errno != ENOENT) {
        die("cannot open existing b-file");
    }
    if (input != NULL) {
        char line[128];
        while (fgets(line, sizeof(line), input) != NULL) {
            int index;
            char number[64];
            char extra;
            if (count > MAX_N ||
                sscanf(line, "%d %63s %c", &index, number, &extra) != 2 ||
                index != count || !parse_u128(number, &values[count])) {
                fclose(input);
                die("existing b-file is malformed or nonconsecutive");
            }
            ++count;
        }
        if (ferror(input) || fclose(input) != 0) {
            die("cannot read existing b-file");
        }
    }

    if (n < count) {
        if (values[n] != value) {
            die("computed term disagrees with existing b-file");
        }
        return;
    }
    if (n != count) {
        die("b-file has a gap; compute the missing earlier terms first");
    }
    values[count++] = value;

    const char suffix[] = ".tmp.XXXXXX";
    const size_t path_length = strlen(output_path);
    if (path_length > SIZE_MAX - sizeof(suffix)) {
        die("b-file path is too long");
    }
    char *temporary = malloc(path_length + sizeof(suffix));
    if (temporary == NULL) {
        die("cannot allocate b-file temporary path");
    }
    memcpy(temporary, output_path, path_length);
    memcpy(temporary + path_length, suffix, sizeof(suffix));

    const int fd = mkstemp(temporary);
    if (fd < 0) {
        free(temporary);
        die("cannot create temporary b-file");
    }
    if (fchmod(fd, output_mode) != 0) {
        close(fd);
        unlink(temporary);
        free(temporary);
        die("cannot set temporary b-file permissions");
    }
    FILE *output = fdopen(fd, "w");
    if (output == NULL) {
        close(fd);
        unlink(temporary);
        free(temporary);
        die("cannot open temporary b-file stream");
    }
    bool failed = false;
    for (int index = 0; index < count; ++index) {
        if (fprintf(output, "%d ", index) < 0 ||
            print_u128(output, values[index]) != 0 ||
            fputc('\n', output) == EOF) {
            failed = true;
            break;
        }
    }
    if (!failed && fflush(output) != 0) {
        failed = true;
    }
    if (!failed && fsync(fd) != 0) {
        failed = true;
    }
    if (fclose(output) != 0) {
        failed = true;
    }
    if (failed) {
        unlink(temporary);
        free(temporary);
        die("cannot write temporary b-file");
    }
    if (rename(temporary, output_path) != 0) {
        unlink(temporary);
        free(temporary);
        die("cannot atomically replace b-file");
    }
    free(temporary);
    fprintf(stderr, "322180_03: updated %s through n=%d\n",
            output_path, n);
}

static bool add_u128(U128 *destination, U128 addend)
{
    const U128 old = *destination;
    *destination += addend;
    return *destination >= old;
}

static bool add_u64(uint64_t *destination, uint64_t addend)
{
    if (UINT64_MAX - *destination < addend) {
        return false;
    }
    *destination += addend;
    return true;
}

/* Return the next mask of the same population count (Gosper's hack). */
static uint64_t next_combination(uint64_t mask)
{
    const uint64_t lowest = mask & (UINT64_C(0) - mask);
    const uint64_t ripple = mask + lowest;
    return ripple | (((ripple ^ mask) >> 2) / lowest);
}

static uint64_t binomial[2 * MAX_N + 1][2 * MAX_N + 1];

static void initialize_binomial(void)
{
    for (unsigned n = 0; n <= 2 * MAX_N; ++n) {
        binomial[n][0] = binomial[n][n] = 1;
        for (unsigned k = 1; k < n; ++k) {
            binomial[n][k] = binomial[n - 1][k - 1] +
                             binomial[n - 1][k];
        }
    }
}

static RankTable make_rank_table(unsigned positions)
{
    RankTable table;
    memset(&table, 0, sizeof(table));
    table.low_bits = positions / 2;
    const unsigned high_bits = positions - table.low_bits;
    const uint64_t low_states = UINT64_C(1) << table.low_bits;
    table.high_states = UINT64_C(1) << high_bits;
    table.low_mask = low_states - 1;

    table.low_rank = malloc((size_t)low_states * sizeof(*table.low_rank));
    table.low_count = malloc((size_t)low_states * sizeof(*table.low_count));
    if (table.high_states > SIZE_MAX /
        ((size_t)(table.low_bits + 1) * sizeof(*table.high_rank))) {
        die("rank lookup size overflow");
    }
    table.high_rank = malloc((size_t)(table.low_bits + 1) *
                             (size_t)table.high_states *
                             sizeof(*table.high_rank));
    if (table.low_rank == NULL || table.low_count == NULL ||
        table.high_rank == NULL) {
        free(table.low_rank);
        free(table.low_count);
        free(table.high_rank);
        die("cannot allocate rank lookup tables");
    }

    for (uint64_t mask = 0; mask < low_states; ++mask) {
        uint64_t bits = mask;
        unsigned ordinal = 1;
        uint64_t rank = 0;
        while (bits != 0) {
            const unsigned position = (unsigned)__builtin_ctzll(bits);
            bits &= bits - 1;
            rank += binomial[position][ordinal++];
        }
        table.low_rank[mask] = rank;
        table.low_count[mask] = (uint8_t)(ordinal - 1);
    }

    for (unsigned offset = 0; offset <= table.low_bits; ++offset) {
        uint64_t *row = table.high_rank +
                        (size_t)offset * (size_t)table.high_states;
        for (uint64_t mask = 0; mask < table.high_states; ++mask) {
            uint64_t bits = mask;
            unsigned ordinal = offset + 1;
            uint64_t rank = 0;
            while (bits != 0) {
                const unsigned position = table.low_bits +
                    (unsigned)__builtin_ctzll(bits);
                bits &= bits - 1;
                rank += binomial[position][ordinal++];
            }
            row[mask] = rank;
        }
    }
    return table;
}

static void free_rank_table(RankTable *table)
{
    free(table->low_rank);
    free(table->low_count);
    free(table->high_rank);
    memset(table, 0, sizeof(*table));
}

static uint64_t mask_rank(const RankTable *table, uint64_t mask)
{
    const uint64_t low = mask & table->low_mask;
    const uint64_t high = mask >> table->low_bits;
    const unsigned offset = table->low_count[low];
    return table->low_rank[low] +
           table->high_rank[(size_t)offset * (size_t)table->high_states +
                            high];
}

static double now_seconds(void)
{
    struct timespec time;
    if (clock_gettime(CLOCK_MONOTONIC, &time) != 0) {
        die("clock_gettime failed");
    }
    return (double)time.tv_sec + (double)time.tv_nsec / 1e9;
}

static size_t layer_bytes(unsigned positions, unsigned occupied)
{
    const uint64_t length = binomial[positions][occupied];
    const size_t element_size = occupied / 2 >= 12 ?
                                sizeof(U128) : sizeof(uint64_t);
    if (length > SIZE_MAX / element_size) {
        die("DP layer size overflow");
    }
    return (size_t)length * element_size;
}

static size_t peak_disk_bytes(unsigned positions)
{
    size_t peak = 0;
    for (unsigned placed = 0; placed < positions / 2; ++placed) {
        const size_t current = layer_bytes(positions, 2 * placed);
        const size_t next = layer_bytes(positions, 2 * placed + 2);
        if (current > SIZE_MAX - next) {
            die("peak disk size overflow");
        }
        if (current + next > peak) {
            peak = current + next;
        }
    }
    return peak;
}

static void check_disk_space(unsigned positions)
{
    struct statvfs status;
    if (statvfs(work_directory, &status) != 0) {
        die("cannot inspect work-directory free space");
    }
    const U128 available = (U128)status.f_bavail * status.f_frsize;
    const size_t peak = peak_disk_bytes(positions);
    const U128 required = (U128)peak + SPACE_RESERVE;
    fprintf(stderr,
            "322180_03: peak temporary disk %.2f GiB; available %.2f GiB\n",
            (double)peak / 1073741824.0,
            (double)available / 1073741824.0);
    if (available < required) {
        die("insufficient free space in --work-dir");
    }
}

static Layer make_layer(unsigned positions, unsigned occupied)
{
    Layer layer;
    memset(&layer, 0, sizeof(layer));
    layer.fd = -1;
    layer.length = binomial[positions][occupied];
    layer.wide = occupied / 2 >= 12;
    layer.bytes = layer_bytes(positions, occupied);

    const size_t directory_length = strlen(work_directory);
    const char suffix[] = "/.322180_03_layer_XXXXXX";
    if (directory_length > SIZE_MAX - sizeof(suffix)) {
        die("work-directory path is too long");
    }
    char *path = malloc(directory_length + sizeof(suffix));
    if (path == NULL) {
        die("cannot allocate temporary path");
    }
    memcpy(path, work_directory, directory_length);
    memcpy(path + directory_length, suffix, sizeof(suffix));
    layer.fd = mkstemp(path);
    if (layer.fd < 0) {
        free(path);
        die("cannot create temporary layer");
    }
    if (ftruncate(layer.fd, (off_t)layer.bytes) != 0) {
        unlink(path);
        close(layer.fd);
        free(path);
        die("cannot size temporary layer");
    }
    layer.values = mmap(NULL, layer.bytes, PROT_READ | PROT_WRITE,
                        MAP_SHARED, layer.fd, 0);
    if (layer.values == MAP_FAILED) {
        unlink(path);
        close(layer.fd);
        free(path);
        die("cannot mmap temporary layer");
    }
    if (unlink(path) != 0) {
        munmap(layer.values, layer.bytes);
        close(layer.fd);
        free(path);
        die("cannot unlink temporary layer");
    }
    free(path);
#if defined(MADV_RANDOM)
    (void)madvise(layer.values, layer.bytes, MADV_RANDOM);
#endif
    return layer;
}

static void destroy_layer(Layer *layer)
{
    if (layer->values != NULL && layer->values != MAP_FAILED) {
        if (munmap(layer->values, layer->bytes) != 0) {
            die("cannot unmap temporary layer");
        }
    }
    if (layer->fd >= 0 && close(layer->fd) != 0) {
        die("cannot close temporary layer");
    }
    memset(layer, 0, sizeof(*layer));
    layer->fd = -1;
}

static U128 layer_value(const Layer *layer, uint64_t index)
{
    if (layer->wide) {
        return ((const U128 *)layer->values)[index];
    }
    return ((const uint64_t *)layer->values)[index];
}

typedef struct {
    const Layer *current;
    Layer *next;
    const RankTable *ranks;
    uint64_t full;
    uint64_t begin;
    uint64_t end;
    uint64_t nonzero;
    uint64_t transitions;
    unsigned positions;
    unsigned occupied;
    int label;
    bool atomic_writes;
    int failed;
} TransitionTask;

static uint64_t unrank_combination(unsigned positions, unsigned count,
                                   uint64_t rank)
{
    uint64_t mask = 0;
    unsigned past_position = positions;
    for (unsigned ordinal = count; ordinal != 0; --ordinal) {
        unsigned position = past_position - 1;
        while (binomial[position][ordinal] > rank) {
            --position;
        }
        mask |= UINT64_C(1) << position;
        rank -= binomial[position][ordinal];
        past_position = position;
    }
    if (rank != 0) {
        die("combination unrank failure");
    }
    return mask;
}

static void *process_transition_range(void *argument)
{
    TransitionTask *task = argument;
    uint64_t mask = task->occupied == 0 ? 0 :
        unrank_combination(task->positions, task->occupied, task->begin);

    for (uint64_t current_index = task->begin;
         current_index < task->end; ++current_index) {
        const U128 ways = layer_value(task->current, current_index);
        if (ways != 0) {
            ++task->nonzero;
            const uint64_t available = task->full ^ mask;
            for (unsigned i = 0; i + 1 < task->positions; ++i) {
                const uint64_t first = UINT64_C(1) << i;
                if ((available & first) == 0) {
                    continue;
                }
                const unsigned last =
                    i + (unsigned)task->label + 1 < task->positions ?
                    i + (unsigned)task->label + 1 : task->positions - 1;
                const uint64_t above_first =
                    task->full & ~((UINT64_C(1) << (i + 1)) - 1);
                const uint64_t through_last =
                    last + 1 == task->positions ? task->full :
                    (UINT64_C(1) << (last + 1)) - 1;
                uint64_t seconds =
                    available & above_first & through_last;
                while (seconds != 0) {
                    const uint64_t second =
                        seconds & (UINT64_C(0) - seconds);
                    seconds ^= second;
                    const uint64_t next_mask = mask | first | second;
                    const uint64_t next_index =
                        mask_rank(task->ranks, next_mask);
                    if (task->atomic_writes) {
                        const uint64_t addend = (uint64_t)ways;
                        const uint64_t old = __atomic_fetch_add(
                            &((uint64_t *)task->next->values)[next_index],
                            addend, __ATOMIC_RELAXED);
                        if (UINT64_MAX - old < addend) {
                            task->failed = 1;
                            return NULL;
                        }
                    } else if (task->next->wide) {
                        if (!add_u128(
                                &((U128 *)task->next->values)[next_index],
                                ways)) {
                            task->failed = 1;
                            return NULL;
                        }
                    } else {
                        if (ways > UINT64_MAX ||
                            !add_u64(
                                &((uint64_t *)task->next->values)[next_index],
                                (uint64_t)ways)) {
                            task->failed = 1;
                            return NULL;
                        }
                    }
                    ++task->transitions;
                }
            }
        }
        if (current_index + 1 < task->end) {
            mask = next_combination(mask);
        }
    }
    return NULL;
}

static void process_layer(const Layer *current, Layer *next,
                          const RankTable *ranks, uint64_t full,
                          unsigned positions, unsigned occupied, int label,
                          uint64_t *nonzero, uint64_t *transitions,
                          int *used_threads)
{
    int threads = requested_threads;
    if (next->wide || current->length < 10000) {
        threads = 1;
    }
    if ((uint64_t)threads > current->length) {
        threads = (int)current->length;
    }
    TransitionTask *tasks = calloc((size_t)threads, sizeof(*tasks));
    pthread_t *ids = calloc((size_t)threads, sizeof(*ids));
    if (tasks == NULL || ids == NULL) {
        free(tasks);
        free(ids);
        die("cannot allocate transition workers");
    }

    for (int id = 0; id < threads; ++id) {
        TransitionTask *task = &tasks[id];
        task->current = current;
        task->next = next;
        task->ranks = ranks;
        task->full = full;
        task->begin = current->length * (uint64_t)id / (uint64_t)threads;
        task->end = current->length * (uint64_t)(id + 1) /
                    (uint64_t)threads;
        task->positions = positions;
        task->occupied = occupied;
        task->label = label;
        task->atomic_writes = threads > 1;
        if (threads == 1) {
            process_transition_range(task);
        } else {
            const int error = pthread_create(&ids[id], NULL,
                                             process_transition_range, task);
            if (error != 0) {
                fprintf(stderr, "error: pthread_create: %s\n",
                        strerror(error));
                exit(EXIT_FAILURE);
            }
        }
    }

    *nonzero = 0;
    *transitions = 0;
    for (int id = 0; id < threads; ++id) {
        if (threads > 1) {
            const int error = pthread_join(ids[id], NULL);
            if (error != 0) {
                fprintf(stderr, "error: pthread_join: %s\n",
                        strerror(error));
                exit(EXIT_FAILURE);
            }
        }
        if (tasks[id].failed) {
            die("DP coefficient overflow");
        }
        if (UINT64_MAX - *nonzero < tasks[id].nonzero ||
            UINT64_MAX - *transitions < tasks[id].transitions) {
            die("statistics overflow");
        }
        *nonzero += tasks[id].nonzero;
        *transitions += tasks[id].transitions;
    }
    *used_threads = threads;
    free(tasks);
    free(ids);
}

static U128 a322180(int n)
{
    if (n == 0) {
        return 1;
    }

    const unsigned positions = 2U * (unsigned)n;
    const uint64_t full = (UINT64_C(1) << positions) - 1;
    check_disk_space(positions);
    RankTable ranks = make_rank_table(positions);
    Layer current = make_layer(positions, 0);
    ((uint64_t *)current.values)[0] = 1;

    for (unsigned placed = 0; placed < (unsigned)n; ++placed) {
        const double started = now_seconds();
        const int label = (int)placed + 1;
        const unsigned occupied = 2 * placed;
        Layer next = make_layer(positions, occupied + 2);
#if defined(MADV_SEQUENTIAL)
        (void)madvise(current.values, current.bytes, MADV_SEQUENTIAL);
#endif

        uint64_t nonzero = 0;
        uint64_t transitions = 0;
        int used_threads = 1;
        process_layer(&current, &next, &ranks, full, positions,
                      occupied, label, &nonzero, &transitions,
                      &used_threads);

        destroy_layer(&current);
        current = next;
        fprintf(stderr,
                "322180_03: n=%d, label=%d, states=%" PRIu64
                ", transitions=%" PRIu64
                ", threads=%d, next=%.2f GiB, %.3f s\n",
                n, label, nonzero, transitions,
                used_threads,
                (double)current.bytes / 1073741824.0,
                now_seconds() - started);
    }

    const U128 answer = layer_value(&current, 0);
    destroy_layer(&current);
    free_rank_table(&ranks);
    return answer;
}

static void verify_known(int n, U128 value)
{
    const int known_count = (int)(sizeof(known) / sizeof(known[0]));
    if (n >= known_count) {
        return;
    }

    U128 expected;
    if (!parse_u128(known[n], &expected)) {
        die("invalid built-in known term");
    }
    if (value != expected) {
        fprintf(stderr, "error: A322180 mismatch at n=%d: got ", n);
        print_u128(stderr, value);
        fprintf(stderr, ", expected %s\n", known[n]);
        exit(EXIT_FAILURE);
    }
}

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s [MAX_N] [--threads T] [--work-dir DIR] [--output FILE]\n"
            "       %s --term N [--threads T] [--work-dir DIR] [--output FILE]\n"
            "       %s --check [--threads T] [--work-dir DIR] [--no-bfile]\n"
            "N must be in 0..%d; T must be in 1..%d.\n",
            program, program, program, MAX_N, MAX_THREADS);
}

int main(int argc, char **argv)
{
    bool term_mode = false;
    bool check_mode = false;
    int maximum = DEFAULT_N;

    initialize_binomial();

    bool have_mode = false;
    bool have_work_directory = false;
    bool have_threads = false;
    bool have_output_option = false;
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")) {
            usage(argv[0]);
            return EXIT_SUCCESS;
        }
        if (!strcmp(argv[i], "--work-dir")) {
            if (have_work_directory || ++i >= argc) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            work_directory = argv[i];
            have_work_directory = true;
        } else if (!strcmp(argv[i], "--threads")) {
            if (have_threads || ++i >= argc) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            requested_threads = parse_threads(argv[i]);
            have_threads = true;
        } else if (!strcmp(argv[i], "--output")) {
            if (have_output_option || ++i >= argc) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            output_path = argv[i];
            write_bfile = true;
            have_output_option = true;
        } else if (!strcmp(argv[i], "--no-bfile")) {
            if (have_output_option) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            write_bfile = false;
            have_output_option = true;
        } else if (!strcmp(argv[i], "--check")) {
            if (have_mode) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            check_mode = true;
            have_mode = true;
        } else if (!strcmp(argv[i], "--term")) {
            if (have_mode || ++i >= argc) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            term_mode = true;
            maximum = parse_n(argv[i]);
            have_mode = true;
        } else if (argv[i][0] != '-' && !have_mode) {
            maximum = parse_n(argv[i]);
            have_mode = true;
        } else {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (check_mode) {
        for (int n = 0; n <= DEFAULT_CHECK_N; ++n) {
            verify_known(n, a322180(n));
        }
        printf("ok: A322180 terms n=0..%d verified\n", DEFAULT_CHECK_N);
        return EXIT_SUCCESS;
    }

    if (term_mode) {
        const U128 value = a322180(maximum);
        verify_known(maximum, value);
        if (write_bfile) {
            store_bfile_term(maximum, value);
        }
        printf("%d ", maximum);
        print_u128(stdout, value);
        putchar('\n');
        return EXIT_SUCCESS;
    }

    for (int n = 0; n <= maximum; ++n) {
        const U128 value = a322180(n);
        verify_known(n, value);
        if (write_bfile) {
            store_bfile_term(n, value);
        }
        printf("%d ", n);
        print_u128(stdout, value);
        putchar('\n');
    }
    return EXIT_SUCCESS;
}
