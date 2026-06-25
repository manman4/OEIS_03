M = 30

# a(n) = (1/n) * Sum_{k=0..n} (-1)^k * binomial(n,k) * binomial(3*n-4*k,n-k-1) for n > 0.
def a(n):
    return 1 if n == 0 else sum((-1)^k * binomial(n, k) * binomial(3*n - 4*k, n - k - 1) for k in (0..n)) / n

print([a(n) for n in range(M + 1)])
