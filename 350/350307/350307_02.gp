M=8;

a(n) = polcoef(prod(j=1, n, sum(k=0, 2*j, x^k))^n, n^2*(n+1)/2);
for(n=0, M, print1(a(n), ", "));