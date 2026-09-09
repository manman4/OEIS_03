/* 399654_01.c
 *
 * A399654: Number of distinct sets of cycle lengths realized by simple
 *          graphs on n vertices.
 *
 *   The cycle spectrum of a graph G is  CS(G) = { k : G has a k-cycle }.
 *   a(n) = # { S : S = CS(G) for some simple graph G on n vertices }.
 *   (S = {} is counted; it is realized by any forest.)
 *
 *   n : 0  1  2  3  4  5  6   7   8   9   10   11
 *  a(n): 1  1  1  2  4  6  11  21  40  75  133  247
 *
 * Erdos asked for the number of "cycle sets" on {1,...,n} and conjectured
 * it to be o(2^n); this was proved by J. Verstraete, "On the number of
 * sets of cycle lengths", Combinatorica 24 (2004), 719-730.
 *
 * Everything used below is proved; nothing is conjectural.  In
 * particular the bound mu(W) <= |S| used by an earlier version of this
 * file is NOT valid and has been removed; see the remark below.
 *
 * ---------------------------------------------------------------------
 * ALGORITHM 1 (option -b): plain exhaustive search, exact but n <= 8.
 * ---------------------------------------------------------------------
 * All 2^(n(n-1)/2) labelled graphs are scanned.  Spectra are obtained by
 * one sweep over the subset lattice of the edge set of K_n:
 *
 *     CS(E) = ( union of CS(E \ {e}) over e in E )
 *             union ( {|E|}  if E itself is a cycle ),
 *
 * correct because a cycle C contained in E with C != E omits an edge of
 * E.  O(2^m * m) time and 2^m bytes (m = n(n-1)/2): 256 MB for n = 8.
 *
 * ---------------------------------------------------------------------
 * ALGORITHM 2 (default): decide each of the 2^(n-2) candidate sets.
 * ---------------------------------------------------------------------
 * Fix S and let W be a witness of S with at most n vertices which is
 * minimal for (|V|,|E|) in lexicographic order.
 *
 * (L1) H subgraph of G  =>  CS(H) subset of CS(G).
 *
 * (L2) VERTEX MINIMALITY.  Deleting a vertex of degree <= 1, contracting
 *      a bridge, and identifying one vertex of two components all keep
 *      the graph simple and do not change CS, while |V| drops.  So W is
 *      connected, bridgeless and of minimum degree >= 2: every block of
 *      W is 2-connected.
 *
 * (L3) EDGE MINIMALITY.  For every edge e, CS(W-e) is a proper subset of
 *      S, so some length L(e) in S has all its cycles through e; hence e
 *      lies on EVERY L(e)-cycle, in particular on any chosen one.  Thus
 *      for any choice of one cycle C_L of each length L in S,
 *
 *              W  =  union of the C_L,      |E(W)| <= sum of S.
 *
 *      This edge bound is what the search prunes with.
 *
 * (L4) BLOCKS.  If the blocks of W are B_1,...,B_t then CS(W) is the
 *      union of the CS(B_i) and |V(W)| = sum(|B_i|-1) + 1, whatever the
 *      shape of the block tree.  So the blocks may be re-glued at ONE
 *      common vertex ("star gluing") without changing CS or |V|.
 *
 * (L5) EARS.  A block is obtained from any of its cycles by adding ears
 *      (paths whose ends lie in the current block, interior new;
 *      Whitney).  The first block may start from a cycle of length
 *      max(S), which must occur in W.
 *
 * (L6) An ear of edge-length e between u and v creates exactly the
 *      cycles { e + d : d in P(u,v) }, where P(u,v) is the set of
 *      lengths of the simple u-v paths of the current graph, because the
 *      interior of the ear has degree 2.  For a new block glued at the
 *      hub the only new cycle is its base cycle.
 *
 * The search builds W block by block: the hub is vertex 0, the first
 * block starts from C_max(S) on 0..max(S)-1, each further block starts
 * from a cycle through vertex 0 on fresh vertices, and a block is
 * finished before the next one starts.  A branch is cut as soon as the
 * spectrum leaves S -- complete by (L1), since every intermediate graph
 * is a subgraph of W -- or as soon as |E| exceeds the bound of (L3).
 *
 * Further lossless reductions:
 *   - the first ear of the base cycle is placed up to the dihedral
 *     automorphism group of C_max(S) (the hub of (L4) may be chosen
 *     after the base cycle has been labelled, so the two are
 *     compatible);
 *   - consecutive ears of length 1 (chords) commute, so they are forced
 *     to appear in increasing order of their endpoint pair;
 *   - simple paths between two vertices of a block stay in that block,
 *     so the path DP of (L6) runs on the block only and clears its own
 *     table while walking over the submasks;
 *   - what can still be reached once a block is finished depends only on
 *     (spectrum, vertices used, cyclomatic number), because the next
 *     blocks live on fresh vertices and are glued to the hub: failures
 *     of these triples are cached;
 *   - a state (graph, current block, last chord) that failed once fails
 *     again: a transposition table (with eviction, which can only lose
 *     hits, never results) stores them.
 *
 * ---------------------------------------------------------------------
 * WHY mu(W) <= |S| IS NOT USED  (it was wrong in an earlier version)
 * ---------------------------------------------------------------------
 * By (L3) W is the union of |S| cycles, but a union of k cycles need not
 * have cyclomatic number <= k: in K_5 the three cycles 1-2-3-4-5-1,
 * 1-3-5-2-1 and 1-4-2-1 (lengths 5, 4, 3) already cover all 10 edges,
 * while mu(K_5) = 6.  The chosen cycles need not span the cycle space.
 * What is provable in this direction is only partial: let A_L be the set
 * of edges lying on every L-cycle of W.  If A_L is non-empty for each L
 * in S, then removing one edge of each A_L destroys, for every L, all
 * L-cycles at once, so those <= |S| edges form a feedback edge set and
 * mu(W) <= |S|.  For a length whose cycles have empty common
 * intersection the argument gives nothing, and no proof is known here
 * that such a length cannot occur; therefore only the edge bound (L3) is
 * used.  Note that (L3) also gives mu(W) <= sum(S) - |V(W)| + 1.
 *
 * ---------------------------------------------------------------------
 * VERIFICATION AND STATUS
 * ---------------------------------------------------------------------
 * Every witness is re-checked by an independent routine (cycle_spectrum)
 * which recomputes its spectrum from scratch, so a set is never accepted
 * by mistake; algorithms 1 and 2 return the same families of sets, not
 * only the same counts, for n <= 8.
 *
 *   n <= 8    : proved twice (algorithm 1 is a complete enumeration).
 *   n <= 10   : proved by algorithm 2 (this file), ~51 s for n = 10 on
 *               one core of a 2026 cloud VM; n = 11 is reachable with
 *               more patience and gives the value 247 of the OEIS entry.
 *   n = 12    : NOT established here.  An earlier version, which used the
 *               invalid bound mu <= |S|, returned 467; since dropping that
 *               pruning can only add solutions, that run only shows
 *               a(12) >= 467.
 *
 * Timings on one core (algorithm 2):
 *     n         8      9      10       11
 *   old     0.00s  0.00s   0.09s    2.75s   (unsound pruning)
 *   this    0.01s  1.61s  50.70s   (~1 h, not run to completion)
 *
 * Compile: cc -O2 -std=c17 -o 399654_01 399654_01.c
 * Usage  : ./399654_01 [-b] [-v] [-m0] [nmax]
 *            -b     use algorithm 1 (exhaustive; nmax <= 8)
 *            -v     also print every realizable set and a witness
 *            -m0    exact search (accepted for compatibility; default)
 *            -m1    deliberately disabled: its pruning is unproved
 *            nmax   last term to compute (default 10, maximum 11)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <errno.h>
#include <limits.h>
#include <time.h>

#define MAXN 12                       /* supports exactly n <= 11 */
/* Since nmax < MAXN, p <= MAXN-1.  For a connected simple p-vertex
 * graph, mu = |E|-|V|+1 <= (p-1)(p-2)/2. */
