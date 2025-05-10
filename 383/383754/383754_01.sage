n = 16

def a(n): return 2^(3*n)*q_binomial(n+3, 3, 3/2)
print([a(n) for n in range(n + 1)])