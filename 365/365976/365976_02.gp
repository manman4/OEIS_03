a(n) = if(n==0, 1, sum(k=0, (n-1)\4, (4*k)! * binomial(n,4*k+1) * a(n-4*k-1) ));                   
for(n=0, 21, print1(a(n),", "))  


