/*
 * A395620 -- exact out-of-core bidirectional DP.
 *
 * Count permutations s_1,...,s_n of [n] whose nonempty prefix sums are
 * never perfect squares.  Put good(S) := "sum(S) is not a perfect square"
 * and choose m=floor(n/2).  Two recurrences meet at |S|=m:
 *
 *   F(empty)=1,
 *   F(S)=good(S) * Sum_{x in S} F(S-{x}),
 *
 * and, for the unused set C=[n]\S,
 *
 *   R(empty)=1,
 *   R(C)=Sum_{x in C, good([n]-(C-{x}))} R(C-{x}).
 *
 * Every full permutation has one unique middle set, hence
 *
 *   a(n) = Sum_{|S|=m} F(S) R([n]\S).
 *
 * This differs from 395620_02.c, which propagates one in-memory DP from
 * empty all the way to [n].  Program 04 builds independent forward and
 * reverse sides and joins complementary states at a fixed middle cut.
 *
 * Unlike 395620_03.c, no complete large layer is allocated on the heap.
 * Every layer is an unlinked temporary file.  The previous layer is mapped
 * read-only and the next layer is produced in fixed-size colex-rank blocks.
 * The middle join streams the forward frontier while mapping the reverse
 * frontier.  Explicit heap use is therefore bounded by one block; clean
 * file-backed pages remain reclaimable by the operating system.  At n=34
 * the peak live temporary-file payload is about 47.64 GiB, so at least
 * about 55 GiB of free disk space is recommended.
 *
 * At the middle cut every forward or reverse count is at most 17!, which
 * fits in uint64_t.  Exact 64-bit values are therefore stored in the layer
 * files.  Products are accumulated in a checked 256-bit integer; the final
 * answer is at most 34! < 2^128.  This removes all seventeen CRT passes from
 * program 03.  Runtime is still O(n*2^n), so n=34 remains a long,
 * disk-intensive computation.
 *
 * Cardinality blocks are evaluated by pthread workers.  A395620_THREADS
 * controls the count (default: online CPU count capped at 8).
 * A395620_MEMORY_MIB controls the heap block budget (default: 256 MiB).
 *
 * Known terms a(0)..a(27) are built in.  Every newly completed term is saved
 * atomically in b395620_04.txt, allowing interrupted --upto runs to resume.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic -pthread \
 *       395620_04.c -o 395620_04
 *
 * Examples:
 *   ./395620_04 --term 31
 *   ./395620_04 --upto 33
 *   ./395620_04 --term 34
 *   ./395620_04 --check
 */

#define _POSIX_C_SOURCE 200809L
#ifdef __APPLE__
#define _DARWIN_C_SOURCE
#endif

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
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
#error "395620_04.c requires unsigned __int128"
#endif

#if SIZE_MAX < UINT64_MAX
#error "395620_04.c requires a 64-bit size_t"
#endif

__extension__ typedef unsigned __int128 U128;

#define MAX_N 34U
#define DEFAULT_MAX_N 31U
#define KNOWN_MAX_N 27U
#define DIRECT_CHECK_MAX_N 10U
#define MAX_PREFIX_SUM (MAX_N * (MAX_N + 1U) / 2U)
#define MAX_THREADS 64U
#define DEFAULT_MEMORY_MIB UINT64_C(256)
#define MIN_MEMORY_MIB UINT64_C(64)
#define MAX_MEMORY_MIB UINT64_C(65536)
#define REPORT_MIN_STATES UINT64_C(134217728)
#define TARGET_BLOCK_BYTES UINT64_C(67108864)
#define BIG_LIMBS 4U
#define BIG_TEXT_SIZE 80U
#define BFILE_PATH "b395620_04.txt"
#define LOCK_PATH "b395620_04.txt.lock"

_Static_assert(MAX_N <= 34U,
               "the exact layer-cell bound relies on floor(MAX_N/2) <= 17");

typedef struct {
    uint64_t limb[BIG_LIMBS];
} U256;

typedef enum {
    MODE_UPTO,
    MODE_TERM,
    MODE_CHECK
} Mode;

typedef enum {
    DIRECTION_FORWARD,
    DIRECTION_REVERSE
} Direction;

typedef struct {
    int descriptor;
    uint64_t count;
} DiskLayer;

static uint64_t binomial[MAX_N + 1U][MAX_N + 1U];
static bool perfect_square_sum[MAX_PREFIX_SUM + 1U];
static bool tables_ready;
static uint64_t memory_limit;
static unsigned configured_threads;
static bool quiet;

static const char *const known_terms[KNOWN_MAX_N + 1U] = {
    "1", "0", "1", "3", "6", "46", "220", "1540", "0",
    "96640", "1003308", "11577332", "150147292", "1804726032",
    "24933668616", "403256334824", "5514533512944",
    "96303188068728", "1748437769928368", "34553805131115392",
    "670580814038650976", "14155521644833590976",
    "323811356025442369136", "7129269277696485742416",
    "172625645396753955332960", "4091834434892742591232864",
    "109483439018513119215194624", "2903413384312431737278567648"
};

static _Noreturn void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static double monotonic_seconds(void)
{
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) != 0) {
        die("clock_gettime failed");
    }
    return (double)now.tv_sec + (double)now.tv_nsec / 1000000000.0;
}

static unsigned parse_unsigned(const char *text, unsigned maximum,
                               const char *label)
{
    char *end = NULL;
    errno = 0;
    const unsigned long value = strtoul(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' || value > maximum) {
        fprintf(stderr, "error: %s must be in 0..%u: %s\n",
                label, maximum, text);
        exit(EXIT_FAILURE);
    }
    return (unsigned)value;
}

