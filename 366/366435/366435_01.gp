a(n) = (-1)^(n-1) * sum(k=0, n, binomial(7*k/2-1,k) * binomial(5*k/2,n-k) / (7*k/2-1) );                    
for(n=0, 21, print1(a(n),", "))  


b(n) = sum(k=0, n, a(k));
for(n=0, 22, print1(b(n),", "))
