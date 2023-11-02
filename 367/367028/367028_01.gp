a(n) = sum(k=0, n\2, (-1)^k * binomial(n+k,k) * binomial(n,n-2*k) / (2*k+1) );
for(n=0, 29, print1(a(n), ", "))
