/*
 * A386581 -- number of normal multisets of size n with no permutation
 * whose run lengths are all distinct.
 *
 * A normal multiset is supported on {1,...,k}, with every multiplicity
 * positive.  Hence its multiplicity vector is a composition of n.
 *
 * The complement is A386580.  In a qualifying A386580 word, sort the
 * distinct run lengths increasingly and group them according to the letter
 * used by each run.  The result is a
 * strict partition of n together with a set partition of its parts.  If a
 * block has c parts, its letter must occur in c runs.  A multiset of run
 * labels can be ordered without equal adjacent labels exactly when
 *
 *                 max(c) <= 1 + (sum(c) - max(c)).
 *
 * Necessity follows by putting a different label between consecutive
 * copies of a most frequent label.  Sufficiency is the standard greedy
 * alternating arrangement (place a most frequent label in every other
 * position, then fill the gaps).  After such an arrangement is chosen,
 * the distinct lengths assigned to each label can be put into its run
 * positions in any order.  Thus the condition is also sufficient for a
 * word with the prescribed run lengths.
 *
 * The program therefore enumerates every strict partition of n, every
 * (unlabelled) set partition of its parts, and retains the balanced set
 * partitions above.  The sums of the blocks are the letter multiplicities;
 * all distinct permutations of those sums restore the ordered labels
 * 1,...,k.  A composition is encoded by its separator subset of the n-1
 * gaps.  An exact open-addressed set removes duplicate multiplicity vectors
 * having more than one witness.  Full 64-bit keys are compared, so hash
 * collisions cannot affect the answer.  There is one normal multiset for
 * every composition of n, hence 2^(n-1) for n>=1 (and one empty multiset
 * for n=0).  A386581 is obtained by subtracting the exact number of marked
 * compositions from this total.  The empty permutation satisfies the
 * distinct-run condition vacuously, so a(0)=0.
 *
 * --check additionally uses a definition-level oracle.  It enumerates all
 * compositions through n=10 and recursively tries to construct runs,
 * choosing a letter different from the previous one and a length not used
 * before.  Failure to construct such a word is counted for A386581.  This
 * oracle does not use strict partitions, set partitions, the balance
 * condition, or the result hash set.
 *
 * MAX_N=64 is the separator-mask representation limit, not a running-time
 * promise.  The answer is at most the 2^(n-1) compositions of n, so it fits
 * uint64_t throughout this range.  Allocation sizes and statistics are
 * checked; no count is inferred from a probabilistic hash.
 *
 * Build:
 *   cc -O3 -std=c11 -Wall -Wextra -Wpedantic 386581_01.c -o 386581_01
 *
 * Usage:
 *   ./386581_01                    # a(0),...,a(15)
 *   ./386581_01 20
 *   ./386581_01 --upto 20
 *   ./386581_01 --term 20
 *   ./386581_01 --check
 *   ./386581_01 --check 15
 *
 * Each newly computed term is recorded immediately in b386581_01.txt.
 * The file must be a contiguous prefix beginning at n=0.  Existing terms
 * are reused by --upto, and --term N requires all earlier terms.  Updates
 * are serialized with an advisory lock and atomically replace the b-file
 * only after the temporary file has been flushed and fsync'ed.  Thus an
 * interrupted calculation never records its unfinished term.  --no-bfile
 * performs a calculation without reading or writing the file; --output
 * selects another b-file path.
 */

#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEFAULT_MAX_N 15
#define KNOWN_MAX_N 15
#define ORACLE_MAX_N 10
#define MAX_N 64
#define MAX_RUNS 10
#define INITIAL_SET_CAPACITY ((size_t)16)
#define OCCUPIED_BIT (UINT64_C(1) << 63)

static const char *output_path = "b386581_01.txt";
static bool write_bfile = true;

#if SIZE_MAX < UINT64_MAX
#error "386581_01.c requires a 64-bit size_t"
#endif

_Static_assert(MAX_RUNS * (MAX_RUNS + 1) / 2 <= MAX_N,
               "MAX_RUNS is too large");
_Static_assert((MAX_RUNS + 1) * (MAX_RUNS + 2) / 2 > MAX_N,
               "MAX_RUNS is too small");

