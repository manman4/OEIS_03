a(n) = sum(k=0, n\5, binomial(n-4*k-1,n-5*k) * binomial(n-k+1,k) / (n-k+1) );
for(n=0, 41, print1(a(n),", "))
