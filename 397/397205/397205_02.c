/*
 * A397205 -- a low-memory exact solver using simultaneous constraint search.
 *
 * This implementation is deliberately different from 397205_01.c.  It does
 * not enumerate one set and solve a maximum-independent-set problem for the
 * other, and it has no large transposition table.  For fixed target sizes it
 * chooses S and T simultaneously as a monotone SAT/cardinality problem.
 *
 * For each a<b and c<d, directly test a*d == b*c.  Equality forbids selecting
 * all four membership variables and gives the negative clause
 *
 *                 not (S_a and S_b and T_c and T_d).
 *
 * The DPLL search below propagates these clauses together with exact
 * cardinalities.  It has no ratio graph, coloring bound, clique search, or
 * product upper bound at a search node.  Primes q with n/2<q<=n are removed
 * from the SAT instance and distributed afterwards (see build_problem).
 * Memory use is O(n^4) with tiny constants (well below 1 MiB for n<=39), not
 * proportional to the number of search nodes.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic 397205_02.c -o 397205_02
 *
 * Examples:
 *   ./397205_02 35 --progress --witness
 *   ./397205_02 --term 35 --progress --witness --no-bfile
 *   ./397205_02 --check --no-bfile
 *   ./397205_02 --check-all --no-bfile
 *
 * The hard limit is 39, but this independent verifier favors simple pruning
 * over speed.  --check tests every feasible size pair for n<=9 against direct
 * enumeration; --check-all additionally recomputes all terms through n=35.
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
#include <time.h>
#include <unistd.h>

#define MAX_N 39
#define MAX_VARS (2 * MAX_N)
#define MAX_CLAUSES 4096
#define MAX_INCIDENCES (4 * MAX_CLAUSES)
#define DEFAULT_N 20

typedef struct {
    uint8_t length;
    uint8_t variable[4];
} Clause;

typedef struct {
    int n;
    int core_n;
    int free_n;
    int clause_count;
    int variable_count;
    int core_value[MAX_N];
    int free_value[MAX_N];
    Clause clause[MAX_CLAUSES];
    int incidence_offset[MAX_VARS + 1];
    uint16_t incidence[MAX_INCIDENCES];
} Problem;

typedef struct {
    Problem *problem;
    int target[2];
    bool disjoint;
    int8_t assignment[MAX_VARS];
    uint8_t clause_ones[MAX_CLAUSES];
    uint8_t clause_unset[MAX_CLAUSES];
    int ones[2];
    int unset[2];
    uint8_t trail[MAX_VARS];
    int trail_size;
    uint64_t nodes;
    uint64_t solution_s;
    uint64_t solution_t;
    double started;
    double next_heartbeat;
    int final_a, final_b;
    int case_number, case_count;
} CSP;

typedef struct {
    int8_t status[2][MAX_N + 1][MAX_N + 1];
    uint64_t s[2][MAX_N + 1][MAX_N + 1];
    uint64_t t[2][MAX_N + 1][MAX_N + 1];
} SizeCache;

static const char *output_path = "b397205_02.txt";
static bool write_bfile = true;
static bool show_progress = false;
static bool show_witness = false;

static const int known[36] = {
    0,
    1, 2, 4, 6, 9, 12, 16, 20, 25, 28,
    35, 40, 48, 50, 55, 60, 72, 78, 91, 98,
    105, 105, 120, 128, 144, 144, 153, 162, 180, 190,
    210, 220, 231, 231, 242
};

static const uint64_t known_witness_s[36] = {
    UINT64_C(0x0), UINT64_C(0x1), UINT64_C(0x1), UINT64_C(0x3),
    UINT64_C(0x3), UINT64_C(0x7), UINT64_C(0xf), UINT64_C(0xf),
    UINT64_C(0x2f), UINT64_C(0x2f), UINT64_C(0xbf), UINT64_C(0xbf),
    UINT64_C(0x8bf), UINT64_C(0x8bf), UINT64_C(0xaff), UINT64_C(0xbff),
    UINT64_C(0x2bff), UINT64_C(0x2bff), UINT64_C(0x22bff),
    UINT64_C(0x22bff), UINT64_C(0xa2bff), UINT64_C(0x2ebff),
    UINT64_C(0x2ebff), UINT64_C(0x2ebff), UINT64_C(0x82ebff),
    UINT64_C(0x8aefff), UINT64_C(0x8aefff), UINT64_C(0x9a6bff),
    UINT64_C(0x9acfff), UINT64_C(0xbadfff), UINT64_C(0x209acfff),
    UINT64_C(0x20badfff), UINT64_C(0x28ba7fff), UINT64_C(0x1289a6fff),
    UINT64_C(0x1289a6fff), UINT64_C(0x52c8a6fff)
};

static const uint64_t known_witness_t[36] = {
    UINT64_C(0x0), UINT64_C(0x1), UINT64_C(0x3), UINT64_C(0x5),
    UINT64_C(0xd), UINT64_C(0x19), UINT64_C(0x31), UINT64_C(0x71),
    UINT64_C(0xd1), UINT64_C(0x1d1), UINT64_C(0x341), UINT64_C(0x741),
    UINT64_C(0x741), UINT64_C(0x1741), UINT64_C(0x3501),
    UINT64_C(0x7401), UINT64_C(0xd401), UINT64_C(0x1d401),
    UINT64_C(0x1d401), UINT64_C(0x5d401), UINT64_C(0x5d401),
    UINT64_C(0x1d1401), UINT64_C(0x1d1401), UINT64_C(0x5d1401),
    UINT64_C(0x5d1401), UINT64_C(0x1751001), UINT64_C(0x1751001),
    UINT64_C(0x5459401), UINT64_C(0xd651001), UINT64_C(0x1f450001),
    UINT64_C(0x1d651001), UINT64_C(0x5f450001), UINT64_C(0xd7450001),
    UINT64_C(0xd5651001), UINT64_C(0xd5651001), UINT64_C(0xd1751001)
};

static _Noreturn void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static double now_seconds(void)
{
    struct timespec value;
    if (clock_gettime(CLOCK_MONOTONIC, &value) != 0)
        die("clock_gettime failed");
    return (double)value.tv_sec + (double)value.tv_nsec / 1e9;
}

static unsigned popcount64(uint64_t x)
{
    return (unsigned)__builtin_popcountll(x);
}

static bool is_prime(int x)
{
    if (x < 2) return false;
    for (int d = 2; d * d <= x; ++d)
        if (x % d == 0) return false;
    return true;
}

static uint64_t universe_mask(int n)
{
    return (UINT64_C(1) << (unsigned)n) - UINT64_C(1);
}

static bool product_distinct(int n, uint64_t s_mask, uint64_t t_mask)
{
    bool seen[MAX_N * MAX_N + 1] = {false};
    for (int s = 1; s <= n; ++s) {
        if ((s_mask & (UINT64_C(1) << (unsigned)(s - 1))) == 0) continue;
        for (int t = 1; t <= n; ++t) {
            if ((t_mask & (UINT64_C(1) << (unsigned)(t - 1))) == 0) continue;
            if (seen[s * t]) return false;
            seen[s * t] = true;
        }
    }
    return true;
}

static int parse_n(const char *text)
{
    char *end = NULL;
    errno = 0;
    const long value = strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' ||
        value < 1 || value > MAX_N) {
        fprintf(stderr, "error: N must be in 1..%d: %s\n", MAX_N, text);
        exit(EXIT_FAILURE);
    }
    return (int)value;
}

static void add_clause(Problem *p, int length, int a, int b, int c, int d)
{
    if (p->clause_count >= MAX_CLAUSES) die("internal clause capacity exceeded");
    Clause *cl = &p->clause[p->clause_count++];
    cl->length = (uint8_t)length;
    cl->variable[0] = (uint8_t)a;
    cl->variable[1] = (uint8_t)b;
    if (length == 4) {
        cl->variable[2] = (uint8_t)c;
        cl->variable[3] = (uint8_t)d;
    }
}

static void build_problem(Problem *p, int n, bool remove_free_primes)
{
    memset(p, 0, sizeof(*p));
    p->n = n;

    /* If q>n/2 is prime, q*t=s*u with s!=q forces u=q and then t=s.
     * Consequently a q assigned to just one side cannot create a collision.
     * A q assigned to both sides is permitted precisely when the rest of the
     * two sets are disjoint; at most one prime may be shared.  Tests also run
     * the unreduced problem to validate this preprocessing independently. */
    for (int value = 1; value <= n; ++value) {
        const bool free_vertex = remove_free_primes &&
            is_prime(value) && 2 * value > n;
        if (free_vertex) p->free_value[p->free_n++] = value;
        else p->core_value[p->core_n++] = value;
    }
    p->variable_count = 2 * p->core_n;

    /* Two distinct equal products must have different row and column indices.
     * Order the rows as a<b; positivity then orders the columns as c<d and
     * the equality is a*d=b*c.  These loops enumerate every such collision. */
    for (int a = 0; a < p->core_n; ++a)
        for (int b = a + 1; b < p->core_n; ++b)
            for (int c = 0; c < p->core_n; ++c)
                for (int d = c + 1; d < p->core_n; ++d)
                    if (p->core_value[a] * p->core_value[d] ==
                        p->core_value[b] * p->core_value[c])
                        add_clause(p, 4, a, b, p->core_n + c, p->core_n + d);

    int counts[MAX_VARS] = {0};
    for (int c = 0; c < p->clause_count; ++c)
        for (int j = 0; j < p->clause[c].length; ++j)
            ++counts[p->clause[c].variable[j]];
    for (int v = 0; v < p->variable_count; ++v)
        p->incidence_offset[v + 1] = p->incidence_offset[v] + counts[v];
    if (p->incidence_offset[p->variable_count] > MAX_INCIDENCES)
        die("internal incidence capacity exceeded");
    int cursor[MAX_VARS];
    memcpy(cursor, p->incidence_offset,
           (size_t)p->variable_count * sizeof(cursor[0]));
    for (int c = 0; c < p->clause_count; ++c)
        for (int j = 0; j < p->clause[c].length; ++j) {
            const int v = p->clause[c].variable[j];
            p->incidence[cursor[v]++] = (uint16_t)c;
        }
}

