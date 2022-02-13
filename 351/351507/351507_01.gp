M=13;

a(n) = polcoef(prod(k=1, n, 1+k*x)^n, n);
for(n=0, M, print1(a(n), ", "));