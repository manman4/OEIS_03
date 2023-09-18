a(n) = sum(k=0, n\4, binomial(n-3*k,k) * binomial(n+2*k+1,n-3*k) / (n+2*k+1) );
for(n=0, 30, print1(a(n),", "))  

b(n) = sum(k=0, n\4, binomial(n+2*k,6*k) * binomial(6*k,k) / (5*k+1) );
for(n=0, 50, print1(a(n)-b(n),", "))

