N = 9

def a(n): return QQ(q_binomial(2*n, n, n))/QQ(q_binomial(n + 1, 1, n))
print([a(n) for n in range(0, N + 1)])