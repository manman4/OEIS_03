\\ a(n) = ((n+1)/8) * (binomial(4*n+3,n) + Sum_{k=0..n+1} 3^(n+1-k) * binomial(4*n+4,k)).
a(n) = ((n+1)/8) * (binomial(4*n+3,n) + sum(k=0, n+1, 3^(n+1-k) * binomial(4*n+4,k)));
for(n=0, 20, print1(a(n), ", "));

\\ Sum_{k>=1} a(k-1) * x^k/k^2 = (1/8) * log( Sum_{k>=0} binomial(4*k+4,k) * x^k ).
my(N=30, x='x+O('x^N)); Vec(sum(k=1, N, a(k-1)*x^k/k^2) - (1/8)*log(sum(k=0, N, binomial(4*k+4,k)*x^k)))