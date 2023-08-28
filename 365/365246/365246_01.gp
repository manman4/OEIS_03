a(n) = (1/(2*n+1)) * sum(k=0, n\2, binomial(n-k-1,k) * binomial(2*n+1,n-2*k) );
for(n=0, 25, print1(a(n), ", "))

