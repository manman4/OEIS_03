a(n) = (-1)^(n-1) * sum(k=0, n, binomial(3*k/2-1,k) * binomial(k/2-1,n-k) / (3*k/2-1) );                    
for(n=0, 31, print1(a(n),", "))  

