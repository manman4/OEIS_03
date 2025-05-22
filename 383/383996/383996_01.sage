n = 22

def a(n): return 4^n*falling_factorial(n/4, n)
print([a(n) for n in range(n + 1)])