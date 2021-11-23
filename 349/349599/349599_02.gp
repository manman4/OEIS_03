M=19;

a(n) = sum(k=0, n, (-1)^(n-k)*(2*n+1)^(k-1)*stirling(n, k, 2));
for(n=0, M, print1(a(n), ", "));