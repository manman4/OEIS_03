a(n) = sum(k=0, n, 9^k*binomial(n/3+k-1, k)*binomial(n-1, n-k));
\\ G.f.: exp( Sum_{k>=1} A378552(k) * x^k/k ).
my(N=30, x='x+O('x^N)); Vec(exp( sum(k=1, N, a(k)*x^k/k) )) 
