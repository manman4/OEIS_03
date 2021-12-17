M=44;

a(n) = sum(k=1, n, sumdiv(k, d, k/d%2*(d^3-(d-1)^3)));
for(n=1, M, print1(a(n), ", "));