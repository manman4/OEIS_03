M=18;

\\ a(n) = (n!/(n+1)) * Sum_{k=0..n} k^(n-k) * binomial(2*n+2,k)/(n-k)!.
a(n) = (n!/(n+1)) * sum(k=0, n, k^(n-k) * binomial(2*n+2,k) / (n-k)!);
for(n=0, M, print1(a(n), ", ")) 