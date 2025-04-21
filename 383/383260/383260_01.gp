M=21;

\\ a(n) = Sum_{k=1..n} k * 3^(n-k) * Stirling2(n,k).
a(n) = sum(k=1, n, k * 3^(n-k) * stirling(n, k, 2));
for(n=0, M, print1(a(n), ", "));



