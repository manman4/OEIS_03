M=23;

a(n) = n!*sum(k=0, n\4, (k+1)^(k-1)*abs(stirling(n-3*k, k, 1))/(6^k*(n-3*k)!));
for(n=0, M, print1(a(n), ", "));