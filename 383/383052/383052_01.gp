M=22;

\\ a(n) = Sum_{k=0..n} (k+1)^3 * Stirling2(n,k).
a(n) = sum(k=0, n, (k+1)^3*stirling(n, k, 2));
for(n=0, M, print1(a(n), ", "));