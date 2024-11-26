a378407(n) = sum(k=0, n\3, binomial(n, k)*binomial(n+2*k, n-3*k));
\\ G.f.: exp( Sum_{k>=1} A378407(k) * x^k/k ).
my(N=30, x='x+O('x^N)); Vec(exp( sum(k=1, N, a378407(k)*x^k/k) ) )


