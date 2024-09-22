M=20;

\\ a(n) = (2 * n!/(2*n+2)!) * Sum_{k=0..floor(n/2)} (2*n+k+1)! * Stirling2(n-k,k)/(n-k)!.
a(n) = 2*n!/(2*n+2)!*sum(k=0, n\2, (2*n+k+1)!*stirling(n-k, k, 2)/(n-k)!);
for(n=0, M, print1(a(n), ", "));