/* Erdos problem #896  (https://www.erdosproblems.com/forum/thread/896)
 *
 *   F(A,B) = #{ m : m = a*b has exactly one solution with a in A, b in B }
 *   a(N)   = max F(A,B) over A,B subseteq {1,...,N}
 *
 * Branch and bound: every v = 1..N is put into "A only / B only / both / neither".
 * cnt[m] = representations of m by already committed pairs,
 * pos[m] = representations of m that are still possible.
 * Upper bounds used at each node:
 *     F <= #{ m : cnt[m] <= 1 && (cnt[m] == 1 || pos[m] >= 1) }
 *     F  = |A||B| - sum_{r_m != 1} r_m
 *       <= (|A|+free)*(|B|+free) - sum_{cnt[m] >= 2} cnt[m]
 * F(A,B) = F(B,A) is broken by forcing the first asymmetric element into A.
 *
 * build:  cc -O2 -o 399711_01 399711_01.c
 * usage:  ./399711_01 [lo] [hi]          (default 1 18, N <= 63)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAXN 63
#define MAXM (MAXN * MAXN + 1)
#define MAXLOG (8 * MAXM)

static int N, MM;
static int cnt[MAXM], pos[MAXM];
static int f;                     /* #{m : cnt[m] == 1} = F(A0,B0)          */
static int lc;                    /* sum of cnt[m] over m with cnt[m] >= 2  */
static int ub2;                   /* #{m : m can still be unique}           */
static int A[MAXN + 1], An, B[MAXN + 1], Bn;
static int lg[MAXLOG], lgn;
static int best, bestA[MAXN + 1], bestAn, bestB[MAXN + 1], bestBn;
static long long nodes;

static int u(int m)
{
    int c = cnt[m];
    return (c == 1 || (c == 0 && pos[m] > 0)) ? 1 : 0;
}

static void inc_cnt(int m)
{
    int c;
    ub2 -= u(m);
    c = cnt[m];
    if (c == 1) f--;
    lc += (c == 1) ? 2 : (c >= 2 ? 1 : 0);
    cnt[m] = c + 1;
    if (c + 1 == 1) f++;
    ub2 += u(m);
    if (lgn >= MAXLOG) { fprintf(stderr, "log overflow\n"); exit(1); }
    lg[lgn++] = m;
}

static void dec_cnt(int m)
{
    int c;
    ub2 -= u(m);
    c = cnt[m];
    if (c == 1) f--;
    lc -= (c == 2) ? 2 : (c > 2 ? 1 : 0);
    cnt[m] = c - 1;
    if (c - 1 == 1) f++;
    ub2 += u(m);
}

static void dec_pos(int m)
{
    ub2 -= u(m);
    pos[m]--;
    ub2 += u(m);
    if (lgn >= MAXLOG) { fprintf(stderr, "log overflow\n"); exit(1); }
    lg[lgn++] = -m;
}

static void inc_pos(int m)
{
    ub2 -= u(m);
    pos[m]++;
    ub2 += u(m);
}

static void apply_move(int v, int ca, int cb)
{
    int i;
    if (cb) for (i = 0; i < An; i++) inc_cnt(A[i] * v);          /* A0 x {v} */
    if (ca) {
        for (i = 0; i < Bn; i++) inc_cnt(v * B[i]);              /* {v} x B0 */
        if (cb) inc_cnt(v * v);
    }
    if (!ca) {                       /* v can never be in A any more */
        for (i = 0; i < Bn; i++) dec_pos(v * B[i]);
        for (i = v; i <= N; i++) dec_pos(v * i);
    }
    if (!cb) {                       /* v can never be in B any more */
        for (i = 0; i < An; i++) dec_pos(A[i] * v);
        if (ca) dec_pos(v * v);
        for (i = v + 1; i <= N; i++) dec_pos(i * v);
    }
    if (ca) A[An++] = v;
    if (cb) B[Bn++] = v;
}

static void undo(int mark)
{
    while (lgn > mark) {
        int m = lg[--lgn];
        if (m > 0) dec_cnt(m); else inc_pos(-m);
    }
}

static void dfs(int v, int sym)
{
    static const int CA[4] = {1, 1, 0, 0}, CB[4] = {1, 0, 1, 0};
    int free_, ub, k, mark;

    nodes++;
    if (f > best) {
        best = f;
        bestAn = An; memcpy(bestA, A, An * sizeof(int));
        bestBn = Bn; memcpy(bestB, B, Bn * sizeof(int));
    }
    if (v > N) return;
    free_ = N - v + 1;
    ub = (An + free_) * (Bn + free_) - lc;
    if (ub2 < ub) ub = ub2;
    if (ub <= best) return;

    for (k = 0; k < 4; k++) {
        int ca = CA[k], cb = CB[k];
        if (sym == 0 && ca == 0 && cb == 1) continue;    /* F(A,B) = F(B,A) */
        mark = lgn;
        apply_move(v, ca, cb);
        dfs(v + 1, (ca == cb) ? sym : 1);
        if (ca) An--;
        if (cb) Bn--;
        undo(mark);
    }
}

static int eval_F(void)
{
    static int c[MAXM];
    int i, j, r = 0;
    memset(c, 0, (MM + 1) * sizeof(int));
    for (i = 0; i < bestAn; i++)
        for (j = 0; j < bestBn; j++) c[bestA[i] * bestB[j]]++;
    for (i = 1; i <= MM; i++) if (c[i] == 1) r++;
    return r;
}

static void show(const char *name, const int *a, int k)
{
    int i;
    printf("%s={", name);
    for (i = 0; i < k; i++) printf(i ? ",%d" : "%d", a[i]);
    printf("}");
}

static void solve(int n)
{
    int a, b;
    N = n; MM = n * n;
    memset(cnt, 0, (MM + 1) * sizeof(int));
    memset(pos, 0, (MM + 1) * sizeof(int));
    for (a = 1; a <= n; a++) for (b = 1; b <= n; b++) pos[a * b]++;
    f = lc = ub2 = An = Bn = lgn = best = bestAn = bestBn = 0;
    for (a = 1; a <= MM; a++) ub2 += u(a);
    nodes = 0;
    dfs(1, 0);
}

int main(int argc, char **argv)
{
    int lo = (argc > 1) ? atoi(argv[1]) : 1;
    int hi = (argc > 2) ? atoi(argv[2]) : 18;
    int n;
    clock_t c0;

    if (lo < 1) lo = 1;
    if (hi > MAXN) hi = MAXN;   /* cnt[]/pos[] are sized for N <= MAXN */
    for (n = lo; n <= hi; n++) {
        c0 = clock();
        solve(n);
        if (eval_F() != best) { fprintf(stderr, "invalid witness at N=%d\n", n); return 1; }
        printf("N=%2d  maxF=%5d  [%9.2fs %14lld nodes]  ",
               n, best, (double)(clock() - c0) / CLOCKS_PER_SEC, nodes);
        show("A", bestA, bestAn); printf(" ");
        show("B", bestB, bestBn); printf("\n");
        fflush(stdout);
    }
    return 0;
}
