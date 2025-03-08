a(n) = (1/(2*n+1)) * sum(k=0, n\2, (-1)^k * binomial(3*n-k,k) * binomial(3*n-2*k,n-2*k) );
for(n=0, 26, print1(a(n), ", "))

