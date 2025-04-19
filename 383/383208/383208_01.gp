M=21;

\\ a(n) = Sum_{k=3..n} 2^(n-k) * Stirling2(n,k) * Stirling2(k,3).
a(n) = sum(k=3, n, 2^(n-k) * stirling(n, k, 2) * stirling(k, 3, 2));
for(n=0, M, print1(a(n), ", "));



