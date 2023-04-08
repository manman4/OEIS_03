a(n) = sum(k=0, n, binomial(n+k-1, k)*binomial(2*k, n-k));

for(n=0, 22, print1(a(n),", "))  