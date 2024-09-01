M=16;

\\ a(n) = 4 * Sum_{k=0..n} (3*n+4)^(k-1) * Stirling2(n,k).
a(n) = 4 * sum(k=0, n, (3*n+4)^(k-1) * stirling(n, k, 2));
for(n=0, M, print1(a(n), ", "));
