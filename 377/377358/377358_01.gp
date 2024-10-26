M=19;

\\ a(n) = 2 * Sum_{k=0..floor((2*n+2)/3)} (2*n-2*k+1)!/(2*n-3*k+2)! * |Stirling1(n,k)|.
a(n) = 2 * sum(k=0, (2*n+2)\3, (2*n-2*k+1)!/(2*n-3*k+2)! * abs(stirling(n,k,1)));
for(n=0, M, print1(a(n), ", ")) 