static void configure_resources(void)
{
    uint64_t mib = DEFAULT_MEMORY_MIB;
    const char *memory_text = getenv("A395620_MEMORY_MIB");
    if (memory_text != NULL && *memory_text != '\0') {
        char *end = NULL;
        errno = 0;
        const unsigned long long parsed =
            strtoull(memory_text, &end, 10);
        if (errno != 0 || end == memory_text || *end != '\0' ||
            parsed < MIN_MEMORY_MIB || parsed > MAX_MEMORY_MIB) {
            fprintf(stderr,
                    "error: A395620_MEMORY_MIB must be in %" PRIu64
                    "..%" PRIu64 ": %s\n",
                    MIN_MEMORY_MIB, MAX_MEMORY_MIB, memory_text);
            exit(EXIT_FAILURE);
        }
        mib = (uint64_t)parsed;
    }
    memory_limit = mib << 20U;

    long online = sysconf(_SC_NPROCESSORS_ONLN);
    configured_threads = online > 0 ? (unsigned)online : 1U;
    if (configured_threads > 8U) {
        configured_threads = 8U;
    }
    const char *thread_text = getenv("A395620_THREADS");
    if (thread_text != NULL && *thread_text != '\0') {
        configured_threads =
            parse_unsigned(thread_text, MAX_THREADS, "A395620_THREADS");
        if (configured_threads == 0U) {
            die("A395620_THREADS must be at least 1");
        }
    }
}

static bool big_is_zero(const U256 *value)
{
    uint64_t combined = 0U;
    for (unsigned i = 0U; i < BIG_LIMBS; ++i) {
        combined |= value->limb[i];
    }
    return combined == 0U;
}

static int big_compare(const U256 *left, const U256 *right)
{
    for (unsigned i = BIG_LIMBS; i-- > 0U;) {
        if (left->limb[i] < right->limb[i]) {
            return -1;
        }
        if (left->limb[i] > right->limb[i]) {
            return 1;
        }
    }
    return 0;
}

static void big_multiply_small(U256 *value, uint32_t factor)
{
    U128 carry = 0U;
    for (unsigned i = 0U; i < BIG_LIMBS; ++i) {
        const U128 product = (U128)value->limb[i] * factor + carry;
        value->limb[i] = (uint64_t)product;
        carry = product >> 64U;
    }
    if (carry != 0U) {
        die("internal 256-bit multiplication overflow");
    }
}

static void big_add_product(U256 *target, const U256 *value,
                            uint32_t factor)
{
    U128 carry = 0U;
    for (unsigned i = 0U; i < BIG_LIMBS; ++i) {
        const U128 sum = (U128)value->limb[i] * factor +
                         target->limb[i] + carry;
        target->limb[i] = (uint64_t)sum;
        carry = sum >> 64U;
    }
    if (carry != 0U) {
        die("internal 256-bit addition overflow");
    }
}

static void big_checked_add(U256 *target, const U256 *addition)
{
    uint64_t carry = 0U;
    for (unsigned i = 0U; i < BIG_LIMBS; ++i) {
        const uint64_t first = target->limb[i] + addition->limb[i];
        const uint64_t carry_first = first < target->limb[i];
        const uint64_t second = first + carry;
        const uint64_t carry_second = second < first;
        target->limb[i] = second;
        carry = carry_first | carry_second;
    }
    if (carry != 0U) {
        die("internal 256-bit addition overflow");
    }
}

static void big_add_u64_product(U256 *target, uint64_t left,
                                uint64_t right)
{
    const U128 product = (U128)left * right;
    const U256 addition = {{
        (uint64_t)product, (uint64_t)(product >> 64U), 0U, 0U
    }};
    big_checked_add(target, &addition);
}

static uint32_t big_divide_small(U256 *value, uint32_t divisor)
{
    uint64_t remainder = 0U;
    for (unsigned i = BIG_LIMBS; i-- > 0U;) {
        const U128 dividend = ((U128)remainder << 64U) | value->limb[i];
        value->limb[i] = (uint64_t)(dividend / divisor);
        remainder = (uint64_t)(dividend % divisor);
    }
    return (uint32_t)remainder;
}

static U256 big_factorial(unsigned n)
{
    U256 result = {{1U, 0U, 0U, 0U}};
    for (unsigned factor = 2U; factor <= n; ++factor) {
        big_multiply_small(&result, factor);
    }
    return result;
}

static bool parse_u256(const char *text, U256 *result)
{
    U256 value = {{0U, 0U, 0U, 0U}};
    const U256 one = {{1U, 0U, 0U, 0U}};
    if (*text == '\0') {
        return false;
    }
    while (*text != '\0') {
        if (*text < '0' || *text > '9') {
            return false;
        }
        const unsigned digit = (unsigned)(*text++ - '0');
        big_multiply_small(&value, 10U);
        big_add_product(&value, &one, digit);
    }
    *result = value;
    return true;
}

static void u256_to_text(const U256 *value, char text[BIG_TEXT_SIZE])
{
    uint32_t chunks[10];
    size_t count = 0U;
    U256 copy = *value;
    do {
        if (count == sizeof(chunks) / sizeof(chunks[0])) {
            die("internal decimal conversion overflow");
        }
        chunks[count++] = big_divide_small(&copy, UINT32_C(1000000000));
    } while (!big_is_zero(&copy));

    int written = snprintf(text, BIG_TEXT_SIZE, "%" PRIu32,
                           chunks[count - 1U]);
    if (written < 0 || (size_t)written >= BIG_TEXT_SIZE) {
        die("cannot format 256-bit integer");
    }
    size_t used = (size_t)written;
    while (--count != 0U) {
        written = snprintf(text + used, BIG_TEXT_SIZE - used,
                           "%09" PRIu32, chunks[count - 1U]);
        if (written != 9 || used + (size_t)written >= BIG_TEXT_SIZE) {
            die("cannot format 256-bit integer");
        }
        used += (size_t)written;
    }
}

static void print_u256(FILE *stream, const U256 *value)
{
    char text[BIG_TEXT_SIZE];
    u256_to_text(value, text);
    if (fputs(text, stream) == EOF) {
        die("cannot write output");
    }
}

