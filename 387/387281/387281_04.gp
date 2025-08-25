M=21;

\\ a(n) = 2^n * Sum_{k=0..n} (-2)^k * binomial(-7/2,k) * binomial(n+6,n-k).
a(n) = 2^n * sum(k=0, n, (-2)^k * binomial(-7/2,k) * binomial(n+6,n-k));
for(n=0, M, print1(a(n), ", "));