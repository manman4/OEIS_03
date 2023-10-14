M=13;

a(n) = sum(k=0, n, k!*3^k*k^n*stirling(n, k, 1));
for(n=0, M, print1(a(n), ", "));