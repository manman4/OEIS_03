M=18;

\\ a(n) = (3 * n!/(3*n+3)!) * Sum_{k=0..floor(n/2)} (3*n+k+2)! * Stirling2(n-k,k)/(n-k)!.
a(n) = 3*n!/(3*n+3)!*sum(k=0, n\2, (3*n+k+2)!*stirling(n-k, k, 2)/(n-k)!);
for(n=0, M, print1(a(n), ", "));