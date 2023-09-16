a(n) = sum(k=0, n\5, binomial(n-4*k-1,n-5*k) * binomial(n-5*k+1,k) / (n-5*k+1) );
for(n=0, 46, print1(a(n),", "))
