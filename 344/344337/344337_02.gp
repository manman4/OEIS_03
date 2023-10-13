M=76;

a(n) = prod(k=1, #f=factor(n)[, 2], 9);
for(n=1, M, print1(a(n), ", "));