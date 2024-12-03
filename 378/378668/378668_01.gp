M=20;

\\ a(n) = 2 * Sum_{k=0..n} 2^k * (-1)^(n-k) * binomial(n,k) * binomial(4*n+k+2,n)/(4*n+k+2).
a(n) = 2 * sum(k=0, n, 2^k*(-1)^(n-k)*binomial(n,k)*binomial(4*n+k+2,n)/(4*n+k+2));
for(n=0, M, print1(a(n), ", "))


