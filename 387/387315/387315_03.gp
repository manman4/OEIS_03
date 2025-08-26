M=18;

\\ a(n) = (-1)^n * Sum_{k=0..n} 13^k * binomial(-5/2,k) * binomial(-5/2,n-k).
a(n) = (-1)^n * sum(k=0, n, 13^k * binomial(-5/2,k) * binomial(-5/2,n-k));
for(n=0, M, print1(a(n), ", "));