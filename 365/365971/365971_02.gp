a(n) = if(n==0, 1, (n-1)! * sum(k=0, (n-2)\3, a(n-3*k-2)/(n-3*k-2)! ));                   
for(n=0, 23, print1(a(n),", "))  


