a(n) = sum(k=0, n\2, binomial(k,n-2*k) * binomial(n+2*k+1,k) / (n+2*k+1) );          
for(n=0, 29, print1(a(n),", "))  
