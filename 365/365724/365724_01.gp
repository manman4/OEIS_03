a(n) = (1/(n+1)) * sum(k=0, n\3, binomial(k,n-3*k) * binomial(n+1,k) );          
for(n=0, 37, print1(a(n),", "));
