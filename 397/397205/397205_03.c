/* A397205
 * a(n) = max |S|*|T| over S,T subset of {1..n} such that the products s*t
 *        (s in S, t in T) are pairwise distinct.
 *
 *   s1*t1 == s2*t2  <=>  s1/s2 == t2/t1
 * so the condition is exactly  R(S) cap R(T) = empty,
 * where R(X) is the set of reduced ratios a/b with a,b in X, a != b.
 *
 * build:  cc -O2 -o 397205_03 397205_03.c
 * usage:  ./397205_03 [lo] [hi]        (default 1 24, n <= 63)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#define MAXN 63
#define MAXC ((MAXN + 1) * (MAXN + 1))

static int      N;
static uint64_t FULL;                 /* bits 1..N                          */
static int      rid[MAXN + 1][MAXN + 1];   /* pair -> ratio class id        */
static int      cq[MAXC], cp[MAXC];        /* class id -> reduced ratio q/p */
static int      nclass;
static unsigned char used[MAXC];           /* class currently in R(S)?      */

static int      S[MAXN + 1], Sn;           /* current S                     */
static int      best, bestS[MAXN + 1], bestSn, bestT[MAXN + 1], bestTn;

static int      cur[MAXN + 1], curn;       /* clique search work space      */
static int      got[MAXN + 1], gotn;

static int gcd_i(int a, int b) { while (b) { int t = a % b; a = b; b = t; } return a; }

static void build(int n)
{
    static int idmap[MAXN + 2][MAXN + 2];
    int i, j, g, q, p;

    N = n;
    FULL = (n == 63) ? ~1ULL : ((((uint64_t)1 << (n + 1)) - 1) & ~1ULL);
    nclass = 0;
    memset(idmap, -1, sizeof idmap);
    memset(used, 0, sizeof used);

    for (j = 2; j <= n; j++)
        for (i = 1; i < j; i++) {
            g = gcd_i(i, j);
            q = i / g; p = j / g;
            if (idmap[q][p] < 0) {
                idmap[q][p] = nclass;
                cq[nclass] = q; cp[nclass] = p;
                nclass++;
            }
            rid[i][j] = rid[j][i] = idmap[q][p];
        }
}

/* greedy colouring: upper bound for the maximum clique */
static int color_bound(const uint64_t *adj, uint64_t cand)
{
    int c = 0;
    uint64_t un = cand, q, b;
    int v;
    while (un) {
        c++;
        q = un;
        while (q) {
            v = __builtin_ctzll(q);
            b = 1ULL << v;
            un &= ~b;
            q &= ~b;
            q &= ~adj[v];              /* one colour class = independent set */
        }
    }
    return c;
}

/* Tomita style: is there a clique of size `need` inside cand?  (result in got) */
static int expand(const uint64_t *adj, uint64_t cand, int need)
{
    int order[MAXN + 1], cols[MAXN + 1], m = 0, c = 0, i, v;
    uint64_t un = cand, q, b;

    while (un) {
        c++;
        q = un;
        while (q) {
            v = __builtin_ctzll(q);
            b = 1ULL << v;
            un &= ~b;
            q &= ~b;
            q &= ~adj[v];
            order[m] = v; cols[m] = c; m++;
        }
    }
    for (i = m - 1; i >= 0; i--) {
        if (curn + cols[i] < need) return 0;          /* colouring bound */
        v = order[i];
        cur[curn++] = v;
        if (curn == need) {
            memcpy(got, cur, curn * sizeof(int));
            gotn = curn;
            curn--;
            return 1;
        }
        if (expand(adj, cand & adj[v], need)) { curn--; return 1; }
        curn--;
        cand &= ~(1ULL << v);
    }
    return 0;
}

