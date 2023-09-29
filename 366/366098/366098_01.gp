a(n) = (1/(n+1)) * sum(k=0, n\4, binomial(n+k,k) * binomial(n-k+1,n-4*k) );
for(n=0, 32, print1(a(n),", "))   
