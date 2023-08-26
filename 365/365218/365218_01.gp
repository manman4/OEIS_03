a(n) = (1/(6*n+1)) * sum(k=0, n, (-1)^(n-k) * binomial(6*n+1,k) * binomial(n-1,n-k) );
for(n=0, 20, print1(a(n),", "))   
