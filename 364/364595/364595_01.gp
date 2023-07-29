a(n) = sum(k=0, n\3, binomial(n-2*k,k) * binomial(3*k,k) / (2*k+1) );                 
for(n=0, 35, print1(a(n),", "))  