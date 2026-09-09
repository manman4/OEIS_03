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
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

/*
 * Exact computation for A399711 / Erdos problem #896.
 *
 * For A,B subsets of [N], let
 *
 *   r(m) = #{(a,b) in A x B : a*b=m},
 *   F(A,B) = #{m : r(m)=1}.
 *
 * This program computes
 *
 *   a(N) = max F(A,B).
 *
 * The default method enumerates the smaller-cardinality side A.  For fixed A
 * it chooses B by branch-and-bound.  For each product m it maintains
 *
 *   selected[m]  = representations using an already selected b,
 *   remaining[m] = representations using an undecided b.
 *
 * Product m can still be unique only if
 *
 *   selected[m] <= 1 and selected[m] + remaining[m] >= 1.
 *
 * Counting all products satisfying this condition independently gives a
 * rigorous upper bound.  Hence pruning when that count is at most the current
 * best value is safe.  The restriction |A| <= |B| is safe because
 * F(A,B)=F(B,A).
 *
 * The alternative --method gray is a simple exhaustive Gray-code search.  It
 * is intentionally independent of the branch-and-bound upper bound and is
 * used by --check for N <= 10.
 *
 * Time is exponential; the fixed limit is an input-safety limit, not a claim
 * that N=30 is practical.  Working memory is O(N^2).  Exact terms are saved
 * atomically to b399711_02.txt by default.
 */

#define PROGRAM_NAME "399711_02"
#define DEFAULT_OUTPUT "b399711_02.txt"
#define MAX_N 30
#define MAX_PRODUCT (MAX_N * MAX_N)
#define NODE_HEARTBEAT_MASK ((UINT64_C(1) << 20) - UINT64_C(1))
#define OUTER_HEARTBEAT_MASK ((UINT64_C(1) << 14) - UINT64_C(1))

typedef enum {
    METHOD_BNB,
    METHOD_GRAY
} Method;

typedef struct {
    int n;
    bool progress;
    uint64_t all_mask;

    int best;
    uint64_t best_a;
    uint64_t best_b;
    uint64_t nodes;
    double started;
    double next_heartbeat;

    uint64_t outer_mask;
    int outer_size;
    uint16_t products[(MAX_N + 1) * MAX_N];
    uint16_t selected[MAX_PRODUCT + 1];
    uint16_t remaining[MAX_PRODUCT + 1];
    uint8_t order[MAX_N];
    int current_unique;
    int upper;
} Solver;

typedef struct {
    int start;
    int term;
    int positional;
    bool start_given;
    bool term_given;
    bool positional_given;
    bool progress;
    bool witness;
    bool check;
    bool write_bfile;
    const char *output_path;
    Method method;
} Options;

static double now_seconds(void)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1.0e-9;
}

static int popcount64(uint64_t value)
{
    return __builtin_popcountll((unsigned long long)value);
}

static int trailing_zeroes64(uint64_t value)
{
    return __builtin_ctzll((unsigned long long)value);
}

static uint16_t *products_for(Solver *solver, int b)
{
    return &solver->products[b * MAX_N];
}

static const uint16_t *const_products_for(const Solver *solver, int b)
{
    return &solver->products[b * MAX_N];
}

static void internal_error(const char *message)
{
    fprintf(stderr, "%s: internal error: %s\n", PROGRAM_NAME, message);
    exit(EXIT_FAILURE);
}

static void increment_nodes(Solver *solver)
{
    if (solver->nodes != UINT64_MAX) {
        ++solver->nodes;
    }
}

static int mask_values(uint64_t mask, int values[MAX_N])
{
    int count = 0;
    while (mask != 0) {
        const int bit = trailing_zeroes64(mask);
        values[count++] = bit + 1;
        mask &= mask - UINT64_C(1);
    }
    return count;
}

