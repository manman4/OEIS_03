a(n) = sum(k=0, n, (-2)^k*(k+3)^(n-k)*binomial(n, k));
for(n=0, 22, print1(a(n), ", "));