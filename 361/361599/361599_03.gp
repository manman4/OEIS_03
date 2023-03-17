M=19;

a(n) = sum(k=0, n, (n+2*k)!/(3*k)! * binomial(n,k));
for(n=0, M, print1(a(n), ", "));
