M=21;

a(n) = n!*sum(k=0, n\2, (n-3*k+1)^(k-1)*stirling(n-k, k, 2)/(n-k)!);
for(n=0, M, print1(a(n), ", "));