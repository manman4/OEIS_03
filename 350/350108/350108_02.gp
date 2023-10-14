M=41;

a(n) = sum(k=1, n, k*sumdiv(k, d, (d^3-(d-1)^3)/d));
for(n=1, M, print1(a(n), ", "));