M=17;

\\ a(n) = 3 * Sum_{k=0..n} (k+3)^(k-1) * Stirling2(n,k).
a(n) = 3 * sum(k=0, n, (k+3)^(k-1) * stirling(n, k, 2));
for(n=0, M, print1(a(n), ", "));
