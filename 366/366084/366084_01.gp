a(n) = (1/(n+1)) * sum(k=0, n\3, (-1)^k * binomial(n+1,k) * binomial(3*n-2*k+1,n-3*k) );
for(n=0, 23, print1(a(n),", "))   


