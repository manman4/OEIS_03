a(n) = sum(k=0, n\4, binomial(k,n-4*k) * binomial(n-k+1,k) / (n-k+1) );          
for(n=0, 42, print1(a(n),", "))  
