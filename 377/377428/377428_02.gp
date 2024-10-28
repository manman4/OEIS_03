M=16;

\\ a(n) = (4/(4*n+4)!) * Sum_{k=0..n} (4*n+k+3)! * Stirling2(n,k).
a(n) = (4/(4*n+4)!) * sum(k=0, n, (4*n+k+3)! * stirling(n,k,2));
for(n=0, M, print1(a(n), ", ")) 