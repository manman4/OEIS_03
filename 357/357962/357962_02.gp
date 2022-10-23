M=23;

a(n) = n!*sum(k=0, n\2, stirling(n-k, n-2*k, 2)/(n-k)!);
for(n=0, M, print1(a(n), ", "));