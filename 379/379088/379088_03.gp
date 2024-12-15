a(n) = sum(k=0, n\2, binomial(3*n+k-1, k)*binomial(3*n+k, n-2*k));
my(N=30, x='x+O('x^N)); Vec(exp( 1/3 * sum(k=1, N, a(k)*x^k/k) ) )  
