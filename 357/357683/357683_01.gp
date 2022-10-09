M=20;

a(n) = sum(k=0, n\2, n^k*abs(stirling(n, 2*k, 1)));
for(n=0, M, print1(a(n), ", "));