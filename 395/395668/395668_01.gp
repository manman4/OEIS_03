\\ a(n) = A395665(n)/(n+1) = (1/10) * (binomial(5*n+4,n) + Sum_{k=0..n+1} 4^(n+1-k) * binomial(5*n+5,k)).
a(n) = (1/10) * (binomial(5*n+4,n) + sum(k=0, n+1, 4^(n+1-k) * binomial(5*n+5,k)));
for(n=0, 20, print1(a(n), ", "));

\\ Sum_{k>=1} a(k-1) * x^k/k = (1/10) * log( Sum_{k>=0} binomial(5*k+5,k) * x^k ).
my(N=30, x='x+O('x^N)); Vec(sum(k=1, N, a(k-1)*x^k/k) - (1/10)*log(sum(k=0, N, binomial(5*k+5,k)*x^k)))