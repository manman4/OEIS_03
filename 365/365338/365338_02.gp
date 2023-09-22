a(n) = if(n==0, 1, sum(k=0, (n-2)\3, binomial(n-1,3*k+1) * a(n-3*k-2) ));                   
for(n=0, 27, print1(a(n),", "))  