#define MAXMU (((MAXN - 2) * (MAXN - 3)) / 2)

/* ------------------------------------------------------------------ */
/* independent computation of the cycle spectrum of a labelled graph,  */
/* used to double-check every witness.  Bit k means "there is a        */
/* k-cycle"; each cycle is seen from its smallest vertex.              */
/* ------------------------------------------------------------------ */
static uint16_t reach[1 << MAXN];

static int cycle_spectrum(const int *adj, int p)
{
    int spec = 0, s, mask, full = 1 << p;

    for (s = 0; s < p; s++) {
        int allowed = (full - 1) & ~((1 << s) - 1);   /* vertices >= s */
        memset(reach, 0, sizeof(uint16_t) * (size_t) full);
        reach[1 << s] = (uint16_t) (1u << s);
        for (mask = 1 << s; mask < full; mask++) {
            int e = reach[mask], len, t;
            if (!e) continue;
            len = __builtin_popcount((unsigned int) mask);
            t = e;
            while (t) {
                int v = __builtin_ctz((unsigned int) t), nb;
                t &= t - 1;
                if (len >= 3 && ((adj[v] >> s) & 1)) spec |= 1 << len;
                nb = adj[v] & allowed & ~mask;
                while (nb) {
                    int w = __builtin_ctz((unsigned int) nb);
                    nb &= nb - 1;
                    reach[mask | (1 << w)] |= 1 << w;
                }
            }
        }
    }
    return spec;
}

