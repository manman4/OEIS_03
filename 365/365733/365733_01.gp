a(n) = sum(k=0, n\6, binomial(n-5*k,k) * binomial(n-2*k+1,n-5*k) / (n-2*k+1) );          
for(n=0, 36, print1(a(n),", "))  
