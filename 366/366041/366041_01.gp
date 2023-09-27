a(n) = (1/(n+1)) * sum(k=0, n\4, binomial(n+k,n) * binomial(2*n-4*k,n) );
for(n=0, 26, print1(a(n),", "))   
