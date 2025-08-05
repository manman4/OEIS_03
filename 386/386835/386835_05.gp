M=25;

\\ a(n) = Sum_{k=0..n} 2^k * binomial(n+k-1,k) * binomial(n-k+2,n-k).
a(n) = sum(k=0, n, 2^k * binomial(n+k-1, k) * binomial(n-k+2, n-k));
for(n=0, M, print1(a(n), ", "))

