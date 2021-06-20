M=16;

a(n) = sum(k=0, n, binomial(5*n+2, 5*k));
for(n=0, M, print1(a(n), ", "));