a(n) = sum(k=0, n\2, (-1)^k * binomial(n+3*k,k) * binomial(n+2*k,n-2*k) / (4*k+1) );
for(n=0, 26, print1(a(n), ", "))