static const uint64_t known_terms[KNOWN_MAX_N + 1] = {
    UINT64_C(0), UINT64_C(0), UINT64_C(1), UINT64_C(1),
    UINT64_C(5), UINT64_C(11), UINT64_C(20), UINT64_C(51),
    UINT64_C(108), UINT64_C(229), UINT64_C(448), UINT64_C(953),
    UINT64_C(1940), UINT64_C(3951), UINT64_C(7986), UINT64_C(15972)
};

typedef struct {
    uint64_t *slot;
    size_t capacity;
    size_t size;
} MaskSet;

typedef struct {
    uint64_t strict_partitions;
    uint64_t set_partitions;
    uint64_t balanced_set_partitions;
    uint64_t multiplicity_permutations;
} Statistics;

typedef struct {
    unsigned n;
    uint8_t parts[MAX_RUNS];
    uint8_t block_sum[MAX_RUNS];
    uint8_t block_size[MAX_RUNS];
    MaskSet answers;
    Statistics statistics;
} Search;

static _Noreturn void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static void *xcalloc(size_t count, size_t size)
{
    if (size != 0U && count > SIZE_MAX / size) {
        die("allocation size overflow");
    }
    void *result = calloc(count, size);
    if (result == NULL) {
        die("out of memory");
    }
    return result;
}

static void checked_increment(uint64_t *value, const char *what)
{
    if (*value == UINT64_MAX) {
        fprintf(stderr, "error: %s overflow\n", what);
        exit(EXIT_FAILURE);
    }
    ++*value;
}

static uint64_t mix64(uint64_t value)
{
    value ^= value >> 30;
    value *= UINT64_C(0xbf58476d1ce4e5b9);
    value ^= value >> 27;
    value *= UINT64_C(0x94d049bb133111eb);
    return value ^ (value >> 31);
}

static void set_initialize(MaskSet *set)
{
    set->capacity = INITIAL_SET_CAPACITY;
    set->size = 0U;
    set->slot = xcalloc(set->capacity, sizeof(*set->slot));
}

static void set_insert_stored(MaskSet *set, uint64_t stored)
{
    size_t index = (size_t)mix64(stored) & (set->capacity - 1U);
    while (set->slot[index] != 0U) {
        if (set->slot[index] == stored) {
            return;
        }
        index = (index + 1U) & (set->capacity - 1U);
    }
    set->slot[index] = stored;
    ++set->size;
}

static void set_grow(MaskSet *set)
{
    if (set->capacity > SIZE_MAX / 2U ||
        2U * set->capacity > SIZE_MAX / sizeof(*set->slot)) {
        die("answer set is too large");
    }
    uint64_t *old_slot = set->slot;
    const size_t old_capacity = set->capacity;
    set->capacity *= 2U;
    set->size = 0U;
    set->slot = xcalloc(set->capacity, sizeof(*set->slot));
    for (size_t i = 0U; i < old_capacity; ++i) {
        if (old_slot[i] != 0U) {
            set_insert_stored(set, old_slot[i]);
        }
    }
    free(old_slot);
}

static void set_insert(MaskSet *set, uint64_t separator_mask)
{
    if ((separator_mask & OCCUPIED_BIT) != 0U) {
        die("internal separator mask exceeds its representation");
    }
    if (set->size + 1U > set->capacity - set->capacity / 4U) {
        set_grow(set);
    }
    set_insert_stored(set, separator_mask | OCCUPIED_BIT);
}

static bool set_contains(const MaskSet *set, uint64_t separator_mask)
{
    const uint64_t stored = separator_mask | OCCUPIED_BIT;
    size_t index = (size_t)mix64(stored) & (set->capacity - 1U);
    while (set->slot[index] != 0U) {
        if (set->slot[index] == stored) {
            return true;
        }
        index = (index + 1U) & (set->capacity - 1U);
    }
    return false;
}

static void sort_bytes(uint8_t *values, unsigned count)
{
    for (unsigned i = 1U; i < count; ++i) {
        const uint8_t value = values[i];
        unsigned j = i;
        while (j != 0U && values[j - 1U] > value) {
            values[j] = values[j - 1U];
            --j;
        }
        values[j] = value;
    }
}

