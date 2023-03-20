M=27;

a(n) = n!*sum(k=0, n\6, 1/(k!^3*(3*k)!*(n-6*k)!));
for(n=0, M, print1(a(n), ", "));
