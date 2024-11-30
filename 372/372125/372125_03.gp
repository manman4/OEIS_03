a(n) = sum(k=0, n, 4^(n-k)*binomial(n+k-1, k)*binomial(k/2, n-k));
\\ G.f.: exp( Sum_{k>=1} A378554(k) * x^k/k ).
my(N=30, x='x+O('x^N)); Vec(exp( sum(k=1, N, a(k)*x^k/k) )) 
