a(n) = sum(k=0, n\2, binomial(n+7*k+2,9*k+2) * binomial(4*k,k) / (3*k+1) );                 
for(n=0, 23, print1(a(n),", "))  