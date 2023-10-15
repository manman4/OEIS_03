a(n) = sum(k=0, n\4, binomial(n-2*k-1,n-4*k) * binomial(2*k,k) / (k+1) );                    
for(n=0, 39, print1(a(n),", "))  
