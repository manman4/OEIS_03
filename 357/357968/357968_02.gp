M=23;

a(n) = n!*sum(k=0, n\4, stirling(k, n-4*k, 2)/k!);
for(n=0, M, print1(a(n), ", "));