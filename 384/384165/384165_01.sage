n = 16

def a(n): return 2^n*rising_factorial(3*n/2, n)
print([a(n) for n in range(n + 1)])