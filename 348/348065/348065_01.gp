M=24;

a(n) = n!*polcoef(sum(k=4, n, binomial(x, k)), 4);
for(n=4, M, print1(a(n), ", "));