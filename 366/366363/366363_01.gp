a(n) = (-1)^(n-1) * sum(k=0, n, binomial(2*k-1,k) * binomial(k-1,n-k)/(2*k-1) );                    
for(n=0, 26, print1(a(n),", "))  

