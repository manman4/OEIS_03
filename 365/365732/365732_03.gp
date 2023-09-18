a(n) = sum(k=0, n\6, binomial(n-3*k,3*k) * binomial(3*k,k) / (2*k+1) );          
for(n=0, 36, print1(a(n),", "))  
