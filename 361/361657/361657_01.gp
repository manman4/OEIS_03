M=27;

a(n) = n!*sum(k=0, n\4, 1/(k!^2*(2*k)!*(n-4*k)!));
for(n=0, M, print1(a(n), ", "));
