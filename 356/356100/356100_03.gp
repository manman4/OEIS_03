M=19;

a(n) = sum(k=1, n, sumdiv(k, d, (d-1)^n));
for(n=1, M, print1(a(n), ", "));