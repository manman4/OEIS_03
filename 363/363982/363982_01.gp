a(n) = if(n==0, 1, sum(k=0, n-1, (-1)^k * binomial(n,k) * binomial(2*n+k,n-1-k) )/n);
for(n=0, 27, print1(a(n),", ")) 