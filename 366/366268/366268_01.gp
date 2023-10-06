a(n) = sum(k=0, n, binomial(4*k+1,n-k) * binomial(5*k,k)/(4*k+1) );                    
for(n=0, 19, print1(a(n),", "))  

\\ A366273
b(n) = sum(k=0, n, binomial(4*k, n-k)*binomial(5*k, k)/(4*k+1));
c(n) = if(n==0, 1, b(n)+b(n-1));
for(n=0, 22, print1(a(n) - c(n),", "))
