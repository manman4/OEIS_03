N = 9

def a(n): return QQ(q_binomial(2*n, n, n))/QQ(q_binomial(n + 1, 1, n))
print([a(n) for n in range(0, N + 1)])

print("The following calculations contain bugs.")
def b(n): return QQ(q_binomial(2*n, n, n)/q_binomial(n + 1, 1, n))
print([b(n) for n in range(0, N + 1)])