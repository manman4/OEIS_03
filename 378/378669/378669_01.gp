M=20;

\\ a(n) = 3 * Sum_{k=0..n} 2^k * (-1)^(n-k) * binomial(n,k) * binomial(4*n+k+3,n)/(4*n+k+3).
a(n) = 3 * sum(k=0, n, 2^k*(-1)^(n-k)*binomial(n,k)*binomial(4*n+k+3,n)/(4*n+k+3));
for(n=0, M, print1(a(n), ", "))