static void csp_heartbeat(CSP *csp)
{
    if (!show_progress || (csp->nodes & UINT64_C(0x3ffff)) != 0) return;
    const double elapsed = now_seconds() - csp->started;
    if (elapsed < csp->next_heartbeat) return;
    fprintf(stderr,
            "397205_02: progress target=%dx%d case=%d/%d "
            "core=%dx%d nodes=%" PRIu64 " depth=%d elapsed=%.1f s\n",
            csp->final_a, csp->final_b, csp->case_number, csp->case_count,
            csp->target[0], csp->target[1], csp->nodes, csp->trail_size,
            elapsed);
    csp->next_heartbeat = elapsed + 10.0;
}

static void undo_to(CSP *csp, int mark)
{
    const Problem *p = csp->problem;
    while (csp->trail_size > mark) {
        const int v = csp->trail[--csp->trail_size];
        const int value = csp->assignment[v];
        const int group = v >= p->core_n;
        for (int k = p->incidence_offset[v];
             k < p->incidence_offset[v + 1]; ++k) {
            const int ci = p->incidence[k];
            ++csp->clause_unset[ci];
            if (value == 1) --csp->clause_ones[ci];
        }
        if (value == 1) --csp->ones[group];
        ++csp->unset[group];
        csp->assignment[v] = -1;
    }
}