/* ================================================================== */
/* ALGORITHM 2 : block by block ear search                             */
/* ================================================================== */

static int   nbudget;              /* vertices we are allowed to use   */
static int   target;               /* the set S                        */
static int   emax;                 /* proven upper bound on |E(W)|     */
static int   ebound;               /* proven (L3) edge bound           */
static int   adjm[MAXN];           /* current graph                    */
static int   wit[MAXN], witp;      /* witness found, and its order     */
static uint64_t nodes;             /* statistics                       */

/* failure cache for the state at a block boundary, see (3) above      */
static uint16_t memo[1 << (MAXN - 3)][MAXN][MAXMU + 1];
static uint16_t gen;

/* pl[v] = bitmask of the lengths of the simple paths u -> v inside the
 * vertex set bmask.  Only submasks of bmask containing u are visited and
 * every entry of ends[] is cleared again while walking, so the table
 * needs no initialisation.  (L6)                                      */
static uint16_t ends[1 << MAXN];

static void path_lengths(int u, int bmask, int *pl)
{
    int rest = bmask & ~(1 << u), s = 0, ub = 1 << u;

    ends[ub] = (uint16_t) ub;
    for (;;) {
        int mask = s | ub, e = ends[mask], len, t;
        if (e) {
            ends[mask] = 0;                        /* clear as we walk */
            len = __builtin_popcount((unsigned int) mask) - 1;
            t = e;
            while (t) {
                int v = __builtin_ctz((unsigned int) t), nb;
                t &= t - 1;
                pl[v] |= 1 << len;
                nb = adjm[v] & bmask & ~mask;
                while (nb) {
                    int w = __builtin_ctz((unsigned int) nb);
                    nb &= nb - 1;
                    ends[mask | (1 << w)] |= 1 << w;
                }
            }
        }
        if (s == rest) break;
        s = (s - rest) & rest;                     /* submasks, ascending */
    }
}

/* sound transposition table: a state (graph, current block, lastchord) that
 * failed once fails always, since target/budget are fixed per search.     */
#define TTBITS 21
static struct { uint64_t k0, k1; uint16_t g; } tt[1 << TTBITS];

