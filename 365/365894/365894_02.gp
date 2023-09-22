a(n) = if(n==0, 1, sum(k=0, (n-4)\3, binomial(n-1,3*k+3) * a(n-3*k-4) ));                   
for(n=0, 29, print1(a(n),", "))  
