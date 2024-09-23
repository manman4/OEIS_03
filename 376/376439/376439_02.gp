M=18;

\\ a(n) = (3 * n!/(3*n+3)!) * Sum_{k=0..floor(n/3)} (3*n+k+2)! * Stirling2(n-2*k,k)/(n-2*k)!.
a(n) = 3 * n!/(3*n+3)!*sum(k=0, n\3, (3*n+k+2)!*stirling(n-2*k, k, 2)/(n-2*k)!);
for(n=0, M, print1(a(n), ", "));