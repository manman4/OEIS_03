a(n) = if(n==0, 1, sum(k=0, (n-3)\4, (4*k+2)! * binomial(n,4*k+3) * a(n-4*k-3) ));                   
for(n=0, 23, print1(a(n),", "))  


