M=17;

\\ a(n) = 2 * Sum_{k=0..n} k^(n-k) * (2*n+2)^(k-1) * binomial(n,k).
a(n) = 2 * sum(k=0, n, k^(n-k) * (2*n+2)^(k-1) * binomial(n,k));
for(n=0, M, print1(a(n),", "));

