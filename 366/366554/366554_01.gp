a(n) = sum(k=0, n\4, binomial(k+1,n-4*k) * binomial(2*k,k)/(k+1) );                    
for(n=0, 46, print1(a(n),", "))  

\\ a(n) = A366589(n) + A366589(n-1).
b(n) = sum(k=0, n\4, binomial(k,n-4*k) * binomial(2*k,k)/(k+1) );                    
for(n=0, 46, print1(b(n) + b(n-1),", "))  