M=16;

a(n) = sum(k=0, n, (-1)^(n-k)*stirling(n+1, k+1, 1)*stirling(k+n, n, 2));
for(n=0, M, print1(a(n), ", "));