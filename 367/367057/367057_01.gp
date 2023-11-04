a(n) = sum(k=0, n\3, binomial(2*n-5*k+1,k) * binomial(3*n-8*k,n-3*k)/(2*n-5*k+1) );
for(n=0, 23, print1(a(n), ", "))
