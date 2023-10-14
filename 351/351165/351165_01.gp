M=16;

a(n) = n!*sumdiv(n, d, d^(d-n/d));
for(n=1, M, print1(a(n), ", "));