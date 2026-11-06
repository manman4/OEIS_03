/*
 * OEIS A334875: number of tribone tilings of an n-triangle.
 * https://oeis.org/A334875
 *
 * Rows i have columns 0..i. Both tile shapes start at (i,j), include
 * (i+1,j+1), and include either (i+1,j) or (i,j+1).
 *
 * Sweep the upper triangle by rows, storing the occupied cells in the
 * cut row. Reflection bijects transitions from a mask and its mirror
 * into each output orbit. Thus the upper DP can expand one representative
 * with the orbit's total multiplicity. Both masks also have equal
 * numbers of strip completions.
 *
 * For each representative, sweep the remaining strip by columns. The
 * two tile shapes exchange roles when row and column are exchanged.
 * The first hole of a column must start a tile, since all earlier
 * cells are covered. Its legal choices give disjoint sets of tilings.
 * A strip state is determined by its column, the incoming occupancy
 * from the left, and the unprocessed portion of the upper mask.
 * Only exact counts are cached. A cache collision discards an entry
 * and may cause recomputation. Multiplying by the upper multiplicity
 * counts each tiling once, since the tiles starting above the cut row
 * determine a unique upper partial tiling.
 *
 * Build:
 *   cc -O3 -std=c11 -Wall -Wextra -Wpedantic 334875_03.c \
 *       -o 334875_03 $(pkg-config --cflags --libs gmp)
 * Usage:
 *   ./334875_03 [LAST [FIRST]]
 *   ./334875_03 54 45
 *
 * Saves each new term atomically to b334875.txt in the current directory.
 * Resumes after the last saved index. FIRST sets the start of a new file
 * and may not create a gap in an existing file. Defaults to 0..26.
 * Newly saved terms go to stdout. Progress and memory go to stderr.
 * The mask limit is 63. Large terms can take much longer.
 * A334875_MEMORY_MIB bounds the live DP allocations (default 1024 MiB).
 * A smaller budget can move the cut upward and shrink the cache.
 */

#define _POSIX_C_SOURCE 200809L
#ifdef __APPLE__
#define _DARWIN_C_SOURCE
#endif

#include <errno.h>
#include <fcntl.h>
#include <gmp.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

enum {
    MAX_N = 63,
    DEFAULT_LAST = 26,
    DEFAULT_MEMORY_MIB = 1024,
    MIB = 1024 * 1024,
    MAX_CUT = 32,
    /* At most two choices per tile and at most n*(n+1)/6 tiles per path.
       There are at most 2^floor(n*(n+1)/6) partial tilings in any layer. */
    COUNT_WORDS = (MAX_N * (MAX_N + 1) / 6) / 64 + 1,
    BFILE_ROW_SIZE = 256
};

#define BFILE_NAME "b334875.txt"
#define LOCK_NAME "b334875.txt.lock"
#define WIDE_FLAG (UINT64_C(1) << 63)
#define KEY_MASK (WIDE_FLAG - 1)

typedef struct {
    uint64_t words[COUNT_WORDS];
} WideCount;

typedef struct {
    uint64_t key; /* Mask plus one in low bits. Zero denotes an empty slot. */
    union {
        uint64_t small;
        WideCount *wide;
    } count;
} Entry;

typedef struct {
    size_t used;
    size_t peak;
    size_t limit;
    unsigned n;
    int table_full;
    time_t last_report;
} Workspace;

typedef struct {
    Entry *entries;
    size_t capacity;
    size_t size;
    unsigned width;
    Workspace *work;
} Layer;

static void add_usage(Workspace *work, size_t bytes)
{
    work->used += bytes;
    if (work->peak < work->used)
        work->peak = work->used;
}

static void print_memory(size_t used, size_t peak)
{
    fprintf(stderr, " | DP %.1f MiB, DP peak %.1f MiB",
            (double)used / MIB, (double)peak / MIB);
#if defined(__APPLE__) || defined(__linux__)
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
#ifdef __APPLE__
        double rss_mib = (double)usage.ru_maxrss / MIB;
#else
        double rss_mib = (double)usage.ru_maxrss / 1024;
#endif
        fprintf(stderr, ", process peak RSS %.1f MiB", rss_mib);
    }
