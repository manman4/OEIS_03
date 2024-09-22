M=18;

\\ a(n) = (3 * n!/(3*n+3)!) * Sum_{k=0..floor(n/2)} (3*n+k+2)! * |Stirling1(n-k,k)|/(n-k)!.
a(n) = 3*n!/(3*n+3)!*sum(k=0, n\2, (3*n+k+2)!*abs(stirling(n-k, k, 1))/(n-k)!);
for(n=0, M, print1(a(n), ", "));