M=20;

a(n) = n!*sum(k=0, n\3, (n-3*k)^k/k!);
for(n=0, M, print1(a(n), ", "));