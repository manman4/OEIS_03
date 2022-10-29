M=20;

a(n) = n!*sum(k=0, n\2, (n-2*k)^k/k!);
for(n=0, M, print1(a(n), ", "));