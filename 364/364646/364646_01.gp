a(n) = sum(k=0, n, (-1)^k * 4^(n-k) * binomial(n+k,2*k) * binomial(3*k,k) / (2*k+1) );            
for(n=0, 26, print1(a(n),", "))  
