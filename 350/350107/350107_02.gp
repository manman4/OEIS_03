M=48;

a(n) = sum(k=1, n, k*sumdiv(k, d, (2*d-1)/d));
for(n=1, M, print1(a(n), ", "));