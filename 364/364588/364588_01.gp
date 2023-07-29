                                                                     
a(n) = sum(k=0, n\2, binomial(n+k,3*k) * binomial(4*k,k) / (3*k+1) );                 
for(n=0, 26, print1(a(n),", "))  