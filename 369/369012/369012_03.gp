a378612(n) = sum(k=0, n, binomial(3*n+k-1, k)*binomial(n-1, n-k));
\\ G.f.: exp( Sum_{k>=1} A378612(k) * x^k/k ).
my(N=30, x='x+O('x^N)); Vec(exp( sum(k=1, N, a378612(k)*x^k/k) ) )
