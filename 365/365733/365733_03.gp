a(n) = sum(k=0, n\6, binomial(n-2*k,4*k) * binomial(4*k,k) / (3*k+1) );          
for(n=0, 36, print1(a(n),", "))  
