a(n) = sum(k=0, n\3, binomial(k,n-3*k) * binomial(n-k+1,k) / (n-k+1) );          
for(n=0, 40, print1(a(n),", "))  
