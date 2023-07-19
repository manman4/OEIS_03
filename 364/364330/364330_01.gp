a(n) = sum(k=0, n\4, binomial(2*n-8*k+1,k) * binomial(2*n-8*k+1,n-4*k) / (2*n-8*k+1) );

for(n=0, 28, print1(a(n),", "))  

