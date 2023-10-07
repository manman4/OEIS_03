a(n) = (-1)^(n-1) * sum(k=0, n, binomial(5*k-1,k) * binomial(n+3*k-2,n-k)/(5*k-1) );                    
for(n=0, 20, print1(a(n),", "))  

