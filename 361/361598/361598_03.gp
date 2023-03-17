M=19;

a(n) = sum(k=0, n, (n+k)!/(2*k)! * binomial(n,k));
for(n=0, M, print1(a(n), ", "));
