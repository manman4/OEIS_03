a(n) = (-1)^(n-1) * sum(k=0, n, binomial(4*k-1,k) * binomial(3*k,n-k) / (4*k-1) );                    
for(n=0, 20, print1(a(n),", "))  


b(n) = sum(k=0, n, a(k));
for(n=0, 22, print1(b(n),", "))
