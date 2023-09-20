a(n) = (1/(n+1)) * sum(k=0, n, binomial(4*n+k+3,k) * binomial(4*(n+1),n-k) );
for(n=0, 17, print1(a(n),", "))  
