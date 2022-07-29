M=18;

a(n) = sum(k=1, n, k*sumdiv(k, d, d^(d-1)));
for(n=1, M, print1(a(n), ", "));