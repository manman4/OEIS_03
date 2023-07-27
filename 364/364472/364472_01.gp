a(n) = sum(k=0, n\2, binomial(n+4*k,k) * binomial(n+3*k,n-2*k) / (5*k+1) );
for(n=0, 24, print1(a(n),", "))  

a(n) = sum(k=0, n\2, binomial(n+4*k,6*k) * binomial(6*k,k) / (5*k+1) );
for(n=0, 24, print1(a(n),", "))  