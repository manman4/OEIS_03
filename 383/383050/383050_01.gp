M=21;

\\ a(n) = Sum_{k=0..n} (k+1)^6 * Stirling1(n,k).
a(n) = sum(k=0, n, (k+1)^6 * stirling(n,k,1));
for(n=0, M, print1(a(n),", "));
