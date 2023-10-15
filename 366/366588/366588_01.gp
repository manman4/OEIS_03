a(n) = sum(k=0, n\3, binomial(k,n-3*k) * binomial(2*k,k)/(k+1) );                    
for(n=0, 41, print1(a(n),", "))  
