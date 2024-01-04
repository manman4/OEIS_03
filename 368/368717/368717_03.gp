\\ a(0) = 0; a(n) = -n*a(n-1) + n^4.
a(n) = if(n==0, 0, -n*a(n-1) + n^4);
for(n=0, 21, print1(a(n), ", "));  
