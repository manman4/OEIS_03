a(n) = sum(k=0, n\4, binomial(2*k,n-4*k) * binomial(3*k,k)/(2*k+1) );                    
for(n=0, 40, print1(a(n),", "))  
