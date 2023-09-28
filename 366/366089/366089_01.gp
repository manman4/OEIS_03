a(n) = (1/(n+1)) * sum(k=0, n\4, (-1)^k * binomial(n+1,k) * binomial(3*n-3*k+1,n-4*k) );
for(n=0, 22, print1(a(n),", "))   


