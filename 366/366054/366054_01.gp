a(n) = (1/(n+1)) * sum(k=0, n\4, binomial(n+1,k) * binomial(2*n-3*k,n-4*k) );
for(n=0, 26, print1(a(n),", "))   


