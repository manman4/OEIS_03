M=17;

\\ a(n) = Sum_{k=1..n} (n+3*k-2)!/(n+2*k-1)! * Stirling1(n,k).
a(n) = sum(k=1, n, (n+3*k-2)!/(n+2*k-1)! * stirling(n,k,1));
for(n=0, M, print1(a(n)", "))