a(n) = if(n==0, 1, sum(k=0, (n-1)\3, (3*k)! * binomial(n,3*k+1) * a(n-3*k-1) ));                   
for(n=0, 21, print1(a(n),", "))  


