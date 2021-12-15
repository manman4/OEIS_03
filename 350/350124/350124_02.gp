M=40;

a(n) = sum(k=1, n, k^2*sumdiv(k, d, (d^3-(d-1)^3)/d^2));
for(n=1, M, print1(a(n), ", "));