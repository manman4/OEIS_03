a(n) = sum(k=0, n, binomial(n+k-1,n-k) * binomial(3*k,k)/(2*k+1) );                    
for(n=0, 22, print1(a(n),", "))  

\\ A199475
b(n) = sum(k=0, n, a(k));
for(n=0, 20, print1(b(n),", "))