static bool assign_value(CSP *csp, int variable, int value)
{
    Problem *p = csp->problem;
    if (csp->assignment[variable] >= 0)
        return csp->assignment[variable] == value;

    csp->assignment[variable] = (int8_t)value;
    csp->trail[csp->trail_size++] = (uint8_t)variable;
    const int group = variable >= p->core_n;
    --csp->unset[group];
    if (value == 1) ++csp->ones[group];

    for (int k = p->incidence_offset[variable];
         k < p->incidence_offset[variable + 1]; ++k) {
        const int ci = p->incidence[k];
        --csp->clause_unset[ci];
        if (value == 1) ++csp->clause_ones[ci];
    }

    if (csp->ones[group] > csp->target[group] ||
        csp->ones[group] + csp->unset[group] < csp->target[group])
        return false;

    if (csp->disjoint && value == 1) {
        const int mate = group == 0 ? variable + p->core_n
                                    : variable - p->core_n;
        if (!assign_value(csp, mate, 0)) return false;
    }

    for (int k = p->incidence_offset[variable];
         k < p->incidence_offset[variable + 1]; ++k) {
        const int ci = p->incidence[k];
        const Clause *cl = &p->clause[ci];
        if (csp->clause_ones[ci] == cl->length) return false;
        if (csp->clause_ones[ci] + 1U == cl->length &&
            csp->clause_unset[ci] == 1) {
            int forced = -1;
            for (int j = 0; j < cl->length; ++j)
                if (csp->assignment[cl->variable[j]] < 0) {
                    forced = cl->variable[j];
                    break;
                }
            if (forced < 0 || !assign_value(csp, forced, 0)) return false;
        }
    }

    for (int g = 0; g < 2; ++g) {
        if (csp->ones[g] > csp->target[g] ||
            csp->ones[g] + csp->unset[g] < csp->target[g])
            return false;
        int forced_value = -1;
        if (csp->ones[g] == csp->target[g]) forced_value = 0;
        else if (csp->ones[g] + csp->unset[g] == csp->target[g])
            forced_value = 1;
        if (forced_value >= 0) {
            const int begin = g * p->core_n;
            const int end = begin + p->core_n;
            for (int v = begin; v < end; ++v)
                if (csp->assignment[v] < 0 &&
                    !assign_value(csp, v, forced_value)) return false;
        }
    }
    return true;
}

static int choose_variable(const CSP *csp)
{
    const Problem *p = csp->problem;
    int best = -1;
    uint64_t best_score = 0;
    for (int v = 0; v < p->variable_count; ++v) {
        if (csp->assignment[v] >= 0) continue;
        uint64_t score = 0;
        for (int k = p->incidence_offset[v];
             k < p->incidence_offset[v + 1]; ++k) {
            const int ci = p->incidence[k];
            const Clause *cl = &p->clause[ci];
            if ((int)csp->clause_ones[ci] +
                (int)csp->clause_unset[ci] != cl->length) continue;
            score += UINT64_C(1) << (3U * csp->clause_ones[ci]);
        }
        const int group = v >= p->core_n;
        const int need = csp->target[group] - csp->ones[group];
        score = score * (uint64_t)(csp->unset[group] + need + 1) /
                (uint64_t)(csp->unset[group] + 1);
        if (best < 0 || score > best_score) {
            best = v;
            best_score = score;
        }
    }
    return best;
}


