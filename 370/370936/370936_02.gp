M=20;

\\ a(n) = (1/(n+1)!) * Sum_{k=0..n} 2^(n-k) * (n+k)! * Stirling1(n,k).
a(n) = (1/(n+1)!) * sum(k=0, n, 2^(n-k) * (n+k)! * stirling(n, k, 1));
for(n=0, M, print1(a(n),", "))   
