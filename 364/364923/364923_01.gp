a(n) = sum(k=0, n, 3^k * (-2)^(n-k) * binomial(n,k) * binomial(3*n+k+1,n) / (3*n+k+1) ); 
for(n=0, 20, print1(a(n),", "))   
