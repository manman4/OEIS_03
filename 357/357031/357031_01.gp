M=20;

a(n) = sum(k=0, n\2, (2*k)!*(n+1)^(k-1)*stirling(n, 2*k, 2)/(2^k*k!));
for(n=0, M, print1(a(n), ", "));