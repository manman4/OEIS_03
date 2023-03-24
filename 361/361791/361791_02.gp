a(n) = sum(k=0, n, (-1)^(n-k) * binomial(2*k,k) * binomial(n+4*k-1,n-k));
for(n=0, 28, print1(a(n),", "))     
