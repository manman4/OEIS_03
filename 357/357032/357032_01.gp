M=21;

a(n) = sum(k=0, n\3, (3*k)!*(n+1)^(k-1)*stirling(n, 3*k, 2)/(6^k*k!));
for(n=0, M, print1(a(n), ", "));