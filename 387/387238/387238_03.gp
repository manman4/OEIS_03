M=20;

\\ a(n) = (-1)^n * Sum_{k=0..n} 5^k * binomial(-7/2,k) * binomial(-7/2,n-k).
a(n) = (-1)^n * sum(k=0, n, 5^k * binomial(-7/2,k) * binomial(-7/2,n-k));
for(n=0, M, print1(a(n), ", "));