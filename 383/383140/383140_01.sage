def a_row(n):
    s = sum(3^(n-k)*stirling_number2(n, k)*falling_factorial(x, k) for k in (0..n))
    return expand(s).list()
for n in (0..10): print(a_row(n))
