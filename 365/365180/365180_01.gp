a(n) = sum(k=0, n, binomial(n+3*k+1,k) * binomial(k,n-k)/(n+3*k+1) );
for(n=0, 21, print1(a(n), ", "))

