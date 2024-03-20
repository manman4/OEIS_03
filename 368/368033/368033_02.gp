M=17;

\\ a(n) = Sum_{k=1..n} (2*n+k-2)!/(2*n-1)! * Stirling1(n,k).
a(n) = sum(k=1, n, (2*n+k-2)!/(2*n-1)! * stirling(n,k,1));
for(n=0, M, print1(a(n)", "))