a(n) = sum(k=0, n\5, binomial(n-4*k,k) * binomial(n-k+1,n-4*k)/(n-k+1) );
for(n=0, 34, print1(a(n), ", "))