#endif
    fputc('\n', stderr);
}

static void report_progress(Workspace *work, const char *phase,
                            size_t done, size_t total, int force)
{
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) == 0) {
        if (!force && now.tv_sec - work->last_report < 5)
            return;
        work->last_report = now.tv_sec;
    } else if (!force) {
        return;
    }
    fprintf(stderr, "n=%u: %s %zu/%zu", work->n, phase, done, total);
    print_memory(work->used, work->peak);
}

static void *allocate(Workspace *work, size_t count, size_t bytes)
{
    if (count > (work->limit - work->used) / bytes) {
        fprintf(stderr, "n=%u: DP memory limit exceeded (A334875_MEMORY_MIB).\n",
                work->n);
        exit(EXIT_FAILURE);
    }
    void *data = calloc(count, bytes);
    if (data == NULL) {
        fprintf(stderr, "n=%u: cannot allocate DP memory.\n", work->n);
        exit(EXIT_FAILURE);
    }
    add_usage(work, count * bytes);
    return data;
}

static uint64_t reverse_mask(uint64_t x, unsigned width)
{
#if defined(__clang__)
    x = __builtin_bitreverse64(x);
#else
    x = ((x >> 1) & UINT64_C(0x5555555555555555)) |
        ((x & UINT64_C(0x5555555555555555)) << 1);
    x = ((x >> 2) & UINT64_C(0x3333333333333333)) |
        ((x & UINT64_C(0x3333333333333333)) << 2);
    x = ((x >> 4) & UINT64_C(0x0f0f0f0f0f0f0f0f)) |
        ((x & UINT64_C(0x0f0f0f0f0f0f0f0f)) << 4);
    x = ((x >> 8) & UINT64_C(0x00ff00ff00ff00ff)) |
        ((x & UINT64_C(0x00ff00ff00ff00ff)) << 8);
    x = ((x >> 16) & UINT64_C(0x0000ffff0000ffff)) |
        ((x & UINT64_C(0x0000ffff0000ffff)) << 16);
    x = (x >> 32) | (x << 32);
#endif
    return x >> (64 - width);
}

static uint64_t full_mask(unsigned width)
{
    return (UINT64_C(1) << width) - 1;
}

