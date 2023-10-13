M=20;

a(n) = sum(k=0, n\4, (4*k)!*abs(stirling(n, 4*k, 1)));
for(n=0, M, print1(a(n), ", "));