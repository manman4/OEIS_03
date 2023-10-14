M=30;

a(n) = sum(k=0, n\7, (7*k)!*stirling(n, 7*k, 2)/(7!^k*k!));
for(n=0, M, print1(a(n), ", "));