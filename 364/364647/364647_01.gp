a(n) = sum(k=0, n, (-1)^k * 5^(n-k) * binomial(n+k,2*k) * binomial(3*k,k) / (2*k+1) );            
for(n=0, 24, print1(a(n),", "))  
