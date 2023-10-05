a(n) = sum(k=0, n, binomial(4*k+1,n-k) * binomial(2*k,k)/(k+1) );                    
for(n=0, 22, print1(a(n),", "))  