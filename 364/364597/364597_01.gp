a(n) = sum(k=0, n\3, binomial(n,3*k) * binomial(5*k,k) / (4*k+1) );                 
for(n=0, 30, print1(a(n),", "))  