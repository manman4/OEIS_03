M=23;

a(n) = n!*polcoef(sum(k=5, n, binomial(x, k)), 5);
for(n=5, M, print1(a(n), ", "));