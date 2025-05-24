from sympy import rf
def a(n): return rf(3*n-2, n)
print([a(n) for n in range(0, 31)])