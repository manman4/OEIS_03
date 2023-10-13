M=28;

a(n) = sum(k=1, n, sumdiv(k, d, 3^(d-1)));
for(n=1, M, print1(a(n), ", "));