a(n) = (1/(n+1)) * sum(k=0, n\4, binomial(k,n-4*k) * binomial(n+1,k) );          
for(n=0, 40, print1(a(n),", "));
