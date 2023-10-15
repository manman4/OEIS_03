a(n) = sum(k=0, n\3, binomial(3*k,n-3*k) * binomial(4*k,k)/(3*k+1) );                    
for(n=0, 32, print1(a(n),", "))  
