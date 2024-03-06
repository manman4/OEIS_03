M=22;

\\ a(n) = sum(k=0, n, 3^(n-k)*(n+k)!*stirling(n, k, 1))/(n+1)!;
a(n) = (1/(n+1)!) * sum(k=0, n, 3^(n-k) * (n+k)! * stirling(n, k, 1));
for(n=0, M, print1(a(n),", "))   
