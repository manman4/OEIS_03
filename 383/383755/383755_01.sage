def a_row(n): return [3^(k*(n-k))*q_binomial(n, k, 4/3) for k in (0..n)]
for n in (0..8): print(a_row(n))