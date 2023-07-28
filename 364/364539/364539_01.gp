a(n) = sum(k=0, n\3, binomial(n+2*k,5*k) * binomial(5*k,k) / (4*k+1) );
for(n=0, 28, print1(a(n),", "))  

