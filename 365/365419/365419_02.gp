a(n) = if(n==0, 1, sum(k=0, (n-2)\5, binomial(n-1,5*k+1) * a(n-5*k-2) ));                   
for(n=0, 28, print1(a(n),", "))  