static int score(const Solver *solver, uint64_t a_mask, uint64_t b_mask)
{
    uint16_t representations[MAX_PRODUCT + 1] = {0};
    int unique = 0;

    a_mask &= solver->all_mask;
    b_mask &= solver->all_mask;
    while (a_mask != 0) {
        const uint64_t a_bit = a_mask & (~a_mask + UINT64_C(1));
        const int a = trailing_zeroes64(a_bit) + 1;
        uint64_t b_work = b_mask;
        while (b_work != 0) {
            const uint64_t b_bit = b_work & (~b_work + UINT64_C(1));
            const int b = trailing_zeroes64(b_bit) + 1;
            const int product = a * b;
            const uint16_t old = representations[product];
            if (old == 0) {
                ++unique;
            } else if (old == 1) {
                --unique;
            }
            representations[product] = (uint16_t)(old + 1);
            b_work ^= b_bit;
        }
        a_mask ^= a_bit;
    }
    return unique;
}

static void accept_candidate(Solver *solver, uint64_t a_mask,
                             uint64_t b_mask, int value)
{
    const int actual = score(solver, a_mask, b_mask);
    if (actual != value) {
        fprintf(stderr,
                "%s: internal score mismatch: maintained=%d direct=%d\n",
                PROGRAM_NAME, value, actual);
        exit(EXIT_FAILURE);
    }

    solver->best = value;
    solver->best_a = a_mask & solver->all_mask;
    solver->best_b = b_mask & solver->all_mask;
    if (solver->progress) {
        fprintf(stderr,
                "%s: improved N=%d lower_bound=%d |A|=%d |B|=%d\n",
                PROGRAM_NAME, solver->n, solver->best,
                popcount64(solver->best_a), popcount64(solver->best_b));
    }
}

static void seed_candidate(Solver *solver, uint64_t a_mask, uint64_t b_mask)
{
    a_mask &= solver->all_mask;
    b_mask &= solver->all_mask;
    if (a_mask == 0 || b_mask == 0) {
        return;
    }
    const int value = score(solver, a_mask, b_mask);
    if (value > solver->best) {
        accept_candidate(solver, a_mask, b_mask, value);
    }
}

static void report_heartbeat(Solver *solver)
{
    if (!solver->progress) {
        return;
    }
    const double elapsed = now_seconds() - solver->started;
    if (elapsed < solver->next_heartbeat) {
        return;
    }
    fprintf(stderr,
            "%s: progress N=%d best=%d A=%" PRIu64 "/%" PRIu64
            " nodes=%" PRIu64 " elapsed=%.1f s\n",
            PROGRAM_NAME, solver->n, solver->best, solver->outer_mask,
            solver->all_mask, solver->nodes, elapsed);
    solver->next_heartbeat = elapsed + 10.0;
}

static void node_heartbeat(Solver *solver)
{
    if (solver->progress &&
        (solver->nodes & NODE_HEARTBEAT_MASK) == UINT64_C(0)) {
        report_heartbeat(solver);
    }
}

static void begin_solve(Solver *solver, const char *method)
{
    solver->best = 0;
    solver->best_a = 0;
    solver->best_b = 0;
    solver->nodes = 0;
    solver->outer_mask = 0;
    solver->started = now_seconds();
    solver->next_heartbeat = 10.0;
    if (solver->progress) {
        fprintf(stderr, "%s: start N=%d method=%s\n",
                PROGRAM_NAME, solver->n, method);
    }
}

static int finish_solve(Solver *solver)
{
    const int actual = score(solver, solver->best_a, solver->best_b);
    if (actual != solver->best) {
        internal_error("final witness verification failed");
    }
    if (solver->progress) {
        fprintf(stderr,
                "%s: N=%d exact=%d nodes=%" PRIu64 " %.3f s\n",
                PROGRAM_NAME, solver->n, solver->best, solver->nodes,
                now_seconds() - solver->started);
    }
    return solver->best;
}

