a(n) = sum(k=0, n\5, binomial(n-4*k,k) * binomial(2*n-5*k+1,n-4*k)/(2*n-5*k+1) );                   
for(n=0, 27, print1(a(n),", "))  

