a(n) = sum(k=0, n\2, (-1)^k * binomial(n+2*k,k) * binomial(n+k,n-2*k) / (3*k+1) );
for(n=0, 27, print1(a(n), ", "))
