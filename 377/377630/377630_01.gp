M=16;

\\ a(n) = (n!/(n+1)) * Sum_{k=0..n} k^(n-k) * binomial(4*n+4,k)/(n-k)!.
a(n) = (n!/(n+1)) * sum(k=0, n, k^(n-k) * binomial(4*n+4,k) / (n-k)!);
for(n=0, M, print1(a(n), ", ")) 