a(n) = sum(k=0, n\2, binomial(n+4*k+1,6*k+1) * binomial(4*k,k) / (3*k+1) );                 
for(n=0, 25, print1(a(n),", "))  