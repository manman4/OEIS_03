a(n) = (1/(n+1)) * sum(k=0, n, binomial(3*n+k+2,k) * binomial(3*(n+1),n-k) );
for(n=0, 19, print1(a(n),", "))  
