M=23;

a(n) = n!*sum(k=0, n\4, (2*k)!*stirling(n-2*k, 2*k, 2)/(4^k*k!*(n-2*k)!));
for(n=0, M, print1(a(n), ", "));