n = 22

def a(n): return 4^n*falling_factorial(3*n/4, n)
print([a(n) for n in range(n + 1)])