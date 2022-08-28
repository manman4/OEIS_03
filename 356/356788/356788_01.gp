M=19;

a(n) = n!*sum(k=0, n, (n+k+1)^(k-1)*stirling(n-k, k, 2)/(n-k)!);
for(n=0, M, print1(a(n), ", "));