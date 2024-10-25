M=18;

\\ a(n) = Sum_{k=0..n} (3*n-k)!/(3*n-2*k+1)! * |Stirling1(n,k)|.
a(n) = sum(k=0, n, (3*n-k)!/(3*n-2*k+1)! * abs(stirling(n,k,1)));
for(n=0, M, print1(a(n), ", ")) 