/* extend S in increasing order; adj keeps only the pairs whose ratio is not in R(S) */
static void dfs(const uint64_t *adj, int start)
{
    uint64_t nadj[MAXN + 2];
    int undo[MAXN + 1], undon;
    int v, k, i, j, id, g, ub, su, need;

    if (Sn > 0) {
        ub = color_bound(adj, FULL);
        if (ub < Sn) return;                       /* |T| >= |S| now impossible */
        su = Sn + (N - start + 1);
        if (su > ub) su = ub;
        if (su * ub <= best) return;

        g = 0;
        for (i = 0; i < Sn; i++) g = gcd_i(g, S[i]);
        if (g == 1) {                              /* R(cS)=R(S): only gcd(S)=1 */
            need = best / Sn + 1;
            if (need < Sn) need = Sn;
            for (;;) {
                curn = 0;
                if (!expand(adj, FULL, need)) break;
                if (Sn * gotn > best) {
                    best = Sn * gotn;
                    bestSn = Sn;
                    memcpy(bestS, S, Sn * sizeof(int));
                    bestTn = gotn;
                    memcpy(bestT, got, gotn * sizeof(int));
                }
                need++;
            }
        }
    }
    for (v = start; v <= N; v++) {
        memcpy(nadj, adj, (N + 1) * sizeof(uint64_t));
        undon = 0;
        for (k = 0; k < Sn; k++) {
            id = rid[S[k]][v];
            if (used[id]) continue;
            used[id] = 1;
            undo[undon++] = id;
            for (i = cq[id], j = cp[id]; j <= N; i += cq[id], j += cp[id]) {
                nadj[i] &= ~(1ULL << j);
                nadj[j] &= ~(1ULL << i);
            }
        }
        S[Sn++] = v;
        dfs(nadj, v + 1);
        Sn--;
        while (undon) used[undo[--undon]] = 0;
    }
}

static int solve(int n)
{
    uint64_t adj[MAXN + 2];
    int v;

    build(n);
    best = n;
    bestSn = 1; bestS[0] = 1;
    bestTn = n;
    for (v = 1; v <= n; v++) bestT[v - 1] = v;

    for (v = 1; v <= n; v++) adj[v] = FULL & ~(1ULL << v);
    adj[0] = 0;
    Sn = 0;
    dfs(adj, 1);
    return best;
}

static int check(int n)
{
    static unsigned char seen[(MAXN + 1) * (MAXN + 1) + 1];
    int i, j, m;
    memset(seen, 0, sizeof seen);
    for (i = 0; i < bestSn; i++)
        for (j = 0; j < bestTn; j++) {
            if (bestS[i] < 1 || bestS[i] > n || bestT[j] < 1 || bestT[j] > n) return 0;
            m = bestS[i] * bestT[j];
            if (seen[m]) return 0;
            seen[m] = 1;
        }
    return bestSn * bestTn == best;
}

static void show(const char *name, const int *a, int k)
{
    int i;
    printf("%s={", name);
    for (i = 0; i < k; i++) printf(i ? ",%d" : "%d", a[i]);
    printf("}");
}

int main(int argc, char **argv)
{
    int lo = (argc > 1) ? atoi(argv[1]) : 1;
    int hi = (argc > 2) ? atoi(argv[2]) : 24;
    int n, i, j, t;
    clock_t c0;

    if (hi > MAXN) hi = MAXN;
    for (n = lo; n <= hi; n++) {
        c0 = clock();
        solve(n);
        if (!check(n)) { fprintf(stderr, "invalid witness at n=%d\n", n); return 1; }
        for (i = 0; i < bestTn; i++)              /* tidy up the witness */
            for (j = i + 1; j < bestTn; j++)
                if (bestT[j] < bestT[i]) { t = bestT[i]; bestT[i] = bestT[j]; bestT[j] = t; }
        printf("a(%d) = %d\t[%.2fs]  ", n, best, (double)(clock() - c0) / CLOCKS_PER_SEC);
        show("S", bestS, bestSn); printf(" ");
        show("T", bestT, bestTn); printf("\n");
        fflush(stdout);
    }
    return 0;
}
