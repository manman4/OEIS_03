a(n) = sum(k=0, n, (-1)^(n-k)*binomial(k, n-k)*binomial(5*k, k)/(4*k+1));
for(n=0, 21, print1(a(n), ", "))