static bool csp_dfs(CSP *csp)
{
    ++csp->nodes;
    csp_heartbeat(csp);
    if (csp->ones[0] == csp->target[0] &&
        csp->ones[1] == csp->target[1]) {
        uint64_t s = 0, t = 0;
        for (int i = 0; i < csp->problem->core_n; ++i) {
            if (csp->assignment[i] == 1)
                s |= UINT64_C(1) << (unsigned)(csp->problem->core_value[i] - 1);
            if (csp->assignment[csp->problem->core_n + i] == 1)
                t |= UINT64_C(1) << (unsigned)(csp->problem->core_value[i] - 1);
        }
        csp->solution_s = s;
        csp->solution_t = t;
        return true;
    }
    const int variable = choose_variable(csp);
    if (variable < 0) return false;

    const int mark = csp->trail_size;
    if (assign_value(csp, variable, 1) && csp_dfs(csp)) return true;
    undo_to(csp, mark);
    if (assign_value(csp, variable, 0) && csp_dfs(csp)) return true;
    undo_to(csp, mark);
    return false;
}

static bool solve_core(Problem *p, int target_s, int target_t, bool disjoint,
                       int final_a, int final_b, int case_number, int case_count,
                       uint64_t *answer_s, uint64_t *answer_t,
                       uint64_t *nodes)
{
    if (target_s < 0 || target_t < 0 || target_s > p->core_n ||
        target_t > p->core_n || target_s + target_t > p->core_n + 1 ||
        (disjoint && target_s + target_t > p->core_n)) return false;
    CSP csp;
    memset(&csp, 0, sizeof(csp));
    csp.problem = p;
    csp.target[0] = target_s;
    csp.target[1] = target_t;
    csp.disjoint = disjoint;
    csp.unset[0] = csp.unset[1] = p->core_n;
    memset(csp.assignment, -1, (size_t)p->variable_count);
    for (int i = 0; i < p->clause_count; ++i)
        csp.clause_unset[i] = p->clause[i].length;
    csp.started = now_seconds();
    csp.next_heartbeat = 10.0;
    csp.final_a = final_a;
    csp.final_b = final_b;
    csp.case_number = case_number;
    csp.case_count = case_count;

    bool initial_ok = true;
    if (target_s == 0)
        for (int v = 0; v < p->core_n && initial_ok; ++v)
            initial_ok = assign_value(&csp, v, 0);
    if (target_t == 0)
        for (int v = p->core_n; v < 2 * p->core_n && initial_ok; ++v)
            initial_ok = assign_value(&csp, v, 0);
    const bool found = initial_ok && csp_dfs(&csp);
    *nodes += csp.nodes;
    if (found) {
        *answer_s = csp.solution_s;
        *answer_t = csp.solution_t;
    }
    return found;
}

typedef struct {
    int common, s_only, t_only;
    int core_s, core_t;
} FreeCase;

static int compare_free_case(const void *left, const void *right)
{
    const FreeCase *a = left, *b = right;
    const int suma = a->core_s + a->core_t;
    const int sumb = b->core_s + b->core_t;
    if (suma != sumb) return suma - sumb;
    const int bala = a->core_s > a->core_t ? a->core_s - a->core_t
                                           : a->core_t - a->core_s;
    const int balb = b->core_s > b->core_t ? b->core_s - b->core_t
                                           : b->core_t - b->core_s;
    return bala - balb;
}

static uint64_t take_elements(uint64_t set, int count)
{
    uint64_t result = 0;
    while (count-- > 0) {
        if (set == 0) die("internal cached-witness size error");
        const unsigned bit = (unsigned)__builtin_ctzll(set);
        const uint64_t one = UINT64_C(1) << bit;
        result |= one;
        set &= ~one;
    }
    return result;
}

/* Feasibility is downward closed in both target sizes.  Infeasibility is
 * upward closed.  Exploiting this tiny 2-D cache avoids solving many nearly
 * identical cardinality instances without storing any search states. */
static int size_cache_query(const SizeCache *cache, int disjoint, int a, int b,
                            uint64_t *s, uint64_t *t)
{
    for (int mode = disjoint; mode <= 1; ++mode)
        for (int i = a; i <= MAX_N; ++i)
            for (int j = b; j <= MAX_N; ++j)
                if (cache->status[mode][i][j] > 0) {
                    *s = take_elements(cache->s[mode][i][j], a);
                    *t = take_elements(cache->t[mode][i][j], b);
                    return 1;
                }
    for (int mode = 0; mode <= disjoint; ++mode)
        for (int i = 0; i <= a; ++i)
            for (int j = 0; j <= b; ++j)
                if (cache->status[mode][i][j] < 0) return -1;
    return 0;
}

static uint64_t free_prime_bit(const Problem *p, int index)
{
    if (index < 0 || index >= p->free_n) die("internal free-prime index error");
    const int value = p->free_value[index];
    if (value < 1 || value > MAX_N) die("internal free-prime value error");
    return UINT64_C(1) << (unsigned)(value - 1);
}

