M=11;

a(n) = sum(k=0, n, k!*k^(3*n)*stirling(n, k, 1));
for(n=0, M, print1(a(n), ", "));