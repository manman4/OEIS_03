a(n) = (1/(n+1)) * sum(k=0, n\2, (-1)^k * binomial(n+1,k) * binomial(3*n-k+1,n-2*k) );
for(n=0, 24, print1(a(n),", "))   


