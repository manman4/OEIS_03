M=30;

a(n) = sum(k=0, n\10, (10*k)!*stirling(n, 10*k, 2)/(10!^k*k!));
for(n=0, M, print1(a(n), ", "));