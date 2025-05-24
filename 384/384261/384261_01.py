from sympy import rf
def a(n): return rf(2*n-1, n)
print([a(n) for n in range(0, 31)])