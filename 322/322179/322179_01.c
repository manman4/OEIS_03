/*
 * A322179
 *
 * Number of permutations of the multiset
 *
 *     {1, 2, ..., n, 1, 2, ..., n}
 *
 * such that at least k entries occur between the two copies of k, for
 * every k=1,...,n.
 *
 * Put the labels down in the fixed order n,n-1,...,1.  A DP state is the
 * bit mask of the occupied positions among the 2*n positions.  When label
 * k is placed at positions i<j, the condition is simply
 *
 *     j - i >= k + 1.
 *
 * Since the label being placed is determined by popcount(mask)/2, the mask
 * is the complete state: no label information has to be stored.  Only two
 * adjacent population-count layers are retained.  Masks in a layer are
 * addressed by their combinatorial-number-system (colexicographic) rank;
 * a split lookup table evaluates that rank in O(1).  Every final labelled
 * placement is reached exactly once, and it is the same thing as one
 * multiset permutation counted by A322179.
 *
 * Layers through 11 placed labels use uint64_t: the number of labelled
 * pairings of 2*r positions is at most (2*r)!/2^r, which is below 2^64 for
 * r<=11.  Later layers use unsigned __int128.  At MAX_N=14 the peak DP
 * storage is about 538 MiB instead of a 4-GiB full-mask table.  The default
 * computes the supplied known range n=0..9.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic \
 *       322179_01.c -o 322179_01
 *
 * Usage:
 *   ./322179_01             # print n=0..9
 *   ./322179_01 10          # print n=0..10
 *   ./322179_01 --term 9    # print a(9)
 *   ./322179_01 --check     # verify the supplied terms
 * Completed terms are atomically recorded in b322179_01.txt by default.
 * Use --output FILE to select another b-file or --no-bfile to disable it.
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
#include <sys/stat.h>
#include <unistd.h>

#if !defined(__SIZEOF_INT128__)
#error "322179_01.c requires unsigned __int128"
#endif

__extension__ typedef unsigned __int128 U128;

#define MAX_N 14
#define DEFAULT_N 9

static const char *output_path = "b322179_01.txt";
static bool write_bfile = true;

typedef struct {
    void *values;
    uint64_t length;
    bool wide;
} Layer;

typedef struct {
    unsigned low_bits;
    uint64_t low_mask;
    uint64_t *low_rank;
    uint64_t *high_rank;
    uint8_t *low_count;
    uint64_t high_states;
} RankTable;

static const char *const known[] = {
    "1", "0", "0", "2", "40", "1070", "38936", "1896220",
    "119912476", "9587033840"
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
    if (errno == 0 && end != text && *end == '\0' &&
        value > MAX_N && value <= 18) {
        fprintf(stderr,
                "error: the layered subset DP is limited to n<=%d; "
                "use ./322179_02 --term %ld --threads T for n=%ld\n",
                MAX_N, value, value);
        exit(EXIT_FAILURE);
    }
    if (errno != 0 || end == text || *end != '\0' ||
        value < 0 || value > MAX_N) {
        fprintf(stderr, "error: N must be in 0..%d: %s\n", MAX_N, text);
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
    fprintf(stderr, "322179_01: updated %s through n=%d\n",
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

static Layer make_layer(unsigned positions, unsigned occupied)
{
    Layer layer;
    layer.length = binomial[positions][occupied];
    layer.wide = occupied / 2 >= 12;
    const size_t element_size = layer.wide ? sizeof(U128) : sizeof(uint64_t);
    if (layer.length > SIZE_MAX / element_size) {
        die("DP layer size overflow");
    }
    layer.values = calloc((size_t)layer.length, element_size);
    if (layer.values == NULL) {
        die("cannot allocate DP layer");
    }
    return layer;
}

static U128 layer_value(const Layer *layer, uint64_t index)
{
    if (layer->wide) {
        return ((const U128 *)layer->values)[index];
    }
    return ((const uint64_t *)layer->values)[index];
}

static void add_to_layer(Layer *layer, uint64_t index, U128 value)
{
    if (layer->wide) {
        if (!add_u128(&((U128 *)layer->values)[index], value)) {
            die("answer overflowed unsigned __int128");
        }
    } else {
        if (value > UINT64_MAX ||
            !add_u64(&((uint64_t *)layer->values)[index], (uint64_t)value)) {
            die("internal 64-bit layer bound failed");
        }
    }
}

static U128 a322179(int n)
{
    if (n == 0) {
        return 1;
    }

    const unsigned positions = 2U * (unsigned)n;
    const uint64_t full = (UINT64_C(1) << positions) - 1;
    RankTable ranks = make_rank_table(positions);
    Layer current = make_layer(positions, 0);
    ((uint64_t *)current.values)[0] = 1;

    for (unsigned placed = 0; placed < (unsigned)n; ++placed) {
        const int label = n - (int)placed;
        const unsigned occupied = 2 * placed;
        Layer next = make_layer(positions, occupied + 2);

        uint64_t mask = occupied == 0 ? 0 :
                        (UINT64_C(1) << occupied) - 1;
        for (;;) {
            const uint64_t current_index = occupied == 0 ? 0 :
                                           mask_rank(&ranks, mask);
            const U128 ways = layer_value(&current, current_index);
            if (ways != 0) {
                const uint64_t available = full ^ mask;
                for (unsigned i = 0;
                     i + (unsigned)label + 1 < positions; ++i) {
                    const uint64_t first = UINT64_C(1) << i;
                    if ((available & first) == 0) {
                        continue;
                    }

                    uint64_t seconds = available &
                        (full << (i + (unsigned)label + 1));
                    while (seconds != 0) {
                        const uint64_t second =
                            seconds & (UINT64_C(0) - seconds);
                        seconds ^= second;
                        const uint64_t next_mask = mask | first | second;
                        const uint64_t next_index =
                            mask_rank(&ranks, next_mask);
                        add_to_layer(&next, next_index, ways);
                    }
                }
            }

            if (occupied == 0) {
                break;
            }
            const uint64_t next_mask = next_combination(mask);
            if (next_mask <= mask || next_mask > full) {
                break;
            }
            mask = next_mask;
        }

        free(current.values);
        current = next;
    }

    const U128 answer = layer_value(&current, 0);
    free(current.values);
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
        fprintf(stderr, "error: A322179 mismatch at n=%d: got ", n);
        print_u128(stderr, value);
        fprintf(stderr, ", expected %s\n", known[n]);
        exit(EXIT_FAILURE);
    }
}

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s [MAX_N] [--output FILE]\n"
            "       %s --term N [--output FILE]\n"
            "       %s --check [--no-bfile]\n"
            "N must be in 0..%d.\n",
            program, program, program, MAX_N);
}

int main(int argc, char **argv)
{
    bool term_mode = false;
    bool check_mode = false;
    int maximum = DEFAULT_N;

    initialize_binomial();

    bool have_mode = false;
    bool have_output_option = false;
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")) {
            usage(argv[0]);
            return EXIT_SUCCESS;
        }
        if (!strcmp(argv[i], "--output")) {
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
        const int known_count = (int)(sizeof(known) / sizeof(known[0]));
        for (int n = 0; n < known_count; ++n) {
            verify_known(n, a322179(n));
        }
        printf("ok: A322179 terms n=0..%d verified\n", known_count - 1);
        return EXIT_SUCCESS;
    }

    if (term_mode) {
        const U128 value = a322179(maximum);
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
        const U128 value = a322179(n);
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
