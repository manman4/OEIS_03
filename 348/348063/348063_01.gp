M=24;

a(n) = n!*polcoef(sum(k=2, n, binomial(x, k)), 2);
for(n=2, M, print1(a(n), ", "));