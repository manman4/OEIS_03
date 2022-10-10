M=21;

a(n) = sum(k=0, n\2, 4^k*abs(stirling(n, 2*k, 1)));
for(n=0, M, print1(a(n), ", "));