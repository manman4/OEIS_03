a(n) = sum(k=0, n, binomial(n+k-1, k)*binomial(n*k, n-k));

for(n=0, 18, print1(a(n),", "))  