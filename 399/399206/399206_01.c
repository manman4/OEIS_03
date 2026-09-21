/*
 * Number of permutations p of [n] such that the n-1 products p(i)*p(i+1)
 * (i = 1, 2, ..., n-1) are all distinct.
 *
 * コンパイルと実行
 *   gcc -O3 -march=native -o 399206_01 399206_01.c
 *   ./399206_01 [N]        # n = 1..N の "n a(n)" を標準出力へ（既定 N = 11, 最大 20）
 *
 * ---------------------------------------------------------------------------
 * 方針
 *   深さ優先探索で数を 1 つずつ置き、既出の積を記録して重複したら枝を切る。
 *
 * 高速化 1: n/2 より大きい素数を取り除く
 *   n/2 < q <= n の素数 q について、q の倍数で n 以下のものは q 自身だけなので、
 *   q*x = a*b（a, b <= n）なら a, b の一方が q で、もう一方は x になる。
 *   つまり q を含む積は他のどの積とも一致しない。
 *   そこで、このような素数 k 個を除いた残り（「本体」、m = n - k 個）の並び
 *   c_1, ..., c_m を考え、隣り合う c_i, c_{i+1} の間に素数が 1 つ以上入る箇所
 *   （「切れ目」）を j 個選ぶ。切れ目でない箇所の積だけがすべて異なればよい。
 *   k 個の素数の並べ方は k! 通り、それを両端（空でもよい）と j 個の切れ目
 *   （空でない）に順に分ける方法は C(k+1, j+1) 通りなので、
 *
 *     a(n) = k! * Σ_j C(k+1, j+1) * N_j
 *
 *   ここで N_j は「本体の順列と j 個の切れ目の選び方」のうち条件を満たすものの数。
 *   探索は本体だけに対して行い、各位置で「つなげる（積を確認）」か
 *   「切る（切れ目を 1 つ使う）」かを選ぶ。
 *
 * 高速化 2: 終盤の局面のキャッシュ
 *   残りが R 個以下の局面について、
 *     「残りの数の集合」「最後の数（切った直後は 0）」「使った切れ目の数」
 *     「既出の積のうち今後現れうるもの（最後の数 × 残り、残りどうしの積）」
 *   をキーに結果を記録する。キーは局面を完全に表すので結果は正確。
 *   キャッシュは上書き型で、メモリ使用量は一定（約 64 MB）。
 *
 * 目安（1 コア）: n = 14 で約 1 秒、n = 15 で約 16 秒。n が 1 増えるごとに 10〜15 倍。
 * 値は 64 ビット整数で保持する（a(n) <= n! <= 20! < 2^64）。
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef unsigned long long u64;

#define MAXN 20
#define R 6            /* 残りがこの個数以下の局面をキャッシュする（R + R(R-1)/2 <= 21 ビット） */
#define CBITS 22       /* キャッシュのエントリ数 2^CBITS */

static int n, k;                     /* k: n/2 より大きい素数の個数 */
static u64 core;                     /* 本体の数の集合（ビット v） */
static unsigned char seen[MAXN * MAXN + 1];
static u64 weight[MAXN + 2];         /* weight[j] = k! * C(k+1, j+1) */
static u64 *ckey, *cval;

static int is_prime(int x)
{
    if (x < 2)
        return 0;
    for (int d = 2; d * d <= x; d++)
        if (x % d == 0)
            return 0;
    return 1;
}

/*
 * last : 最後に置いた数（先頭、または切った直後は 0）
 * used : 使った数の集合
 * left : 本体の残りの個数
 * j    : 使った切れ目の数
 * 戻り値: 残りの置き方すべてについての weight[最終的な j] の和
 */
static u64 count(int last, u64 used, int left, int j)
{
    if (left == 0)
        return weight[j];

    u64 avail = core & ~used;
    u64 key = 0;
    size_t slot = 0;

    if (left <= R) {
        /* 今後現れうる積について、既出かどうかを決まった順に並べる */
        u64 bits = 0;
        int nb = 0;
        u64 a = avail;
        while (a) {
            int v = __builtin_ctzll(a);
            a &= a - 1;
            if (last)
                bits |= (u64)seen[last * v] << nb++;
            u64 b = a;
            while (b) {
                int w = __builtin_ctzll(b);
                b &= b - 1;
                bits |= (u64)seen[v * w] << nb++;
            }
        }
        /* キー: [積のビット] [残りの集合] [最後の数 5 ビット] [切れ目の数 3 ビット] */
        key = (bits << (n + 8)) | ((avail >> 1) << 8) | ((u64)last << 3) | (u64)j;
        key += 1; /* 0 を空きの印にする */
        slot = (size_t)((key * 0x9E3779B97F4A7C15ULL) >> (64 - CBITS));
        if (ckey[slot] == key)
            return cval[slot];
    }

    u64 c = 0;
    u64 a = avail;
    while (a) {
        int v = __builtin_ctzll(a);
        a &= a - 1;
        u64 nu = used | (1ULL << v);
        if (last == 0) {
            /* 先頭、または切った直後：積は生じない */
            c += count(v, nu, left - 1, j);
            continue;
        }
        /* つなげる */
        int p = last * v;
        if (!seen[p]) {
            seen[p] = 1;
            c += count(v, nu, left - 1, j);
            seen[p] = 0;
        }
        /* 切る（間に大きい素数が 1 つ以上入る） */
        if (j < k)
            c += count(v, nu, left - 1, j + 1);
    }

    if (left <= R) {
        ckey[slot] = key;
        cval[slot] = c;
    }
    return c;
}

static u64 binom(int a, int b)
{
    if (b < 0 || b > a)
        return 0;
    u64 r = 1;
    for (int i = 1; i <= b; i++)
        r = r * (u64)(a - b + i) / (u64)i;
    return r;
}

static u64 a399206(int m)
{
    n = m;
    k = 0;
    core = 0;
    for (int v = 1; v <= n; v++) {
        if (2 * v > n && is_prime(v))
            k++;
        else
            core |= 1ULL << v;
    }

    u64 kf = 1;
    for (int i = 2; i <= k; i++)
        kf *= (u64)i;
    for (int j = 0; j <= k; j++)
        weight[j] = kf * binom(k + 1, j + 1);

    memset(seen, 0, sizeof seen);
    memset(ckey, 0, ((size_t)1 << CBITS) * sizeof(u64)); /* n ごとにキーの意味が変わる */

    return count(0, 0, n - k, 0);
}

int main(int argc, char **argv)
{
    int nmax = 11;
    if (argc > 1)
        nmax = atoi(argv[1]);
    if (nmax < 1 || nmax > MAXN) {
        fprintf(stderr, "N must be in 1..%d\n", MAXN);
        return 1;
    }

    ckey = calloc((size_t)1 << CBITS, sizeof(u64));
    cval = calloc((size_t)1 << CBITS, sizeof(u64));
    if (!ckey || !cval) {
        fprintf(stderr, "out of memory\n");
        return 1;
    }

    clock_t t0 = clock();
    for (int m = 1; m <= nmax; m++) {
        u64 v = a399206(m);
        printf("%d %llu\n", m, v);
        fflush(stdout);
        fprintf(stderr, "n=%d done (%.2f s)\n", m, (double)(clock() - t0) / CLOCKS_PER_SEC);
    }
    return 0;
}
