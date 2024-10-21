M=28;

\\ a(n) = 4^n * Sum_{k=0..n} (-1)^k * binomial(1/2,k) * binomial(n-3*k/2-1,n-k).
a(n) = 4^n * sum(k=0, n, (-1)^k * binomial(1/2,k) * binomial(n-3*k/2-1,n-k));
for(n=0, M, print1(a(n), ", ")) 