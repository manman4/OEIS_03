M=20;

\\ a(n) = (3 * n!/(3n+3)!) * Sum_{k=0..floor(n/2)} (4*n-2*k+2)! * |Stirling1(k,n-2*k)|/k!.
a(n) = 3 * n!/(3*n+3)!*sum(k=0, n\2, (4*n-2*k+2)!*abs(stirling(k, n-2*k, 1))/k!);
for(n=0, M, print1(a(n), ", "));