static bool feasible_sizes(Problem *p, SizeCache *cache, int final_s, int final_t,
                           uint64_t *answer_s, uint64_t *answer_t,
                           uint64_t *total_nodes)
{
    FreeCase cases[2 * (MAX_N + 1) * (MAX_N + 1)];
    int case_count = 0;
    for (int common = 0; common <= 1 && common <= p->free_n; ++common) {
        for (int so = 0; so <= p->free_n - common; ++so) {
            for (int to = 0; to <= p->free_n - common - so; ++to) {
                /* When each requested size exceeds the number of free primes,
                 * an unused free prime can replace a core element on a side
                 * without changing its size or introducing a collision.  Thus
                 * some solution uses every free prime, if a solution exists. */
                if (final_s > p->free_n && final_t > p->free_n &&
                    common + so + to != p->free_n)
                    continue;
                const int cs = final_s - so - common;
                const int ct = final_t - to - common;
                if (cs < 0 || ct < 0 || cs > p->core_n || ct > p->core_n)
                    continue;
                cases[case_count++] = (FreeCase){common, so, to, cs, ct};
            }
        }
    }
    qsort(cases, (size_t)case_count, sizeof(cases[0]), compare_free_case);

    for (int k = 0; k < case_count; ++k) {
        FreeCase fc = cases[k];
        const int disjoint = fc.common != 0;
        bool swapped = false;
        if (fc.core_s < fc.core_t) {
            const int tmp = fc.core_s;
            fc.core_s = fc.core_t;
            fc.core_t = tmp;
            swapped = true;
        }
        uint64_t core_s = 0, core_t = 0;
        bool found;
        const int cached = size_cache_query(cache, disjoint, fc.core_s,
                                            fc.core_t, &core_s, &core_t);
        if (cached != 0) {
            found = cached > 0;
        } else {
            found = solve_core(p, fc.core_s, fc.core_t, disjoint != 0,
                               final_s, final_t, k + 1, case_count,
                               &core_s, &core_t, total_nodes);
            cache->status[disjoint][fc.core_s][fc.core_t] = found ? 1 : -1;
            if (found) {
                cache->s[disjoint][fc.core_s][fc.core_t] = core_s;
                cache->t[disjoint][fc.core_s][fc.core_t] = core_t;
            }
        }
        if (!found) continue;
        if (swapped) {
            const uint64_t tmp = core_s;
            core_s = core_t;
            core_t = tmp;
        }

        uint64_t s = core_s, t = core_t;
        int free_at = 0;
        if (fc.common) {
            const uint64_t bit = free_prime_bit(p, free_at++);
            s |= bit;
            t |= bit;
        }
        for (int i = 0; i < fc.s_only; ++i)
            s |= free_prime_bit(p, free_at++);
        for (int i = 0; i < fc.t_only; ++i)
            t |= free_prime_bit(p, free_at++);
        if ((int)popcount64(s) != final_s || (int)popcount64(t) != final_t ||
            !product_distinct(p->n, s, t))
            die("internal SAT witness verification failed");
        *answer_s = s;
        *answer_t = t;
        return true;
    }
    return false;
}

static void seed_candidate(int n, uint64_t s, uint64_t t,
                           int *best, uint64_t *best_s, uint64_t *best_t)
{
    const uint64_t all = universe_mask(n);
    s &= all;
    t &= all;
    if (s == 0 || t == 0 || !product_distinct(n, s, t)) return;
    const int product = (int)(popcount64(s) * popcount64(t));
    if (product > *best) {
        *best = product;
        *best_s = s;
        *best_t = t;
    }
}

static void greedy_extend(int n, int *best, uint64_t *best_s, uint64_t *best_t)
{
    bool changed;
    do {
        changed = false;
        int candidate_best = *best;
        uint64_t candidate_s = *best_s, candidate_t = *best_t;
        for (int value = 1; value <= n; ++value) {
            const uint64_t bit = UINT64_C(1) << (unsigned)(value - 1);
            if ((*best_s & bit) == 0) {
                const uint64_t s = *best_s | bit;
                const int product = (int)(popcount64(s) * popcount64(*best_t));
                if (product > candidate_best && product_distinct(n, s, *best_t)) {
                    candidate_best = product;
                    candidate_s = s;
                    candidate_t = *best_t;
                }
            }
            if ((*best_t & bit) == 0) {
                const uint64_t t = *best_t | bit;
                const int product = (int)(popcount64(*best_s) * popcount64(t));
                if (product > candidate_best && product_distinct(n, *best_s, t)) {
                    candidate_best = product;
                    candidate_s = *best_s;
                    candidate_t = t;
                }
            }
        }
        if (candidate_best > *best) {
            *best = candidate_best;
            *best_s = candidate_s;
            *best_t = candidate_t;
            changed = true;
        }
    } while (changed);
}

typedef struct {
    int a, b, distance;
} Frontier;

static int compare_frontier(const void *left, const void *right)
{
    const Frontier *a = left, *b = right;
    if (a->distance != b->distance) return a->distance - b->distance;
    return a->a * a->b - b->a * b->b;
}

