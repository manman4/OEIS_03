a(n) = sum(k=0, n\3, binomial(2*k,n-3*k) * binomial(3*k,k)/(2*k+1) );                    
for(n=0, 35, print1(a(n),", "))  
