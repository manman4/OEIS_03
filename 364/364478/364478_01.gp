a(n) = sum(k=0, n\2, binomial(3*n+2*k,k) * binomial(3*n+k,n-2*k) / (2*n+3*k+1) );
for(n=0, 21, print1(a(n),", "))  

