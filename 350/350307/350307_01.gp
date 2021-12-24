M=8;

a(n) = polcoef(prod(j=1, n, sum(k=-j, j, x^k))^n, 0);
for(n=0, M, print1(a(n), ", "));