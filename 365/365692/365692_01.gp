a(n) = sum(k=0, n\2, binomial(n-k-1,k) * binomial(n+2*k+1,n-2*k) / (n+2*k+1) );
for(n=0, 27, print1(a(n),", "))
