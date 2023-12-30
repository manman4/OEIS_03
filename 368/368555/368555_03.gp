a(n) = if(n<2, 1+3*n, (n+3)*a(n-1) - 3*n*a(n-2));
for(n=0, 21, print1(a(n), ", "));  
