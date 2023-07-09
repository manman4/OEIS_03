a(n) = sumdiv(n, d, (d%2==1)*d^(d+1));

my(N=20, x='x+O('x^N)); Vec(exp(sum(k=1, N, a(k) * x^k/k )))