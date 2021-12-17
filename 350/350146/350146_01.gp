M=54;

a(n) = sum(k=1, n, sumdiv(k, d, k/d%2*d));
for(n=1, M, print1(a(n), ", "));