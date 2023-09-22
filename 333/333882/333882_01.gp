a(n) = if(n==0, 1, sum(k=0, (n-1)\5, binomial(n-1,5*k) * a(n-5*k-1) ));                   
for(n=0, 25, print1(a(n),", "))  