/* Build a feasible B greedily and put all B variables into search order. */
static void greedy_seed_and_order(Solver *solver, int minimum_size)
{
    uint16_t counts[MAX_PRODUCT + 1] = {0};
    bool chosen[MAX_N + 1] = {false};
    int collision_score[MAX_N + 1] = {0};
    uint64_t b_mask = 0;
    int chosen_count = 0;
    int unique = 0;

    while (chosen_count < solver->n) {
        int best_b = 0;
        int best_delta = INT_MIN;
        for (int b = 1; b <= solver->n; ++b) {
            if (chosen[b]) {
                continue;
            }
            int delta = 0;
            const uint16_t *products = const_products_for(solver, b);
            for (int i = 0; i < solver->outer_size; ++i) {
                const uint16_t product = products[i];
                if (counts[product] == 0) {
                    ++delta;
                } else if (counts[product] == 1) {
                    --delta;
                }
            }
            if (delta > best_delta) {
                best_delta = delta;
                best_b = b;
            }
        }

        if (chosen_count >= minimum_size && best_delta <= 0) {
            break;
        }
        if (best_b == 0) {
            internal_error("greedy search failed to select a variable");
        }

        chosen[best_b] = true;
        solver->order[chosen_count++] = (uint8_t)best_b;
        b_mask |= UINT64_C(1) << (best_b - 1);
        const uint16_t *products = const_products_for(solver, best_b);
        for (int i = 0; i < solver->outer_size; ++i) {
            const uint16_t product = products[i];
            if (counts[product] == 1) {
                --unique;
            }
            ++counts[product];
            if (counts[product] == 1) {
                ++unique;
            }
        }
    }

    if (chosen_count >= minimum_size && unique > solver->best) {
        accept_candidate(solver, solver->outer_mask, b_mask, unique);
    }

    int order_length = chosen_count;
    for (int b = 1; b <= solver->n; ++b) {
        const uint16_t *products = const_products_for(solver, b);
        for (int i = 0; i < solver->outer_size; ++i) {
            collision_score[b] += (int)solver->remaining[products[i]] - 1;
        }
        if (!chosen[b]) {
            solver->order[order_length++] = (uint8_t)b;
        }
    }
    if (order_length != solver->n) {
        internal_error("incomplete variable order");
    }

    /* Stable insertion sort of the non-greedy suffix by decreasing score. */
    for (int i = chosen_count + 1; i < solver->n; ++i) {
        const uint8_t key = solver->order[i];
        int j = i;
        while (j > chosen_count &&
               collision_score[solver->order[j - 1]] < collision_score[key]) {
            solver->order[j] = solver->order[j - 1];
            --j;
        }
        solver->order[j] = key;
    }
}

static bool can_still_be_unique(uint16_t selected, uint16_t remaining)
{
    return selected <= 1 && (unsigned int)selected + remaining >= 1U;
}

static void adjust_product(Solver *solver, uint16_t product,
                           int selected_delta, int remaining_delta)
{
    const uint16_t old_selected = solver->selected[product];
    const uint16_t old_remaining = solver->remaining[product];
    if (old_selected == 1) {
        --solver->current_unique;
    }
    if (can_still_be_unique(old_selected, old_remaining)) {
        --solver->upper;
    }

    const int new_selected = (int)old_selected + selected_delta;
    const int new_remaining = (int)old_remaining + remaining_delta;
    if (new_selected < 0 || new_remaining < 0 ||
        new_selected > UINT16_MAX || new_remaining > UINT16_MAX) {
        internal_error("invalid representation counter");
    }
    solver->selected[product] = (uint16_t)new_selected;
    solver->remaining[product] = (uint16_t)new_remaining;

    if (new_selected == 1) {
        ++solver->current_unique;
    }
    if (can_still_be_unique((uint16_t)new_selected,
                            (uint16_t)new_remaining)) {
        ++solver->upper;
    }
}

