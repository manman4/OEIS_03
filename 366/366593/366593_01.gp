a(n) = sum(k=0, n\2, binomial(3*k,n-2*k) * binomial(4*k,k)/(3*k+1) );                    
for(n=0, 27, print1(a(n),", "))  
