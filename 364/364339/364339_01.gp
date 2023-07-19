a(n) = sum(k=0, n, binomial(6*k+1,k) * binomial(6*k+1,n-k) / (6*k+1) );

for(n=0, 20, print1(a(n),", "))  