static void search_b(Solver *solver, int index, uint64_t b_mask,
                     int b_size, int minimum_size)
{
    increment_nodes(solver);
    node_heartbeat(solver);

    /* Excluding every undecided b realizes current_unique exactly. */
    if (b_size >= minimum_size && solver->current_unique > solver->best) {
        accept_candidate(solver, solver->outer_mask, b_mask,
                         solver->current_unique);
    }
    if (solver->upper <= solver->best) {
        return;
    }
    if (b_size + solver->n - index < minimum_size) {
        return;
    }
    if (index == solver->n) {
        return;
    }

    const int b = solver->order[index];
    const uint16_t *products = const_products_for(solver, b);

    /* Include b. */
    for (int i = 0; i < solver->outer_size; ++i) {
        adjust_product(solver, products[i], 1, -1);
    }
    search_b(solver, index + 1,
             b_mask | (UINT64_C(1) << (b - 1)),
             b_size + 1, minimum_size);
    for (int i = solver->outer_size; i-- > 0;) {
        adjust_product(solver, products[i], -1, 1);
    }

    /* Exclude b. */
    for (int i = 0; i < solver->outer_size; ++i) {
        adjust_product(solver, products[i], 0, -1);
    }
    search_b(solver, index + 1, b_mask, b_size, minimum_size);
    for (int i = solver->outer_size; i-- > 0;) {
        adjust_product(solver, products[i], 0, 1);
    }
}

static int solve_bnb(Solver *solver, uint64_t seed_a, uint64_t seed_b)
{
    begin_solve(solver, "branch-and-bound");
    seed_candidate(solver, seed_a, seed_b);
    seed_candidate(solver, seed_b, seed_a);
    seed_candidate(solver, UINT64_C(1), solver->all_mask);

    const uint64_t limit = solver->all_mask + UINT64_C(1);
    for (uint64_t a_mask = UINT64_C(1); a_mask < limit; ++a_mask) {
        int a_values[MAX_N];
        solver->outer_mask = a_mask;
        solver->outer_size = mask_values(a_mask, a_values);

        memset(solver->remaining, 0,
               ((size_t)solver->n * (size_t)solver->n + 1U) *
               sizeof(solver->remaining[0]));
        for (int b = 1; b <= solver->n; ++b) {
            uint16_t *products = products_for(solver, b);
            for (int i = 0; i < solver->outer_size; ++i) {
                const int product = a_values[i] * b;
                products[i] = (uint16_t)product;
                ++solver->remaining[product];
            }
        }

        int root_upper = 0;
        const int product_limit = solver->n * solver->n;
        for (int product = 1; product <= product_limit; ++product) {
            if (solver->remaining[product] != 0) {
                ++root_upper;
            }
        }

        if (root_upper > solver->best) {
            greedy_seed_and_order(solver, solver->outer_size);
            memset(solver->selected, 0,
                   ((size_t)product_limit + 1U) *
                   sizeof(solver->selected[0]));
            solver->current_unique = 0;
            solver->upper = root_upper;
            search_b(solver, 0, 0, 0, solver->outer_size);
        }

        if (solver->progress &&
            (a_mask & OUTER_HEARTBEAT_MASK) == UINT64_C(0)) {
            report_heartbeat(solver);
        }
    }
    return finish_solve(solver);
}

