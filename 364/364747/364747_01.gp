a(n) = if(n==0, 1, sum(k=0, n-1, binomial(n,k) * binomial(4*n-3*k,n-1-k) )/n);
for(n=0, 21, print1(a(n),", ")) 