a(n) = sum(k=0, n, binomial(n+8*k+2,9*k+2) * binomial(4*k,k) / (3*k+1) );                 
for(n=0, 19, print1(a(n),", "))  