M=21;

\\ a(n) = (2 * n!/(2n+2)!) * Sum_{k=0..floor(n/2)} (3*n-2*k+1)! * Stirling2(k,n-2*k)/k!.
a(n) = 2 * n!/(2*n+2)!*sum(k=0, n\2, (3*n-2*k+1)!*stirling(k, n-2*k, 2)/k!);
for(n=0, M, print1(a(n), ", "));