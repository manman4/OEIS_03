from sage.combinat.q_analogues import q_catalan_number

N = 9

def c(n): return QQ(q_binomial(2*n, n, n))/QQ(q_binomial(n + 1, 1, n))
print([c(n) for n in range(N + 1)])

print("The following calculations contain bugs.")
def b(n): return QQ(q_binomial(2*n, n, n)/q_binomial(n + 1, 1, n))
print([b(n) for n in range(N + 1)])

def a(n): return q_catalan_number(n, n)
print([a(n) for n in range(N + 1)])