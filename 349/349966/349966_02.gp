M=15;

a(n) = polcoef(sum(k=0, n, k^n*x^k)^2, n);
for(n=0, M, print1(a(n), ", "));