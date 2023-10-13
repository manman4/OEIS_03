M=30;

a(n) = sum(k=0, n\9, (9*k)!*stirling(n, 9*k, 2)/(9!^k*k!));
for(n=0, M, print1(a(n), ", "));