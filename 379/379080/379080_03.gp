a(n) = sum(k=0, n\2, binomial(2*n+k-1, k)*binomial(2*n+k, n-2*k));
my(N=30, x='x+O('x^N)); Vec(exp( sum(k=1, N, a(k)*x^k/k) ) )  
