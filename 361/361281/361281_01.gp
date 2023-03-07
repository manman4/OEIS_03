a(n) = n!*sum(k=0, n, binomial(n*k, n-k)/k!);
for(n=0, 17, print1(a(n),", ")) 