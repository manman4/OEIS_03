M=22;

\\ a(n) = Sum_{k=0..n} binomial(n+k-1,k) * binomial(n+k-1,n-k).
a(n) = sum(k=0, n, binomial(n+k-1, k) * binomial(n+k-1, n-k));
for(n=0, M, print1(a(n), ", "));