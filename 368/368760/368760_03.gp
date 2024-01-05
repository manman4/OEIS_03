\\ a(0) = 1; a(n) = n*a(n-1) + n^3.
a(n) = if(n==0, 1, n*a(n-1) + n^3);
for(n=0, 21, print1(a(n), ", "));  
