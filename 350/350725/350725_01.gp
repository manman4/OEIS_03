M=19;

a(n) = sum(k=0, n, k!*k^(n-k)*stirling(n, k, 1));
for(n=0, M, print1(a(n), ", "));