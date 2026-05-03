\\ a(n) = A395667(n)/(n+1) = (1/14) * (binomial(7*n+6,n) + Sum_{k=0..n+1} 6^(n+1-k) * binomial(7*n+7,k)).
a(n) = (1/14) * (binomial(7*n+6,n) + sum(k=0, n+1, 6^(n+1-k) * binomial(7*n+7,k)));
for(n=0, 20, print1(a(n), ", "));

\\ Sum_{k>=1} a(k-1) * x^k/k = (1/14) * log( Sum_{k>=0} binomial(7*k+7,k) * x^k ).
my(N=30, x='x+O('x^N)); Vec(sum(k=1, N, a(k-1)*x^k/k) - (1/14)*log(sum(k=0, N, binomial(7*k+7,k)*x^k)))