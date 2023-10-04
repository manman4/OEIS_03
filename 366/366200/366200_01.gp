a(n) = sum(k=0, n, binomial(3*k,n-k) * binomial(3*k,k)/(2*k+1) );                    
for(n=0, 22, print1(a(n),", "))  