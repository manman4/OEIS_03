M=22;

a(n) = n!*sum(k=0, n\2, stirling(k, n-2*k, 2)/k!);
for(n=0, M, print1(a(n), ", "));