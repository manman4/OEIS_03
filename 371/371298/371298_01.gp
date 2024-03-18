M=17;

\\ a(n) = (1/(2*n+1)!) * Sum_{k=0..n} 2^(n-k) * (2*n+k)! * Stirling2(n,k).
a(n) = (1/(2*n+1)!) * sum(k=0, n, 2^(n-k) * (2*n+k)! * stirling(n,k,2));
for(n=0, M, print1(a(n)", "))