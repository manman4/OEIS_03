M=20;

a(n) = n!*sum(k=0, n\2, 2^(n-2*k)*k!*stirling(n-k, k, 2)/(n-k)!);
for(n=0, M, print1(a(n), ", "));