a(n) = n!*(n+1)!*(n+2)!*sum(k=0, n, 1/(k!*(k+1)!*(k+2)!));
for(n=0, 13, print1(a(n), ", "))