a(n) = (1/(n+1)) * sum(k=0, n\3, binomial(n+k,k) * binomial(2*n-k+2,n-3*k) );
for(n=0, 25, print1(a(n),", "))   
