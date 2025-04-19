M=22;

\\ a(n) = Sum_{k=2..n} 2^(n-k) * binomial(k,2) * Stirling2(n,k).
a(n) = sum(k=2, n, 2^(n-k) * binomial(k, 2) * stirling(n, k, 2));
for(n=0, M, print1(a(n), ", "));

