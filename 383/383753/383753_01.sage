def a_row(n): return [2^(k*(n-k))*q_binomial(n, k, 3/2) for k in (0..n)]
for n in (0..9): print(a_row(n))