a(n) = n!*sum(k=0, n, binomial(4*k, n-k)/k!);
for(n=0, 19, print1(a(n),", ")) 