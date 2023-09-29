a(n) = (1/(n+1)) * sum(k=0, n\4, binomial(n+k,k) * binomial(3*n-k+3,n-4*k) );
for(n=0, 22, print1(a(n),", "))   
