M=26;

a(n) = n!*sum(k=0, n\7, 1/(k!^4*(3*k)!*(n-7*k)!));
for(n=0, M, print1(a(n), ", "));
