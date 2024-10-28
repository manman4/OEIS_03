M=16;

\\ a(n) = (1/(4*n+1)!) * Sum_{k=0..n} (4*n+k)! * |Stirling1(n,k)|.
a(n) = (1/(4*n+1)!) * sum(k=0, n, (4*n+k)! * abs(stirling(n,k,1)));
for(n=0, M, print1(a(n), ", ")) 