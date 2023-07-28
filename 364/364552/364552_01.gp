a(n) = sum(k=0, n\4, binomial(n-k,3*k) * binomial(3*k,k) / (2*k+1) );
for(n=0, 34, print1(a(n),", "))  

