a(n) = if(n==0, 1, (n-1)! * sum(k=0, (n-3)\4, a(n-4*k-3)/(n-4*k-3)! ));                   
for(n=0, 23, print1(a(n),", "))  


