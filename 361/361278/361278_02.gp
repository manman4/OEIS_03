a(n) = n!*sum(k=0, n, binomial(2*k, n-k)/k!);
for(n=0, 21, print1(a(n),", ")) 