a(n) = sum(k=0, n, (-1)^(n-k) * binomial(2*n+3*k+1,k) * binomial(n-1,n-k)/(2*n+3*k+1) );
for(n=0, 21, print1(a(n), ", "))

