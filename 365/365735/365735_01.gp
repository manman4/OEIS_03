a(n) = sum(k=0, n\5, binomial(n-4*k-1,k) * binomial(n-2*k+1,n-5*k) / (n-2*k+1) );          
for(n=0, 36, print1(a(n),", "))  
