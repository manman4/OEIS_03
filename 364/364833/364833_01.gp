a(n) = sum(k=0, n\3, binomial(n-2*k-1,k) * binomial(2*n-3*k+1,n-3*k)/(2*n-3*k+1) );
for(n=0, 26, print1(a(n), ", "))

