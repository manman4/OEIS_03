a(n) = n!*sum(k=0, n, binomial(3*k, n-k)/k!);
for(n=0, 20, print1(a(n),", ")) 