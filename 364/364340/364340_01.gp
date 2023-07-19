a(n) = sum(k=0, n, binomial(n+5*k+1,k) * binomial(n+5*k+1,n-k) / (n+5*k+1) );

for(n=0, 20, print1(a(n),", "))  

