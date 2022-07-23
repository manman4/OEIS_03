M=22;

a(n) = n!*sum(k=1, n, 1/(k*(n\k)!));
for(n=1, M, print1(a(n), ", "));