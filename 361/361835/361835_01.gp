a(n) = sum(k=0, n, (-1)^(n-k)*binomial(2*k, k)*binomial(n*k, n-k));
for(n=0, 21, print1(a(n),", ")) 

