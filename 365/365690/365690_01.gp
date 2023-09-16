a(n) = sum(k=0, n\2, binomial(n-k-1,n-2*k) * binomial(n+2*k+1,k) / (n+2*k+1) );
for(n=0, 28, print1(a(n),", "))