static int solve_term(int n, uint64_t seed_s, uint64_t seed_t,
                      uint64_t *answer_s, uint64_t *answer_t,
                      bool use_known_witness)
{
    Problem problem;
    build_problem(&problem, n, true);
    SizeCache cache;
    memset(&cache, 0, sizeof(cache));
    int best = 0;
    uint64_t best_s = 0, best_t = 0;
    seed_candidate(n, seed_s, seed_t, &best, &best_s, &best_t);
    if (use_known_witness) {
        const int wn = n < 36 ? n : 35;
        seed_candidate(n, known_witness_s[wn], known_witness_t[wn],
                       &best, &best_s, &best_t);
    }
    seed_candidate(n, UINT64_C(1), universe_mask(n), &best, &best_s, &best_t);
    greedy_extend(n, &best, &best_s, &best_t);
    const double started = now_seconds();
    uint64_t total_nodes = 0;
    if (show_progress)
        fprintf(stderr,
                "397205_02: start n=%d lower_bound=%d core=%d free=%d "
                "clauses=%d mode=direct-product-CSP work_memory<1MiB\n",
                n, best, problem.core_n, problem.free_n,
                problem.clause_count);

    for (;;) {
        Frontier frontier[MAX_N];
        int frontier_count = 0;
        int preferred = (int)popcount64(best_s);
        const int other = (int)popcount64(best_t);
        if (preferred > other) preferred = other;
        for (int b = 1; b <= (n + 1) / 2; ++b) {
            int a = best / b + 1;
            if (a < b) a = b;
            if (a > n || a + b > n + 1) continue;
            const int distance = b > preferred ? b - preferred : preferred - b;
            frontier[frontier_count++] = (Frontier){a, b, distance};
        }
        qsort(frontier, (size_t)frontier_count, sizeof(frontier[0]),
              compare_frontier);

        bool improved = false;
        for (int i = 0; i < frontier_count; ++i) {
            const int a = frontier[i].a, b = frontier[i].b;
            const double test_started = now_seconds();
            const uint64_t before = total_nodes;
            if (show_progress)
                fprintf(stderr, "397205_02: test n=%d target=%dx%d (%d)\n",
                        n, a, b, a * b);
            uint64_t s, t;
            if (feasible_sizes(&problem, &cache, a, b, &s, &t, &total_nodes)) {
                best = a * b;
                best_s = s;
                best_t = t;
                greedy_extend(n, &best, &best_s, &best_t);
                if (show_progress)
                    fprintf(stderr,
                            "397205_02: improved n=%d lower_bound=%d "
                            "nodes=%" PRIu64 " %.3f s\n",
                            n, best, total_nodes - before,
                            now_seconds() - test_started);
                improved = true;
                break;
            }
            if (show_progress)
                fprintf(stderr,
                        "397205_02: ruled_out n=%d target=%dx%d "
                        "nodes=%" PRIu64 " %.3f s\n",
                        n, a, b, total_nodes - before,
                        now_seconds() - test_started);
        }
        if (!improved) break;
    }

    if (!product_distinct(n, best_s, best_t) ||
        (int)(popcount64(best_s) * popcount64(best_t)) != best)
        die("internal final witness verification failed");
    if (n < 36 && best != known[n]) {
        fprintf(stderr, "error: regression mismatch at n=%d: got %d expected %d\n",
                n, best, known[n]);
        exit(EXIT_FAILURE);
    }
    if (show_progress)
        fprintf(stderr,
                "397205_02: n=%d exact=%d nodes=%" PRIu64 " %.3f s\n",
                n, best, total_nodes, now_seconds() - started);
    *answer_s = best_s;
    *answer_t = best_t;
    return best;
}

static void print_set(FILE *stream, int n, uint64_t mask)
{
    fputc('{', stream);
    bool first = true;
    for (int value = 1; value <= n; ++value) {
        if ((mask & (UINT64_C(1) << (unsigned)(value - 1))) == 0) continue;
        if (!first) fputc(',', stream);
        fprintf(stream, "%d", value);
        first = false;
    }
    fputc('}', stream);
}

static int lock_bfile(void)
{
    char *path = malloc(strlen(output_path) + 6U);
    if (path == NULL) die("cannot allocate lock path");
    sprintf(path, "%s.lock", output_path);
    const int fd = open(path, O_RDWR | O_CREAT, 0666);
    free(path);
    if (fd < 0) die("cannot open b-file lock");
    struct flock lock = {.l_type = F_WRLCK, .l_whence = SEEK_SET};
    while (fcntl(fd, F_SETLKW, &lock) != 0)
        if (errno != EINTR) die("cannot lock b-file");
    return fd;
}

static void unlock_bfile(int fd)
{
    struct flock lock = {.l_type = F_UNLCK, .l_whence = SEEK_SET};
    if (fcntl(fd, F_SETLK, &lock) != 0 || close(fd) != 0)
        die("cannot unlock b-file");
}

