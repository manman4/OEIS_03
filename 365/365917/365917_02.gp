a(n) = if(n==0, 1, sum(k=0, (n-5)\4, binomial(n,4*k+5) * a(n-4*k-5) ));                   
for(n=0, 31, print1(a(n),", "))  
