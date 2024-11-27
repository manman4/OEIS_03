a378461(n) = sum(k=0, n, binomial(n+k-1, k)*binomial(2*n+2*k-1, n-k));
\\ G.f.: exp( Sum_{k>=1} A378461(k) * x^k/k ).
my(N=30, x='x+O('x^N)); Vec(exp( sum(k=1, N, a378461(k)*x^k/k) ) )


