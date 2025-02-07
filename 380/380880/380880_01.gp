M=17;

\\ a(n) = 3 * Sum_{k=0..n} k^(n-k) * (3*n+3)^(k-1) * binomial(n,k).
a(n) = 3 * sum(k=0, n, k^(n-k) * (3*n+3)^(k-1) * binomial(n,k));
for(n=0, M, print1(a(n),", "));

