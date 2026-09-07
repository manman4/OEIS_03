#!/usr/bin/env python3
"""パスの反Ramsey数 AR(n, P_k) の計算.

AR(n, P_k) は、虹色の P_k (k 頂点のパス) を含まない K_n の辺彩色が
使用できる色数の最大値である。色数とは彩色 c の像の大きさ |c(E(K_n))|、
すなわち実際に使用される色の個数を指す。

実装している探索は2種類:

  search  : 制限増加列 (restricted growth string) による枝刈り探索。
            色の名前の付け替えを自動的に除去し、部分彩色の段階で
            虹色 P_k が現れた時点で枝を切る。n <= 7 程度まで実用的。

  naive   : 辺集合の集合分割を全列挙する素朴な方法。辺数が少ない場合
            (n <= 5) のみ。search の結果を独立に照合するために使う。

使い方:
    python3 anti_ramsey_paths.py --table 6          # 三角配列を表示
    python3 anti_ramsey_paths.py --check            # 2つの方法で照合
    python3 anti_ramsey_paths.py --witness 4 4      # 極値彩色を表示
    python3 anti_ramsey_paths.py --value 7 4        # 値のみ

実行時間の目安 (CPython 3.12): n <= 6 は全体で数秒、T(7,4) は約7秒、
T(7,5) は約2分、T(7,6) 以降はさらに長い。--table と --check は
1件ごとに所要時間を出力する。
"""

from __future__ import annotations

import argparse
import sys
import time
from itertools import combinations, permutations


# ---------------------------------------------------------------------------
# K_n の辺とパスの列挙
# ---------------------------------------------------------------------------

def edge_list(n: int) -> list[tuple[int, int]]:
    """K_n の辺を「頂点を1個ずつ追加する」順に並べる。

    辺 {u,v} (u < v) を (v, u) で整列させる。この順序だと、頂点集合
    {0,...,m} の内部の辺がすべて頂点 m+1 に触れる辺より前に来る。
    パスが早い段階で完成するので枝刈りがよく効く。
    """
    return sorted(combinations(range(n), 2), key=lambda e: (e[1], e[0]))


def vertex_paths(n: int, k: int) -> list[tuple[int, ...]]:
    """K_n に含まれる k 頂点のパスを頂点列として列挙する。

    逆向きの重複を避けるため、最初の頂点が最後の頂点より小さいものだけを
    採用する。個数は n! / (n-k)! / 2 になる。
    """
    if k < 2:
        raise ValueError("k >= 2 が必要")
    return [p for p in permutations(range(n), k) if p[0] < p[-1]]


def paths_by_last_edge(n: int, k: int, edges: list[tuple[int, int]]):
    """各パスを「その辺番号の最大値」に割り当てて返す。

    辺を番号順 0,1,...,m-1 に彩色していくと、あるパスの全辺が塗り終わる
    のは、そのパスの辺番号の最大値が塗られた瞬間である。したがって
    番号 i を塗った直後に buckets[i] のパスだけを検査すれば、
    「全辺が塗られたパス」をちょうど1回ずつ調べたことになる。

    最後に塗られる辺そのものは取り除き、「残りの辺」の組だけを保存する。
    その残りの辺の色は検査の時点ですべて確定している。
    """
    index = {e: i for i, e in enumerate(edges)}
    buckets: list[list[tuple[int, ...]]] = [[] for _ in edges]
    for p in vertex_paths(n, k):
        eidx = tuple(index[(min(a, b), max(a, b))] for a, b in zip(p, p[1:]))
        last = max(eidx)
        buckets[last].append(tuple(e for e in eidx if e != last))
    return buckets


# ---------------------------------------------------------------------------
# 枝刈り探索
# ---------------------------------------------------------------------------

