n = 22

def a(n): return 5^n*falling_factorial(n/5, n)
print([a(n) for n in range(n + 1)])