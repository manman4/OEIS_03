a(n) = (1/(5*n+1)) * sum(k=0, n, binomial(5*n+1,k) * binomial(k,n-k) );
for(n=0, 20, print1(a(n), ", "))

