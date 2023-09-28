a(n) = (1/(n+1)) * sum(k=0, n\2, (-1)^k * binomial(n+1,k) * binomial(4*n-k+2,n-2*k) );
for(n=0, 21, print1(a(n),", "))   


