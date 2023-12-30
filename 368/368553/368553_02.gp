\\ a(0) = 1; a(n) = 2*a(n-1) + n^n.
a(n) = if(n==0, 1, 2*a(n-1) + n^n);
for(n=0, 18, print1(a(n), ", "));  
