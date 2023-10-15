a(n) = sum(k=0, n\4, binomial(3*k,n-4*k) * binomial(4*k,k)/(3*k+1) );                    
for(n=0, 36, print1(a(n),", "))  
