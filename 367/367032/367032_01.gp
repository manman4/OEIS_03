a(n) = sum(k=0, n\2, (-1)^k * binomial(2*n+k,k) * binomial(2*n,n-2*k) / (n+2*k+1) );
for(n=0, 26, print1(a(n), ", "))
