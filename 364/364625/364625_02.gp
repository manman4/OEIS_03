a(n) = sum(k=0, n\2, binomial(n+k+2,3*k+2) * binomial(2*k,k) / (k+1) );                 
for(n=0, 28, print1(a(n),", "))  