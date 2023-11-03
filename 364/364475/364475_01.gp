a(n) = sum(k=0, n\2, binomial(3*n-3*k,k) * binomial(3*n-4*k,n-2*k) / (2*n-2*k+1) );
for(n=0, 22, print1(a(n),", "))  

b(n) = sum(k=0, n\2, binomial(2*n-k+1, k)*binomial(3*n-3*k, n-2*k)/(2*n-k+1));
for(n=0, 23, print1(a(n)-b(n),", "))