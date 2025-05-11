def a365878(n):
    h = binomial(5*n + 3, n) * hypergeometric([-n, 3*(n + 1)], [-5 * n - 3], -1) / (n + 1)
    return simplify(h)
print([a365878(n) for n in range(24)])