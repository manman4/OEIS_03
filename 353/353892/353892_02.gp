M=23;

a(n) = n!*sum(k=0, n\6, (3*k)!*abs(stirling(n-3*k, 3*k, 1))/(36^k*k!*(n-3*k)!));
for(n=0, M, print1(a(n), ", "));