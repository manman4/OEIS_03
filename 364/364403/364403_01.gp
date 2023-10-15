a(n) = sum(k=0, n\4, binomial(n-k-1,n-4*k) * binomial(3*k,k) / (2*k+1) );                    
for(n=0, 31, print1(a(n),", "))  

for(n=0, 39, print1(sum(k=0, n, a(k)),", "))
