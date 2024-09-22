M=17;

\\ a(n) = (2/(3*n+2)!) * Sum_{k=0..n} (3*n+k+1)! * Stirling2(n,k).
a(n) = 2/(3*n+2)!*sum(k=0, n, (3*n+k+1)!*stirling(n, k, 2));
for(n=0, M, print1(a(n), ", "));