static bool next_permutation(uint8_t *values, unsigned count)
{
    if (count < 2U) {
        return false;
    }
    int i = (int)count - 2;
    while (i >= 0 && values[i] >= values[i + 1]) {
        --i;
    }
    if (i < 0) {
        return false;
    }
    int j = (int)count - 1;
    while (values[j] <= values[i]) {
        --j;
    }
    const uint8_t temporary = values[i];
    values[i] = values[j];
    values[j] = temporary;
    for (unsigned left = (unsigned)i + 1U, right = count - 1U;
         left < right; ++left, --right) {
        const uint8_t swap = values[left];
        values[left] = values[right];
        values[right] = swap;
    }
    return true;
}

static void record_block_sums(Search *search, unsigned block_count)
{
    uint8_t multiplicity[MAX_RUNS];
    memcpy(multiplicity, search->block_sum,
           block_count * sizeof(*multiplicity));
    sort_bytes(multiplicity, block_count);

    do {
        unsigned partial_sum = 0U;
        uint64_t separator_mask = 0U;
        for (unsigned i = 0U; i + 1U < block_count; ++i) {
            partial_sum += multiplicity[i];
            if (partial_sum == 0U || partial_sum >= search->n) {
                die("invalid multiplicity composition");
            }
            separator_mask |= UINT64_C(1) << (partial_sum - 1U);
        }
        set_insert(&search->answers, separator_mask);
        checked_increment(&search->statistics.multiplicity_permutations,
                          "multiplicity-permutation statistic");
    } while (next_permutation(multiplicity, block_count));
}

static void enumerate_set_partitions(Search *search, unsigned run_count,
                                     unsigned index, unsigned block_count)
{
    if (index == run_count) {
        checked_increment(&search->statistics.set_partitions,
                          "set-partition statistic");
        unsigned largest = 0U;
        for (unsigned block = 0U; block < block_count; ++block) {
            if (search->block_size[block] > largest) {
                largest = search->block_size[block];
            }
        }
        if (largest > run_count - largest + 1U) {
            return;
        }
        checked_increment(&search->statistics.balanced_set_partitions,
                          "balanced-set-partition statistic");
        record_block_sums(search, block_count);
        return;
    }

    const uint8_t part = search->parts[index];
    for (unsigned block = 0U; block < block_count; ++block) {
        search->block_sum[block] =
            (uint8_t)(search->block_sum[block] + part);
        ++search->block_size[block];
        enumerate_set_partitions(search, run_count, index + 1U,
                                 block_count);
        --search->block_size[block];
        search->block_sum[block] =
            (uint8_t)(search->block_sum[block] - part);
    }

    search->block_sum[block_count] = part;
    search->block_size[block_count] = 1U;
    enumerate_set_partitions(search, run_count, index + 1U,
                             block_count + 1U);
    search->block_sum[block_count] = 0U;
    search->block_size[block_count] = 0U;
}

static void process_strict_partition(Search *search, unsigned run_count)
{
    checked_increment(&search->statistics.strict_partitions,
                      "strict-partition statistic");
    memset(search->block_sum, 0, sizeof(search->block_sum));
    memset(search->block_size, 0, sizeof(search->block_size));
    search->block_sum[0] = search->parts[0];
    search->block_size[0] = 1U;
    enumerate_set_partitions(search, run_count, 1U, 1U);
}

/* List increasing positive parts.  The final part is always 'remaining'. */
static void enumerate_strict_partitions(Search *search, unsigned remaining,
                                        unsigned minimum, unsigned depth)
{
    if (remaining < minimum || depth >= MAX_RUNS) {
        return;
    }

    search->parts[depth] = (uint8_t)remaining;
    process_strict_partition(search, depth + 1U);

    for (unsigned part = minimum; part < remaining; ++part) {
        const unsigned rest = remaining - part;
        if (rest <= part) {
            break;
        }
        search->parts[depth] = (uint8_t)part;
        enumerate_strict_partitions(search, rest, part + 1U, depth + 1U);
    }
}

static void generate_answers(unsigned n, Search *search)
{
    memset(search, 0, sizeof(*search));
    search->n = n;
    set_initialize(&search->answers);
    enumerate_strict_partitions(search, n, 1U, 0U);
}

