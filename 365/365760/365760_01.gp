a(n) = sum(k=0, n\5, binomial(n-4*k,k) * binomial(n+k+1,n-4*k) / (n+k+1) );
for(n=0, 32, print1(a(n),", "))  

b(n) = sum(k=0, n\5, binomial(n+k,6*k) * binomial(6*k,k) / (5*k+1) );
for(n=0, 50, print1(a(n)-b(n),", "))

