a(n) = (-1)^(n-1) * sum(k=0, n, binomial(7*k/2-1,k) * binomial(n+5*k/2-2,n-k) / (7*k/2-1) );                    
for(n=0, 20, print1(a(n),", "))  

