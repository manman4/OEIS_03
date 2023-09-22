a(n) = if(n==0, 1, sum(k=0, (n-5)\3, binomial(n-1,3*k+4) * a(n-3*k-5) ));                   
for(n=0, 30, print1(a(n),", "))  
