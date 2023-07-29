a(n) = sum(k=0, n\2, binomial(n-k,k) * binomial(3*k,k) / (2*k+1) );                 
for(n=0, 30, print1(a(n),", "))  