/*
 * Erdős Problem #863  (A399962)
 * a(n) = 最大の |A|、A ⊆ {1,...,n} は B_2[2] 集合：
 *        どの m についても m = x + y（x <= y、x, y ∈ A）の表し方が 2 通り以下。
 * n = 1..N を計算し、"1, 2, 3, 4, 4, ..." の形で標準出力に 1 項ずつ書き出す（ファイルは作らない）。
 *
 * 使う数学的事実（どれも証明つき、結果を変えない）:
 *  (1) B_2[2] 集合の部分集合も B_2[2]（表し方の数は減るだけ）。
 *      平行移動 A + c と反転 (n+1) - A も B_2[2]
 *      （x+y の多重集合が一様にずれる／反転するだけなので、各和の表し方の数は保たれる）。
 *      特に、区間 [u, v] に入る B_2[2] 集合の要素数は a(v-u+1) 以下。
 *  (2) a(n-1) <= a(n) <= a(n-1) + 1（n >= 2）。
 *      下側: {1..n-1} の集合は {1..n} の集合でもある。
 *      上側: {1..n} の集合から n を除けば {1..n-1} の集合（(1)）。
 *  (3) a(n) = a(n-1) + 1 となるなら、大きさ t = a(n-1)+1 の B_2[2] 集合 S ⊆ {1..n} は
 *      必ず 1 と n を含む。n を含まなければ S ⊆ {1..n-1} で |S| > a(n-1)。
 *      1 を含まなければ S - 1 ⊆ {1..n-1} が (1) より B_2[2] で、やはり矛盾。
 *      よって「1 と n を含む大きさ t の集合」があるかだけを調べればよい。
 *  (4) 対称性の除去: S（t >= 3）を s_1 = 1 < s_2 < ... < s_t = n とする。
 *      反転 (n+1) - S も 1 と n を含む大きさ t の B_2[2] 集合で、
 *      g_top = n - s_{t-1} と g_bot = s_2 - 1 が入れ替わる。
 *      よって g_top <= g_bot を満たす S だけを探せばよい。
 *
 * 探索（両端から）: 1 と n を選んだ状態から、上側の次の要素（残りの最大）と下側の次の要素
 *   （残りの最小）を交互に決める。上側で y を選ぶとそれより上の未定の値は使わず、
 *   下側も同様。よって未定の要素はつねに開区間 (L, H) に入る
 *   （L = 下側で選んだ最大の要素、H = 上側で選んだ最小の要素）。
 *   最初に上側で s_{t-1} を、次に下側で s_2 を決め、(4) の g_top <= g_bot を確かめる。
 *   t = 3 のときは s_2 = s_{t-1} なので、上側で選んだ時点で n - y <= y - 1 を確かめる。
 *
 * 枝刈り（どれも証明つき）:
 *  (5) 区間の上限: (1) より、下側で選んだ b（または L+1）と上側で選んだ c（または H-1）の
 *      各組について、[b, c] の選び済みの要素数 + これから選ぶ個数 <= a(c-b+1)
 *      （未定の要素はすべて (L, H) ⊆ [b, c] に入る）。c - b + 1 <= n - 1 となる組だけ使う
 *      （a は n-1 まで計算済み）。
 *      新しい要素 y を加えるときも、y を含む区間 [b, c] で同じ不等式（これから選ぶ個数 >= 0）を
 *      確かめる。
 *  (6) 要素を加えると各和の表し方の数（cnt）は増える一方なので、いま加えられない値は、
 *      その枝ではその後も加えられない。よって (L, H) のいま加えられる値の個数が
 *      これから選ぶ個数より少なければ打ち切る。上側で y を選ぶときは (L, y) に、
 *      下側で y を選ぶときは (y, H) に残りが入るので、その範囲でも同様に数える。
 *
 * 判定: cnt[m] = 選んだ集合で m = x+y（x <= y）となる組の数。y を加えられるのは、
 *   選んだ各 a について cnt[a+y] <= 1、かつ cnt[2y] <= 1 のとき
 *   （a+y は a ごとに値が異なり、2y はどの a+y（a != y）とも異なるので、
 *    y を加えると各値の cnt はちょうど 1 増える）。
 *
 * 検証: n <= 25 を、全部分集合を調べる独立な総当たりと照合した（コメント末尾の値）。
 *       n <= 50 を、片側から探索する別の版の出力と照合した。
 *
 * コンパイル: g++ -O2 -std=c++17 -Wall -Wextra -Wpedantic -Werror 399962_01.cpp -o 399962_01
 * 実行:       ./399962_01 [N]   （N の既定値は 70、上限 MAXN）
 *
 * 総当たりの値（n = 1..25）:
 *   1,2,3,4,4,5,5,6,6,6,7,7,7,8,8,8,8,9,9,9,9,10,10,10,10
 */
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace {

constexpr int MAXN = 200;

int aval[MAXN + 1];                       // この実行で計算した a(n)
int curN, target;
int elems[MAXN + 1], k_all;               // 選んだ要素すべて（判定用）
int lowv[MAXN + 1], k_low;                // 下側で選んだ要素（小さい順、lowv[0] = 1）
int highv[MAXN + 1], k_high;              // 上側で選んだ要素（大きい順、highv[0] = n）
int cnt[2 * MAXN + 2];
bool found;

inline bool can_add(int y) {
    if (cnt[2 * y] >= 2) return false;
    for (int i = 0; i < k_all; i++)
        if (cnt[elems[i] + y] >= 2) return false;
    return true;
}

inline void push(int y) {
    for (int i = 0; i < k_all; i++) cnt[elems[i] + y]++;
    cnt[2 * y]++;
    elems[k_all++] = y;
}

inline void pop() {
    int y = elems[--k_all];
    for (int i = 0; i < k_all; i++) cnt[elems[i] + y]--;
    cnt[2 * y]--;
}

// 区間の長さ len（>= 1）に入る要素数の上限。len >= n なら上限なし扱い（大きな値）。
inline int cap_len(int len) { return len < curN ? aval[len] : MAXN; }

// 事実 (5): 未定 need 個が (L, H) に入るとき、区間 [b, c] の不等式をすべて確かめる。
// 下側の要素 lowv[i] 以上の下側要素は k_low - i 個、上側の要素 highv[j] 以下の上側要素は k_high - j 個。
bool interval_ok(int L, int H, int need) {
    // b は lowv[i]（i < k_low）または L+1、c は highv[j]（j < k_high）または H-1
    for (int i = 0; i <= k_low; i++) {
        int b = (i < k_low) ? lowv[i] : L + 1;
        int below = (i < k_low) ? k_low - i : 0;          // [b, L] の下側要素
        for (int j = 0; j <= k_high; j++) {
            int c = (j < k_high) ? highv[j] : H - 1;
            int above = (j < k_high) ? k_high - j : 0;    // [H, c] の上側要素
            if (c < b) continue;
            if (below + above + need > cap_len(c - b + 1)) return false;
        }
    }
    return true;
}

void dfs(bool top_turn) {
    if (found) return;
    const int need = target - k_all;
    if (need == 0) { found = true; return; }
    const int L = lowv[k_low - 1], H = highv[k_high - 1];
    if (H - L - 1 < need) return;
    if (!interval_ok(L, H, need)) return;
    // 事実 (6)
    int avail[MAXN + 1], na = 0;
    for (int y = L + 1; y < H; y++) if (can_add(y)) avail[na++] = y;
    if (na < need) return;
    // 事実 (5)(6): 未定の要素はすべて [avail[0], avail[na-1]] に入るので、
    // その範囲を (L, H) の代わりに使って区間の不等式を確かめ直す。
    if (!interval_ok(avail[0] - 1, avail[na - 1] + 1, need)) return;

    if (top_turn) {
        // 上側で y を選ぶ（大きい順）。残り need-1 個は (L, y) の avail に入る。
        for (int idx = na - 1; idx >= 0; idx--) {
            int y = avail[idx];
            if (idx < need - 1) break;                    // (L, y) の avail は idx 個
            if (k_high == 1) {                            // y = s_{t-1}
                // (4): g_top = n - y <= g_bot = s_2 - 1 <= y - 1。t = 3 なら s_2 = y。
                if (curN - y > y - 1) break;              // y が小さくなるほど厳しい
            }
            highv[k_high++] = y;
            push(y);
            bool ok = interval_ok(L, y, need - 1);
            if (ok) dfs(!top_turn);
            pop();
            k_high--;
            if (found) return;
        }
    } else {
        // 下側で y を選ぶ（小さい順）。残り need-1 個は (y, H) の avail に入る。
        for (int idx = 0; idx < na; idx++) {
            int y = avail[idx];
            if (na - 1 - idx < need - 1) break;           // (y, H) の avail は na-1-idx 個
            if (k_low == 1) {                             // y = s_2
                int g_top = curN - highv[1];              // highv[1] = s_{t-1}（すでに決定）
                if (y - 1 < g_top) continue;              // (4)
            }
            lowv[k_low++] = y;
            push(y);
            bool ok = interval_ok(y, H, need - 1);
            if (ok) dfs(!top_turn);
            pop();
            k_low--;
            if (found) return;
        }
    }
}

int next_value(int n) {
    if (n == 1) return 1;
    target = aval[n - 1] + 1;
    if (target <= 2) return target;       // {1, n} は常に B_2[2]（和 2, n+1, 2n は相異なる）
    curN = n;
    std::memset(cnt, 0, sizeof cnt);
    k_all = 0; k_low = 0; k_high = 0;
    push(1); lowv[k_low++] = 1;
    push(n); highv[k_high++] = n;
    found = false;
    dfs(true);                            // 最初は上側で s_{t-1} を決める
    return found ? target : aval[n - 1];
}

bool parse_limit(const char *text, int *value) {
    if (!*text) return false;
    for (const char *p = text; *p; p++)
        if (*p < '0' || *p > '9') return false;
    errno = 0;
    char *end;
    long parsed = std::strtol(text, &end, 10);
    if (errno == ERANGE || *end || parsed < 1 || parsed > MAXN) return false;
    *value = static_cast<int>(parsed);
    return true;
}

}  // namespace

int main(int argc, char **argv) {
    int N = 70;
    if (argc == 2 && (!std::strcmp(argv[1], "--help") || !std::strcmp(argv[1], "-h"))) {
        std::printf("Usage: %s [N] (N is 1..%d; default 70)\n", argv[0], MAXN);
        return 0;
    }
    if (argc > 2 || (argc == 2 && !parse_limit(argv[1], &N))) {
        std::fprintf(stderr, "Usage: %s [N]; N must be a decimal integer in 1..%d.\n", argv[0], MAXN);
        return 1;
    }
    aval[0] = 0;
    for (int n = 1; n <= N; n++) {
        aval[n] = next_value(n);
        if (std::printf(n == 1 ? "%d" : ", %d", aval[n]) < 0 || std::fflush(stdout) != 0) {
            std::perror("write standard output");
            return 1;
        }
    }
    if (std::printf("\n") < 0 || std::fflush(stdout) != 0) {
        std::perror("write standard output");
        return 1;
    }
    return 0;
}
