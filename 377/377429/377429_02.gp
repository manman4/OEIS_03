M=16;

\\ a(n) = (4/(4*n+4)!) * Sum_{k=0..n} (4*n+k+3)! * |Stirling1(n,k)|.
a(n) = (4/(4*n+4)!) * sum(k=0, n, (4*n+k+3)! * abs(stirling(n,k,1)));
for(n=0, M, print1(a(n), ", ")) 