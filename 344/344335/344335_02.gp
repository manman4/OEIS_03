M=71;

a(n) = prod(k=1, #f=factor(n)[, 2], 8*f[k]+1);
for(n=1, M, print1(a(n), ", "));