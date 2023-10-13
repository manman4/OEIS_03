M=15;

a(n) = sum(k=0, n, k!*k^k*stirling(n, k, 2));
for(n=0, M, print1(a(n), ", "));