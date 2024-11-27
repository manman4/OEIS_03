a378462(n) = sum(k=0, n\2, binomial(n+k-1, k)*binomial(2*n+k-1, n-2*k));
\\ G.f.: exp( Sum_{k>=1} A378462(k) * x^k/k ).
my(N=30, x='x+O('x^N)); Vec(exp( sum(k=1, N, a378462(k)*x^k/k) ) )


