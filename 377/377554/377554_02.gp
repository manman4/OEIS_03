M=17;

\\ a(n) = (n!/(n+1)) * Sum_{k=0..n} k^(n-k) * binomial(3*n+3,k)/(n-k)!.
a(n) = (n!/(n+1)) * sum(k=0, n, k^(n-k) * binomial(3*n+3,k) / (n-k)!);
for(n=0, M, print1(a(n), ", ")) 