a(n) = (1/(n+1)) * sum(k=0, n\4, (-1)^(n-k) * binomial(n+k,k) * binomial(n-3*k-1,n-4*k) );
for(n=0, 37, print1(a(n),", "))   
