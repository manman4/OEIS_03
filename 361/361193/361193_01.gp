a(n) = n!*sum(k=0, n, (-2)^k*(k+1)^(k-1)*binomial(n, k)/k!);
for(n=0, 17, print1(a(n),", "))
