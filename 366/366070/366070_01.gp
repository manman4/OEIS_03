a(n) = (1/(n+1)) * sum(k=0, n\2, binomial(n+k,k) * binomial(4*n-k+4,n-2*k) );
for(n=0, 20, print1(a(n),", "))   
