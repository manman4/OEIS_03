M=13;

a(n) = (2*n)!*polcoef(sum(k=n, 2*n, binomial(x-2*n, k)), n);
for(n=0, M, print1(a(n), ", "));