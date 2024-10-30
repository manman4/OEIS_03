M=16;

\\ a(n) = 2 * Sum_{k=0..n} (5*k+1)!/(4*k+2)! * |Stirling1(n,k)|.
a(n) = 2 * sum(k=0, n, (5*k+1)!/(4*k+2)! * abs(stirling(n,k,1)));
for(n=0, M, print1(a(n), ", ")) 