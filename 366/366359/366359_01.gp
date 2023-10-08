a(n) = (-1)^(n-1) * sum(k=0, n, binomial(5*k-1,k) * binomial(5*k-1,n-k)/(5*k-1) );                    
for(n=0, 19, print1(a(n),", "))  

