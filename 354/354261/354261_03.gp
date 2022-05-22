M=16;

a(n) = sum(k=0, n, (3/2)^k*(2*k)!*abs(stirling(n, k, 1))/k!);
for(n=0, M, print1(a(n), ", "));