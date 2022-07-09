M=23;

a(n) = n!*sum(k=0, n\5, stirling(n-4*k, k, 1)/(24^k*(n-4*k)!));
for(n=0, M, print1(a(n), ", "));