static uint64_t count_a386581(unsigned n, Statistics *statistics)
{
    if (n == 0U) {
        if (statistics != NULL) {
            memset(statistics, 0, sizeof(*statistics));
        }
        return UINT64_C(0);
    }

    Search search;
    generate_answers(n, &search);
    const uint64_t total = UINT64_C(1) << (n - 1U);
    if ((uint64_t)search.answers.size > total) {
        die("marked-composition count exceeds all compositions");
    }
    const uint64_t answer = total - (uint64_t)search.answers.size;
    if (statistics != NULL) {
        *statistics = search.statistics;
    }
    free(search.answers.slot);
    return answer;
}

/* Directly choose successive maximal runs; used only by --check. */
static bool oracle_has_word(uint8_t *remaining, unsigned letter_count,
                            unsigned remaining_total, unsigned previous,
                            uint64_t used_lengths)
{
    if (remaining_total == 0U) {
        return true;
    }
    for (unsigned letter = 0U; letter < letter_count; ++letter) {
        if (letter == previous) {
            continue;
        }
        const unsigned available = remaining[letter];
        for (unsigned length = 1U; length <= available; ++length) {
            const uint64_t bit = UINT64_C(1) << (length - 1U);
            if ((used_lengths & bit) != 0U) {
                continue;
            }
            remaining[letter] = (uint8_t)(available - length);
            if (oracle_has_word(remaining, letter_count,
                                remaining_total - length, letter,
                                used_lengths | bit)) {
                remaining[letter] = (uint8_t)available;
                return true;
            }
            remaining[letter] = (uint8_t)available;
        }
    }
    return false;
}

static bool oracle_has_composition(unsigned n, uint64_t separators)
{
    if (n == 0U) {
        return true;
    }
    uint8_t multiplicity[ORACLE_MAX_N];
    unsigned letter_count = 0U;
    unsigned current = 1U;
    for (unsigned gap = 0U; gap + 1U < n; ++gap) {
        if ((separators & (UINT64_C(1) << gap)) != 0U) {
            multiplicity[letter_count++] = (uint8_t)current;
            current = 1U;
        } else {
            ++current;
        }
    }
    multiplicity[letter_count++] = (uint8_t)current;
    return oracle_has_word(multiplicity, letter_count, n, letter_count, 0U);
}

static unsigned parse_n(const char *text, unsigned maximum,
                        const char *name)
{
    char *end = NULL;
    if (text == NULL || *text == '\0' || *text == '-') {
        fprintf(stderr, "error: invalid %s: %s\n", name,
                text == NULL ? "(null)" : text);
        exit(EXIT_FAILURE);
    }
    errno = 0;
    const unsigned long value = strtoul(text, &end, 10);
    if (errno == ERANGE || end == text || *end != '\0' || value > maximum) {
        fprintf(stderr, "error: %s must be in 0..%u: %s\n",
                name, maximum, text);
        exit(EXIT_FAILURE);
    }
    return (unsigned)value;
}

static void verify_known(unsigned n, uint64_t value)
{
    if (n <= KNOWN_MAX_N && value != known_terms[n]) {
        fprintf(stderr,
                "error: built-in value mismatch at n=%u: "
                "got %" PRIu64 ", expected %" PRIu64 "\n",
                n, value, known_terms[n]);
        exit(EXIT_FAILURE);
    }
}

static char *path_with_suffix(const char *suffix)
{
    const size_t path_length = strlen(output_path);
    const size_t suffix_length = strlen(suffix);
    if (path_length > SIZE_MAX - suffix_length - 1U) {
        die("output path is too long");
    }
    char *path = malloc(path_length + suffix_length + 1U);
    if (path == NULL) {
        die("cannot allocate derived output path");
    }
    memcpy(path, output_path, path_length);
    memcpy(path + path_length, suffix, suffix_length + 1U);
    return path;
}

static int lock_bfile(void)
{
    char *path = path_with_suffix(".lock");
    const int descriptor = open(path, O_RDWR | O_CREAT, 0666);
    free(path);
    if (descriptor < 0) {
        die("cannot open b-file lock");
    }
    struct flock lock = {
        .l_type = F_WRLCK,
        .l_whence = SEEK_SET
    };
    while (fcntl(descriptor, F_SETLKW, &lock) != 0) {
        if (errno != EINTR) {
            close(descriptor);
            die("cannot lock b-file");
        }
    }
    return descriptor;
}

