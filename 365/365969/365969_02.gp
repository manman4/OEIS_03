a(n) = if(n==0, 1, (n-1)! * sum(k=0, (n-1)\5, a(n-5*k-1)/(n-5*k-1)! ));                   
for(n=0, 23, print1(a(n),", "))  


