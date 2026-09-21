/*
 * A386580: Number of normal multisets of size n having a permutation
 *          with all distinct run lengths.
 *
 * 出力: b386580_02.txt（OEIS の b-file 形式 "n a(n)"）
 *
 * コンパイルと実行
 *   gcc -O2 -o 386580_02 386580_02.c
 *   ./386580_02 [N]        # n = 0..N（既定 N = 100, 最大 127）
 *
 *   目安: N = 100 で約 15 秒・350 MB、N = 110 で約 50 秒・850 MB。
 *   N を 10 増やすごとに時間・メモリともおよそ 2.5〜3.5 倍になる。
 *
 * ---------------------------------------------------------------------------
 * 1. 組成への帰着
 *   大きさ n の正規多重集合は、重複度の列 (m_1, ..., m_k) すなわち n の組成と
 *   一対一に対応する。条件は重複度の多重集合（分割 λ）だけで決まるので、
 *
 *     a(n) = Σ_{λ ⊢ n, λ が条件を満たす} ℓ(λ)! / Π_j m_j(λ)!
 *
 *   となる（各分割を、その並べ替えである組成の個数で重み付け）。
 *
 * 2. 判定条件
 *   λ = (m_1, ..., m_k) がランの長さのすべて異なる並べ替えをもつ ⇔
 *     各 m_i を相異なる部分に分割 π_i し、全 π_i の部分が互いに重ならない
 *     ように選べる。
 *   つまり λ は、{1, 2, ...} の有限部分集合 W を集合分割したときの
 *   各ブロックの和の多重集合として表せる。このような W を λ の「証拠」と呼ぶ。
 *
 *   （同じ値のランが隣接しないように並べる釣り合い条件 2 max r_i <= Σ r_i + 1
 *     は自動的に満たせる。ある値 x のランが r 個、他の値のランが計 O 個で
 *     r >= O + 2 とすると、x のランの長さの 2 つの和は 2r - 3 通り以上あり、
 *     衝突しうる長さは他の値の O 個と x 自身の r - 2 個しかない。
 *     2r - 3 > O + r - 2 なので、衝突しない 2 つを 1 つのランにまとめられる。
 *     これを繰り返せば r <= O + 1 にできる。）
 *
 * 3. 和ごとの動的計画法
 *   条件を満たす分割 λ について、μ(λ) = （証拠 W の最大要素）の最小値 とおく
 *   （空の分割は μ = 0）。証拠の最大要素 x を取り除くと、
 *
 *     λ が条件を満たし μ(λ) = x
 *       ⇔ x は、μ(λ') < x を満たす条件付き分割 λ' に、x を新しいブロックとして
 *          加えるか既存のブロックに足して λ が得られるような最小の x
 *
 *   が成り立つ。そこで和 n の層を、和 n - x の層（x = 1, 2, ... の順）から
 *   生成する。最初に現れたときの x がそのまま μ(λ) になる。
 *   重複の除去は同じ和の中だけで済むので、ハッシュ表は現在の層の分だけ持てばよい。
 *   生成されるのは条件を満たす分割だけなので、条件を満たさない分割を
 *   調べる必要がない。
 *
 * 値は 128 ビット整数で保持する（a(n) <= 2^(n-1) なので n <= 127 まで安全）。
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef unsigned __int128 u128;

#define MAXN 127
#define MAXL 15 /* 相異なる正整数 15 個の和は 120、16 個なら 136 > 127 */

/*
 * 分割の表現（u128）
 *   ビット 0..104   : 部分を降順に 7 ビットずつ（最大 15 個、0 は「なし」）
 *   ビット 112..118 : μ(λ)（層の配列に保存するときだけ使う）
 *   ビット 127      : 常に 1（ハッシュ表で 0 を空きの印にするため）
 */
#define MARK (((u128)1) << 127)
#define MU_SHIFT 112
#define PARTS_MASK ((((u128)1) << 105) - 1)

static u128 encode(const int *p, int t)
{
    u128 k = MARK;
    for (int j = 0; j < t; j++)
        k |= (u128)p[j] << (7 * j);
    return k;
}

static int decode(u128 k, int *p)
{
    int t = 0;
    for (int j = 0; j < MAXL; j++) {
        int v = (int)((k >> (7 * j)) & 127);
        if (v == 0)
            break;
        p[t++] = v;
    }
    return t;
}

/* ---------------- 現在の層の重複除去用ハッシュ集合 ---------------- */

static u128 *hkey;
static size_t hcap, hused;

static size_t hash_u128(u128 k)
{
    unsigned long long lo = (unsigned long long)k;
    unsigned long long hi = (unsigned long long)(k >> 64);
    unsigned long long h = lo * 0x9E3779B97F4A7C15ULL ^ (hi + 0x632BE59BD9B4E019ULL) * 0xC2B2AE3D27D4EB4FULL;
    h ^= h >> 29;
    h *= 0xBF58476D1CE4E5B9ULL;
    h ^= h >> 32;
    return (size_t)h;
}

static void *xcalloc(size_t n, size_t sz)
{
    void *p = calloc(n, sz);
    if (!p) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    return p;
}

static int hset_insert_raw(u128 k)
{
    size_t i = hash_u128(k) & (hcap - 1);
    while (hkey[i] != 0) {
        if (hkey[i] == k)
            return 0;
        i = (i + 1) & (hcap - 1);
    }
    hkey[i] = k;
    hused++;
    return 1;
}

