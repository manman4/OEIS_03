M=23;

a(n) = n!*polcoef(sum(k=3, n, binomial(x, k)), 3);
for(n=3, M, print1(a(n), ", "));