a(n) = (1/(n+1)) * sum(k=0, n\5, binomial(n+k,k) * binomial(n-4*k-1,n-5*k) );
for(n=0, 40, print1(a(n),", "))   
