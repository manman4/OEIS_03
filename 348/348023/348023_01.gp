M=18;

a(n) = polcoef(prod(k=0, 2*n-1, x+(-1)^k*k), n);
for(n=0, M, print1(a(n), ", "));