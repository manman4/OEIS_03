M=20;

\\ a(n) = Sum_{k=1..n} k * 2^(k-1) * 3^(n-k) * Stirling2(n,k).
a(n) = sum(k=1, n, k * 2^(k-1) * 3^(n-k) * stirling(n, k, 2));
for(n=0, M, print1(a(n), ", "));



