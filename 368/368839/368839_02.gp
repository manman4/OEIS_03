\\ a(n) = n*(n+1)*(n+2)*a(n-1) + 1.
a(n) = if(n==0, 1, n*(n+1)*(n+2)*a(n-1) + 1);
for(n=0, 20, print1(a(n), ", "));
