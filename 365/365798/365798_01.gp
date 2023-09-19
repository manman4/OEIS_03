a(n) = sum(k=0, n\6, binomial(n-5*k,k) * binomial(n-k+1,n-5*k) / (n-k+1) );
for(n=0, 34, print1(a(n),", "))  

b(n) = sum(k=0, n\6, binomial(n-k,5*k) * binomial(5*k,k) / (4*k+1) );
for(n=0, 50, print1(a(n)-b(n),", "))

