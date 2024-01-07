a(n) = n!*(n+3)!*sum(k=0, n, 1/(k!*(k+3)!));
for(n=0, 16, print1(a(n), ", "))