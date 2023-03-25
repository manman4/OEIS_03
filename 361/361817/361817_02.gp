a(n) = sum(k=0, n, (-1)^(n-k) * binomial(2*k,k) * binomial(4*k,n-k));         
for(n=0, 27, print1(a(n),", ")) 