a(n) = sum(k=0, n\3, binomial(2*n-6*k+1,k) * binomial(2*n-6*k+1,n-3*k) / (2*n-6*k+1) );

for(n=0, 27, print1(a(n),", "))  

