a(n) = sum(k=0, n, binomial(3*n+k+1,k) * binomial(k,n-k)/(3*n+k+1) );
for(n=0, 20, print1(a(n), ", "))

