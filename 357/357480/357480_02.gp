M=23;

a(n) = n!*sum(k=0, n, binomial(k, 4)/k!);
for(n=0, M, print1(a(n), ", "));