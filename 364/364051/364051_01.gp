a(n) = if(n==0, 1, sum(k=0, n-1, (-1)^k * binomial(n,k) * binomial(2*n+3*k,n-1-k) )/n);
for(n=0, 25, print1(a(n),", ")) 