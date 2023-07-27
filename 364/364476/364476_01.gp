a(n) = sum(k=0, n\2, binomial(n+5*k,k) * binomial(n+4*k,n-2*k) / (6*k+1) );
for(n=0, 23, print1(a(n),", "))  

a(n) = sum(k=0, n\2, binomial(n+5*k,7*k) * binomial(7*k,k) / (6*k+1) );
for(n=0, 23, print1(a(n),", "))  
