M=25;

\\ a(n) = Sum_{k=0..n} 4^k * 6^(n-k) * binomial(-9/2,k) * binomial(n+8,n-k).
a(n) = sum(k=0, n, 4^k * 6^(n-k) * binomial(-9/2,k) * binomial(n+8,n-k));
for(n=0, M, print1(a(n), ", "));