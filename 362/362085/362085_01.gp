a(n) = sum(k=0, n, binomial(k+2, k)*binomial(n*k, n-k));

for(n=0, 20, print1(a(n),", "))  