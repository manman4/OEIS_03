a(n) = (-1)^(n-1) * sum(k=0, n, binomial(n+2*k-1,n-k) * binomial(3*k-1,k) / (3*k-1) );                    
for(n=0, 21, print1(a(n),", "))  