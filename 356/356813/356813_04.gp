M=20;

a(n) = n!*sum(k=0, n\2, (-1)^k*3^(n-k)*stirling(n-k, k, 2)/(n-k)!);
for(n=0, M, print1(a(n), ", "));