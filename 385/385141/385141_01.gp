M=19;

\\ a(n) = 3 * Sum_{k=0..n} (n-k+3)^(k-1) * (-k)^(n-k) * binomial(n,k).
a(n) = 3 * sum(k=0, n, (n-k+3)^(k-1) * (-k)^(n-k) * binomial(n,k));
for(n=0, M, print1(a(n), ", "));