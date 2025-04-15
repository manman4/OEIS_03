M=18;

\\ a(n) = Sum_{k=0..n} (k+1)^n * Stirling1(n,k).
a(n) = sum(k=0, n, (k+1)^n * stirling(n,k,1));
for(n=0, M, print1(a(n),", "));
