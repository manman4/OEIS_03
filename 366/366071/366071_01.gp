a(n) = (1/(n+1)) * sum(k=0, n\3, (-1)^(n-k) * binomial(n+k,k) * binomial(n-2*k-1,n-3*k) );
for(n=0, 35, print1(a(n),", "))   
