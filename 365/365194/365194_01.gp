a(n) = sum(k=0, n, binomial(6*n-k+1,k) * binomial(n-1,n-k)/(6*n-k+1) );
for(n=0, 20, print1(a(n), ", "))

