M=25;

\\ a(n) = Sum_{k=0..n} 8^k * 9^(n-k) * binomial(-7/2,k) * binomial(n+6,n-k).

a(n) = sum(k=0, n, 8^k * 9^(n-k) * binomial(-7/2,k) * binomial(n+6,n-k));
for(n=0, M, print1(a(n), ", "));