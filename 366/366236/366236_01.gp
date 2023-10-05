a(n) = sum(k=0, n, binomial(3*k+1,n-k) * binomial(2*k,k)/(k+1) );                    
for(n=0, 23, print1(a(n),", "))  