a(n) = sum(k=0, n, (k+2)^(n-k)*binomial(n, k));
for(n=0, 21, print1(a(n), ", "));