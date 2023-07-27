a(n) = sum(k=0, n\2, binomial(2*n+3*k,k) * binomial(2*n+2*k,n-2*k) / (n+4*k+1) );
for(n=0, 22, print1(a(n),", "))  

