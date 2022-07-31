M=39;

a(n) = sum(k=1, n, k^3*sumdiv(k, d, 1-(1-1/d)^3));
for(n=1, M, print1(a(n), ", "));