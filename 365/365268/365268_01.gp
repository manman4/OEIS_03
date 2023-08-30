a(n) = sum(k=0, n\4, binomial(n-3*k,k) * binomial(2*n-4*k+1,n-3*k)/(2*n-4*k+1) );
for(n=0, 26, print1(a(n), ", "))