static int read_bfile(int values[MAX_N + 1])
{
    FILE *input = fopen(output_path, "r");
    if (input == NULL) {
        if (errno == ENOENT) return 1;
        die("cannot read b-file");
    }
    char line[256];
    int next = 1;
    while (fgets(line, sizeof(line), input) != NULL) {
        char *p = line;
        while (*p == ' ' || *p == '\t') ++p;
        if (*p == '\0' || *p == '\n' || *p == '#') continue;
        int index, value;
        char extra;
        if (sscanf(p, "%d %d %c", &index, &value, &extra) != 2 ||
            index != next || index < 1 || index > MAX_N || value < 1) {
            fclose(input);
            die("b-file is malformed or has a gap");
        }
        values[next++] = value;
    }
    if (ferror(input) || fclose(input) != 0) die("cannot finish b-file read");
    return next;
}

static void require_recordable(int n)
{
    if (!write_bfile) return;
    int values[MAX_N + 1];
    const int fd = lock_bfile();
    const int next = read_bfile(values);
    unlock_bfile(fd);
    if (n > next) die("b-file prefix is missing; use --no-bfile or compute it first");
}

static void record_term(int n, int value)
{
    if (!write_bfile) return;
    int values[MAX_N + 1];
    const int fd = lock_bfile();
    int next = read_bfile(values);
    if (n < next) {
        if (values[n] != value) die("computed value disagrees with b-file");
        unlock_bfile(fd);
        return;
    }
    if (n != next) die("b-file gap while recording");
    values[next++] = value;
    char temporary[4096];
    if (snprintf(temporary, sizeof(temporary), "%s.tmp.%ld", output_path,
                 (long)getpid()) >= (int)sizeof(temporary))
        die("output path is too long");
    FILE *output = fopen(temporary, "w");
    if (output == NULL) die("cannot create temporary b-file");
    bool failed = false;
    for (int i = 1; i < next; ++i)
        if (fprintf(output, "%d %d\n", i, values[i]) < 0) failed = true;
    if (!failed && fflush(output) != 0) failed = true;
    if (!failed && fsync(fileno(output)) != 0) failed = true;
    if (fclose(output) != 0) failed = true;
    if (failed || rename(temporary, output_path) != 0) {
        unlink(temporary);
        die("cannot atomically update b-file");
    }
    unlock_bfile(fd);
}

static int brute_force_term(int n)
{
    const uint64_t limit = UINT64_C(1) << (unsigned)n;
    int best = 0;
    for (uint64_t s = 1; s < limit; ++s)
        for (uint64_t t = 1; t < limit; ++t) {
            const int product = (int)(popcount64(s) * popcount64(t));
            if (product > best && product_distinct(n, s, t)) best = product;
        }
    return best;
}

static void check_witness(int n, int a, int b, uint64_t s, uint64_t t,
                           bool disjoint)
{
    if ((int)popcount64(s) != a || (int)popcount64(t) != b ||
        ((s | t) & ~universe_mask(n)) != 0 ||
        (disjoint && (s & t) != 0) || !product_distinct(n, s, t))
        die("self-check: invalid feasibility witness");
}

/* Check SAT construction, DPLL, prime removal and size-cache pruning against
 * a complete truth table built from multiplication alone, not just the maximum
 * objective value.  This catches false infeasibility even below a known seed. */
static int check_all_sizes(int n)
{
    Problem full, reduced;
    build_problem(&full, n, false);
    build_problem(&reduced, n, true);
    bool feasible[2][MAX_N + 1][MAX_N + 1] = {{{false}}};
    const uint64_t limit = UINT64_C(1) << (unsigned)n;
    int maximum = 0;
    for (uint64_t s = 0; s < limit; ++s) {
        for (uint64_t t = 0; t < limit; ++t) {
            const bool valid = product_distinct(n, s, t);
            bool satisfies = true;
            const uint64_t assignment = s | (t << (unsigned)n);
            for (int i = 0; i < full.clause_count && satisfies; ++i) {
                const Clause *cl = &full.clause[i];
                bool all_selected = true;
                for (int j = 0; j < cl->length; ++j)
                    if ((assignment & (UINT64_C(1) << cl->variable[j])) == 0)
                        all_selected = false;
                if (all_selected) satisfies = false;
            }
            if (satisfies != valid) die("self-check: clause encoding mismatch");
            if (!valid) continue;
            const int a = (int)popcount64(s), b = (int)popcount64(t);
            feasible[0][a][b] = true;
            if ((s & t) == 0) feasible[1][a][b] = true;
            if (a * b > maximum) maximum = a * b;
        }
    }

    uint64_t nodes = 0;
    for (int disjoint = 0; disjoint <= 1; ++disjoint)
        for (int a = 0; a <= n; ++a)
            for (int b = 0; b <= n; ++b) {
                uint64_t s = 0, t = 0;
                const bool found = solve_core(&full, a, b, disjoint != 0,
                                               a, b, 1, 1, &s, &t, &nodes);
                if (found != feasible[disjoint][a][b])
                    die("self-check: unreduced fixed-size search mismatch");
                if (found) check_witness(n, a, b, s, t, disjoint != 0);
            }

    for (int descending = 0; descending <= 1; ++descending) {
        SizeCache cache;
        memset(&cache, 0, sizeof(cache));
        for (int i = 0; i <= n; ++i)
            for (int j = 0; j <= n; ++j) {
                const int a = descending ? n - i : i;
                const int b = descending ? n - j : j;
                uint64_t s = 0, t = 0;
                const bool found = feasible_sizes(&reduced, &cache, a, b,
                                                   &s, &t, &nodes);
                if (found != feasible[0][a][b])
                    die("self-check: prime reduction or size-cache mismatch");
                if (found) check_witness(n, a, b, s, t, false);
            }
    }
    return maximum;
}

