M=73;

a(n) = sum(k=1, n, sumdiv(k, d, d^4<=k));
for(n=1, M, print1(a(n), ", "));