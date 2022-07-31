M=18;

a(n) = sum(k=1, n, k^n*sumdiv(k, d, 1-(1-1/d)^n));
for(n=1, M, print1(a(n), ", "));