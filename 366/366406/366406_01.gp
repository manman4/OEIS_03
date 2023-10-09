a(n) = (-1)^(n-1) * sum(k=0, n, binomial(9*k/2-1,k) * binomial(7*k/2-1,n-k) / (9*k/2-1) );                    
for(n=0, 20, print1(a(n),", "))  

