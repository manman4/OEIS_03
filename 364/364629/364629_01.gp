a(n) = sum(k=0, n, binomial(n+5*k+1,6*k+1) * binomial(3*k,k) / (2*k+1) );                 
for(n=0, 20, print1(a(n),", "))  