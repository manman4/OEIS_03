a(n) = (-1)^n * sum(k=0, n, (-3)^k * binomial(n,k) * binomial(5*n+k+1,n) / (5*n+k+1) ); 
for(n=0, 17, print1(a(n),", "))   

