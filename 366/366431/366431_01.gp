a(n) = sum(k=0, n, binomial(n+3*k/2-1,n-k) * binomial(5*k/2,k) / (3*k/2+1) );                    
for(n=0, 22, print1(a(n),", "))  


b(n) = sum(k=0, n, a(k));
for(n=0, 22, print1(b(n),", "))