static void prepare_tables(void)
{
    if (tables_ready) {
        return;
    }
    binomial[0][0] = 1U;
    for (unsigned n = 1U; n <= MAX_N; ++n) {
        binomial[n][0] = 1U;
        binomial[n][n] = 1U;
        for (unsigned k = 1U; k < n; ++k) {
            binomial[n][k] =
                binomial[n - 1U][k - 1U] + binomial[n - 1U][k];
        }
    }
    /* Prefix sums are positive, so mark exactly 1^2, 2^2, ... . */
    for (unsigned root = 1U; root <= MAX_PREFIX_SUM / root; ++root) {
        perfect_square_sum[root * root] = true;
    }
    tables_ready = true;
}

static uint64_t next_combination(uint64_t mask)
{
    const uint64_t low_bit = mask & (UINT64_C(0) - mask);
    if (low_bit == 0U) {
        die("internal zero combination mask");
    }
    const uint64_t ripple = mask + low_bit;
    return ripple | (((mask ^ ripple) >> 2U) / low_bit);
}

static uint64_t combination_from_rank(unsigned n, unsigned cardinality,
                                      uint64_t rank)
{
    uint64_t mask = 0U;
    unsigned upper = n;
    for (unsigned i = cardinality; i > 0U; --i) {
        unsigned bit = upper - 1U;
        while (binomial[bit][i] > rank) {
            if (bit == 0U) {
                die("internal combinatorial unranking error");
            }
            --bit;
        }
        mask |= UINT64_C(1) << bit;
        rank -= binomial[bit][i];
        upper = bit;
    }
    if (rank != 0U) {
        die("internal combinatorial unranking remainder");
    }
    return mask;
}

static void prepare_removal_ranks(const unsigned positions[MAX_N],
                                  unsigned cardinality,
                                  uint64_t ranks[MAX_N])
{
    uint64_t prefix[MAX_N + 1U];
    uint64_t suffix[MAX_N + 1U];
    prefix[0] = 0U;
    for (unsigned i = 0U; i < cardinality; ++i) {
        prefix[i + 1U] = prefix[i] +
            binomial[positions[i]][i + 1U];
    }
    suffix[cardinality] = 0U;
    for (unsigned i = cardinality; i-- > 0U;) {
        suffix[i] = suffix[i + 1U] + binomial[positions[i]][i];
    }
    for (unsigned removed = 0U; removed < cardinality; ++removed) {
        ranks[removed] = prefix[removed] + suffix[removed + 1U];
    }
}

typedef struct {
    const uint64_t *previous;
    uint64_t *current;
    uint64_t previous_count;
    uint64_t output_begin;
    uint64_t begin;
    uint64_t end;
    unsigned n;
    unsigned cardinality;
    unsigned total_sum;
    Direction direction;
} LayerTask;

static void *compute_layer_range(void *argument)
{
    LayerTask *task = argument;
    if (task->begin == task->end) {
        return NULL;
    }
    uint64_t mask = combination_from_rank(
        task->n, task->cardinality, task->begin);

    for (uint64_t index = task->begin; index < task->end; ++index) {
        unsigned positions[MAX_N];
        uint64_t ranks[MAX_N];
        uint64_t bits = mask;
        unsigned subset_sum = 0U;
        for (unsigned i = 0U; i < task->cardinality; ++i) {
            const unsigned bit = (unsigned)__builtin_ctzll(bits);
            bits &= bits - 1U;
            positions[i] = bit;
            subset_sum += bit + 1U;
        }

        uint64_t value = 0U;
        if (task->direction == DIRECTION_FORWARD &&
            perfect_square_sum[subset_sum]) {
            value = 0U;
        } else {
            prepare_removal_ranks(positions, task->cardinality, ranks);
            uint64_t total = 0U;
            for (unsigned removed = 0U;
                 removed < task->cardinality; ++removed) {
                if (ranks[removed] >= task->previous_count) {
                    die("internal predecessor rank error");
                }
                if (task->direction == DIRECTION_REVERSE) {
                    const unsigned value = positions[removed] + 1U;
                    const unsigned next_prefix_sum =
                        task->total_sum - subset_sum + value;
                    if (perfect_square_sum[next_prefix_sum]) {
                        continue;
                    }
                }
                const uint64_t addition =
                    task->previous[ranks[removed]];
                if (total > UINT64_MAX - addition) {
                    die("internal layer count overflow");
                }
                total += addition;
            }
            value = total;
        }
        task->current[index - task->output_begin] = value;
        if (index + 1U < task->end) {
            mask = next_combination(mask);
        }
    }
    return NULL;
}

static void run_layer_block(const uint64_t *previous, uint64_t *current,
                            uint64_t previous_count, uint64_t begin,
                            uint64_t end, unsigned n,
                            unsigned cardinality, unsigned total_sum,
                            Direction direction)
{
    const uint64_t current_count = end - begin;
    unsigned thread_count = configured_threads;
    if (current_count < UINT64_C(1048576)) {
        thread_count = 1U;
    }
    if ((uint64_t)thread_count > current_count) {
        thread_count = (unsigned)current_count;
    }

    LayerTask tasks[MAX_THREADS];
    pthread_t threads[MAX_THREADS];
    for (unsigned thread = 0U; thread < thread_count; ++thread) {
        tasks[thread] = (LayerTask){
            .previous = previous,
            .current = current,
            .previous_count = previous_count,
            .output_begin = begin,
            .begin = begin + current_count * thread / thread_count,
            .end = begin + current_count * (thread + 1U) / thread_count,
            .n = n,
            .cardinality = cardinality,
            .total_sum = total_sum,
            .direction = direction
        };
    }
    if (thread_count == 1U) {
        (void)compute_layer_range(&tasks[0]);
        return;
    }

    unsigned created = 0U;
    for (; created < thread_count; ++created) {
        if (pthread_create(&threads[created], NULL,
                           compute_layer_range, &tasks[created]) != 0) {
            break;
        }
    }
    if (created != thread_count) {
        for (unsigned thread = 0U; thread < created; ++thread) {
            (void)pthread_join(threads[thread], NULL);
        }
        die("cannot create worker thread");
    }
    for (unsigned thread = 0U; thread < thread_count; ++thread) {
        if (pthread_join(threads[thread], NULL) != 0) {
            die("cannot join worker thread");
        }
    }
}

