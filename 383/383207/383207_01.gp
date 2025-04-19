M=22;

\\ a(n) = Sum_{k=2..n} 2^(n-k) * Stirling2(n,k) * Stirling2(k,2).
a(n) = sum(k=2, n, 2^(n-k) * stirling(n, k, 2) * stirling(k, 2, 2));
for(n=0, M, print1(a(n), ", "));

