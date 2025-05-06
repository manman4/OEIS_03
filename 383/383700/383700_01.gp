M=18;

a(n) = polcoef(prod(k=0, n-1, x+4*k+1), 2);
for(n=0, M, print1(a(n), ", "));



