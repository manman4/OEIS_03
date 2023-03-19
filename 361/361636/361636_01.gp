M=22;

a(n) = sum(k=0, n\4, (n+k)!/(k!^5*(n-4*k)!));
for(n=0, M, print1(a(n), ", "))