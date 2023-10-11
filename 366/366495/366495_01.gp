a(n) = sum(k=0, n, binomial(3*k/2,n-k) * binomial(5*k/2,k) / (3*k/2+1) );                    
for(n=0, 23, print1(a(n),", "))  