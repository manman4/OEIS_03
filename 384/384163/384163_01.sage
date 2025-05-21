n = 16

def a(n): return 3^n*rising_factorial(2*n/3, n)
print([a(n) for n in range(n + 1)])