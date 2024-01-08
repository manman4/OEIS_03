a(n) = n!*(n+1)!*sum(k=0, n, (-1)^k/(k!*(k+1)!));
for(n=0, 16, print1(a(n), ", "))