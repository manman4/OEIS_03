a(n) = n!*sum(k=0, n, (-1)^(n-k)*binomial(-4*k, n-k)/k!);
for(n=0, 17, print1(a(n),", ")) 