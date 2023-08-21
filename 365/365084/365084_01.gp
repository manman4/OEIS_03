a(n) = sum(k=0, n, (-1)^(n-k) * binomial(n+4*k-1,n-k) );                                
for(n=0, 30, print1(a(n),", "))   
