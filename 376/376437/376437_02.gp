M=18;

\\ a(n) = (3 * n!/(3*n+3)!) * Sum_{k=0..floor(n/3)} (3*n+k+2)! * |Stirling1(n-2*k,k)|/(n-2*k)!.
a(n) = 3 * n!/(3*n+3)!*sum(k=0, n\3, (3*n+k+2)!*abs(stirling(n-2*k, k, 1))/(n-2*k)!);
for(n=0, M, print1(a(n), ", "));