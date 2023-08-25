a(n) = sum(k=0, n, binomial(4*n+k+1,k) * binomial(k,n-k)/(4*n+k+1) );
for(n=0, 20, print1(a(n), ", "))

