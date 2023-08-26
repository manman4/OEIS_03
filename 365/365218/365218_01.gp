a(n) = sum(k=0, n, (-1)^k * 2^(n-k) * binomial(n,k) * binomial(6*n+k+1,n)/(6*n+k+1) ); 
for(n=0, 20, print1(a(n),", "))   
