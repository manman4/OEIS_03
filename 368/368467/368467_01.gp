M=24;

\\ a(n) = Sum_{k=0..n} (-1)^k * binomial(2*n,k) * binomial(4*n,n-k).
a(n) = sum(k=0, n, (-1)^k * binomial(2*n, k) * binomial(4*n, n-k));
for(n=0, M, print1(a(n), ", "))