def anti_ramsey(n: int, k: int, want_witness: bool = False):
    """AR(n, P_k) を枝刈り探索で計算する。

    返り値は want_witness が偽なら整数、真なら (値, 極値彩色) の組。
    極値彩色は辺番号順の色番号のリストで与える。
    """
    if not 3 <= k <= n:
        raise ValueError("3 <= k <= n が必要")

    edges = edge_list(n)
    m = len(edges)
    buckets = paths_by_last_edge(n, k, edges)

    color = [-1] * m
    best = 0
    witness: list[int] | None = None

    def allowed_mask(i: int, used: int) -> int:
        """辺 i に塗ってよい色のビットマスクを求める。

        辺 i を通り、i が最後に塗られるパスを考える。そのパスの他の辺の色は
        すでに確定している。他の辺の色に重複があれば、辺 i を何色にしても
        そのパスは虹色にならないので無視してよい。他の辺の色がすべて異なる
        場合、パスが虹色になるのは color(i) がそれらのどれとも違うときだけ
        なので、辺 i の色はその色集合の中から選ばねばならない。
        よって許容色は、そうしたパスの色集合すべての共通部分になる。
        """
        allowed = (1 << (used + 1)) - 1  # 色 0..used-1 と新色 used
        for path in buckets[i]:
            mask = 0
            distinct = True
            for e in path:
                bit = 1 << color[e]
                if mask & bit:
                    distinct = False
                    break
                mask |= bit
            if distinct:
                allowed &= mask
                if allowed == 0:
                    return 0
        return allowed

    def rec(i: int, used: int) -> None:
        nonlocal best, witness
        # 残りの辺をすべて新色にしても best を超えられないなら打ち切る
        if used + (m - i) <= best:
            return
        if i == m:
            best = used
            if want_witness:
                witness = color[:]
            return
        # 制限増加列: 色番号は 0..used のみ。used が「新しい色」。
        # 新色を先に試すと良い解が早く見つかり、上の枝刈りがよく効く。
        allowed = allowed_mask(i, used)
        if allowed == 0:
            return
        for c in range(used, -1, -1):
            if not (allowed >> c) & 1:
                continue
            color[i] = c
            rec(i + 1, used + (1 if c == used else 0))
        color[i] = -1

    rec(0, 0)
    return (best, witness) if want_witness else best


# ---------------------------------------------------------------------------
# 素朴な全列挙 (照合用)
# ---------------------------------------------------------------------------

def _set_partitions(size: int):
    """0..size-1 の制限増加列をすべて生成する (集合分割と1対1)。"""
    assign = [0] * size

    def rec(i: int, used: int):
        if i == size:
            yield assign, used
            return
        for c in range(used + 1):
            assign[i] = c
            yield from rec(i + 1, used + (1 if c == used else 0))

    yield from rec(0, 0)


def anti_ramsey_naive(n: int, k: int) -> int:
    """辺集合の分割を全列挙して AR(n, P_k) を求める。n <= 5 向け。"""
    if not 3 <= k <= n:
        raise ValueError("3 <= k <= n が必要")

    edges = edge_list(n)
    index = {e: i for i, e in enumerate(edges)}
    all_paths = [
        tuple(index[(min(a, b), max(a, b))] for a, b in zip(p, p[1:]))
        for p in vertex_paths(n, k)
    ]

    best = 0
    for assign, used in _set_partitions(len(edges)):
        if used <= best:
            continue
        if all(len({assign[e] for e in path}) < len(path) for path in all_paths):
            best = used
    return best


# ---------------------------------------------------------------------------
# 検証と表示
# ---------------------------------------------------------------------------

def verify_witness(n: int, k: int, coloring: list[int]) -> tuple[bool, int]:
    """与えられた彩色が虹色 P_k を含まないことを独立に確認する。"""
    edges = edge_list(n)
    index = {e: i for i, e in enumerate(edges)}
    for p in vertex_paths(n, k):
        cols = [coloring[index[(min(a, b), max(a, b))]] for a, b in zip(p, p[1:])]
        if len(set(cols)) == len(cols):
            return False, len(set(coloring))
    return True, len(set(coloring))


def show_witness(n: int, k: int) -> None:
    value, coloring = anti_ramsey(n, k, want_witness=True)
    ok, ncolors = verify_witness(n, k, coloring)
    print(f"AR({n}, P_{k}) = {value}")
    print(f"独立検証: 虹色 P_{k} なし = {ok}, 使用色数 = {ncolors}")
    print("極値彩色 (色ごとの辺):")
    classes: dict[int, list[str]] = {}
    for e, c in zip(edge_list(n), coloring):
        classes.setdefault(c, []).append(f"{e[0]}{e[1]}")
    for c in sorted(classes):
        print(f"  色 {c}: {{{', '.join(classes[c])}}}")


