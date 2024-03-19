M=19;

\\ a(n) = Sum_{k=1..n} (n+k-2)!/(n-1)! * Stirling1(n,k).
a(n) = sum(k=1, n, (n+k-2)!/(n-1)! * stirling(n,k,1));
for(n=0, M, print1(a(n)", "))