a(n) = sum(k=0, n, binomial(4*k+1,k) * binomial(4*k+1,n-k) / (4*k+1) );

for(n=0, 21, print1(a(n),", "))  

