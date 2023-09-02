a(n) = (1/(5*n+1)) * sum(k=0, n\2, binomial(n-k,k) * binomial(5*n+1,n-k) );                        
for(n=0, 17, print1(a(n),", ")) 
