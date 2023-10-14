M=18;

a(n) = polcoef(prod(k=1, 2*n, x+(-1)^k*k), n);
for(n=0, M, print1(a(n), ", "));