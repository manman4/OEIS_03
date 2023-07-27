a(n) = sum(k=0, n\6, binomial(n,6*k) * binomial(6*k,k) / (5*k+1) );
for(n=0, 33, print1(a(n),", "))  

