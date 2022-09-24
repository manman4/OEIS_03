M=22;

a(n) = n!*sum(k=1, n\3, k^(k-1)*stirling(n-2*k, k, 2)/(2^k*(n-2*k)!));
for(n=0, M, print1(a(n), ", "));