M=22;

a(n) = n!*sum(k=0, n\3, abs(stirling(n-2*k, k, 1))/(2^k*(n-2*k)!));
for(n=0, M, print1(a(n), ", "));