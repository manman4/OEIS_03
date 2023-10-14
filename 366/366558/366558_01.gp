a(n) = sum(k=0, n\4, binomial(3*k+1,n-4*k) * binomial(4*k,k)/(3*k+1) );                    
for(n=0, 34, print1(a(n),", "))  

\\ a(n) = A366595(n) + A366595(n-1).
b(n) = sum(k=0, n\4, binomial(3*k,n-4*k) * binomial(4*k,k)/(3*k+1) );                  
for(n=0, 46, print1(b(n) + b(n-1),", "))  