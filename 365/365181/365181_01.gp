a(n) = sum(k=0, n, binomial(2*n+2*k+1,k) * binomial(k,n-k)/(2*n+2*k+1) );
for(n=0, 21, print1(a(n), ", "))

