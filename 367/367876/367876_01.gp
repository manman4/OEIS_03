a(n) = sum(k=0, n, (-1)^k*(k+2)^(n-k)*binomial(n, k));
for(n=0, 23, print1(a(n), ", "));