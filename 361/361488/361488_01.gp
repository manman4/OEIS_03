a(n) = sum(k=0, n\3, binomial(2*k, k)*binomial(k, n-3*k));
for(n=0, 39, print1(a(n),", "))  
