a378405(n) = sum(k=0, n\2, binomial(n, k)*binomial(n+k, n-2*k));
\\ G.f.: exp( Sum_{k>=1} A378405(k) * x^k/k ).
my(N=30, x='x+O('x^N)); Vec(exp( sum(k=1, N, a378405(k)*x^k/k) ) )


