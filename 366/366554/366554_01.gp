a(n) = sum(k=0, n\4, binomial(k+1,n-4*k) * binomial(2*k,k)/(k+1) );                    
for(n=0, 46, print1(a(n),", "))  