a(n) = sum(k=0, n, binomial(4*n+k-1, k)*binomial(4*n+k, n-k));
my(N=30, x='x+O('x^N)); Vec(exp( sum(k=1, N, a(k)*x^k/k) ) )  