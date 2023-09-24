a(n) = if(n==0, 1, sum(k=0, (n-2)\3, (3*k+1)! * binomial(n,3*k+2) * a(n-3*k-2) ));                   
for(n=0, 22, print1(a(n),", "))  