/* 新しく入れたら 1、既にあれば 0 */
static int hset_insert(u128 k)
{
    if (2 * (hused + 1) > hcap) {
        u128 *old = hkey;
        size_t oc = hcap;
        hcap *= 2;
        hkey = xcalloc(hcap, sizeof(u128));
        hused = 0;
        for (size_t i = 0; i < oc; i++)
            if (old[i] != 0)
                hset_insert_raw(old[i]);
        free(old);
    }
    return hset_insert_raw(k);
}

static void hset_clear(void)
{
    memset(hkey, 0, hcap * sizeof(u128));
    hused = 0;
}

/* ---------------- 各層（和 n の条件付き分割）の保存 ---------------- */

static u128 *layer[MAXN + 1];
static size_t layer_len[MAXN + 1], layer_cap[MAXN + 1];

static void layer_push(int n, u128 v)
{
    if (layer_len[n] == layer_cap[n]) {
        layer_cap[n] = layer_cap[n] ? layer_cap[n] * 2 : 16;
        layer[n] = realloc(layer[n], layer_cap[n] * sizeof(u128));
        if (!layer[n]) {
            fprintf(stderr, "out of memory\n");
            exit(1);
        }
    }
    layer[n][layer_len[n]++] = v;
}

static void layer_shrink(int n)
{
    if (layer_len[n] > 0 && layer_len[n] < layer_cap[n]) {
        layer[n] = realloc(layer[n], layer_len[n] * sizeof(u128));
        layer_cap[n] = layer_len[n];
    }
}

/* ---------------- 集計 ---------------- */

static unsigned long long fact[MAXL + 1];

/* 分割（降順の配列）の並べ替え（組成）の個数 ℓ(λ)! / Π m_j(λ)! */
static unsigned long long compositions_count(const int *p, int t)
{
    unsigned long long r = fact[t];
    int j = 0;
    while (j < t) {
        int k = j;
        while (k < t && p[k] == p[j])
            k++;
        r /= fact[k - j];
        j = k;
    }
    return r;
}

static u128 layer_total;

/* 和 n の層に分割 p[0..t-1]（降順）を μ = x で加える（初出のときだけ） */
static void offer(int n, const int *p, int t, int x)
{
    u128 k = encode(p, t);
    if (hset_insert(k)) {
        layer_push(n, k | ((u128)x << MU_SHIFT));
        layer_total += compositions_count(p, t);
    }
}

/* 和 n の層を作り、a(n) を返す */
static u128 build_layer(int n)
{
    layer_total = 0;
    hset_clear();

    if (n == 0) {
        offer(0, NULL, 0, 0);
        layer_shrink(0);
        return layer_total;
    }

    int p[MAXL], q[MAXL + 1];
    for (int x = 1; x <= n; x++) {
        int m = n - x;
        for (size_t i = 0; i < layer_len[m]; i++) {
            u128 v = layer[m][i];
            int mu = (int)((v >> MU_SHIFT) & 127);
            if (mu >= x)
                continue;
            int t = decode(v & (PARTS_MASK | MARK), p);

            /* x を新しいブロックとして加える */
            if (t < MAXL) {
                int nt = 0, done = 0;
                for (int j = 0; j < t; j++) {
                    if (!done && x >= p[j]) {
                        q[nt++] = x;
                        done = 1;
                    }
                    q[nt++] = p[j];
                }
                if (!done)
                    q[nt++] = x;
                offer(n, q, nt, x);
            }

            /* x を既存のブロック（値ごとに 1 回）に足す */
            for (int j = 0; j < t; j++) {
                if (j > 0 && p[j] == p[j - 1])
                    continue;
                int w = p[j] + x;
                int nt = 0, done = 0;
                for (int r = 0; r < t; r++) {
                    if (r == j)
                        continue;
                    if (!done && w >= p[r]) {
                        q[nt++] = w;
                        done = 1;
                    }
                    q[nt++] = p[r];
                }
                if (!done)
                    q[nt++] = w;
                offer(n, q, nt, x);
            }
        }
    }
    layer_shrink(n);
    return layer_total;
}

/* ---------------- 出力 ---------------- */

static void print_u128(FILE *fp, u128 x)
{
    char buf[64];
    int k = 0;
    do {
        buf[k++] = (char)('0' + (int)(x % 10));
        x /= 10;
    } while (x > 0);
    while (k > 0)
        fputc(buf[--k], fp);
}

int main(int argc, char **argv)
{
    int nmax = 100;
    if (argc > 1)
        nmax = atoi(argv[1]);
    if (nmax < 0 || nmax > MAXN) {
        fprintf(stderr, "N must be in 0..%d\n", MAXN);
        return 1;
    }

    fact[0] = 1;
    for (int k = 1; k <= MAXL; k++)
        fact[k] = fact[k - 1] * (unsigned long long)k;
    hcap = (size_t)1 << 16;
    hkey = xcalloc(hcap, sizeof(u128));

    const char *fname = "b386580_02.txt";
    FILE *fp = fopen(fname, "w");
    if (!fp) {
        perror(fname);
        return 1;
    }

    clock_t t0 = clock();
    size_t stored = 0;
    for (int n = 0; n <= nmax; n++) {
        u128 v = build_layer(n);
        stored += layer_len[n];

        fprintf(fp, "%d ", n);
        print_u128(fp, v);
        fputc('\n', fp);
        fflush(fp);

        fprintf(stderr, "%d ", n);
        print_u128(stderr, v);
        fprintf(stderr, "  (%.2f s, partitions %zu, stored %zu)\n",
                (double)(clock() - t0) / CLOCKS_PER_SEC, layer_len[n], stored);
    }
    fclose(fp);
    fprintf(stderr, "wrote %s\n", fname);
    return 0;
}
