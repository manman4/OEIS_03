def a365879(n):
    h = binomial(6*n + 4, n) * hypergeometric([-n, 3*(n + 1)], [-6 * n - 4], -1) / (n + 1)
    return simplify(h)
print([a365879(n) for n in range(22)]) 