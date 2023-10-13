M=52;

a(n) = sum(k=1, n, sumdiv(k, d, k/d%2*(2*d-1)));
for(n=1, M, print1(a(n), ", "));