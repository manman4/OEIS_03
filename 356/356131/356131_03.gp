M=18;

a(n) = sum(k=1, n, k*sumdiv(k, d, (d-1)^n/d));
for(n=1, M, print1(a(n), ", "));