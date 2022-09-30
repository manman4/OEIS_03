M=22;

a(n) = n!*sum(k=0, n, binomial(k, 3)/k!);
for(n=0, M, print1(a(n), ", "));