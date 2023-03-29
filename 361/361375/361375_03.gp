a(n) = if(n==0, 1, (3/n) * sum(k=0, n-1, (n+2*k) * a(k)));
for(n=0, 15, print1(a(n),", "))
