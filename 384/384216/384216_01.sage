N = 10

# A(n,k) = k^n * FallingFactorial(n/k,n).
def a(n, k): return k^n*falling_factorial(QQ(n)/k, n)
print([a(k, n - k + 1) for n in range(0, N + 1) for k in range(n + 1)])