static int solve_gray(Solver *solver, uint64_t seed_a, uint64_t seed_b)
{
    begin_solve(solver, "Gray-code exhaustive");
    seed_candidate(solver, seed_a, seed_b);
    seed_candidate(solver, seed_b, seed_a);
    seed_candidate(solver, UINT64_C(1), solver->all_mask);

    const uint64_t limit = solver->all_mask + UINT64_C(1);
    for (uint64_t a_mask = UINT64_C(1); a_mask < limit; ++a_mask) {
        int a_values[MAX_N];
        uint16_t representations[MAX_PRODUCT + 1] = {0};
        const int a_size = mask_values(a_mask, a_values);
        uint64_t previous = 0;
        int b_size = 0;
        int unique = 0;
        solver->outer_mask = a_mask;

        for (uint64_t index = UINT64_C(1); index < limit; ++index) {
            const uint64_t b_mask = index ^ (index >> 1);
            const uint64_t changed = b_mask ^ previous;
            const int b = trailing_zeroes64(changed) + 1;
            if ((b_mask & changed) != 0) {
                ++b_size;
                for (int i = 0; i < a_size; ++i) {
                    const int product = a_values[i] * b;
                    const uint16_t old = representations[product];
                    if (old == 1) {
                        --unique;
                    }
                    representations[product] = (uint16_t)(old + 1);
                    if (old == 0) {
                        ++unique;
                    }
                }
            } else {
                --b_size;
                for (int i = 0; i < a_size; ++i) {
                    const int product = a_values[i] * b;
                    const uint16_t old = representations[product];
                    if (old == 0) {
                        internal_error("Gray-code representation underflow");
                    }
                    if (old == 1) {
                        --unique;
                    }
                    representations[product] = (uint16_t)(old - 1);
                    if (old == 2) {
                        ++unique;
                    }
                }
            }

            increment_nodes(solver);
            node_heartbeat(solver);
            if (b_size >= a_size && unique > solver->best) {
                accept_candidate(solver, a_mask, b_mask, unique);
            }
            previous = b_mask;
        }

        if (solver->progress &&
            (a_mask & OUTER_HEARTBEAT_MASK) == UINT64_C(0)) {
            report_heartbeat(solver);
        }
    }
    return finish_solve(solver);
}

static void initialize_solver(Solver *solver, int n, bool progress)
{
    memset(solver, 0, sizeof(*solver));
    solver->n = n;
    solver->progress = progress;
    solver->all_mask = (UINT64_C(1) << n) - UINT64_C(1);
}

static void print_set(FILE *stream, uint64_t mask, int n)
{
    bool first = true;
    fputc('{', stream);
    for (int value = 1; value <= n; ++value) {
        if ((mask & (UINT64_C(1) << (value - 1))) != 0) {
            if (!first) {
                fputc(',', stream);
            }
            fprintf(stream, "%d", value);
            first = false;
        }
    }
    fputc('}', stream);
}

static void print_witness(const Solver *solver)
{
    fprintf(stderr, "%s: N=%d A=", PROGRAM_NAME, solver->n);
    print_set(stderr, solver->best_a, solver->n);
    fputs(" B=", stderr);
    print_set(stderr, solver->best_b, solver->n);
    fputc('\n', stderr);
}

static void file_error(const char *operation, const char *path)
{
    fprintf(stderr, "%s: %s '%s': %s\n",
            PROGRAM_NAME, operation, path, strerror(errno));
    exit(EXIT_FAILURE);
}

static int safe_open_flags(int base_flags)
{
#ifdef O_CLOEXEC
    base_flags |= O_CLOEXEC;
#endif
#ifdef O_NOFOLLOW
    base_flags |= O_NOFOLLOW;
#endif
    return base_flags;
}

static void require_regular_file(int fd, const char *path)
{
    struct stat status;
    if (fstat(fd, &status) != 0) {
        file_error("cannot inspect", path);
    }
    if (!S_ISREG(status.st_mode)) {
        fprintf(stderr, "%s: '%s' is not a regular file\n",
                PROGRAM_NAME, path);
        exit(EXIT_FAILURE);
    }
}

static int lock_bfile(const char *output_path)
{
    const size_t length = strlen(output_path);
    if (length > SIZE_MAX - 6U) {
        internal_error("b-file path is too long");
    }
    char *lock_path = malloc(length + 6U);
    if (lock_path == NULL) {
        internal_error("cannot allocate b-file lock path");
    }
    (void)snprintf(lock_path, length + 6U, "%s.lock", output_path);

    const int fd = open(lock_path,
                        safe_open_flags(O_RDWR | O_CREAT),
                        S_IRUSR | S_IWUSR);
    if (fd < 0) {
        file_error("cannot open lock file", lock_path);
    }
    require_regular_file(fd, lock_path);
    free(lock_path);

    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    while (fcntl(fd, F_SETLKW, &lock) != 0) {
        if (errno != EINTR) {
            file_error("cannot lock b-file", output_path);
        }
    }
    return fd;
}

