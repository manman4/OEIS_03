a(n) = if(n==0, 1, sum(k=0, (n-5)\2, binomial(n,2*k+5) * a(n-2*k-5) ));                   
for(n=0, 28, print1(a(n),", "))  
