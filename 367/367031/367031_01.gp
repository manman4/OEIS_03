a(n) = sum(k=0, n\2, (-1)^k * binomial(2*n,k) * binomial(2*n-k,n-2*k) / (n+k+1) );
for(n=0, 27, print1(a(n), ", "))
