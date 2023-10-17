a(n) = sum(k=0, n, binomial(2*(3*k+1),n-k) * binomial(4*k,k)/(3*k+1) );                    
for(n=0, 20, print1(a(n),", "))  

