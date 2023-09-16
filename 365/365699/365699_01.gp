a(n) = sum(k=0, n\5, binomial(n-4*k-1,n-5*k) * binomial(n-3*k+1,k) / (n-3*k+1) );
for(n=0, 43, print1(a(n),", "))
