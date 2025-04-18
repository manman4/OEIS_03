def a_row(n):
    s = sum(2^(n-k)*stirling_number2(n, k)*rising_factorial(x, k) for k in (0..n))
    return expand(s).list()
for n in (0..9): print(a_row(n))

