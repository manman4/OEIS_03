a(n) = sum(k=0, n\5, binomial(n-4*k-1,k) * binomial(n-3*k+1,n-5*k) / (n-3*k+1) );          
for(n=0, 39, print1(a(n),", "))  
