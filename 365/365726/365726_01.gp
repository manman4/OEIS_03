a(n) = sum(k=0, n\3, binomial(k,n-3*k) * binomial(n+2*k+1,k) / (n+2*k+1) );          
for(n=0, 34, print1(a(n),", "))  
