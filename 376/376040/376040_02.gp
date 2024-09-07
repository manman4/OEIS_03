M=16;

\\ a(n) = Sum_{k=1..n} (3*n+k-2)!/(3*n-1)! * |Stirling1(n,k)|.
a(n) = sum(k=1, n, (3*n+k-2)!/(3*n-1)! * abs(stirling(n,k,1)));
for(n=0, M, print1(a(n)", "))