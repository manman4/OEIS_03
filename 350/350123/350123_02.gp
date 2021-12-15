M=43;

a(n) = sum(k=1, n, k^2*sumdiv(k, d, (2*d-1)/d^2));
for(n=1, M, print1(a(n), ", "));