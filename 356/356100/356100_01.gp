M=19;

a(n) = sum(k=1, n, (k-1)^n*(n\k));
for(n=1, M, print1(a(n), ", "));