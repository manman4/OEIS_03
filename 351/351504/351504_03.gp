M=21;

a(n) = n!*sum(k=0, n\4, k!*abs(stirling(n-3*k, k, 1))/(n-3*k)!);
for(n=0, M, print1(a(n), ", "));