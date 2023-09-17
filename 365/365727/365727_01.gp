a(n) = sum(k=0, n\4, binomial(k,n-4*k) * binomial(n-3*k+1,k) / (n-3*k+1) );          
for(n=0, 51, print1(a(n),", "))  
