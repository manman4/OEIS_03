M=24;

a(n) = n!*sum(k=0, n\6, (3*k)!*stirling(n-3*k, 3*k, 2)/(36^k*k!*(n-3*k)!));
for(n=0, M, print1(a(n), ", "));