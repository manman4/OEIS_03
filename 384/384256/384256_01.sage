n = 22

def a(n): return 3^n*rising_factorial((n+1)/3, n)
print([a(n) for n in range(n + 1)])