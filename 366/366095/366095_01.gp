a(n) = (1/(n+1)) * sum(k=0, n\3, binomial(n+k,k) * binomial(n-k+1,n-3*k) );
for(n=0, 30, print1(a(n),", "))   
