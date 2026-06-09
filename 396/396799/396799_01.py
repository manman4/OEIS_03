from math import comb

def a_list(n: int) -> list[int]:
    T = [[0] * (n + 1) for _ in range(n + 1)]
    a = [0] * (n + 1); a[0] = 1
    for m in range(0, n + 1):
        g = [0] + [j * a[j - 1] * comb(m - 1, j - 1) for j in range(1, m + 1)]
        T[m][1] = g[m]
        for k in range(2, m + 1):
            T[m][k] = sum(g[j] * T[m - j][k - 1] for j in range(1, m - k + 2))
        a[m] = 1 - sum(a[k] * T[m][k] for k in range(1, m))
    return [T[m][1] for m in range(1, n + 1)]

print(a_list(10)) 
