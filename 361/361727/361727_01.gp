a(n) = sum(k=0, n\3, binomial(2*k, k)*binomial(2*k, n-3*k));
for(n=0, 35, print1(a(n),", "))  