static void unlock_bfile(int descriptor)
{
    struct flock lock = {
        .l_type = F_UNLCK,
        .l_whence = SEEK_SET
    };
    if (fcntl(descriptor, F_SETLK, &lock) != 0 || close(descriptor) != 0) {
        die("cannot unlock b-file");
    }
}

/* Read a strict contiguous prefix.  Only record_term() extends the file. */
static unsigned read_bfile(uint64_t values[MAX_N + 1])
{
    FILE *input = fopen(output_path, "r");
    if (input == NULL) {
        if (errno == ENOENT) {
            return 0U;
        }
        die("cannot read b-file");
    }

    char line[256];
    unsigned next = 0U;
    while (fgets(line, sizeof(line), input) != NULL) {
        char *cursor = line;
        const size_t length = strlen(line);
        if (length == sizeof(line) - 1U && line[length - 1U] != '\n' &&
            !feof(input)) {
            fclose(input);
            die("b-file contains an overlong line");
        }
        while (*cursor == ' ' || *cursor == '\t') {
            ++cursor;
        }
        if (*cursor == '\0' || *cursor == '\n' || *cursor == '#') {
            continue;
        }
        if (*cursor == '-') {
            fclose(input);
            die("b-file is malformed or has a gap");
        }
        char *end = NULL;
        errno = 0;
        const uintmax_t parsed_index = strtoumax(cursor, &end, 10);
        if (errno == ERANGE || end == cursor || parsed_index > MAX_N) {
            fclose(input);
            die("b-file is malformed or has a gap");
        }
        cursor = end;
        if (*cursor != ' ' && *cursor != '\t') {
            fclose(input);
            die("b-file is malformed or has a gap");
        }
        while (*cursor == ' ' || *cursor == '\t') {
            ++cursor;
        }
        if (*cursor == '-') {
            fclose(input);
            die("b-file contains an invalid value");
        }
        errno = 0;
        const uintmax_t parsed_value = strtoumax(cursor, &end, 10);
        if (errno == ERANGE || end == cursor || parsed_value > UINT64_MAX) {
            fclose(input);
            die("b-file contains an invalid value");
        }
        cursor = end;
        while (*cursor == ' ' || *cursor == '\t') {
            ++cursor;
        }
        if (*cursor == '\r') {
            ++cursor;
        }
        if (*cursor == '\n') {
            ++cursor;
        }
        if (*cursor != '\0') {
            fclose(input);
            die("b-file contains trailing data");
        }
        const unsigned index = (unsigned)parsed_index;
        const uint64_t value = (uint64_t)parsed_value;
        if (index != next) {
            fclose(input);
            die("b-file is malformed or has a gap");
        }
        const uint64_t upper_bound = index == 0U ? UINT64_C(1) :
            UINT64_C(1) << (index - 1U);
        if (value > upper_bound) {
            fclose(input);
            die("b-file value exceeds the number of compositions");
        }
        verify_known(index, value);
        values[next++] = value;
    }
    if (ferror(input) || fclose(input) != 0) {
        die("cannot finish reading b-file");
    }
    return next;
}

static unsigned load_bfile(uint64_t values[MAX_N + 1])
{
    if (!write_bfile) {
        return 0U;
    }
    const int descriptor = lock_bfile();
    const unsigned prefix = read_bfile(values);
    unlock_bfile(descriptor);
    return prefix;
}

