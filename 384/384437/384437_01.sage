from sage.combinat.q_analogues import q_catalan_number

N = 10
M = 50

def c(n, k): return QQ(q_binomial(2*n, n, k))/QQ(q_binomial(n + 1, 1, k))
print([c(k, n - k) for n in range(N + 1) for k in range(n + 1)])

print("The following calculations contain bugs.")
def b(n, k): return QQ(q_binomial(2*n, n, k)/q_binomial(n + 1, 1, k))
print([b(k, n - k) for n in range(N + 1) for k in range(n + 1)])
print([b(k, n - k) - c(k, n - k) for n in range(N + 1) for k in range(n + 1)])

def a(n, k): return q_catalan_number(n, k)
print([a(k, n - k) for n in range(N + 1) for k in range(n + 1)])
print([c(k, n - k) - a(k, n - k) for n in range(M + 1) for k in range(n + 1)])
