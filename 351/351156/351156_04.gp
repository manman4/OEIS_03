M=25;

a(n) = n!*sum(k=0, n\3, abs(stirling(k, n-3*k, 1))/(6^k*k!));
for(n=0, M, print1(a(n), ", "));