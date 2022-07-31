M=19;

a(n) = sum(k=1, n, (k-1)^n*sum(j=1, n\k, j^2));
for(n=1, M, print1(a(n), ", "));