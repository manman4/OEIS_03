M=19;

a(n) = polcoef(prod(k=0, n-1, x+3*k+1), 4);
for(n=0, M, print1(a(n), ", "));