static void unlock_bfile(int fd, const char *output_path)
{
    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    if (fcntl(fd, F_SETLK, &lock) != 0) {
        file_error("cannot unlock b-file", output_path);
    }
    if (close(fd) != 0) {
        file_error("cannot close b-file lock", output_path);
    }
}

/* Return the first missing index.  A399711 starts at index 1 here. */
static int read_bfile(const char *output_path, int values[MAX_N + 1])
{
    const int fd = open(output_path, safe_open_flags(O_RDONLY));
    if (fd < 0) {
        if (errno == ENOENT) {
            return 1;
        }
        file_error("cannot open b-file", output_path);
    }
    require_regular_file(fd, output_path);
    FILE *input = fdopen(fd, "r");
    if (input == NULL) {
        const int saved_errno = errno;
        (void)close(fd);
        errno = saved_errno;
        file_error("cannot create b-file stream", output_path);
    }

    char line[256];
    int next = 1;
    while (fgets(line, sizeof(line), input) != NULL) {
        if (strchr(line, '\n') == NULL && !feof(input)) {
            fprintf(stderr, "%s: b-file '%s' contains an overlong line\n",
                    PROGRAM_NAME, output_path);
            exit(EXIT_FAILURE);
        }
        char *text = line;
        while (*text == ' ' || *text == '\t') {
            ++text;
        }
        if (*text == '\0' || *text == '\n' || *text == '#') {
            continue;
        }
        int index = 0;
        int value = 0;
        char extra = '\0';
        if (sscanf(text, "%d %d %c", &index, &value, &extra) != 2 ||
            index != next || index < 1 || index > MAX_N ||
            value < 0 || value > MAX_PRODUCT) {
            fprintf(stderr,
                    "%s: b-file '%s' is malformed or has a gap at index %d\n",
                    PROGRAM_NAME, output_path, next);
            exit(EXIT_FAILURE);
        }
        values[next++] = value;
    }
    const bool failed = ferror(input) != 0;
    if (fclose(input) != 0 || failed) {
        file_error("cannot finish reading b-file", output_path);
    }
    return next;
}

static void require_recordable(const Options *options, int n)
{
    if (!options->write_bfile) {
        return;
    }
    int values[MAX_N + 1];
    const int lock_fd = lock_bfile(options->output_path);
    const int next = read_bfile(options->output_path, values);
    unlock_bfile(lock_fd, options->output_path);
    if (n > next) {
        fprintf(stderr,
                "%s: '%s' ends before N=%d; compute the missing prefix "
                "first or use --no-bfile\n",
                PROGRAM_NAME, options->output_path, n);
        exit(EXIT_FAILURE);
    }
}

