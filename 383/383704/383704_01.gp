M=16;

a(n) = polcoef(prod(k=0, 2*n-1, x-(-1)^k*(2*k+1)), n);
for(n=0, M, print1(a(n), ", "));



