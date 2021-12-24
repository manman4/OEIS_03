M=10;

a(n) = polcoef(prod(k=1, n, x^k+1+1/x^k)^n, 0);
for(n=0, M, print1(a(n), ", "));