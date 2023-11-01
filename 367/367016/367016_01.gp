a(n) = sum(k=0, n, (-1)^(n-k)*binomial(k, n-k)*binomial(4*k, k)/(3*k+1));
for(n=0, 22, print1(a(n), ", "))
