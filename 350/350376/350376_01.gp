M=16;

a(n) = sum(k=0, n, stirling(n+k, n, 2)*stirling(2*n-k, n, 2));
for(n=0, M, print1(a(n), ", "));