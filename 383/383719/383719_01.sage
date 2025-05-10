n = 17

def a(n): return ((1+sqrt(2))^(5*(n-5))*q_binomial(n, 5, -(3-2*sqrt(2)))).simplify_full()
print([a(n) for n in range(5, n + 1)])
