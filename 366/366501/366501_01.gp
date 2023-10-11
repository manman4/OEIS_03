a(n) = (-1)^(n-1) * sum(k=0, n, binomial(n+3*k-1,n-k) * binomial(4*k-1,k) / (4*k-1) );                    
for(n=0, 20, print1(a(n),", "))  