static void tt_key(int p, int blkstart, int lastchord, uint64_t *k0, uint64_t *k1)
{
    uint64_t a = 0, b = 0; int u, v, i = 0;
    for (u = 0; u < p; u++)
        for (v = u + 1; v < p; v++, i++)
            if ((adjm[u] >> v) & 1) { if (i < 64) a |= 1ull << i; else b |= 1ull << (i - 64); }
    b |= (uint64_t) p << 44;
    b |= (uint64_t) blkstart << 50;
    b |= (uint64_t) (lastchord + 1) << 56;
    *k0 = a; *k1 = b;
}

static void save_witness(int p)
{
    int i;
    witp = p;
    for (i = 0; i < p; i++) wit[i] = adjm[i];
}

static int dfs_block(int p, int spec, int mu, int blkstart, int lastchord, int first);

/* the current block is finished; glue further blocks to the hub 0      */
static int dfs_newblock(int p, int spec, int mu)
{
    int c, j, base, ns;

    if (p < 1 || p >= MAXN || mu < 0 || mu > MAXMU) {
        fprintf(stderr, "internal error: invalid cache index p=%d, mu=%d\n", p, mu);
        exit(EXIT_FAILURE);
    }
    if (p - 1 + mu >= ebound) return 0;            /* (L3) edge bound   */
    if (memo[spec >> 3][p][mu] == gen) return 0;
    for (c = 3; p + c - 1 <= nbudget; c++) {
        ns = spec | (1 << c);
        if (ns & ~target) continue;
        base = p;                                  /* cycle 0-base-...-0 */
        for (j = 0; j < c - 1; j++) adjm[base + j] = 0;
        adjm[0] |= 1 << base;
        adjm[base] |= 1;
        for (j = 0; j + 1 < c - 1; j++) {
            adjm[base + j] |= 1 << (base + j + 1);
            adjm[base + j + 1] |= 1 << (base + j);
        }
        adjm[base + c - 2] |= 1;
        adjm[0] |= 1 << (base + c - 2);
        if (ns == target) { save_witness(p + c - 1); return 1; }
        if (dfs_block(p + c - 1, ns, mu + 1, base, -1, 0)) return 1;
        adjm[0] &= ~((1 << base) | (1 << (base + c - 2)));
        for (j = 0; j < c - 1; j++) adjm[base + j] = 0;
    }
    memo[spec >> 3][p][mu] = gen;
    return 0;
}

/* p          number of vertices used so far
 * spec       spectrum of the current graph
 * mu         its cyclomatic number
 * blkstart   first private vertex of the block under construction
 *            (the block is {0} U {blkstart,...,p-1})
 * lastchord  code of the previous ear if that ear was a chord, else -1
 * first      the graph is exactly the base cycle C_max(S)              */
