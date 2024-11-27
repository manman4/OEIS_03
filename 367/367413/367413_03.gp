a378464(n) = sum(k=0, n\3, binomial(n+k-1, k)*binomial(2*n-1, n-3*k));
\\ G.f.: exp( Sum_{k>=1} A378464(k) * x^k/k ).
my(N=30, x='x+O('x^N)); Vec(exp( sum(k=1, N, a378464(k)*x^k/k) ) )


