a(n) = (1/(n+1)) * sum(k=0, n, binomial(n+k,k) * binomial(4*(n+1),n-k) );
for(n=0, 20, print1(a(n),", "))  

