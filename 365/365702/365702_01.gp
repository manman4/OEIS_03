a(n) = (1/(n+1)) * sum(k=0, n\5, binomial(n-4*k-1,n-5*k) * binomial(n+1,k) );
for(n=0, 41, print1(a(n),", "))
