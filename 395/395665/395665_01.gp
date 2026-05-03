\\ a(n) = ((n+1)/6) * (binomial(3*n+2,n) + Sum_{k=0..n+1} 2^(n+1-k) * binomial(3*n+3,k)).
a(n) = ((n+1)/6) * (binomial(3*n+2,n) + sum(k=0, n+1, 2^(n+1-k) * binomial(3*n+3,k)));
for(n=0, 20, print1(a(n), ", "));

\\ Sum_{k>=1} a(k-1) * x^k/k^2 = (1/6) * log( Sum_{k>=0} binomial(3*k+3,k) * x^k ).
my(N=30, x='x+O('x^N)); Vec(sum(k=1, N, a(k-1)*x^k/k^2) - (1/6)*log(sum(k=0, N, binomial(3*k+3,k)*x^k)))