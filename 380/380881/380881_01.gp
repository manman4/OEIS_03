M=17;

\\ a(n) = Sum_{k=0..n} k^(n-k) * (3*n+1)^(k-1) * binomial(n,k).
a(n) = sum(k=0, n, k^(n-k) * (3*n+1)^(k-1) * binomial(n,k));
for(n=0, M, print1(a(n),", "));