static uint64_t block_capacity(uint64_t count)
{
    uint64_t bytes = TARGET_BLOCK_BYTES;
    const uint64_t budget = memory_limit / 2U;
    if (bytes > budget) {
        bytes = budget;
    }
    uint64_t capacity = bytes / sizeof(uint64_t);
    if (capacity == 0U) {
        die("A395620_MEMORY_MIB leaves no room for a work block");
    }
    if (capacity > count) {
        capacity = count;
    }
    if (capacity > SIZE_MAX) {
        die("work block exceeds SIZE_MAX");
    }
    return capacity;
}

static uint64_t layer_bytes(uint64_t count)
{
    if (count > UINT64_MAX / sizeof(uint64_t) ||
        count > SIZE_MAX / sizeof(uint64_t)) {
        die("temporary-layer size overflow");
    }
    return count * sizeof(uint64_t);
}

static int create_layer_file(uint64_t count)
{
    char path[] = "395620_04.layer.XXXXXX";
    const int descriptor = mkstemp(path);
    if (descriptor < 0) {
        die("cannot create temporary layer file");
    }
    const uint64_t bytes = layer_bytes(count);
    const off_t length = (off_t)bytes;
    if (length < 0 || (uint64_t)length != bytes ||
        fchmod(descriptor, 0600) != 0 ||
        ftruncate(descriptor, length) != 0) {
        (void)close(descriptor);
        (void)unlink(path);
        die("cannot size temporary layer file");
    }
    if (unlink(path) != 0) {
        (void)close(descriptor);
        die("cannot unlink temporary layer file");
    }
#ifdef F_NOCACHE
    /* Keep sequential output from filling macOS's unified file cache. */
    (void)fcntl(descriptor, F_NOCACHE, 1);
#endif
    return descriptor;
}

static void write_layer_block(int descriptor, const uint64_t *data,
                              size_t count, uint64_t offset)
{
    const uint64_t byte_offset = layer_bytes(offset);
    const size_t byte_count = (size_t)layer_bytes((uint64_t)count);
    const unsigned char *bytes = (const unsigned char *)data;
    size_t written = 0U;
    while (written < byte_count) {
        if (byte_offset > UINT64_MAX - written) {
            die("temporary-layer write offset overflow");
        }
        const uint64_t absolute = byte_offset + written;
        const off_t position = (off_t)absolute;
        if (position < 0 || (uint64_t)position != absolute) {
            die("temporary-layer write offset overflow");
        }
        const ssize_t amount = pwrite(
            descriptor, bytes + written, byte_count - written, position);
        if (amount < 0) {
            if (errno == EINTR) {
                continue;
            }
            die("cannot write temporary layer file");
        }
        if (amount == 0) {
            die("short write to temporary layer file");
        }
        written += (size_t)amount;
    }
}

static void read_layer_block(int descriptor, uint64_t *data,
                             size_t count, uint64_t offset)
{
    const uint64_t byte_offset = layer_bytes(offset);
    const size_t byte_count = (size_t)layer_bytes((uint64_t)count);
    unsigned char *bytes = (unsigned char *)data;
    size_t received = 0U;
    while (received < byte_count) {
        if (byte_offset > UINT64_MAX - received) {
            die("temporary-layer read offset overflow");
        }
        const uint64_t absolute = byte_offset + received;
        const off_t position = (off_t)absolute;
        if (position < 0 || (uint64_t)position != absolute) {
            die("temporary-layer read offset overflow");
        }
        const ssize_t amount = pread(
            descriptor, bytes + received, byte_count - received, position);
        if (amount < 0) {
            if (errno == EINTR) {
                continue;
            }
            die("cannot read temporary layer file");
        }
        if (amount == 0) {
            die("short read from temporary layer file");
        }
        received += (size_t)amount;
    }
}

static const uint64_t *map_layer(const DiskLayer *layer)
{
    if (layer->count == 0U) {
        die("invalid temporary-layer mapping size");
    }
    const size_t bytes = (size_t)layer_bytes(layer->count);
#ifdef F_NOCACHE
    (void)fcntl(layer->descriptor, F_NOCACHE, 0);
#endif
    void *mapped = mmap(NULL, bytes, PROT_READ,
                        MAP_PRIVATE, layer->descriptor, 0);
    if (mapped == MAP_FAILED) {
        die("cannot map temporary layer file");
    }
#ifdef MADV_RANDOM
    (void)madvise(mapped, bytes, MADV_RANDOM);
#endif
    return mapped;
}

static void unmap_layer(const DiskLayer *layer, const uint64_t *mapped)
{
    if (munmap((void *)mapped, (size_t)layer_bytes(layer->count)) != 0) {
        die("cannot unmap temporary layer file");
    }
}

static void close_layer(DiskLayer *layer)
{
    if (layer->descriptor >= 0 && close(layer->descriptor) != 0) {
        die("cannot close temporary layer file");
    }
    layer->descriptor = -1;
    layer->count = 0U;
}

static DiskLayer seed_layer(void)
{
    DiskLayer layer = {create_layer_file(1U), 1U};
    const uint64_t one = 1U;
    write_layer_block(layer.descriptor, &one, 1U, 0U);
    return layer;
}

static void report_block(unsigned n, Direction direction,
                         unsigned cardinality,
                         uint64_t completed, uint64_t total,
                         double started)
{
    if (!quiet && total >= REPORT_MIN_STATES) {
        fprintf(stderr,
                "395620_04: n=%u %s-layer=%u "
                "processed=%" PRIu64 "/%" PRIu64 " time=%.1fs\n",
                n,
                direction == DIRECTION_FORWARD ? "forward" : "reverse",
                cardinality, completed, total,
                monotonic_seconds() - started);
    }
}

