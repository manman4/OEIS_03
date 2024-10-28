M=16;

\\ a(n) = (2/(4*n+2)!) * Sum_{k=0..n} (4*n+k+1)! * |Stirling1(n,k)|.
a(n) = (2/(4*n+2)!) * sum(k=0, n, (4*n+k+1)! * abs(stirling(n,k,1)));
for(n=0, M, print1(a(n), ", ")) 