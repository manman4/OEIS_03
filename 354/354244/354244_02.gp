M=14;

a(n) = sum(k=0, n, (2*k)!*abs(stirling(n, k, 1)));
for(n=0, M, print1(a(n), ", "));