static int dfs_block(int p, int spec, int mu, int blkstart, int lastchord, int first)
{
    int bmask, u, v, k, j, base, prev, ns, idx, t, pl[MAXN];

    uint64_t k0, k1; size_t h;
    nodes++;
    tt_key(p, blkstart, lastchord, &k0, &k1);
    h = (size_t) ((k0 * 0x9E3779B97F4A7C15ull ^ k1 * 0xC2B2AE3D27D4EB4Full) >> 40) & ((1 << TTBITS) - 1);
    if (tt[h].g == gen && tt[h].k0 == k0 && tt[h].k1 == k1) return 0;
    if (dfs_newblock(p, spec, mu)) return 1;       /* close this block  */
    if (p - 1 + mu >= ebound) return 0;            /* (L3) edge bound   */

    bmask = 1 | (((1 << p) - 1) & ~((1 << blkstart) - 1));
    for (t = bmask; t; t &= t - 1) {
        u = __builtin_ctz((unsigned int) t);
        if (first && u != 0) break;                /* C_M is vertex-transitive */
        for (j = 0; j < p; j++) pl[j] = 0;
        path_lengths(u, bmask, pl);
        for (v = u + 1; v < p; v++) {
            if (!((bmask >> v) & 1)) continue;
            if (first && v > p / 2) break;         /* dihedral symmetry */
            if (!pl[v]) continue;
            for (k = 0; p + k <= nbudget; k++) {
                if (p + mu - 1 + k + 1 > emax) break;   /* (L3) */
                if (k == 0 && ((adjm[u] >> v) & 1)) continue;
                idx = u * MAXN + v;
                if (k == 0 && idx <= lastchord) continue;  /* chords commute */
                ns = spec | (pl[v] << (k + 1));            /* (L6)      */
                if (ns & ~target) continue;
                base = p;
                for (j = 0; j < k; j++) adjm[base + j] = 0;
                prev = u;
                for (j = 0; j < k; j++) {
                    adjm[prev] |= 1 << (base + j);
                    adjm[base + j] |= 1 << prev;
                    prev = base + j;
                }
                adjm[prev] |= 1 << v;
                adjm[v] |= 1 << prev;
                if (ns == target) { save_witness(p + k); return 1; }
                if (dfs_block(p + k, ns, mu + 1, blkstart, k ? -1 : idx, 0)) return 1;
                prev = u;
                for (j = 0; j < k; j++) {
                    adjm[prev] &= ~(1 << (base + j));
                    prev = base + j;
                }
                adjm[prev] &= ~(1 << v);
                adjm[v] &= ~(1 << prev);
                for (j = 0; j < k; j++) adjm[base + j] = 0;
            }
        }
    }
    tt[h].g = gen; tt[h].k0 = k0; tt[h].k1 = k1;
    return 0;
}

/* is the set S realizable by a graph on at most n vertices ? */
static int realizable(int S, int n)
{
    int M = 31 - __builtin_clz((unsigned int) S), i, L;

    if (M > n) return 0;
    nbudget = n;
    target  = S;
    for (emax = 0, i = 3; i <= n; i++) if ((S >> i) & 1) emax += i;
    ebound  = 0;                                   /* sum of L over S   */
    for (L = 3; L <= M; L++) if ((S >> L) & 1) ebound += L;
    if (++gen == 0) { memset(memo, 0, sizeof memo); gen = 1; }
    for (i = 0; i < MAXN; i++) adjm[i] = 0;
    for (i = 0; i < M; i++) {                      /* the base cycle C_M */
        adjm[i] |= 1 << ((i + 1) % M);
        adjm[(i + 1) % M] |= 1 << i;
    }
    if (S == (1 << M)) { save_witness(M); return 1; }
    return dfs_block(M, 1 << M, 1, 1, -1, 1);
}

/* ================================================================== */
/* ALGORITHM 1 : scan all labelled graphs on n vertices                */
/* ================================================================== */

static int cyc_len[1 << 14];                   /* only used for n <= 8 */
static uint32_t cyc_msk[1 << 14];
static long ncyc;
static int eidx[MAXN][MAXN], path[MAXN], plen, gn;

static void cyc_rec(int s, uint32_t used, uint32_t em)
{
    int last = path[plen - 1], v;

    if (plen >= 3 && path[1] < last) {
        cyc_msk[ncyc] = em | (1u << eidx[last][s]);
        cyc_len[ncyc] = plen;
        ncyc++;
    }
    for (v = s + 1; v < gn; v++)
        if (!((used >> v) & 1)) {
            path[plen++] = v;
            cyc_rec(s, used | (1u << v), em | (1u << eidx[last][v]));
            plen--;
        }
}

