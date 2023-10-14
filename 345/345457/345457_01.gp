M=15;

a(n) = sum(k=0, n, binomial(5*n+3, 5*k));
for(n=0, M, print1(a(n), ", "));