a(n) = if(n==0, 1, (3/n) * sum(k=0, n-1, (-1)^(n-1-k) * (n+2*k) * (n-k) * a(k)));
for(n=0, 15, print1(a(n),", "))
