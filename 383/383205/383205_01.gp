M=22;

\\ a(n) = Sum_{k=3..n} 2^(n-k) * binomial(k,3) * Stirling2(n,k).
a(n) = sum(k=3, n, 2^(n-k) * binomial(k,3) * stirling(n, k, 2));
for(n=0, M, print1(a(n), ", "));

