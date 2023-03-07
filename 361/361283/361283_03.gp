a(n) = n!*sum(k=0, n, binomial(n+3*k-1, n-k)/k!);
for(n=0, 17, print1(a(n),", ")) 