M=19;

\\ a(n) = Sum_{k=0..floor((3*n+1)/4)} (3*n-3*k)!/(3*n-4*k+1)! * |Stirling1(n,k)|.
a(n) = sum(k=0, (3*n+1)\4, (3*n-3*k)!/(3*n-4*k+1)! * abs(stirling(n,k,1)));
for(n=0, M, print1(a(n), ", ")) 