a(n) = sum(k=0, n\2, binomial(2*(n-2*k), k)*binomial(2*(n-2*k), n-2*k));
for(n=0, 25, print1(a(n),", "))  
