M=19;

\\ a(n) = 9^n * Sum_{k=0..n} (-1)^k * binomial(-2/3,k) * binomial(n-k/3-1,n-k).
a(n) = 9^n * sum(k=0, n, (-1)^k * binomial(-2/3,k) * binomial(n-k/3-1,n-k));
for(n=0, M, print1(a(n), ", ")) 