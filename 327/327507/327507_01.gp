M=30;

a(n) = sum(k=0, n\6, (6*k)!*stirling(n, 6*k, 2)/(6!^k*k!));
for(n=0, M, print1(a(n), ", "));