static DiskLayer generate_layer(DiskLayer *previous, uint64_t current_count,
                                unsigned n, unsigned cardinality,
                                unsigned total_sum, Direction direction)
{
    const uint64_t *mapped_previous = map_layer(previous);
    DiskLayer current = {create_layer_file(current_count), current_count};
    const uint64_t capacity = block_capacity(current_count);
    uint64_t *block = malloc((size_t)layer_bytes(capacity));
    if (block == NULL) {
        unmap_layer(previous, mapped_previous);
        close_layer(previous);
        close_layer(&current);
        die("cannot allocate layer work block");
    }

    const double started = monotonic_seconds();
    uint64_t next_report = (current_count + 7U) / 8U;
    for (uint64_t begin = 0U; begin < current_count;) {
        const uint64_t amount = current_count - begin < capacity ?
            current_count - begin : capacity;
        const uint64_t end = begin + amount;
        run_layer_block(mapped_previous, block, previous->count,
                        begin, end, n, cardinality, total_sum,
                        direction);
        write_layer_block(current.descriptor, block, (size_t)amount, begin);
        if (end >= next_report || end == current_count) {
            report_block(n, direction, cardinality,
                         end, current_count, started);
            while (next_report <= end &&
                   next_report <= UINT64_MAX -
                       (current_count + 7U) / 8U) {
                next_report += (current_count + 7U) / 8U;
            }
        }
        begin = end;
    }

    free(block);
    unmap_layer(previous, mapped_previous);
    close_layer(previous);
    return current;
}

static uint64_t peak_temporary_bytes(unsigned n)
{
    const unsigned middle = n / 2U;
    const unsigned remaining = n - middle;
    const uint64_t forward_middle = binomial[n][middle];
    uint64_t peak = 1U;
    for (unsigned k = 1U; k <= middle; ++k) {
        const uint64_t amount = binomial[n][k - 1U] + binomial[n][k];
        if (amount > peak) {
            peak = amount;
        }
    }
    for (unsigned r = 1U; r < remaining; ++r) {
        const uint64_t amount = forward_middle +
            binomial[n][r - 1U] + binomial[n][r];
        if (amount > peak) {
            peak = amount;
        }
    }
    const uint64_t join =
        forward_middle + binomial[n][remaining - 1U];
    const uint64_t cells = join > peak ? join : peak;
    return layer_bytes(cells);
}

static uint64_t available_temporary_bytes(void)
{
    struct statvfs status;
    if (statvfs(".", &status) != 0) {
        die("cannot determine free temporary-file space");
    }
    const U128 available =
        (U128)status.f_bavail * (U128)status.f_frsize;
    return available > UINT64_MAX ? UINT64_MAX : (uint64_t)available;
}

typedef struct {
    const uint64_t *forward;
    const uint64_t *reverse_previous;
    uint64_t reverse_previous_count;
    uint64_t forward_begin;
    uint64_t begin;
    uint64_t end;
    U256 partial;
    uint64_t full_mask;
    unsigned n;
    unsigned middle;
    unsigned remaining;
    unsigned total_sum;
} JoinTask;

static void *join_range(void *argument)
{
    JoinTask *task = argument;
    if (task->begin == task->end) {
        task->partial = (U256){{0U, 0U, 0U, 0U}};
        return NULL;
    }
    uint64_t prefix_mask = combination_from_rank(
        task->n, task->middle, task->begin);
    U256 partial = {{0U, 0U, 0U, 0U}};

    for (uint64_t index = task->begin; index < task->end; ++index) {
        const uint64_t forward_value =
            task->forward[index - task->forward_begin];
        if (forward_value != 0U) {
            const uint64_t complement = task->full_mask ^ prefix_mask;
            uint64_t bits = complement;
            unsigned positions[MAX_N];
            uint64_t ranks[MAX_N];
            unsigned complement_sum = 0U;
            for (unsigned i = 0U; i < task->remaining; ++i) {
                const unsigned bit = (unsigned)__builtin_ctzll(bits);
                bits &= bits - 1U;
                positions[i] = bit;
                complement_sum += bit + 1U;
            }
            prepare_removal_ranks(positions, task->remaining, ranks);

            uint64_t reverse_total = 0U;
            for (unsigned removed = 0U;
                 removed < task->remaining; ++removed) {
                if (ranks[removed] >= task->reverse_previous_count) {
                    die("internal join predecessor rank error");
                }
                const unsigned value = positions[removed] + 1U;
                const unsigned next_prefix_sum =
                    task->total_sum - complement_sum + value;
                if (!perfect_square_sum[next_prefix_sum]) {
                    const uint64_t addition =
                        task->reverse_previous[ranks[removed]];
                    if (reverse_total > UINT64_MAX - addition) {
                        die("internal reverse-join count overflow");
                    }
                    reverse_total += addition;
                }
            }
            big_add_u64_product(&partial, forward_value, reverse_total);
        }
        if (index + 1U < task->end) {
            prefix_mask = next_combination(prefix_mask);
        }
    }
    task->partial = partial;
    return NULL;
}

