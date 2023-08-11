a(n) = sum(k=0, n, (-1)^k * 2^(n-k) * binomial(n,k) * binomial(4*n+k+1,n) / (4*n+k+1) ); 
for(n=0, 24, print1(a(n),", "))   
