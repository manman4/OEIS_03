n = 22

def a(n): return 4^n*rising_factorial((n+3)/4, n)
print([a(n) for n in range(n + 1)])