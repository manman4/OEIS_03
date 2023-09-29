a(n) = (1/(n+1)) * sum(k=0, n\3, binomial(n+k,k) * binomial(3*n-k+3,n-3*k) );
for(n=0, 22, print1(a(n),", "))   
