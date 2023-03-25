a(n) = sum(k=0, n, binomial(2*k,k) * binomial(5*k,n-k));         
for(n=0, 21, print1(a(n),", ")) 