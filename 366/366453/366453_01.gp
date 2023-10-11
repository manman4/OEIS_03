a(n) = sum(k=0, n, binomial(5*k/2+1,n-k) * binomial(7*k/2,k) / (5*k/2+1) );                    
for(n=0, 21, print1(a(n),", "))  

