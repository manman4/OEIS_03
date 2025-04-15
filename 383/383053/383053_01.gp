M=21;

\\ a(n) = Sum_{k=0..n} (k+1)^4 * Stirling2(n,k).
a(n) = sum(k=0, n, (k+1)^4*stirling(n, k, 2));
for(n=0, M, print1(a(n), ", "));