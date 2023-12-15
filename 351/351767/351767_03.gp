M=18;

a(n) = sum(k=0, n, (n+2*k+2)!/(3*k+2)! * binomial(n,k));
for(n=0, M, print1(a(n), ", "));

