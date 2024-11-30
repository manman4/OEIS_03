M=20;

\\ a(n) = Sum_{k=0..n} 4^k * binomial(n/2+k-1,k) * binomial(n-1,n-k).
a(n) = sum(k=0, n, 4^k * binomial(n/2+k-1, k) * binomial(n-1, n-k));
for(n=0, M, print1(a(n), ", "));