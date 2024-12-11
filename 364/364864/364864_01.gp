a(n) = sum(k=0, n, (-1)^k * 2^(n-k) * binomial(n,k) * binomial(3*n+k+1,n) / (3*n+k+1) ); 
for(n=0, 27, print1(a(n),", "))   

