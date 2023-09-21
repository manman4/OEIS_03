a(n) = (1/(n+1)) * sum(k=0, n, (-1)^k * binomial(2*n+k+1,k) * binomial(5*n-k+3,n-k) );
for(n=0, 22, print1(a(n),", "))  
