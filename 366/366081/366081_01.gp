a(n) = (1/(n+1)) * sum(k=0, n\2, (-1)^k * binomial(n+1,k) * binomial(2*n-k,n-2*k) );
for(n=0, 29, print1(a(n),", "))   


