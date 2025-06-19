M=20;

\\ a(n) = 2 * Sum_{k=0..n} (n-k+2)^(k-1) * (-k)^(n-k) * binomial(n,k).
a(n) = 2 * sum(k=0, n, (n-k+2)^(k-1) * (-k)^(n-k) * binomial(n,k));
for(n=0, M, print1(a(n), ", "));