M=73;

a(n) = prod(k=1, #f=factor(n)[, 2], 6*f[k]+1);
for(n=1, M, print1(a(n), ", "));