def show_table(nmax: int, timing: bool = True) -> None:
    print(f"T(n,k) = AR(n, P_k),  3 <= k <= n <= {nmax}")
    print()
    rows = []
    for n in range(3, nmax + 1):
        row = []
        for k in range(3, n + 1):
            t = time.perf_counter()
            v = anti_ramsey(n, k)
            row.append(v)
            if timing:
                print(f"  T({n},{k}) = {v}   ({time.perf_counter() - t:.2f}s)",
                      flush=True)
        rows.append(row)

    print()
    header = "  n \\ k |" + "".join(f"{k:>5}" for k in range(3, nmax + 1))
    print(header)
    print("  " + "-" * (len(header) - 2))
    for n, row in zip(range(3, nmax + 1), rows):
        print(f"  {n:>5} |" + "".join(f"{v:>5}" for v in row))
    print()
    flat = [v for row in rows for v in row]
    print("読み下し順の数列:")
    print("  " + ", ".join(map(str, flat)))


def run_checks(nmax_naive: int = 5, nmax_fast: int = 7) -> int:
    """枝刈り探索を素朴な列挙および証明済みの値と照合する。

    [1] は素朴な列挙を使うので n <= 5 程度まで。
    [2][3] は k=3,4 に限るので n=7 まででも数秒から十数秒で終わる。
    k >= 5 を含む全体像は --table を使う (n=7, k=5 以降は数分かかる)。
    """
    failures = 0

    print(f"[1] 枝刈り探索 vs 素朴な全列挙 (n <= {nmax_naive})")
    for n in range(3, nmax_naive + 1):
        for k in range(3, n + 1):
            t = time.perf_counter()
            fast = anti_ramsey(n, k)
            slow = anti_ramsey_naive(n, k)
            ok = fast == slow
            failures += not ok
            print(f"    n={n} k={k}: 探索={fast} 列挙={slow}  "
                  f"{'ok' if ok else 'NG'}  ({time.perf_counter() - t:.2f}s)",
                  flush=True)

    print()
    print(f"[2] ノートで証明した値との照合 (n <= {nmax_fast})")
    expected: list[tuple[int, int, int]] = []
    for n in range(3, nmax_fast + 1):
        expected.append((n, 3, 1))                      # T(n,3) = 1
    if nmax_fast >= 4:
        expected.append((4, 4, 3))                      # T(4,4) = 3
    for n in range(5, nmax_fast + 1):
        expected.append((n, 4, 2))                      # T(n,4) = 2
    for n, k, want in expected:
        t = time.perf_counter()
        got = anti_ramsey(n, k)
        ok = got == want
        failures += not ok
        print(f"    T({n},{k}): 予想={want} 計算={got}  {'ok' if ok else 'NG'}"
              f"  ({time.perf_counter() - t:.2f}s)", flush=True)

    print()
    print("[3] 極値彩色の妥当性")
    for n, k, _ in expected:
        value, coloring = anti_ramsey(n, k, want_witness=True)
        no_rainbow, ncolors = verify_witness(n, k, coloring)
        ok = no_rainbow and ncolors == value
        failures += not ok
        print(f"    n={n} k={k}: 値={value} 色数={ncolors} 虹色なし={no_rainbow}"
              f"  {'ok' if ok else 'NG'}", flush=True)

    print()
    print("失敗なし" if failures == 0 else f"失敗 {failures} 件")
    return failures


def main() -> int:
    ap = argparse.ArgumentParser(
        description="AR(n, P_k) の計算",
        epilog="目安の実行時間 (CPython): n<=6 は数秒、T(7,4) は約7秒、"
               "T(7,5) は約2分、T(7,6) 以降はさらに長い。",
    )
    ap.add_argument("--table", type=int, metavar="NMAX",
                    help="3 <= k <= n <= NMAX の三角配列を表示")
    ap.add_argument("--check", action="store_true",
                    help="素朴な列挙およびノートの証明値と照合する")
    ap.add_argument("--naive-max", type=int, default=5, metavar="N",
                    help="--check で素朴な列挙を使う上限 (既定 5)")
    ap.add_argument("--check-max", type=int, default=7, metavar="N",
                    help="--check で k=3,4 を確認する上限 (既定 7)")
    ap.add_argument("--witness", type=int, nargs=2, metavar=("N", "K"),
                    help="AR(N, P_K) を達成する彩色を表示")
    ap.add_argument("--value", type=int, nargs=2, metavar=("N", "K"),
                    help="AR(N, P_K) の値のみ表示")
    args = ap.parse_args()

    if not any([args.table, args.check, args.witness, args.value]):
        ap.print_help()
        return 0

    if args.value:
        print(anti_ramsey(*args.value))
    if args.witness:
        show_witness(*args.witness)
    if args.table:
        show_table(args.table)
    if args.check:
        return 1 if run_checks(args.naive_max, args.check_max) else 0
    return 0


if __name__ == "__main__":
    sys.exit(main())
