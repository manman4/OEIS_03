a(n) = sum(k=0, n, binomial(n+4*k-1,n-k) * binomial(5*k,k) / (4*k+1) );                    
for(n=0, 19, print1(a(n),", "))  

for(n=0, 39, print1(sum(k=0, n, a(k)),", "))
