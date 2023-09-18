a(n) = (1/(n+1)) * sum(k=0, n, binomial(n+k,k) * binomial(5*(n+1),n-k) );
for(n=0, 19, print1(a(n),", "))  

