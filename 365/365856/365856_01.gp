a(n) = (1/(n+1)) * sum(k=0, n, (-1)^k * binomial(2*n+k+1,k) * binomial(6*n-k+4,n-k) );
for(n=0, 20, print1(a(n),", "))  
