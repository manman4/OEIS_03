a(n) = sum(k=0, n, binomial(n+7*k+3,n-k) * binomial(4*k,k)/(3*k+1) );                    
for(n=0, 19, print1(a(n),", "))  