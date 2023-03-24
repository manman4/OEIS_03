a(n) = sum(k=0, n, binomial(2*k,k) * binomial(n+2*k-1,n-k));
for(n=0, 22, print1(a(n),", "))     
