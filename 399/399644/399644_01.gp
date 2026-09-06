\\ a(n) = ((n+1)/2) * (binomial(6*n+5,n) + Sum_{k=0..n} 5^(n-k) * binomial(6*n+6,k)).
a(n) = ((n+1)/2) * (binomial(6*n+5,n) + sum(k=0, n, 5^(n-k) * binomial(6*n+6,k)));
for(n=0, 20, print1(a(n), ", "));

\\ Sum_{k>=1} a(k-1) * x^k/k^2 = (1/8) * log( Sum_{k>=0} binomial(5*k+3,k) * x^k ).
my(N=30, x='x+O('x^N)); Vec(sum(k=1, N, a(k-1)*x^k/k^2) - (1/8)*log(sum(k=0, N, binomial(5*k+3,k)*x^k)))