static U256 join_frontier_block(const uint64_t *forward,
                                uint64_t begin, uint64_t end,
                                const uint64_t *reverse_previous,
                                uint64_t reverse_previous_count,
                                unsigned n, unsigned middle)
{
    const uint64_t count = end - begin;
    unsigned thread_count = configured_threads;
    if (count < UINT64_C(1048576)) {
        thread_count = 1U;
    }
    if ((uint64_t)thread_count > count) {
        thread_count = (unsigned)count;
    }

    JoinTask tasks[MAX_THREADS];
    pthread_t threads[MAX_THREADS];
    const unsigned remaining = n - middle;
    const unsigned total_sum = n * (n + 1U) / 2U;
    const uint64_t full_mask = (UINT64_C(1) << n) - 1U;
    for (unsigned thread = 0U; thread < thread_count; ++thread) {
        tasks[thread] = (JoinTask){
            .forward = forward,
            .reverse_previous = reverse_previous,
            .reverse_previous_count = reverse_previous_count,
            .forward_begin = begin,
            .begin = begin + count * thread / thread_count,
            .end = begin + count * (thread + 1U) / thread_count,
            .partial = {{0U, 0U, 0U, 0U}},
            .full_mask = full_mask,
            .n = n,
            .middle = middle,
            .remaining = remaining,
            .total_sum = total_sum
        };
    }
    if (thread_count == 1U) {
        (void)join_range(&tasks[0]);
    } else {
        unsigned created = 0U;
        for (; created < thread_count; ++created) {
            if (pthread_create(&threads[created], NULL,
                               join_range, &tasks[created]) != 0) {
                break;
            }
        }
        if (created != thread_count) {
            for (unsigned thread = 0U; thread < created; ++thread) {
                (void)pthread_join(threads[thread], NULL);
            }
            die("cannot create join worker thread");
        }
        for (unsigned thread = 0U; thread < thread_count; ++thread) {
            if (pthread_join(threads[thread], NULL) != 0) {
                die("cannot join middle-frontier worker");
            }
        }
    }

    U256 total = {{0U, 0U, 0U, 0U}};
    for (unsigned thread = 0U; thread < thread_count; ++thread) {
        big_checked_add(&total, &tasks[thread].partial);
    }
    return total;
}

static U256 join_disk_frontiers(DiskLayer *forward,
                                DiskLayer *reverse_previous,
                                unsigned n, unsigned middle)
{
    const uint64_t *mapped_reverse = map_layer(reverse_previous);
    const uint64_t capacity = block_capacity(forward->count);
    uint64_t *block = malloc((size_t)layer_bytes(capacity));
    if (block == NULL) {
        unmap_layer(reverse_previous, mapped_reverse);
        close_layer(reverse_previous);
        close_layer(forward);
        die("cannot allocate middle-join work block");
    }

    const double started = monotonic_seconds();
    uint64_t next_report = (forward->count + 7U) / 8U;
    U256 answer = {{0U, 0U, 0U, 0U}};
    for (uint64_t begin = 0U; begin < forward->count;) {
        const uint64_t amount = forward->count - begin < capacity ?
            forward->count - begin : capacity;
        const uint64_t end = begin + amount;
        read_layer_block(forward->descriptor, block, (size_t)amount, begin);
        const U256 addition = join_frontier_block(
            block, begin, end, mapped_reverse, reverse_previous->count,
            n, middle);
        big_checked_add(&answer, &addition);
        if (!quiet && forward->count >= REPORT_MIN_STATES &&
            (end >= next_report || end == forward->count)) {
            fprintf(stderr,
                    "395620_04: n=%u middle-join "
                    "processed=%" PRIu64 "/%" PRIu64 " time=%.1fs\n",
                    n, end, forward->count,
                    monotonic_seconds() - started);
            while (next_report <= end &&
                   next_report <= UINT64_MAX -
                       (forward->count + 7U) / 8U) {
                next_report += (forward->count + 7U) / 8U;
            }
        }
        begin = end;
    }

    free(block);
    unmap_layer(reverse_previous, mapped_reverse);
    close_layer(reverse_previous);
    close_layer(forward);
    return answer;
}

static void report_layer(unsigned n, Direction direction,
                         unsigned cardinality,
                         uint64_t count, double seconds)
{
    if (!quiet && count >= REPORT_MIN_STATES) {
        fprintf(stderr,
                "395620_04: n=%u %s-layer=%u states=%" PRIu64
                " time=%.1fs\n",
                n,
                direction == DIRECTION_FORWARD ? "forward" : "reverse",
                cardinality, count, seconds);
    }
}

static U256 exact_middle_pass(unsigned n)
{
    const unsigned middle = n / 2U;
    const unsigned remaining = n - middle;
    const unsigned total_sum = n * (n + 1U) / 2U;
    const double started = monotonic_seconds();
    if (!quiet) {
        fprintf(stderr, "395620_04: n=%u exact middle pass started\n", n);
    }

    DiskLayer forward = seed_layer();
    for (unsigned cardinality = 1U;
         cardinality <= middle; ++cardinality) {
        const uint64_t current_count = binomial[n][cardinality];
        const double layer_started = monotonic_seconds();
        forward = generate_layer(
            &forward, current_count, n, cardinality, total_sum,
            DIRECTION_FORWARD);
        report_layer(n, DIRECTION_FORWARD,
                     cardinality, current_count,
                     monotonic_seconds() - layer_started);
    }

    DiskLayer reverse = seed_layer();
    for (unsigned cardinality = 1U;
         cardinality < remaining; ++cardinality) {
        const uint64_t current_count = binomial[n][cardinality];
        const double layer_started = monotonic_seconds();
        reverse = generate_layer(
            &reverse, current_count, n, cardinality, total_sum,
            DIRECTION_REVERSE);
        report_layer(n, DIRECTION_REVERSE,
                     cardinality, current_count,
                     monotonic_seconds() - layer_started);
    }

    const double join_started = monotonic_seconds();
    const U256 answer = join_disk_frontiers(
        &forward, &reverse, n, middle);
    if (!quiet) {
        fprintf(stderr,
                "395620_04: n=%u middle join time=%.1fs\n",
                n,
                monotonic_seconds() - join_started);
    }

    if (!quiet) {
        fprintf(stderr,
                "395620_04: n=%u exact middle pass done time=%.3fs\n",
                n,
                monotonic_seconds() - started);
    }
    return answer;
}

