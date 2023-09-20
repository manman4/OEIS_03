a(n) = (1/(n+1)) * sum(k=0, n, binomial(n+1,k) * binomial(5*n-k+3,n-k) );
for(n=0, 19, print1(a(n),", "))  

b(n) = (1/(n+1)) * sum(k=0, n, binomial(4*n+k+3,k) * binomial(n+1,n-k) );
for(n=0, 50, print1(a(n)-b(n),", "))  