a(n) = sum(k=0, n, binomial(2*k,k) * binomial(4*k,n-k));         
for(n=0, 22, print1(a(n),", ")) 