a(n) = (1/(n+1)) * sum(k=0, n\5, binomial(n+1,k) * binomial(2*n-5*k,n-5*k) );                   
for(n=0, 27, print1(a(n),", "))  

