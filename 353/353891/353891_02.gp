M=22;

a(n) = n!*sum(k=0, n\4, (2*k)!*abs(stirling(n-2*k, 2*k, 1))/(4^k*k!*(n-2*k)!));
for(n=0, M, print1(a(n), ", "));