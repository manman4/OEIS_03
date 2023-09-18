a(n) = (1/(n+1)) * sum(k=0, n, (-1)^k * binomial(n+k,k) * binomial(4*n-k+2,n-k) );
for(n=0, 22, print1(a(n),", "))  

