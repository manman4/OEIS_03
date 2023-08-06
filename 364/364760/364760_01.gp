a(n) = sum(k=0, n, (-1)^k * binomial(n+1,k) * binomial(2*k,n-k) )/(n+1);     
for(n=0, 29, print1(a(n),", ")) 