static void record_term(unsigned n, uint64_t value)
{
    if (!write_bfile) {
        return;
    }

    uint64_t values[MAX_N + 1];
    const int lock_descriptor = lock_bfile();
    unsigned prefix = read_bfile(values);
    if (n < prefix) {
        if (values[n] != value) {
            unlock_bfile(lock_descriptor);
            die("computed term disagrees with the b-file");
        }
        unlock_bfile(lock_descriptor);
        return;
    }
    if (n != prefix) {
        unlock_bfile(lock_descriptor);
        die("b-file gap detected while recording");
    }
    values[prefix++] = value;

    char *temporary = path_with_suffix(".tmp.XXXXXX");
    const int temporary_descriptor = mkstemp(temporary);
    if (temporary_descriptor < 0) {
        unlock_bfile(lock_descriptor);
        free(temporary);
        die("cannot create temporary b-file");
    }
    FILE *output = fdopen(temporary_descriptor, "w");
    if (output == NULL) {
        close(temporary_descriptor);
        unlink(temporary);
        unlock_bfile(lock_descriptor);
        free(temporary);
        die("cannot open temporary b-file stream");
    }

    bool failed = false;
    for (unsigned index = 0U; index < prefix; ++index) {
        if (fprintf(output, "%u %" PRIu64 "\n", index, values[index]) < 0) {
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
    if (failed || rename(temporary, output_path) != 0) {
        unlink(temporary);
        unlock_bfile(lock_descriptor);
        free(temporary);
        die("cannot atomically update b-file");
    }
    free(temporary);
    unlock_bfile(lock_descriptor);
    fprintf(stderr, "386581_01: recorded n=%u in %s\n", n, output_path);
}

static int check_implementation(unsigned maximum_n)
{
    for (unsigned n = 0U; n <= maximum_n; ++n) {
        Search search;
        uint64_t generated;
        if (n == 0U) {
            generated = UINT64_C(0);
        } else {
            generate_answers(n, &search);
            generated = (UINT64_C(1) << (n - 1U)) -
                (uint64_t)search.answers.size;
        }
        if (generated != known_terms[n]) {
            fprintf(stderr,
                    "error: built-in value mismatch at n=%u: "
                    "got %" PRIu64 ", expected %" PRIu64 "\n",
                    n, generated, known_terms[n]);
            if (n != 0U) {
                free(search.answers.slot);
            }
            return EXIT_FAILURE;
        }
        if (n <= ORACLE_MAX_N) {
            const uint64_t composition_count = n == 0U ? UINT64_C(1) :
                UINT64_C(1) << (n - 1U);
            uint64_t direct = 0U;
            for (uint64_t separators = 0U;
                 separators < composition_count; ++separators) {
                const bool oracle =
                    !oracle_has_composition(n, separators);
                const bool generator = n == 0U ? false :
                    !set_contains(&search.answers, separators);
                if (oracle != generator) {
                    fprintf(stderr,
                            "error: candidate mismatch at n=%u, "
                            "separator mask=0x%" PRIx64 ": "
                            "generator=%d, oracle=%d\n",
                            n, separators, generator, oracle);
                    if (n != 0U) {
                        free(search.answers.slot);
                    }
                    return EXIT_FAILURE;
                }
                if (oracle) {
                    ++direct;
                }
            }
            if (generated != direct) {
                fprintf(stderr,
                        "error: direct run-search mismatch at n=%u: "
                        "generator=%" PRIu64 ", oracle=%" PRIu64 "\n",
                        n, generated, direct);
                if (n != 0U) {
                    free(search.answers.slot);
                }
                return EXIT_FAILURE;
            }
        }
        if (n != 0U) {
            free(search.answers.slot);
        }
    }
    const unsigned oracle_limit =
        maximum_n < ORACLE_MAX_N ? maximum_n : ORACLE_MAX_N;
    printf("ok: A386581 complement generator agrees with built-in terms "
           "for n=0..%u and direct run search for n=0..%u\n",
           maximum_n, oracle_limit);
    return EXIT_SUCCESS;
}

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s [MAX_N] [--output FILE|--no-bfile]\n"
            "       %s --upto MAX_N [--output FILE|--no-bfile]\n"
            "       %s --term N [--output FILE|--no-bfile]\n"
            "       %s --check [CHECK_N]\n"
            "\n"
            "MAX_N and N may be 0..%d; the default is %d.\n"
            "CHECK_N may be 0..%d; the default is %d.\n"
            "--upto prints a(0),...,a(MAX_N), comma-separated.\n"
            "--term prints 'N a(N)' and search statistics.\n"
            "Each new term is saved immediately in %s.\n"
            "An existing contiguous prefix is reused; --term requires all "
            "earlier terms.\n"
            "--no-bfile disables b-file access; --output selects its path.\n",
            program, program, program, program, MAX_N, DEFAULT_MAX_N,
            KNOWN_MAX_N, KNOWN_MAX_N, output_path);
}

static int print_upto(unsigned maximum_n)
{
    uint64_t saved[MAX_N + 1];
    const unsigned prefix = load_bfile(saved);
    for (unsigned n = 0U; n <= maximum_n; ++n) {
        uint64_t answer;
        if (write_bfile && n < prefix) {
            answer = saved[n];
        } else {
            answer = count_a386581(n, NULL);
            verify_known(n, answer);
            record_term(n, answer);
        }
        if (n != 0U) {
            fputs(", ", stdout);
        }
        printf("%" PRIu64, answer);
    }
    putchar('\n');
    return ferror(stdout) ? EXIT_FAILURE : EXIT_SUCCESS;
}