static void self_check(bool all_terms)
{
    uint64_t seed_s = 0, seed_t = 0;
    for (int n = 1; n <= 10; ++n) {
        uint64_t s, t;
        const int want = n <= 9 ? check_all_sizes(n) : brute_force_term(n);
        const int got = solve_term(n, seed_s, seed_t, &s, &t, true);
        if (got != want) die("self-check disagrees with direct brute force");
        seed_s = s;
        seed_t = t;
        if (solve_term(n, 0, 0, &s, &t, false) != want)
            die("self-check: unseeded search mismatch");
        fprintf(stderr, "397205_02: checked n=%d exact=%d%s\n", n, want,
                n <= 9 ? " (all size pairs, reduced/unreduced)" : "");
    }
    if (all_terms) {
        for (int n = 11; n <= 35; ++n) {
            uint64_t s, t;
            const int got = solve_term(n, seed_s, seed_t, &s, &t, true);
            if (got != known[n]) die("self-check: known term mismatch");
            seed_s = s;
            seed_t = t;
            fprintf(stderr, "397205_02: checked n=%d exact=%d\n", n, got);
        }
    }
    fprintf(stderr, "397205_02: self-check passed through n=%d\n",
            all_terms ? 35 : 10);
}

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s [N] [--start N] [--progress] [--witness] "
            "[--output FILE|--no-bfile]\n"
            "       %s --term N [--progress] [--witness] "
            "[--output FILE|--no-bfile]\n"
            "       %s --check|--check-all [--progress] [--no-bfile]\n",
            program, program, program);
}

int main(int argc, char **argv)
{
    int n = DEFAULT_N, start_n = 1;
    bool positional = false, term_only = false, check = false, have_start = false;
    bool check_all = false;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--term") == 0) {
            if (term_only || positional || check || have_start || ++i == argc)
                die("invalid --term usage");
            term_only = true;
            n = parse_n(argv[i]);
        } else if (strcmp(argv[i], "--start") == 0) {
            if (term_only || check || have_start || ++i == argc)
                die("invalid --start usage");
            have_start = true;
            start_n = parse_n(argv[i]);
        } else if (strcmp(argv[i], "--check") == 0 ||
                   strcmp(argv[i], "--check-all") == 0) {
            if (term_only || positional || check || have_start)
                die("invalid --check usage");
            check = true;
            check_all = strcmp(argv[i], "--check-all") == 0;
        } else if (strcmp(argv[i], "--progress") == 0 ||
                   strcmp(argv[i], "--stats") == 0) {
            show_progress = true;
        } else if (strcmp(argv[i], "--witness") == 0) {
            show_witness = true;
        } else if (strcmp(argv[i], "--output") == 0) {
            if (++i == argc || argv[i][0] == '\0') die("--output needs a file");
            output_path = argv[i];
            write_bfile = true;
        } else if (strcmp(argv[i], "--no-bfile") == 0) {
            write_bfile = false;
        } else if (strcmp(argv[i], "--help") == 0 ||
                   strcmp(argv[i], "-h") == 0) {
            usage(argv[0]);
            return EXIT_SUCCESS;
        } else if (argv[i][0] == '-') {
            usage(argv[0]);
            return EXIT_FAILURE;
        } else {
            if (positional || term_only || check) die("multiple N arguments");
            positional = true;
            n = parse_n(argv[i]);
        }
    }
    if (check) {
        self_check(check_all);
        return EXIT_SUCCESS;
    }
    if (have_start && start_n > n) die("--start must not exceed N");

    const int first = term_only ? n : start_n;
    uint64_t seed_s = 0, seed_t = 0;
    for (int k = first; k <= n; ++k) {
        require_recordable(k);
        uint64_t s, t;
        const int answer = solve_term(k, seed_s, seed_t, &s, &t, true);
        printf("%d %d\n", k, answer);
        if (fflush(stdout) != 0) die("cannot flush result");
        if (show_witness) {
            fprintf(stderr, "397205_02: n=%d S=", k);
            print_set(stderr, k, s);
            fprintf(stderr, " T=");
            print_set(stderr, k, t);
            fputc('\n', stderr);
        }
        record_term(k, answer);
        seed_s = s;
        seed_t = t;
    }
    return EXIT_SUCCESS;
}
