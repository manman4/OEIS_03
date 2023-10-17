a(n) = sum(k=0, n, binomial(2*(k+1),n-k) * binomial(2*k,k)/(k+1) );                    
for(n=0, 24, print1(a(n),", "))  

