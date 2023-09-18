a(n) = sum(k=0, n\3, binomial(n-2*k,k) * binomial(n+3*k+1,n-2*k) / (n+3*k+1) );
for(n=0, 27, print1(a(n),", "))  

b(n) = sum(k=0, n\3, binomial(n+3*k,6*k) * binomial(6*k,k) / (5*k+1) );
for(n=0, 50, print1(a(n)-b(n),", "))

