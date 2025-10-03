a(n) = sum(k=0, n, binomial(n+1,k) * binomial(5*k,n-k) )/(n+1);     
for(n=0, 22, print1(a(n),", ")) 
