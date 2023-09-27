a(n) = (1/(n+1)) * sum(k=0, n\2, binomial(n+1,k) * binomial(4*n-k+2,n-2*k) );
for(n=0, 20, print1(a(n),", "))   


