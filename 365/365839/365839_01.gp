a(n) = (1/(n+1)) * sum(k=0, n, binomial(3*n+k+2,k) * binomial(2*(n+1),n-k) );
for(n=0, 20, print1(a(n),", "))  
