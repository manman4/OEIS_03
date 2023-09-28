a(n) = (1/(n+1)) * sum(k=0, n\4, (-1)^(n-k) * binomial(n+1,k) * binomial(n-k+1,n-4*k) );
for(n=0, 33, print1(a(n),", "))   


