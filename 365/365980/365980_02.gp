a(n) = if(n==0, 1, sum(k=0, (n-3)\2, (2*k+2)! * binomial(n,2*k+3) * a(n-2*k-3) ));                   
for(n=0, 22, print1(a(n),", "))  


