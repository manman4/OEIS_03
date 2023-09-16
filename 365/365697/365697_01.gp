a(n) = sum(k=0, n\4, binomial(n-3*k-1,n-4*k) * binomial(n-k+1,k) / (n-k+1) );
for(n=0, 38, print1(a(n),", "))
