a(n) = sum(k=0, n, binomial(n, k)*binomial(4*n+2*k+1, n)/(4*n+2*k+1));

for(n=0, 17, print1(a(n), ", ")) 
