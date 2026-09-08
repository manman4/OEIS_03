"""A397205: max |S|*|T| over S,T ⊆ {1..n} with all products s*t distinct."""
import os
from math import gcd, isqrt
from ortools.sat.python import cp_model   # pip install ortools

def ratio_classes(n):
    # 既約比 q/p -> その比を持つペア (kq, kp) 全体
    return {(q, p): [(k*q, k*p) for k in range(1, n//p + 1)]
            for p in range(2, n+1) for q in range(1, p) if gcd(q, p) == 1}

def primes_upto(n):
    s = bytearray([1]) * (n+1)
    for i in range(2, isqrt(n)+1):
        if s[i]: s[i*i::i] = bytearray(len(s[i*i::i]))
    return [i for i in range(2, n+1) if s[i]]

def find_swaps(n):
    """構造を保つ互換 (m*p <-> m*q) を実際に検証して集める（対称性除去用）"""
    cid = {(i, j): (i//gcd(i, j), j//gcd(i, j))
           for j in range(2, n+1) for i in range(1, j)}
    buckets = {}
    for p in primes_upto(n):
        if p*p > n: buckets.setdefault(n//p, []).append(p)
    out = []
    for k, ps in buckets.items():
        for p, q in zip(ps, ps[1:]):
            perm = {}
            for m in range(1, k+1):
                perm[m*p], perm[m*q] = m*q, m*p
            f, ok = {}, True
            for (i, j), c in cid.items():
                a, b = perm.get(i, i), perm.get(j, j)
                if f.setdefault(c, cid[(min(a, b), max(a, b))]) != cid[(min(a, b), max(a, b))]:
                    ok = False; break
            if ok and len(set(f.values())) == len(f):
                out.append((p, q, k))
    return out

def _lex_ge(m, A, B):
    e = m.NewConstant(1)
    for i in range(len(A)):
        m.AddBoolOr([e.Not(), A[i], B[i].Not()])
        if i+1 < len(A):
            eq, e2 = m.NewBoolVar(""), m.NewBoolVar("")
            m.Add(A[i] == B[i]).OnlyEnforceIf(eq)
            m.Add(A[i] != B[i]).OnlyEnforceIf(eq.Not())
            m.AddBoolOr([e.Not(), eq.Not(), e2])
            e = e2

def _feasible(n, cls, swaps, a, tgt, workers):
    """|S| = a かつ |T| >= tgt な解はあるか？"""
    m = cp_model.CpModel()
    x = [None] + [m.NewBoolVar("") for _ in range(n)]
    y = [None] + [m.NewBoolVar("") for _ in range(n)]
    m.Add(sum(x[1:]) == a)
    m.Add(sum(y[1:]) >= tgt)
    for pairs in cls.values():                      # R(S) ∩ R(T) = ∅
        if len(pairs) == 1:
            i, j = pairs[0]
            m.AddBoolOr([x[i].Not(), x[j].Not(), y[i].Not(), y[j].Not()])
        else:
            u, v = m.NewBoolVar(""), m.NewBoolVar("")   # u: Sがこの比を使う, v: Tが使う
            for i, j in pairs:
                m.AddBoolOr([u, x[i].Not(), x[j].Not()])
                m.AddBoolOr([v, y[i].Not(), y[j].Not()])
            m.Add(u + v <= 1)
    for p in primes_upto(n):                        # R(cX)=R(X) なので gcd(S)=gcd(T)=1 と仮定してよい
        keep = [i for i in range(1, n+1) if i % p]
        m.AddBoolOr([x[i] for i in keep])
        m.AddBoolOr([y[i] for i in keep])
    for (p, q, k) in swaps:                         # 大きい素数の入れ替え対称性を除去
        _lex_ge(m, [v for mm in range(1, k+1) for v in (x[mm*p], y[mm*p])],
                   [v for mm in range(1, k+1) for v in (x[mm*q], y[mm*q])])
    s = cp_model.CpSolver()
    s.parameters.num_workers = workers
    if s.Solve(m) not in (cp_model.OPTIMAL, cp_model.FEASIBLE):
        return None
    return ([i for i in range(1, n+1) if s.Value(x[i])],
            [i for i in range(1, n+1) if s.Value(y[i])])

def solve(n, workers=None):
    """a(n) と証拠 (S, T) を返す"""
    workers = workers or os.cpu_count() or 1
    cls, swaps = ratio_classes(n), find_swaps(n)
    best, wit = n, ([1], list(range(1, n+1)))       # 初期下界（素数べきの鎖）
    for p in primes_upto(n):
        T = [p**k for k in range(64) if p**k <= n]
        S = [i for i in range(1, n+1) if i % p]
        if len(S)*len(T) > best: best, wit = len(S)*len(T), (S, T)
    ub, a = n, 1                                    # b_a = max|T| は a について単調非増加
    while a <= ub:
        while True:
            tgt = max(a, best//a + 1)
            if tgt > ub: break
            r = _feasible(n, cls, swaps, a, tgt, workers)
            if r is None:
                ub = tgt - 1; break
            best, wit = a*len(r[1]), r
        a += 1
    return best, wit

def check(n, S, T):
    p = [s*t for s in S for t in T]
    return len(set(p)) == len(p) and max(S+T) <= n

if __name__ == "__main__":
    for n in range(1, 26):
        v, (S, T) = solve(n)
        assert check(n, S, T)
        print(f"a({n}) = {v}\t S={S} T={T}", flush=True)