static U256 count_exact(unsigned n)
{
    if (n == 0U) {
        return (U256){{1U, 0U, 0U, 0U}};
    }
    prepare_tables();
    if (perfect_square_sum[n * (n + 1U) / 2U]) {
        return (U256){{0U, 0U, 0U, 0U}};
    }

    const U256 bound = big_factorial(n);
    const uint64_t temporary_peak = peak_temporary_bytes(n);
    const uint64_t available = available_temporary_bytes();
    if (available < temporary_peak) {
        fprintf(stderr,
                "error: n=%u needs up to %.3f GiB of temporary-file "
                "space, but only %.3f GiB is available here\n",
                n,
                (double)temporary_peak /
                    (1024.0 * 1024.0 * 1024.0),
                (double)available /
                    (1024.0 * 1024.0 * 1024.0));
        exit(EXIT_FAILURE);
    }
    if (!quiet) {
        const uint64_t mapped_peak = layer_bytes(
            binomial[n][n - n / 2U - 1U]);
        const uint64_t work_block = layer_bytes(
            block_capacity(binomial[n][n / 2U]));
        fprintf(stderr,
                "395620_04: n=%u mapped_layer=%.3f GiB block=%.1f MiB "
                "temp_peak=%.3f GiB disk_free=%.3f GiB "
                "exact_passes=1 threads=%u\n",
                n, (double)mapped_peak /
                    (1024.0 * 1024.0 * 1024.0),
                (double)work_block / (1024.0 * 1024.0),
                (double)temporary_peak /
                    (1024.0 * 1024.0 * 1024.0),
                (double)available /
                    (1024.0 * 1024.0 * 1024.0),
                configured_threads);
    }
    const double started = monotonic_seconds();
    const U256 answer = exact_middle_pass(n);
    if (big_compare(&answer, &bound) > 0) {
        die("factorial-bound check failed");
    }
    if (!quiet) {
        fprintf(stderr, "395620_04: n=%u exact answer time=%.3fs\n",
                n, monotonic_seconds() - started);
    }
    return answer;
}

static uint64_t direct_search(unsigned n, unsigned position, uint64_t used,
                              unsigned prefix_sum)
{
    if (position == n) {
        return 1U;
    }
    uint64_t count = 0U;
    for (unsigned value = 1U; value <= n; ++value) {
        const uint64_t bit = UINT64_C(1) << (value - 1U);
        if ((used & bit) != 0U ||
            perfect_square_sum[prefix_sum + value]) {
            continue;
        }
        const uint64_t addition = direct_search(
            n, position + 1U, used | bit, prefix_sum + value);
        if (count > UINT64_MAX - addition) {
            die("direct-check counter overflow");
        }
        count += addition;
    }
    return count;
}

static int lock_bfile(void)
{
    const int descriptor = open(LOCK_PATH, O_RDWR | O_CREAT, 0666);
    if (descriptor < 0) {
        die("cannot open b-file lock");
    }
    struct flock lock = {
        .l_type = F_WRLCK, .l_whence = SEEK_SET, .l_start = 0, .l_len = 0
    };
    while (fcntl(descriptor, F_SETLKW, &lock) != 0) {
        if (errno != EINTR) {
            (void)close(descriptor);
            die("cannot lock b-file");
        }
    }
    return descriptor;
}

static void unlock_bfile(int descriptor)
{
    struct flock lock = {
        .l_type = F_UNLCK, .l_whence = SEEK_SET, .l_start = 0, .l_len = 0
    };
    if (fcntl(descriptor, F_SETLK, &lock) != 0 || close(descriptor) != 0) {
        die("cannot unlock b-file");
    }
}

static void read_bfile(bool present[MAX_N + 1U],
                       char values[MAX_N + 1U][BIG_TEXT_SIZE])
{
    memset(present, 0, (MAX_N + 1U) * sizeof(*present));
    FILE *input = fopen(BFILE_PATH, "r");
    if (input == NULL) {
        if (errno == ENOENT) {
            return;
        }
        die("cannot read b-file");
    }
    char line[256];
    while (fgets(line, sizeof(line), input) != NULL) {
        unsigned index;
        char number[BIG_TEXT_SIZE];
        char extra;
        U256 parsed;
        if (sscanf(line, "%u %79s %c", &index, number, &extra) != 2 ||
            index > MAX_N || present[index] ||
            !parse_u256(number, &parsed)) {
            (void)fclose(input);
            die("b-file is malformed or contains a duplicate index");
        }
        present[index] = true;
        strcpy(values[index], number);
    }
    if (ferror(input) || fclose(input) != 0) {
        die("cannot finish reading b-file");
    }
}

static bool load_saved_term(unsigned n, U256 *value)
{
    bool present[MAX_N + 1U];
    char values[MAX_N + 1U][BIG_TEXT_SIZE];
    const int lock_descriptor = lock_bfile();
    read_bfile(present, values);
    const bool found = present[n];
    if (found && !parse_u256(values[n], value)) {
        unlock_bfile(lock_descriptor);
        die("saved term is invalid");
    }
    unlock_bfile(lock_descriptor);
    return found;
}

static void record_term(unsigned n, const U256 *value)
{
    bool present[MAX_N + 1U];
    char values[MAX_N + 1U][BIG_TEXT_SIZE];
    char text[BIG_TEXT_SIZE];
    u256_to_text(value, text);
    const int lock_descriptor = lock_bfile();
    read_bfile(present, values);
    if (present[n]) {
        if (strcmp(values[n], text) != 0) {
            unlock_bfile(lock_descriptor);
            die("computed term disagrees with the b-file");
        }
        unlock_bfile(lock_descriptor);
        return;
    }
    present[n] = true;
    strcpy(values[n], text);

    char temporary[] = "b395620_04.txt.tmp.XXXXXX";
    const int descriptor = mkstemp(temporary);
    if (descriptor < 0 || fchmod(descriptor, 0644) != 0) {
        if (descriptor >= 0) {
            (void)close(descriptor);
            (void)unlink(temporary);
        }
        unlock_bfile(lock_descriptor);
        die("cannot create temporary b-file");
    }
    FILE *output = fdopen(descriptor, "w");
    if (output == NULL) {
        (void)close(descriptor);
        (void)unlink(temporary);
        unlock_bfile(lock_descriptor);
        die("cannot open temporary b-file stream");
    }
    bool failed = false;
    for (unsigned index = 0U; index <= MAX_N; ++index) {
        if (present[index] &&
            fprintf(output, "%u %s\n", index, values[index]) < 0) {
            failed = true;
        }
    }
    if (!failed && fflush(output) != 0) {
        failed = true;
    }
    if (!failed && fsync(fileno(output)) != 0) {
        failed = true;
    }
    if (fclose(output) != 0) {
        failed = true;
    }
    if (failed || rename(temporary, BFILE_PATH) != 0) {
        (void)unlink(temporary);
        unlock_bfile(lock_descriptor);
        die("cannot atomically update b-file");
    }
    unlock_bfile(lock_descriptor);
    if (!quiet) {
        fprintf(stderr, "395620_04: saved a(%u) in %s\n", n, BFILE_PATH);
    }
}

