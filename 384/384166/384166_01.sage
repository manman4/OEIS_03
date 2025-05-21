n = 16

def a(n): return 4^n*rising_factorial(3*n/4, n)
print([a(n) for n in range(n + 1)])