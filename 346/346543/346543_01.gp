M=12;

a(n) = polcoef(prod(k=1, 2*n, x+(2*k-1)^2), n);
for(n=0, M, print1(a(n), ", "));