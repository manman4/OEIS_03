a(n) = (-1)^(n-1) * sum(k=0, n, binomial(3*k-1,k) * binomial(n+k-2,n-k)/(3*k-1) );                    
for(n=0, 22, print1(a(n),", "))  

