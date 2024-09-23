M=21;

\\ a(n) = (2 * n!/(2*n+2)!) * Sum_{k=0..floor(n/3)} (2*n+k+1)! * Stirling2(n-2*k,k)/(n-2*k)!.
a(n) = 2 * n!/(2*n+2)!*sum(k=0, n\3, (2*n+k+1)!*stirling(n-2*k, k, 2)/(n-2*k)!);
for(n=0, M, print1(a(n), ", "));