static int print_term(unsigned n)
{
    uint64_t saved[MAX_N + 1];
    const unsigned prefix = load_bfile(saved);
    uint64_t answer;
    if (write_bfile && n < prefix) {
        answer = saved[n];
        fprintf(stderr, "386581_01: loaded n=%u from %s\n", n, output_path);
    } else {
        if (write_bfile && n > prefix) {
            fprintf(stderr,
                    "error: %s currently ends before n=%u; compute the "
                    "missing prefix first, or use --no-bfile\n",
                    output_path, n);
            return EXIT_FAILURE;
        }
        Statistics statistics;
        answer = count_a386581(n, &statistics);
        verify_known(n, answer);
        record_term(n, answer);
        fprintf(stderr,
                "386581_01: n=%u, strict partitions=%" PRIu64
                ", set partitions=%" PRIu64
                ", balanced=%" PRIu64
                ", multiplicity permutations=%" PRIu64 "\n",
                n, statistics.strict_partitions,
                statistics.set_partitions,
                statistics.balanced_set_partitions,
                statistics.multiplicity_permutations);
    }
    printf("%u %" PRIu64 "\n", n, answer);
    return ferror(stdout) || ferror(stderr) ? EXIT_FAILURE : EXIT_SUCCESS;
}

typedef enum {
    MODE_UPTO,
    MODE_TERM,
    MODE_CHECK
} RunMode;

int main(int argc, char **argv)
{
    const char *program = argv[0];
    RunMode mode = MODE_UPTO;
    unsigned n = DEFAULT_MAX_N;
    bool mode_selected = false;
    bool output_selected = false;
    bool no_bfile_selected = false;

    for (int argument = 1; argument < argc; ++argument) {
        const char *text = argv[argument];
        if (strcmp(text, "--help") == 0 || strcmp(text, "-h") == 0) {
            usage(program);
            return EXIT_SUCCESS;
        }
        if (strcmp(text, "--no-bfile") == 0) {
            if (no_bfile_selected) {
                die("duplicate --no-bfile");
            }
            no_bfile_selected = true;
            continue;
        }
        if (strcmp(text, "--output") == 0) {
            if (output_selected || argument + 1 >= argc ||
                argv[argument + 1][0] == '\0') {
                die("invalid --output");
            }
            output_selected = true;
            output_path = argv[++argument];
            continue;
        }
        if (strcmp(text, "--term") == 0 || strcmp(text, "--upto") == 0) {
            if (mode_selected || argument + 1 >= argc) {
                die("invalid or duplicate calculation mode");
            }
            mode = strcmp(text, "--term") == 0 ? MODE_TERM : MODE_UPTO;
            n = parse_n(argv[++argument], MAX_N,
                        mode == MODE_TERM ? "N" : "MAX_N");
            mode_selected = true;
            continue;
        }
        if (strcmp(text, "--check") == 0) {
            if (mode_selected) {
                die("invalid or duplicate calculation mode");
            }
            mode = MODE_CHECK;
            n = KNOWN_MAX_N;
            mode_selected = true;
            if (argument + 1 < argc && argv[argument + 1][0] != '-') {
                n = parse_n(argv[++argument], KNOWN_MAX_N, "CHECK_N");
            }
            continue;
        }
        if (text[0] == '-' || mode_selected) {
            usage(program);
            return EXIT_FAILURE;
        }
        n = parse_n(text, MAX_N, "MAX_N");
        mode = MODE_UPTO;
        mode_selected = true;
    }

    if (output_selected && no_bfile_selected) {
        die("--output and --no-bfile are mutually exclusive");
    }
    if (mode == MODE_CHECK && output_selected) {
        die("--check cannot be combined with --output");
    }
    write_bfile = mode != MODE_CHECK && !no_bfile_selected;

    if (mode == MODE_CHECK) {
        return check_implementation(n);
    }
    if (mode == MODE_TERM) {
        return print_term(n);
    }
    return print_upto(n);
}
