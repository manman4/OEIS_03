M=19;

\\ a(n) = (-2)^n * Sum_{k=0..n} 3^k * binomial(-9/2,k) * binomial(-9/2,n-k).
a(n) = (-2)^n * sum(k=0, n, 3^k * binomial(-9/2,k) * binomial(-9/2,n-k));
for(n=0, M, print1(a(n), ", "));