a(n) = sum(k=0, n\5, binomial(n,5*k) * binomial(5*k,k) / (4*k+1) );
for(n=0, 33, print1(a(n),", "))  

