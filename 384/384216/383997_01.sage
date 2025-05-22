n = 11

# A(n,k) = k^n * FallingFactorial(n/k,n).
def a(n, k): return k^n*falling_factorial(QQ(n)/k, n)
print([a(k, n - k) for n in range(1, n+1) for k in range(n)])