static U256 known_term(unsigned n)
{
    U256 value;
    if (n > KNOWN_MAX_N || !parse_u256(known_terms[n], &value)) {
        die("internal known-term table error");
    }
    return value;
}

static U256 obtain_term(unsigned n)
{
    U256 value;
    if (load_saved_term(n, &value)) {
        if (!quiet) {
            fprintf(stderr, "395620_04: reusing saved a(%u)\n", n);
        }
        return value;
    }
    value = n <= KNOWN_MAX_N ? known_term(n) : count_exact(n);
    record_term(n, &value);
    return value;
}

static void check_implementation(unsigned maximum)
{
    prepare_tables();
    const bool saved_quiet = quiet;
    const uint64_t saved_memory_limit = memory_limit;
    quiet = true;
    /* Force many tiny blocks so --check also exercises block boundaries. */
    memory_limit = 64U;
    for (unsigned n = 0U; n <= maximum; ++n) {
        const U256 middle = count_exact(n);
        const U256 direct = {{direct_search(n, 0U, 0U, 0U), 0U, 0U, 0U}};
        const U256 expected = known_term(n);
        if (big_compare(&middle, &direct) != 0 ||
            big_compare(&middle, &expected) != 0) {
            memory_limit = saved_memory_limit;
            quiet = saved_quiet;
            fprintf(stderr, "error: check mismatch at n=%u\n", n);
            exit(EXIT_FAILURE);
        }
    }
    memory_limit = saved_memory_limit;
    quiet = saved_quiet;
    printf("ok: tiny-block out-of-core middle-frontier DP, direct "
           "enumeration, and known terms agree for n=0..%u\n", maximum);
}

static void print_bfile_line(unsigned n, const U256 *value)
{
    if (printf("%u ", n) < 0) {
        die("cannot write standard output");
    }
    print_u256(stdout, value);
    if (putchar('\n') == EOF || fflush(stdout) != 0) {
        die("cannot write standard output");
    }
}

static void usage(const char *program, FILE *stream)
{
    fprintf(stream,
            "Usage:\n"
            "  %s [MAX_N] [--quiet]\n"
            "  %s --upto MAX_N [--quiet]\n"
            "  %s --term N [--quiet]\n"
            "  %s --check [CHECK_N]\n"
            "\n"
            "MAX_N and N may be 0..%u; default MAX_N is %u.\n"
            "CHECK_N may be 0..%u and defaults to %u.\n"
            "Completed terms are saved atomically in %s.\n"
            "A395620_MEMORY_MIB sets the heap block budget (default %" PRIu64
            " MiB).\n"
            "A395620_THREADS sets 1..%u workers (default: up to 8).\n",
            program, program, program, program,
            MAX_N, DEFAULT_MAX_N,
            DIRECT_CHECK_MAX_N, DIRECT_CHECK_MAX_N,
            BFILE_PATH, DEFAULT_MEMORY_MIB, MAX_THREADS);
}

int main(int argc, char **argv)
{
    configure_resources();
    prepare_tables();
    Mode mode = MODE_UPTO;
    bool mode_seen = false;
    bool n_seen = false;
    unsigned n = DEFAULT_MAX_N;

    for (int i = 1; i < argc; ++i) {
        const char *argument = argv[i];
        if (strcmp(argument, "--help") == 0 || strcmp(argument, "-h") == 0) {
            usage(argv[0], stdout);
            return EXIT_SUCCESS;
        }
        if (strcmp(argument, "--quiet") == 0 || strcmp(argument, "-q") == 0) {
            quiet = true;
            continue;
        }
        if (strcmp(argument, "--term") == 0 ||
            strcmp(argument, "--upto") == 0) {
            if (mode_seen || n_seen || i + 1 >= argc) {
                usage(argv[0], stderr);
                return EXIT_FAILURE;
            }
            mode = strcmp(argument, "--term") == 0 ? MODE_TERM : MODE_UPTO;
            mode_seen = true;
            n = parse_unsigned(argv[++i], MAX_N,
                               mode == MODE_TERM ? "N" : "MAX_N");
            n_seen = true;
            continue;
        }
        if (strcmp(argument, "--check") == 0) {
            if (mode_seen || n_seen) {
                usage(argv[0], stderr);
                return EXIT_FAILURE;
            }
            mode = MODE_CHECK;
            mode_seen = true;
            n = DIRECT_CHECK_MAX_N;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                n = parse_unsigned(argv[++i], DIRECT_CHECK_MAX_N, "CHECK_N");
                n_seen = true;
            }
            continue;
        }
        if (argument[0] == '-' || mode_seen || n_seen) {
            usage(argv[0], stderr);
            return EXIT_FAILURE;
        }
        n = parse_unsigned(argument, MAX_N, "MAX_N");
        n_seen = true;
    }

    if (mode == MODE_CHECK) {
        check_implementation(n);
    } else if (mode == MODE_TERM) {
        const U256 value = obtain_term(n);
        print_u256(stdout, &value);
        if (putchar('\n') == EOF || fflush(stdout) != 0) {
            die("cannot write standard output");
        }
    } else {
        for (unsigned index = 0U; index <= n; ++index) {
            const U256 value = obtain_term(index);
            print_bfile_line(index, &value);
        }
    }
    return EXIT_SUCCESS;
}
