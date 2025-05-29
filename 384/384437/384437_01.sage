N = 9

def a(n, k): return QQ(q_binomial(2*n, n, k)/q_binomial(n + 1, 1, k))
print([a(k, n - k) for n in range(0, N + 1) for k in range(n + 1)])