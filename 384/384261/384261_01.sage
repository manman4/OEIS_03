n = 17

def a(n): return rising_factorial(2*n-1, n)
print([a(n) for n in range(n + 1)])