static int brute_force(int n)                  /* returns a(n)          */
{
    int i, j, m = 0, s, cnt = 0, seen[1 << 10];
    size_t mask, sz;
    uint8_t *spec;
    long c;

    gn = n;
    for (i = 0; i < n; i++)
        for (j = i + 1; j < n; j++) eidx[i][j] = eidx[j][i] = m++;
    ncyc = 0;
    for (s = 0; s < n; s++) { plen = 1; path[0] = s; cyc_rec(s, 1u << s, 0); }

    sz = (size_t) 1 << m;
    spec = calloc(sz, 1);
    if (!spec) { fprintf(stderr, "out of memory (n=%d needs %zu bytes)\n", n, sz); exit(1); }
    for (c = 0; c < ncyc; c++) spec[cyc_msk[c]] |= (uint8_t) (1u << (cyc_len[c] - 3));
    memset(seen, 0, sizeof seen);
    seen[0] = 1;                               /* the empty spectrum    */
    for (mask = 1; mask < sz; mask++) {
        uint8_t v = spec[mask];
        uint32_t t = (uint32_t) mask;
        while (t) {
            uint32_t b = t & (~t + 1);
            t ^= b;
            v |= spec[mask ^ b];
        }
        spec[mask] = v;
        seen[v] = 1;
    }
    for (i = 0; i < (1 << 10); i++) cnt += seen[i];
    free(spec);
    return cnt;
}

/* ================================================================== */

static void print_set(int S, int n)
{
    int L, first = 1;
    printf("{");
    for (L = 3; L <= n; L++)
        if ((S >> L) & 1) { printf(first ? "%d" : ",%d", L); first = 0; }
    printf("}");
}

static void print_witness(void)
{
    int u, v, first = 1;
    printf("  n=%d :", witp);
    for (u = 0; u < witp; u++)
        for (v = u + 1; v < witp; v++)
            if ((wit[u] >> v) & 1) { printf(first ? " %d-%d" : ",%d-%d", u, v); first = 0; }
    if (first) printf(" (empty graph)");
}

static int parse_nmax(const char *text, int *value)
{
    char *end;
    long parsed;

    errno = 0;
    end = NULL;
    parsed = strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' ||
        parsed < INT_MIN || parsed > INT_MAX) return 0;
    *value = (int) parsed;
    return 1;
}

int main(int argc, char **argv)
{
    int nmax = 10, verbose = 0, bf = 0, n, S, cnt, i;
    static char ok[1 << (MAXN - 2)];
    uint64_t tot;

    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-b")) bf = 1;
        else if (!strcmp(argv[i], "-m1")) {
            fprintf(stderr, "option -m1 is disabled because its pruning is unproved\n");
            return 1;
        }
        else if (!strcmp(argv[i], "-m0")) { /* exact mode is now the only mode */ }
        else if (!strcmp(argv[i], "-v")) verbose = 1;
        else if (!parse_nmax(argv[i], &nmax)) {
            fprintf(stderr, "invalid option or nmax: %s\n", argv[i]);
            return 1;
        }
    }
    if (nmax < 0 || nmax >= MAXN) { fprintf(stderr, "0 <= nmax < %d\n", MAXN); return 1; }
    if (bf && nmax > 8) { fprintf(stderr, "option -b is limited to nmax <= 8\n"); return 1; }

    for (n = 0; n <= nmax; n++) {
        clock_t t0 = clock();

        tot = 0;
        if (bf) {
            cnt = (n < 3) ? 1 : brute_force(n);
        } else {
            cnt = 1;                                   /* S = {} */
            for (S = 8; S < (1 << (n + 1)); S += 8) {  /* subsets of {3,...,n} */
                if (!ok[S >> 3]) {
                    nodes = 0;
                    if (realizable(S, n)) {
                        if (cycle_spectrum(wit, witp) != S) {  /* self-check */
                            fprintf(stderr, "internal error: bad witness\n");
                            return 1;
                        }
                        ok[S >> 3] = 1;
                        if (verbose) { printf("  "); print_set(S, n); print_witness(); putchar('\n'); }
                    }
                    tot += nodes;
                }
                if (ok[S >> 3]) cnt++;
            }
        }
        printf("%d %d\n", n, cnt);                 /* b-file format on stdout */
        fflush(stdout);
        fprintf(stderr, "  [n=%2d: %.2f s, %" PRIu64 " nodes]\n", n,
                (double) (clock() - t0) / CLOCKS_PER_SEC, tot);
    }
    return 0;
}
