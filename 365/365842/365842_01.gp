a(n) = (1/(n+1)) * sum(k=0, n, binomial(2*n+k+1,k) * binomial(3*(n+1),n-k) );
for(n=0, 20, print1(a(n),", "))  
