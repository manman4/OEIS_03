a(n) = (1/(2*n+1)) * sum(k=0, n\3, binomial(2*n+1,k) * binomial(3*n-3*k,n-3*k) );
for(n=0, 23, print1(a(n), ", "))
