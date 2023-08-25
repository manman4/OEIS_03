a(n) = sum(k=0, n, binomial(3*n+2*k+1,k) * binomial(n-1,n-k)/(3*n+2*k+1) );
for(n=0, 20, print1(a(n), ", "))

