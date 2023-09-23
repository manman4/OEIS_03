a(n) = if(n==0, 1, sum(k=0, (n-3)\5, binomial(n,5*k+3) * a(n-5*k-3) ));                   
for(n=0, 27, print1(a(n),", "))  