static void record_term(const Options *options, int n, int value)
{
    if (!options->write_bfile) {
        return;
    }

    int values[MAX_N + 1];
    const int lock_fd = lock_bfile(options->output_path);
    int next = read_bfile(options->output_path, values);
    if (n < next) {
        if (values[n] != value) {
            fprintf(stderr,
                    "%s: computed a(%d)=%d disagrees with %s value %d\n",
                    PROGRAM_NAME, n, value, options->output_path, values[n]);
            exit(EXIT_FAILURE);
        }
        unlock_bfile(lock_fd, options->output_path);
        return;
    }
    if (n != next) {
        internal_error("b-file gap while recording");
    }
    values[next++] = value;

    const size_t length = strlen(options->output_path);
    if (length > SIZE_MAX - 12U) {
        internal_error("b-file path is too long");
    }
    char *temporary = malloc(length + 12U);
    if (temporary == NULL) {
        internal_error("cannot allocate temporary b-file path");
    }
    (void)snprintf(temporary, length + 12U, "%s.XXXXXX",
                   options->output_path);
    const int temporary_fd = mkstemp(temporary);
    if (temporary_fd < 0) {
        file_error("cannot create temporary b-file", temporary);
    }
    require_regular_file(temporary_fd, temporary);
    if (fchmod(temporary_fd, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) != 0) {
        file_error("cannot set temporary b-file permissions", temporary);
    }
    FILE *output = fdopen(temporary_fd, "w");
    if (output == NULL) {
        const int saved_errno = errno;
        (void)close(temporary_fd);
        (void)unlink(temporary);
        errno = saved_errno;
        file_error("cannot create temporary b-file stream", temporary);
    }

    bool failed = false;
    for (int index = 1; index < next; ++index) {
        if (fprintf(output, "%d %d\n", index, values[index]) < 0) {
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
    if (failed || rename(temporary, options->output_path) != 0) {
        const int saved_errno = errno;
        (void)unlink(temporary);
        free(temporary);
        errno = saved_errno;
        file_error("cannot atomically update b-file", options->output_path);
    }
    free(temporary);
    unlock_bfile(lock_fd, options->output_path);
}

static bool parse_integer(const char *text, int *result)
{
    char *end = NULL;
    errno = 0;
    const long value = strtol(text, &end, 10);
    if (errno == ERANGE || end == text || *end != '\0' ||
        value < INT_MIN || value > INT_MAX) {
        return false;
    }
    *result = (int)value;
    return true;
}

static void usage(FILE *stream)
{
    fprintf(stream,
            "Usage: ./%s [N] [options]\n"
            "  --start N          start at N when computing a range\n"
            "  --term N           compute only a(N)\n"
            "  --method bnb|gray  exact method (default: bnb)\n"
            "  --progress         report progress about every 10 seconds\n"
            "  --witness          print maximizing sets A and B\n"
            "  --output FILE      save terms to FILE (default: %s)\n"
            "  --no-bfile         do not save terms to a b-file\n"
            "  --check            compare both methods for N=1..10\n"
            "  -h, --help         show this help\n",
            PROGRAM_NAME, DEFAULT_OUTPUT);
}

static bool option_value(int argc, char **argv, int *index,
                         const char *name, const char **value)
{
    const size_t length = strlen(name);
    if (strcmp(argv[*index], name) == 0) {
        if (*index + 1 >= argc) {
            fprintf(stderr, "%s: option %s requires a value\n",
                    PROGRAM_NAME, name);
            return false;
        }
        *value = argv[++(*index)];
        return true;
    }
    if (strncmp(argv[*index], name, length) == 0 &&
        argv[*index][length] == '=') {
        *value = argv[*index] + length + 1;
        return true;
    }
    return false;
}

static bool parse_options(int argc, char **argv, Options *options)
{
    memset(options, 0, sizeof(*options));
    options->start = 1;
    options->method = METHOD_BNB;
    options->write_bfile = true;
    options->output_path = DEFAULT_OUTPUT;

    for (int i = 1; i < argc; ++i) {
        const char *value = NULL;
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            usage(stdout);
            exit(EXIT_SUCCESS);
        } else if (strcmp(argv[i], "--progress") == 0) {
            options->progress = true;
        } else if (strcmp(argv[i], "--witness") == 0) {
            options->witness = true;
        } else if (strcmp(argv[i], "--no-bfile") == 0) {
            options->write_bfile = false;
        } else if (strcmp(argv[i], "--check") == 0) {
            options->check = true;
        } else if (option_value(argc, argv, &i, "--start", &value)) {
            if (!parse_integer(value, &options->start)) {
                fprintf(stderr, "%s: invalid --start value: %s\n",
                        PROGRAM_NAME, value);
                return false;
            }
            options->start_given = true;
        } else if (option_value(argc, argv, &i, "--term", &value)) {
            if (!parse_integer(value, &options->term)) {
                fprintf(stderr, "%s: invalid --term value: %s\n",
                        PROGRAM_NAME, value);
                return false;
            }
            options->term_given = true;
        } else if (option_value(argc, argv, &i, "--method", &value)) {
            if (strcmp(value, "bnb") == 0) {
                options->method = METHOD_BNB;
            } else if (strcmp(value, "gray") == 0) {
                options->method = METHOD_GRAY;
            } else {
                fprintf(stderr, "%s: method must be bnb or gray\n",
                        PROGRAM_NAME);
                return false;
            }
        } else if (option_value(argc, argv, &i, "--output", &value)) {
            if (*value == '\0') {
                fprintf(stderr, "%s: --output requires a nonempty path\n",
                        PROGRAM_NAME);
                return false;
            }
            options->output_path = value;
            options->write_bfile = true;
        } else if (argv[i][0] == '-') {
            fprintf(stderr, "%s: unknown option: %s\n",
                    PROGRAM_NAME, argv[i]);
            return false;
        } else {
            if (options->positional_given ||
                !parse_integer(argv[i], &options->positional)) {
                fprintf(stderr, "%s: invalid or duplicate positional N: %s\n",
                        PROGRAM_NAME, argv[i]);
                return false;
            }
            options->positional_given = true;
        }
    }
    return true;
}

static int run_self_check(void)
{
    static const int expected[11] = {
        0, 1, 2, 4, 7, 10, 13, 18, 22, 29, 32
    };
    uint64_t previous_a = 0;
    uint64_t previous_b = 0;

    for (int n = 1; n <= 10; ++n) {
        Solver bnb;
        Solver gray;
        initialize_solver(&bnb, n, false);
        const int bnb_value = solve_bnb(&bnb, previous_a, previous_b);
        initialize_solver(&gray, n, false);
        const int gray_value = solve_gray(&gray, 0, 0);
        if (bnb_value != gray_value || bnb_value != expected[n]) {
            fprintf(stderr,
                    "%s: self-check failed at N=%d: bnb=%d gray=%d expected=%d\n",
                    PROGRAM_NAME, n, bnb_value, gray_value, expected[n]);
            return EXIT_FAILURE;
        }
        previous_a = bnb.best_a;
        previous_b = bnb.best_b;
        fprintf(stderr, "%s: checked N=%d exact=%d\n",
                PROGRAM_NAME, n, bnb_value);
    }
    fprintf(stderr, "%s: self-check passed through N=10\n", PROGRAM_NAME);
    return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
    Options options;
    if (!parse_options(argc, argv, &options)) {
        usage(stderr);
        return EXIT_FAILURE;
    }

    if (options.check) {
        if (options.start_given || options.term_given ||
            options.positional_given) {
            fprintf(stderr,
                    "%s: --check takes no N, --start, or --term\n",
                    PROGRAM_NAME);
            return EXIT_FAILURE;
        }
        return run_self_check();
    }
    if (options.term_given &&
        (options.start_given || options.positional_given)) {
        fprintf(stderr,
                "%s: do not combine --term with positional N or --start\n",
                PROGRAM_NAME);
        return EXIT_FAILURE;
    }

    const int last = options.term_given ? options.term :
                     (options.positional_given ? options.positional : 10);
    const int first = options.term_given ? options.term : options.start;
    if (first < 1 || last < first || last > MAX_N) {
        fprintf(stderr,
                "%s: range must satisfy 1 <= start <= N <= %d\n",
                PROGRAM_NAME, MAX_N);
        return EXIT_FAILURE;
    }

    uint64_t seed_a = 0;
    uint64_t seed_b = 0;
    for (int n = first; n <= last; ++n) {
        require_recordable(&options, n);
        Solver solver;
        initialize_solver(&solver, n, options.progress);
        const int answer = options.method == METHOD_GRAY ?
            solve_gray(&solver, seed_a, seed_b) :
            solve_bnb(&solver, seed_a, seed_b);
        printf("%d %d\n", n, answer);
        fflush(stdout);
        if (options.witness) {
            print_witness(&solver);
        }
        record_term(&options, n, answer);
        seed_a = solver.best_a;
        seed_b = solver.best_b;
    }
    return EXIT_SUCCESS;
}
