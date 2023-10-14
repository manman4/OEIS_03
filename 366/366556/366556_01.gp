a(n) = sum(k=0, n\4, binomial(2*k+1,n-4*k) * binomial(3*k,k)/(2*k+1) );                    
for(n=0, 34, print1(a(n),", "))  

\\ a(n) = A366592(n) + A366592(n-1).
b(n) = sum(k=0, n\4, binomial(2*k,n-4*k) * binomial(3*k,k)/(2*k+1) );                  
for(n=0, 46, print1(b(n) + b(n-1),", "))  