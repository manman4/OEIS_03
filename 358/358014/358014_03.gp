M=22;

a(n) = n!*sum(k=0, n\4, k!*stirling(n-3*k, k, 2)/(n-3*k)!);
for(n=0, M, print1(a(n), ", "));