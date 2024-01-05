\\ a(0) = 0; a(n) = n^3 * a(n-1) + n.
a(n) = if(n==0, 0, n^3 * a(n-1) + n);
for(n=0, 14, print1(a(n), ", "));  
