a(n) = sum(k=0, n, (-1)^(n-k)*binomial(n+4*k+1, k)*binomial(k, n-k)/(n+4*k+1));
for(n=0, 21, print1(a(n), ", "))
