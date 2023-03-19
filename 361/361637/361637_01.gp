M=26;

a(n) = n!*sum(k=0, n\4, 1/(k!^4*(n-4*k)!));
for(n=0, M, print1(a(n), ", "))