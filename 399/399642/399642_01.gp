\\ a(n) = ((n+1)/2) * (binomial(4*n+3,n) + Sum_{k=0..n} 3^(n-k) * binomial(4*n+4,k)).
a(n) = (n+1)*(binomial(4*n+3, n)+sum(k=0, n, 3^(n-k)*binomial(4*n+4, k)))/2;
for(n=0, 25, print1(a(n), ", "));

\\ Sum_{k>=1} a(k-1) * x^k/k^2 = (1/6) * log( Sum_{k>=0} binomial(4*k+2,k) * x^k ).
my(N=30, x='x+O('x^N)); Vec(sum(k=1, N, a(k-1)*x^k/k^2) - (1/6)*log(sum(k=0, N, binomial(4*k+2,k)*x^k)))