static uint64_t hash_key(uint64_t key)
{
    key = (key ^ (key >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    key = (key ^ (key >> 27)) * UINT64_C(0x94d049bb133111eb);
    return key ^ (key >> 31);
}

static Entry *find_slot(const Layer *layer, uint64_t key)
{
    size_t mask = layer->capacity - 1;
    size_t i = (size_t)hash_key(key) & mask;
    while (layer->entries[i].key != 0 &&
           (layer->entries[i].key & KEY_MASK) != key)
        i = (i + 1) & mask;
    return &layer->entries[i];
}

/* If the next row cannot fit, its caller keeps the previous full layer. */
static int grow_layer(Layer *layer)
{
    Layer old = *layer;
    size_t available = layer->work->limit - layer->work->used;
    if (old.capacity > SIZE_MAX / 2)
        return 0;
    size_t capacity = old.capacity ? 2 * old.capacity : 16;
    if (capacity > available / sizeof(Entry))
        return 0;
    Entry *entries = calloc(capacity, sizeof(*entries));
    if (entries == NULL)
        return 0;
    add_usage(layer->work, capacity * sizeof(*entries));
    layer->capacity = capacity;
    layer->entries = entries;
    for (size_t i = 0; i < old.capacity; ++i) {
        Entry *entry = &old.entries[i];
        if (entry->key != 0)
            *find_slot(layer, entry->key & KEY_MASK) = *entry;
    }
    free(old.entries);
    layer->work->used -= old.capacity * sizeof(Entry);
    return 1;
}

static void add_wide(Entry *dest, const Entry *source, Workspace *work)
{
    if ((dest->key & WIDE_FLAG) == 0) {
        WideCount *wide = allocate(work, 1, sizeof(*wide));
        wide->words[0] = dest->count.small;
        dest->count.wide = wide;
        dest->key |= WIDE_FLAG;
    }

    const uint64_t *words = &source->count.small;
    size_t length = 1;
    if (source->key & WIDE_FLAG) {
        words = source->count.wide->words;
        length = COUNT_WORDS;
    }
    uint64_t carry = 0;
    for (size_t i = 0; i < length || (carry != 0 && i < COUNT_WORDS); ++i) {
        uint64_t old = dest->count.wide->words[i];
        uint64_t part = i < length ? words[i] : 0;
        uint64_t sum = old + part;
        uint64_t with_carry = sum + carry;
        dest->count.wide->words[i] = with_carry;
        carry = (sum < old) || (with_carry < sum);
    }
    if (carry != 0) {
        fputs("DP count overflow.\n", stderr);
        exit(EXIT_FAILURE);
    }
}

static void accumulate(Entry *dest, const Entry *source, Workspace *work)
{
    if (((dest->key | source->key) & WIDE_FLAG) == 0 &&
        UINT64_MAX - dest->count.small >= source->count.small) {
        dest->count.small += source->count.small;
    } else {
        add_wide(dest, source, work);
    }
}

static void add_count(Layer *layer, uint64_t mask, const Entry *source)
{
    uint64_t reflected = reverse_mask(mask, layer->width);
    uint64_t key = 1 + (mask < reflected ? mask : reflected);

    if (layer->capacity == 0 && !grow_layer(layer)) {
        layer->work->table_full = 1;
        return;
    }
    Entry *entry = find_slot(layer, key);
    if (entry->key == 0) {
        if (layer->size >= layer->capacity - layer->capacity / 4) {
            if (!grow_layer(layer)) {
                layer->work->table_full = 1;
                return;
            }
            entry = find_slot(layer, key);
        }
        entry->key = key;
        ++layer->size;
    }

    accumulate(entry, source, layer->work);
}

static void release_count(Workspace *work, const Entry *entry)
{
    if (entry->key & WIDE_FLAG) {
        free(entry->count.wide);
        work->used -= sizeof(WideCount);
    }
}

static void clear_layer(Layer *layer)
{
    for (size_t i = 0; i < layer->capacity; ++i)
        release_count(layer->work, &layer->entries[i]);
    free(layer->entries);
    layer->work->used -= layer->capacity * sizeof(Entry);
}

static void sweep_down(uint64_t holes, uint64_t below,
                       const Entry *source, Layer *next)
{
    while (holes != 0) {
        uint64_t first = holes & -holes;
        uint64_t right = first << 1;
        if (below & right)
            return;
        holes ^= first;
        if ((below & first) == 0) {
            if (holes & right) {
                sweep_down(holes ^ right, below | right, source, next);
                if (next->work->table_full)
                    return;
            }
            below |= first | right;
        } else {
            if ((holes & right) == 0)
                return;
            holes ^= right;
            below |= right;
        }
    }
    add_count(next, below, source);
}

static Layer build_top(Workspace *work, unsigned cut)
{
    Layer current = {.width = 1, .work = work};
    const Entry seed = {.key = 1, .count.small = 1};
    add_count(&current, 0, &seed);
    if (work->table_full) {
        fprintf(stderr, "n=%u: cannot allocate the initial row table.\n", work->n);
        exit(EXIT_FAILURE);
    }

    report_progress(work, "upper DP rows", current.width, cut, 1);
    while (current.width < cut && current.size != 0) {
        Layer next = {.width = current.width + 1, .work = work};
        uint64_t full = full_mask(current.width);
        for (size_t i = 0; i < current.capacity && !work->table_full; ++i) {
            if ((i & 4095) == 0)
                report_progress(work, "upper DP rows", current.width, cut, 0);
            const Entry *entry = &current.entries[i];
            if (entry->key != 0)
                sweep_down(full ^ ((entry->key & KEY_MASK) - 1), 0, entry, &next);
        }
        if (work->table_full) {
            clear_layer(&next);
            break;
        }
        clear_layer(&current);
        current = next;
    }
    report_progress(work, "upper DP finished at row", current.width, cut, 1);
    return current;
}

static void import_count(mpz_t result, const Entry *entry)
{
    if (entry->key & WIDE_FLAG) {
        mpz_import(result, COUNT_WORDS, -1, sizeof(uint64_t), 0, 0,
                   entry->count.wide->words);
    } else {
#if ULONG_MAX >= UINT64_MAX
        mpz_set_ui(result, entry->count.small);
#else
        mpz_import(result, 1, -1, sizeof(uint64_t), 0, 0, &entry->count.small);
#endif
    }
}

typedef struct {
    uint64_t key;
    uint64_t count;
} MemoEntry;

typedef struct {
    Workspace *work;
    unsigned cut;
    unsigned height;
    uint64_t full[MAX_N + 1];
    MemoEntry *memo;
    size_t memo_mask;
    size_t profiles_done;
    size_t profiles_total;
    unsigned calls;
} Strip;

static Entry count_column(Strip *strip, unsigned column, uint64_t incoming,
                          uint64_t suffix);

static Entry fill_column(Strip *strip, unsigned column, uint64_t holes,
                         uint64_t next, uint64_t suffix)
{
    Entry result = {0};
    uint64_t valid = strip->full[column + 1];
    while (holes != 0) {
        uint64_t first = holes & -holes;
        uint64_t right = first << 1;
        if ((valid & right) == 0 || (next & right) != 0)
            return result;
        holes ^= first;
        if ((valid & first) != 0 && (next & first) == 0) {
            if (holes & right) {
                Entry branch = fill_column(strip, column, holes ^ right,
                                           next | right, suffix);
                accumulate(&result, &branch, strip->work);
                release_count(strip->work, &branch);
            }
            next |= first | right;
        } else {
            if ((holes & right) == 0)
                return result;
            holes ^= right;
            next |= right;
        }
    }
    Entry tail = count_column(strip, column + 1, next, suffix);
    if (result.key == 0 && result.count.small == 0)
        return tail;
    accumulate(&result, &tail, strip->work);
    release_count(strip->work, &tail);
    return result;
}

static Entry count_column(Strip *strip, unsigned column, uint64_t incoming,
                          uint64_t suffix)
{
    if ((++strip->calls & 262143U) == 0)
        report_progress(strip->work, "strip profiles", strip->profiles_done,
                        strip->profiles_total, 0);
    if (column == strip->work->n)
        return (Entry){.count.small = incoming == 0};

    uint64_t fixed = 0;
    uint64_t key;
    if (column < strip->cut) {
        fixed = suffix & 1;
        if (incoming & fixed)
            return (Entry){0};
        /* The high sentinel of the suffix encodes the current column. */
        key = (suffix << strip->height) | incoming;
    } else {
        /* The sentinel sits immediately below the first valid cell.
           These keys are nonzero and smaller than every pre-cut key. */
        key = (UINT64_C(1) << (column - strip->cut)) | incoming;
    }
    MemoEntry *entry = &strip->memo[(size_t)hash_key(key) & strip->memo_mask];
    if (entry->key == key)
        return (Entry){.count.small = entry->count};

    uint64_t holes = strip->full[column] ^ incoming ^ fixed;
    Entry result = fill_column(strip, column, holes, 0, suffix >> 1);
    /* A recursive call can replace the slot, so publish only afterwards.
       Large counts use exact limbs and are recomputed on a later visit. */
    if ((result.key & WIDE_FLAG) == 0)
        *entry = (MemoEntry){.key = key, .count = result.count.small};
    return result;
}

static int compare_masks(const void *a, const void *b)
{
    uint64_t left = ((const Entry *)a)->key & KEY_MASK;
    uint64_t right = ((const Entry *)b)->key & KEY_MASK;
    return (left > right) - (left < right);
}

static void order_profiles(Layer *top)
{
    size_t size = 0;
    for (size_t i = 0; i < top->capacity; ++i) {
        if (top->entries[i].key == 0)
            continue;
        if (i != size) {
            top->entries[size] = top->entries[i];
            top->entries[i].key = 0;
        }
        ++size;
    }
    /* The column sweep consumes low bits first. Nearby numeric masks
       share high-bit suffixes and can reuse cached strip states. */
    qsort(top->entries, size, sizeof(Entry), compare_masks);
}

static void count_strip_completions(Layer *top, size_t memo_bytes, mpz_t result)
{
    Workspace *work = top->work;
    report_progress(work, "ordering strip profiles", 0, top->size, 1);
    order_profiles(top);
    Strip strip = {.work = work, .cut = top->width,
                   .height = work->n - top->width + 1, .profiles_total = top->size};
    uint64_t full = full_mask(strip.height);
    for (unsigned j = 0; j < work->n; ++j) {
        unsigned skip = j < strip.cut ? 0 : j - strip.cut + 1;
        strip.full[j] = full ^ full_mask(skip);
    }
    /* The zero-initialized mask at column n is outside the triangle. */
    size_t capacity = 1;
    while (capacity <= memo_bytes / sizeof(MemoEntry) / 2)
        capacity *= 2;
    while ((strip.memo = calloc(capacity, sizeof(*strip.memo))) == NULL) {
        if (capacity == 1) {
            fprintf(stderr, "n=%u: cannot allocate the strip cache.\n", work->n);
            exit(EXIT_FAILURE);
        }
        capacity /= 2;
    }
    strip.memo_mask = capacity - 1;
    add_usage(work, capacity * sizeof(*strip.memo));
    report_progress(work, "strip profiles", 0, top->size, 1);

    mpz_t upper, lower;
    mpz_inits(upper, lower, NULL);
    for (size_t i = 0; i < top->size; ++i) {
        const Entry *entry = &top->entries[i];
        uint64_t suffix = (UINT64_C(1) << strip.cut) | ((entry->key & KEY_MASK) - 1);
        Entry completions = count_column(&strip, 0, 0, suffix);
        if (completions.key != 0 || completions.count.small != 0) {
            import_count(upper, entry);
            import_count(lower, &completions);
            mpz_addmul(result, upper, lower);
        }
        release_count(work, &completions);
        ++strip.profiles_done;
    }
    mpz_clears(upper, lower, NULL);
    work->used -= capacity * sizeof(*strip.memo);
    free(strip.memo);
}

static size_t count_tilings(unsigned n, size_t limit, mpz_t result)
{
    mpz_set_ui(result, n == 0);
    if (n == 0)
        return 0;

    /* Conway-Lagarias criterion. It rules out n=63, so the key's
       highest possible bit is cut + height = n + 1 <= 63. */
    unsigned residue = n % 12;
    if (residue != 0 && residue != 2 && residue != 9 && residue != 11)
        return 0;

    unsigned cut = n <= 20 ? (n + 1) / 2 : n - 10;
    if (cut > MAX_CUT)
        cut = MAX_CUT;
    size_t memo_bytes = n < 20 ? MIB : (n < 40 ? 64 : 256) * (size_t)MIB;
    if (memo_bytes > limit / 4)
        memo_bytes = limit / 4;
    /* Reserve room for the cache and for wide sums on the recursion
       stack. Each nonterminal placement uses a tile, and depth can
       also include up to n column calls and n final-placement frames.
       At most two live wide counts are needed per recursive level. */
    size_t wide_reserve = 2 * (n * (n + 1) / 6 + 3 * n + 2) * sizeof(WideCount);
    Workspace work = {.limit = limit - memo_bytes - wide_reserve, .n = n};
    Layer top = build_top(&work, cut);
    work.limit = limit;
    if (top.size != 0)
        count_strip_completions(&top, memo_bytes, result);
    clear_layer(&top);
    return work.peak;
}

static unsigned long long parse_number(const char *arg)
{
    if (*arg < '0' || *arg > '9')
        return ULLONG_MAX;
    char *end;
    errno = 0;
    unsigned long long value = strtoull(arg, &end, 10);
    return errno == 0 && *end == '\0' ? value : ULLONG_MAX;
}

typedef struct {
    char rows[MAX_N + 1][BFILE_ROW_SIZE];
    unsigned first;
    unsigned count;
    mode_t mode;
} BFile;

static int lock_bfile(void)
{
    int fd = open(LOCK_NAME, O_RDWR | O_CREAT | O_NOFOLLOW | O_NONBLOCK, 0600);
    if (fd < 0) {
        perror(LOCK_NAME);
        return -1;
    }
    struct stat info;
    if (fstat(fd, &info) < 0 || !S_ISREG(info.st_mode) || info.st_nlink != 1) {
        fputs("The b-file lock path must be a regular file with one link.\n", stderr);
        close(fd);
        return -1;
    }
    struct flock lock = {.l_type = F_WRLCK, .l_whence = SEEK_SET};
    if (fcntl(fd, F_SETLK, &lock) < 0) {
        if (errno == EACCES || errno == EAGAIN)
            fputs("Another process is using " BFILE_NAME ".\n", stderr);
        else
            perror(LOCK_NAME);
        close(fd);
        return -1;
    }
    return fd;
}

static int read_index(const char *line)
{
    unsigned n = 0;
    if (*line < '0' || *line > '9')
        return -1;
    while (*line >= '0' && *line <= '9') {
        n = 10 * n + (unsigned)(*line++ - '0');
        if (n > MAX_N)
            return -1;
    }
    if (*line != ' ' && *line != '\t')
        return -1;
    do {
        ++line;
    } while (*line == ' ' || *line == '\t');
    if (*line < '0' || *line > '9')
        return -1;
    do {
        ++line;
    } while (*line >= '0' && *line <= '9');
    while (*line == ' ' || *line == '\t')
        ++line;
    if (*line == '\r')
        ++line;
    return *line == '\n' && line[1] == '\0' ? (int)n : -1;
}

static int load_bfile(BFile *file)
{
    int fd = open(BFILE_NAME, O_RDONLY | O_NOFOLLOW | O_NONBLOCK);
    if (fd < 0) {
        if (errno == ENOENT)
            return 1;
        perror(BFILE_NAME);
        return 0;
    }
    struct stat info;
    if (fstat(fd, &info) < 0 || !S_ISREG(info.st_mode)) {
        fputs(BFILE_NAME " must be a regular file.\n", stderr);
        close(fd);
        return 0;
    }
    file->mode = info.st_mode & 0777;
    FILE *input = fdopen(fd, "r");
    if (input == NULL) {
        perror(BFILE_NAME);
        close(fd);
        return 0;
    }
    int ok = 1;
    char row[BFILE_ROW_SIZE];
    while (fgets(row, sizeof(row), input) != NULL) {
        int n = read_index(row);
        if (n < 0 || file->count > MAX_N ||
            (file->count != 0 && (unsigned)n != file->first + file->count)) {
            fprintf(stderr, "%s: invalid or nonconsecutive row %u.\n",
                    BFILE_NAME, file->count + 1);
            ok = 0;
            break;
        }
        if (file->count == 0)
            file->first = (unsigned)n;
        memcpy(file->rows[file->count], row, strlen(row) + 1);
        ++file->count;
    }
    if (ferror(input)) {
        perror(BFILE_NAME);
        ok = 0;
    }
    if (fclose(input) != 0) {
        perror(BFILE_NAME);
        ok = 0;
    }
    return ok;
}

static int save_term(BFile *file, unsigned n, const mpz_t result, int directory)
{
    char *row = file->rows[file->count];
    int length = gmp_snprintf(row, BFILE_ROW_SIZE, "%u %Zd\n", n, result);
    if (length < 0 || length >= BFILE_ROW_SIZE) {
        fputs("Cannot format the b-file row.\n", stderr);
        return 0;
    }

    char temporary[] = "." BFILE_NAME ".tmp.XXXXXX";
    int fd = mkstemp(temporary);
    if (fd < 0) {
        perror("Creating temporary b-file");
        return 0;
    }
    FILE *output = fdopen(fd, "w");
    if (output == NULL) {
        perror("Opening temporary b-file");
        close(fd);
        unlink(temporary);
        return 0;
    }
    int ok = 1;
    for (unsigned i = 0; i <= file->count && ok; ++i)
        ok = fputs(file->rows[i], output) != EOF;
    if (!ok || fflush(output) == EOF ||
        fchmod(fd, file->mode) < 0 || fsync(fd) < 0) {
        perror("Writing temporary b-file");
        ok = 0;
    }
    if (fclose(output) == EOF) {
        perror("Closing temporary b-file");
        ok = 0;
    }
    if (ok && rename(temporary, BFILE_NAME) < 0) {
        perror("Replacing " BFILE_NAME);
        ok = 0;
    }
    if (!ok) {
        unlink(temporary);
        return 0;
    }
    ++file->count;
    if (fsync(directory) < 0) {
        perror("Synchronizing b-file directory after replacement");
        return 0;
    }
    return 1;
}

int main(int argc, char **argv)
{
    unsigned long long last = argc >= 2 ? parse_number(argv[1]) : DEFAULT_LAST;
    unsigned long long first = argc >= 3 ? parse_number(argv[2]) : 0;
    if (argc > 3 || last > MAX_N || first > last) {
        fprintf(stderr, "Usage: %s [LAST [FIRST]], 0 <= FIRST <= LAST <= %d\n",
                argv[0], MAX_N);
        return EXIT_FAILURE;
    }

    const char *memory = getenv("A334875_MEMORY_MIB");
    unsigned long long mib = memory ? parse_number(memory) : DEFAULT_MEMORY_MIB;
    if (mib == 0 || mib > SIZE_MAX / MIB) {
        fputs("A334875_MEMORY_MIB must be a positive memory limit in MiB.\n", stderr);
        return EXIT_FAILURE;
    }

    int lock = lock_bfile();
    if (lock < 0)
        return EXIT_FAILURE;
    BFile file = {.first = (unsigned)first, .mode = 0600};
    if (!load_bfile(&file)) {
        close(lock);
        return EXIT_FAILURE;
    }
    unsigned next = file.first + file.count;
    if (file.count != 0) {
        if (argc >= 3 && (first < file.first || first > next)) {
            fprintf(stderr, "%s contains n=%u..%u. FIRST must be in %u..%u to resume.\n",
                    BFILE_NAME, file.first, next - 1, file.first, next);
            close(lock);
            return EXIT_FAILURE;
        }
        fprintf(stderr, "%s: saved through n=%u.\n", BFILE_NAME, next - 1);
    }
    if (next > last) {
        fprintf(stderr, "No new terms through n=%llu.\n", last);
        close(lock);
        return EXIT_SUCCESS;
    }

    int directory = open(".", O_RDONLY | O_DIRECTORY);
    if (directory < 0) {
        perror("Opening b-file directory");
        close(lock);
        return EXIT_FAILURE;
    }
    mpz_t result;
    mpz_init(result);
    int status = EXIT_SUCCESS;
    for (unsigned n = next; n <= (unsigned)last; ++n) {
        fprintf(stderr, "n=%u: starting (last requested n=%llu)\n", n, last);
        size_t peak = count_tilings(n, (size_t)mib * MIB, result);
        if (!save_term(&file, n, result, directory)) {
            status = EXIT_FAILURE;
            break;
        }
        fprintf(stderr, "%s: saved through n=%u", BFILE_NAME, n);
        print_memory(0, peak);
        if (fputs(file.rows[file.count - 1], stdout) == EOF || fflush(stdout) == EOF) {
            fputs("Cannot write result.\n", stderr);
            status = EXIT_FAILURE;
            break;
        }
    }
    mpz_clear(result);
    close(directory);
    close(lock);
    return status;
}
