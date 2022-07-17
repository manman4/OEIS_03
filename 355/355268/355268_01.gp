M=30;

a(n) = n!*sum(k=0, n\2, 1/k!);
for(n=0, M, print1(a(n), ", "));