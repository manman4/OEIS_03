M=23;

a(n) = n!*sum(k=0, n\8, (4*k)!*abs(stirling(n-4*k, 4*k, 1))/(576^k*(n-4*k)!));
for(n=0, M, print1(a(n), ", "));