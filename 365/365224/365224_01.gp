a(n) = sum(k=0, n, (-1)^(n-k) * binomial(5*n-k+1,k) * binomial(n-1,n-k)/(5*n-k+1) );
for(n=0, 23, print1(a(n), ", "))

