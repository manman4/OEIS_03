a(n) = sum(k=0, n\4, binomial(n-1,n-4*k) * binomial(4*k,k) / (3*k+1) );                    
for(n=0, 32, print1(a(n),", "))  

for(n=0, 39, print1(sum(k=0, n, a(k)),", "))
