a(n) = (1/(n+1)) * sum(k=0, n\5, binomial(n+k,n) * binomial(2*n-5*k,n) );
for(n=0, 27, print1(a(n),", "))   
