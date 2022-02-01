M=8;

a(n) = sum(k=0, n, k!*k^(k*n)*stirling(n, k, 2));
for(n=0, M, print1(a(n), ", "));