M=14;

a(n) = sum(k=0, n, (-1)^(n-k)*2^k*k^n*k!*stirling(n, k, 2));
for(n=0, M, print1(a(n), ", "));