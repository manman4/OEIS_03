\\ a(0) = 1; a(n) = n*a(n-1) + (-1)^n * n.
a(n) = if(n==0, 1, n*a(n-1) + (-1)^n * n);
for(n=0, 23, print1(a(n), ", "));  
